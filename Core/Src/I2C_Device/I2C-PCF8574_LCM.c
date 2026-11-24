/*
 * I2C-PCF8574_LCM.c
 *
 *  Created on: 2026年11月22日
 *      Author: Ethan
 */

#include  "I2C_Device\I2C-PCF8574_LCM.h"



// 記錄目前的背光狀態（預設開啟）
static uint8_t lcm_backlight_state = LCM_BACKLIGHT;

/**
 * @brief 內部函式：發送一個 byte 給 PCF8574
 */
static void LCM_WriteI2C(uint8_t data) {
    BSP_I2C_Write(LCM_I2C_ADDR, &data, 1);

    //HAL_I2C_Master_Transmit(hi2c_main, LCM_I2C_ADDR << 1, &data, 1, 100);
}

/**
 * @brief 內部函式：觸發 LCM 的 Enable 腳（產生一個高到低的脈衝，讓 LCM 讀取資料）
 */
static void LCM_PulseEnable(uint8_t data) {
    LCM_WriteI2C(data | LCM_EN);  // 拉高 EN
    HAL_Delay(1);                 // 保持一段時間（大於 450ns）
    LCM_WriteI2C(data & ~LCM_EN); // 拉低 EN
    HAL_Delay(1);
}

/**
 * @brief 內部函式：發送 4 個 bit 的資料或指令給 LCM
 * @param value 要發送的 4-bit 數值
 * @param mode  LCM_RS (代表是資料) 或 0 (代表是指令)
 */
static void LCM_Write4Bits(uint8_t value, uint8_t mode) {
    // 組合 PCF8574 要輸出的 8 bits：背光 + 模式(RS) + RW(寫入) + 4-bit 資料
    uint8_t data = (value & 0xF0) | mode | lcm_backlight_state;
    // 注意：RW 腳 (0x02) 保持為 0，代表我們要「寫入」LCM

    LCM_PulseEnable(data);
}

/**
 * @brief 內部函式：發送完整的 8-bit 指令（拆成兩次 4-bit）
 */
static void LCM_Send(uint8_t value, uint8_t mode) {
    LCM_Write4Bits(value & 0xF0, mode);        // 先送高 4 位
    LCM_Write4Bits((value << 4) & 0xF0, mode); // 再送低 4 位
}

/**
 * @brief 發送指令給 LCM
 */
static void LCM_Command(uint8_t cmd) {
    LCM_Send(cmd, 0); // mode = 0
}

/**
 * @brief 發送資料（字元）給 LCM
 */
static void LCM_Data(uint8_t data) {
    LCM_Send(data, LCM_RS); // mode = RS
}

/**
 * @brief 初始化 LCM (HD44780 標準初始化流程)
 */

HAL_StatusTypeDef PCF8574_Init(void) {
    uint8_t default_state = 0xFF; // 預設 8 個腳位全為高電位

#ifdef use_TCA9548

	if(TCA9548_SelectByAddress(LCM_I2C_ADDR) != HAL_OK){
		printf("TCA9555_I/O expander not exist, or Not connected to TCA9548");
	}

#endif

    // 1. 確認晶片在位 (呼叫你的 BSP)
    if (BSP_I2C_IsReady(LCM_I2C_ADDR) != HAL_OK) {
        return HAL_ERROR;
    }

    // 2. 寫入初始狀態
    if (BSP_I2C_Write(LCM_I2C_ADDR, &default_state, 1) != HAL_OK) {
        return HAL_ERROR;
    }
    HAL_Delay(50);


    // 3. 強制進入 4-bit 模式 (這是 HD44780 手冊規定的特殊喚醒流程)
    LCM_Write4Bits(0x30, 0); HAL_Delay(5);
    LCM_Write4Bits(0x30, 0); HAL_Delay(1);
    LCM_Write4Bits(0x30, 0); HAL_Delay(1);
    LCM_Write4Bits(0x20, 0); // 切換到 4-bit 模式

    // 4. 設定模式：4-bit, 2 行, 5x8 字型
    LCM_Command(0x28);

    // 5. 關閉顯示，清除畫面，設定輸入模式
    LCM_Command(0x08); // 顯示關閉
    LCM_Command(0x01); // 清除畫面
    HAL_Delay(2);      // Clear 指令需要較長時間
    LCM_Command(0x06); // 游標向右移動
    LCM_Command(0x0C); // 開啟顯示，關閉游標

    return HAL_OK;
}


I2C_Status PCF8574_DeviceInit_Wrapper(void) {
    if (PCF8574_Init() == HAL_OK) {
        return I2C_OK;
    }
    return I2C_ERR_INIT;
}


/**
 * @brief 清除畫面
 */
HAL_StatusTypeDef LCM_Clear(void) {
    LCM_Command(0x01);
    HAL_Delay(2); // 清除需要至少 1.52ms
    return HAL_OK;
}

/**
 * @brief 設定游標位置
 * @param row 行 (0 或 1)
 * @param col 列 (0 ~ 15)
 */
HAL_StatusTypeDef LCM_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address;
    if (row == 0) {
        address = 0x00 + col;
    } else {
        address = 0x40 + col; // 第二行的起始位址是 0x40
    }
    LCM_Command(0x80 | address);
    return HAL_OK;
}

/**
 * @brief 在螢幕上印出字串
 */
HAL_StatusTypeDef LCM_Print(const char *str) {
    while (*str) {
        LCM_Data(*str++);
    }
    return HAL_OK;
}

/**
 * @brief 控制背光開關
 */
HAL_StatusTypeDef LCM_Backlight(uint8_t on) {
    if (on) {
        lcm_backlight_state = LCM_BACKLIGHT;
    } else {
        lcm_backlight_state = 0;
    }
    // 重新寫入一次目前的背光狀態
    uint8_t data = lcm_backlight_state;
    LCM_WriteI2C(data);
    return HAL_OK;
}


