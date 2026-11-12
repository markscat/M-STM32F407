/*
 * BPS_i2c.h
 *
 *  Created on: 2026年11月12日
 *      Author: user
 */

// bsp_i2c.h
#ifndef BSP_I2C_H
#define BSP_I2C_H

#include "stm32f4xx_hal.h" // 目前還是依賴 HAL，但未來換 MCU 只要改這裡

// 定義一個通用的 I2C 寫入函式

HAL_StatusTypeDef BSP_I2C_Write(uint8_t dev_addr_7bit, const uint8_t *pData, uint16_t size);
HAL_StatusTypeDef BSP_I2C_IsReady(uint8_t dev_addr_7bit);

#endif/*BPS_I2C_H_ */
