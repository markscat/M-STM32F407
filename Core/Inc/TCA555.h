/*
 * TCA555.h
 *
 *  Created on: Feb 28, 2025
 *      Author: user
 */

#ifndef INC_TCA555_H_
#define INC_TCA555_H_
//不加下面兩行,編譯就會有問題
#include "main.h"  
#define TCA9555_ADDR  (0x20 << 1)  // I2C 地址 (ADDR=GND)


void TCA9555_Init();
void TCA9555_WriteOutput(uint16_t data);
uint16_t TCA9555_ReadInput();

#define TCA9555_GPIO_MASK(port, pin) ((uint16_t)(1 << (port * 8 + pin)))
extern I2C_HandleTypeDef hi2c1;  // CubeMX 生成的 I2C 句柄



#endif /* INC_TCA555_H_ */
