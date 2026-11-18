/*
 * TCA555.h
 *
 *  Created on: Feb 28, 2025
 *      Author: user
 */

#ifndef INC_I2C_TCA555_H_
#define INC_I2C_TCA555_H_
#include "main.h"
#include "i2c.h"
#include <I2C_Device/BPS_i2c.h>

// TCA9555_Config.h

// --- Port 0 配置 (0x06) ---
// 0 = 輸出 (Output)
// 1 = 輸入 (Input)

#define TCA9555_P0_PIN_0_DIR  0  // 輸出
#define TCA9555_P0_PIN_1_DIR  1  // 輸入
#define TCA9555_P0_PIN_2_DIR  0  // 輸出
#define TCA9555_P0_PIN_3_DIR  1  // 輸入
#define TCA9555_P0_PIN_4_DIR  0  // 輸出
#define TCA9555_P0_PIN_5_DIR  1  // 輸入
#define TCA9555_P0_PIN_6_DIR  0  // 輸出
#define TCA9555_P0_PIN_7_DIR  1  // 輸入

// 利用位元運算，自動算出這 8 個 bit 的組合
#define TCA9555_P0_DIR_MASK ( \
    (TCA9555_P0_PIN_0_DIR << 0) | \
    (TCA9555_P0_PIN_1_DIR << 1) | \
    (TCA9555_P0_PIN_2_DIR << 2) | \
    (TCA9555_P0_PIN_3_DIR << 3) | \
    (TCA9555_P0_PIN_4_DIR << 4) | \
    (TCA9555_P0_PIN_5_DIR << 5) | \
    (TCA9555_P0_PIN_6_DIR << 6) | \
    (TCA9555_P0_PIN_7_DIR << 7)  \
)

// --- Port 1 配置 (0x02) ---
// 0 = 輸出 (Output)
// 1 = 輸入 (Input)
#define TCA9555_P1_PIN_0_DIR  0  // 輸出
#define TCA9555_P1_PIN_1_DIR  1  // 輸入
#define TCA9555_P1_PIN_2_DIR  0  // 輸出
#define TCA9555_P1_PIN_3_DIR  1  // 輸入
#define TCA9555_P1_PIN_4_DIR  0  // 輸出
#define TCA9555_P1_PIN_5_DIR  1  // 輸入
#define TCA9555_P1_PIN_6_DIR  0  // 輸出
#define TCA9555_P1_PIN_7_DIR  1  // 輸入


// 利用位元運算，自動算出這 8 個 bit 的組合
#define TCA9555_P1_DIR_MASK ( \
    (TCA9555_P1_PIN_0_DIR << 0) | \
    (TCA9555_P1_PIN_1_DIR << 1) | \
    (TCA9555_P1_PIN_2_DIR << 2) | \
    (TCA9555_P1_PIN_3_DIR << 3) | \
    (TCA9555_P1_PIN_4_DIR << 4) | \
    (TCA9555_P1_PIN_5_DIR << 5) | \
    (TCA9555_P1_PIN_6_DIR << 6) | \
    (TCA9555_P1_PIN_7_DIR << 7)  \
)

// 定義 Port 0 和 Port 1 的暫存器
#define TCA9555_REG_INPUT_P0   0x00
#define TCA9555_REG_INPUT_P1   0x01
#define TCA9555_REG_OUTPUT_P0  0x02
#define TCA9555_REG_OUTPUT_P1  0x03
#define TCA9555_REG_CONFIG_P0  0x06
#define TCA9555_REG_CONFIG_P1  0x07

// 專屬 TCA9555 的錯誤碼
typedef enum {
    TCA9555_OK = 0,
    TCA9555_ERR_NOT_FOUND,     // 設備無回應
    TCA9555_ERR_WRITE_FAIL,    // 寫入失敗
    TCA9555_ERR_PARAM          // 參數錯誤
} TCA9555_Status;



#define TCA9555_ADDR  0x20  // I2C 地址 (ADDR=GND)

/**
 *  @brief 初始化TCA9555
 *  1. 確認設備是否就緒
 *  2. 配置 TCA9555 的 16 個 GPIO 為輸出模式（默認全部輸入）
 */

// 定義 Port 方向常數
#define TCA9555_DIR_OUTPUT  0x00  // 0 = 輸出
#define TCA9555_DIR_INPUT   0xFF  // 1 = 輸入
#define TCA9555_GPIO_MASK(port, pin) ((uint16_t)(1 << (port * 8 + pin)))

extern I2C_HandleTypeDef hi2c1;  // CubeMX 生成的 I2C 句柄


TCA9555_Status TCA9555_Init(void);

int TCA9555_DeviceInit_Wrapper(void);

/**
 *  @brief 輸出
 *  @param data 輸出的
 *  2. 配置 TCA9555 的 16 個 GPIO 為輸出模式（默認全部輸入）
 */
HAL_StatusTypeDef TCA9555_WriteOutput(uint8_t P0_data,uint8_t P1_data);


uint16_t TCA9555_ReadInput();

/*測試程式
 * 把所有的io全部設定為輸出,輪流輸出,確認io都有輸出
 *
 * */
void Test_TCA9555_LED(void);

/*
 * @brief 動態設定 TCA9555 的 Port 0 和 Port 1 方向
 * @param p0_dir Port 0 (P00~P07) 的方向 (使用 TCA9555_DIR_OUTPUT 或 TCA9555_DIR_INPUT)
 * @param p1_dir Port 1 (P10~P17) 的方向
 * */
void TCA9555_ConfigPorts(uint8_t p0_dir, uint8_t p1_dir);

/*
 * @brief 動態設定 TCA9555 的 Port 0 的方向
 * @param p0_dir Port 0 (P00~P07) 的方向 (使用 TCA9555_DIR_OUTPUT 或 TCA9555_DIR_INPUT)
 */
void TCA9555_SetPort0Dir(uint8_t dir);

/*
 * @brief 動態設定 TCA9555 的 Port 1 的方向
 * @param p0_dir Port 1 (P10~P17) 的方向 (使用 TCA9555_DIR_OUTPUT 或 TCA9555_DIR_INPUT)
 */
void TCA9555_SetPort1Dir(uint8_t dir);

/**
 * @brief 獨立設定 TCA9555 某個腳位是輸入還是輸出 (不影響同 Port 的其他腳位)
 * @param pin 腳位編號 (0~15，0代表P00，8代表P10)
 * @param is_input 1 = 設為輸入，0 = 設為輸出
 */
void TCA9555_SetPinDirection(uint8_t pin, uint8_t is_input);

uint8_t TCA9555_ReadPin(uint8_t pin);

void TCA9555_WritePin(uint8_t pin, uint8_t level);




#endif /* INC_I2C_TCA555_H_ */
