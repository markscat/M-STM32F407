/*
 * I2C-PCF8574_CLM.h
 *
 *  Created on: 2026年11月22日
 *      Author: Ethan
 */

#ifndef INC_I2C_DEVICE_I2C_PCF8574_LCM_H_
#define INC_I2C_DEVICE_I2C_PCF8574_LCM_H_

#include "stm32f4xx_hal.h"
#include "i2c.h"
#include <I2C_Device\I2C_Peripherals.h>
#include "I2C_Device\I2C_TCA9548.h"

/* =========================================================
 * LCM 與 PCF8574 設定
 * ========================================================= */
#define LCM_I2C_ADDR        0x27   // 你的掃描結果
#define LCM_COLS            16     // 螢幕寬度 (16 字元)
#define LCM_ROWS            2      // 螢幕高度 (2 行)

/* 根據常見的 PCF8574 轉接板定義引腳映射 (對應到 PCF8574 的 P0~P7) */
#define LCM_BACKLIGHT       0x08   // P3：背光控制 (1=亮, 0=暗)
#define LCM_EN              0x04   // P2：Enable 腳
#define LCM_RW              0x02   // P1：Read/Write 腳
#define LCM_RS              0x01   // P0：Register Select 腳

/* 函式宣告 */
HAL_StatusTypeDef LCM_Init(void);
HAL_StatusTypeDef LCM_Print(const char *str);
HAL_StatusTypeDef LCM_SetCursor(uint8_t row, uint8_t col);
HAL_StatusTypeDef LCM_Clear(void);
HAL_StatusTypeDef LCM_Backlight(uint8_t on);
HAL_StatusTypeDef PCF8574_Init(void);
int PCF8574_DeviceInit_Wrapper(void);

#endif /* INC_I2C_DEVICE_I2C_PCF8574_LCM_H_ */
