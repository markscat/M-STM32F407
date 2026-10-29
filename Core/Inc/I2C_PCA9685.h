/*
 * I2C_PCA9685.h
 *
 *  Created on: 2026年10月29日
 *      Author: user
 */

#ifndef INC_I2C_PCA9685_H_
#define INC_I2C_PCA9685_H_

#define PCA9685_ADDR 0x40 // 假設 A0~A5 全接地
#define PCA9685_MODE1 0x00
#define PCA9685_PRESCALE 0xFE
#define PCA9685_LED0_ON_L 0x06

// 喚醒並設定頻率
HAL_StatusTypeDef  PCA9685_Init(void);

// 設定指定通道的脈寬 (off_ticks)
HAL_StatusTypeDef  PCA9685_SetPWM(uint8_t channel, uint16_t off_ticks);

#endif /* INC_I2C_PCA9685_H_ */
