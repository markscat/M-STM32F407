/*
 * TCA555.h
 *
 *  Created on: Feb 28, 2025
 *      Author: user
 */

#ifndef INC_TCA555_H_
#define INC_TCA555_H_
#include "main.h"
#include "i2c.h"

#define TCA9555_ADDR  (0x20 << 1)  // I2C 地址 (ADDR=GND)

/**
 *  @brief 初始化TCA9555
 *  1. 確認設備是否就緒
 *  2. 配置 TCA9555 的 16 個 GPIO 為輸出模式（默認全部輸入）
 */
void TCA9555_Init();
/**
 *  @brief 輸出
 *  @param data 輸出的
 *  2. 配置 TCA9555 的 16 個 GPIO 為輸出模式（默認全部輸入）
 */
void TCA9555_WriteOutput(uint16_t data);


uint16_t TCA9555_ReadInput();

#define TCA9555_GPIO_MASK(port, pin) ((uint16_t)(1 << (port * 8 + pin)))

extern I2C_HandleTypeDef hi2c1;  // CubeMX 生成的 I2C 句柄



#endif /* INC_TCA555_H_ */
