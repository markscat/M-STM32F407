/*
 * I2C-PCF8574_LCM.c
 *
 *  Created on: 2026年11月22日
 *      Author: Ethan
 */

#include  "I2C_Device\I2C-PCF8574_LCM.h"


#define LCM_MAX_ROWS 2
#define LCM_MAX_COLS 16

// 記錄目前游標位置，供自動換行使用
static uint8_t lcm_row = 0;
static uint8_t lcm_col = 0;
// 記錄目前的背光狀態（預設開啟）
static uint8_t lcm_backlight_state = LCM_BACKLIGHT;


/**
 * @brief 內部函式：發送一個 byte 給 PCF8574
 */


static HAL_StatusTypeDef LCM_WriteI2C(uint8_t data) {
    return BSP_I2C_Write(LCM_I2C_ADDR, &data, 1);
}

/**
 * @brief 內部函式：觸發 LCM 的 Enable 腳（產生一個高到低的脈衝，讓 LCM 讀取資料）
 */
static HAL_StatusTypeDef  LCM_PulseEnable(uint8_t data) {

    HAL_StatusTypeDef status;

    status = LCM_WriteI2C(data & ~LCM_EN); // 先讓 RS, RW, D4~D7 穩定
    if (status != HAL_OK) return status;
    /*'return' with a value, in function returning void [-Wreturn-mismatch]*/


    status = LCM_WriteI2C(data | LCM_EN);  // 拉高 EN
    if (status != HAL_OK) return status;

    HAL_Delay(1);                 // 保持一段時間（大於 450ns）
    status = LCM_WriteI2C(data & ~LCM_EN); // 拉低 EN
    if (status != HAL_OK) return status;

    HAL_Delay(1);

    return HAL_OK;                         // 最後記得回傳 HAL_OK

}

/**
 * @brief 內部函式：發送 4 個 bit 的資料或指令給 LCM
 * @param value 要發送的 4-bit 數值
 * @param mode  LCM_RS (代表是資料) 或 0 (代表是指令)
 */
static HAL_StatusTypeDef  LCM_Write4Bits(uint8_t value, uint8_t mode) {
    // 組合 PCF8574 要輸出的 8 bits：背光 + 模式(RS) + RW(寫入) + 4-bit 資料
    uint8_t data = (value & 0xF0) | mode | lcm_backlight_state;
    // 注意：RW 腳 (0x02) 保持為 0，代表我們要「寫入」LCM

    return LCM_PulseEnable(data);
}

/**
 * @brief 內部函式：發送完整的 8-bit 指令（拆成兩次 4-bit）
 */
static HAL_StatusTypeDef LCM_Send(uint8_t value, uint8_t mode) {
    HAL_StatusTypeDef status;

    status = LCM_Write4Bits(value & 0xF0, mode);        // 先送高 4 位
    if (status != HAL_OK) return status;

    return LCM_Write4Bits((value << 4) & 0xF0, mode); // 再送低 4 位
}

/**
 * @brief 發送指令給 LCM
 */
static HAL_StatusTypeDef  LCM_Command(uint8_t cmd) {
    return LCM_Send(cmd, 0); // mode = 0
}

/**
 * @brief 發送資料（字元）給 LCM
 */
static HAL_StatusTypeDef  LCM_Data(uint8_t data) {
    return LCM_Send(data, LCM_RS); // mode = RS
}

/**
 * @brief 初始化 LCM (HD44780 標準初始化流程)
 */

HAL_StatusTypeDef PCF8574_Init(void) {
    uint8_t default_state = LCM_BACKLIGHT; // EN=0, RW=0, RS=0, 開啟背光

    //uint8_t default_state = 0xFF; // 預設 8 個腳位全為高電位

#ifdef use_TCA9548

	if(TCA9548_SelectByAddress(LCM_I2C_ADDR) != HAL_OK){
		printf("TCA9555_I/O expander not exist, or Not connected to TCA9548");
	}else{
		TCA9548_ShowChAndAddress_SelectByAddress(LCM_I2C_ADDR,"PCF8574_LCM_Controller");
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

    lcm_row = 0;
    lcm_col = 0;
    return HAL_OK;
}


int PCF8574_DeviceInit_Wrapper(void) {
    return (int) PCF8574_Init();
}


/**
 * @brief 清除畫面
 */
HAL_StatusTypeDef LCM_Clear(void) {
#ifdef use_TCA9548
    if (TCA9548_SelectByAddress(LCM_I2C_ADDR) != HAL_OK) {
        return HAL_ERROR;
    }
#endif
    if (LCM_Command(0x01) != HAL_OK) {
        return HAL_ERROR;
    }
    HAL_Delay(2); // 清除需要至少 1.52ms
    lcm_row = 0;
    lcm_col = 0;
    return HAL_OK;
}

/**
 * @brief 設定游標位置
 * @param row 行 (0 或 1)
 * @param col 列 (0 ~ 15)
 */
HAL_StatusTypeDef LCM_SetCursor(uint8_t row, uint8_t col) {

#ifdef use_TCA9548
    if (TCA9548_SelectByAddress(LCM_I2C_ADDR) != HAL_OK) {
        return HAL_ERROR;
    }
#endif

    if (row >= LCM_MAX_ROWS) row = LCM_MAX_ROWS - 1;
    if (col >= LCM_MAX_COLS) col = LCM_MAX_COLS - 1;

    uint8_t address;
    if (row == 0) {
        address = 0x00 + col;
    } else {
        address = 0x40 + col; // 第二行的起始位址是 0x40
    }

    if (LCM_Command(0x80 | address) != HAL_OK) {
           return HAL_ERROR;
       }

    lcm_row = row;
    lcm_col = col;
    return HAL_OK;
}

/**
 * @brief 在螢幕上印出字串
 */
HAL_StatusTypeDef LCM_Print(const char *str) {

#ifdef use_TCA9548

	if(TCA9548_SelectByAddress(LCM_I2C_ADDR) != HAL_OK){
		printf("PCF8574 LCM 控制器 not exist, or Not connected to TCA9548");
	}

#endif


    while (*str) {
    	 char ch = *str++;

    	        if (ch == '\r') {
    	            continue; // 略過 '\r'
    	        }

    	        if (ch == '\n') {
    	            if (lcm_row == 0) {
    	                if (LCM_SetCursor(1, 0) != HAL_OK) return HAL_ERROR;
    	            } else {
    	                break; // 第二行已經沒辦法再往下換行，結束輸出
    	            }
    	            continue;
    	        }
    	        // 第一行滿 16 字，自動跳到第二行第 0 格
    	        if (lcm_col >= LCM_MAX_COLS) {
    	        	if (lcm_row == 0) {
    	        		if (LCM_SetCursor(1, 0) != HAL_OK) return HAL_ERROR;
    	        	} else {
    	        		break; // 超過 32 字（兩行皆滿），停止輸出避免覆蓋
    	        		}
    	        }


    	        if (LCM_Data((uint8_t)ch) != HAL_OK) {
    	            return HAL_ERROR;
    	        }
    	        lcm_col++;
    }

    return HAL_OK;
}

/**
 * @brief 控制背光開關
 */
HAL_StatusTypeDef LCM_Backlight(uint8_t on) {

#ifdef use_TCA9548

	if(TCA9548_SelectByAddress(LCM_I2C_ADDR) != HAL_OK){
		printf("PCF8574 LCM 控制器 not exist, or Not connected to TCA9548");
	}

#endif

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


