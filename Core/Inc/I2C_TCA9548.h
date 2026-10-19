/**
  ******************************************************************************
  * @file    eeprom_handler.h
  * @brief   AT24C32 EEPROM驱动模块
  * Created on: Mar 16, 2025
  * @aurhor Deepseek and Ethan
  ******************************************************************************
  */


#ifndef INC_I2C_TCA9548_H_
#define INC_I2C_TCA9548_H_

// eeprom_handler.h
#pragma once
#include "stm32f4xx_hal.h"       // 核心HAL库
#include "stm32f4xx_hal_i2c.h"   // I2C外设支持
#include "stdbool.h"
#include "i2c.h"
#include "I2C_parameter.h"


// 通过外部声明引用全局变量
//extern I2C_HandleTypeDef hi2c1;  // I2C实例（在main.c中定义）

/* [共用 I2C 配置] 修改原 EEPROM 的 I2C 句柄为通用名称 */
// 原文件 eeprom_handler.h 中修改：
// extern I2C_HandleTypeDef* hi2c_eeprom; --> 修改为：
//extern I2C_HandleTypeDef* hi2c_main;    // [MOD] 重命名为通用I2C句柄


/* 初始化（可做一次 bus scan 確認在位） */
TCA9548_Status TCA9548_Init(void);

/* 選擇單一通道（0~7），會關閉其他通道 */
TCA9548_Status TCA9548_SelectChannel(uint8_t ch);

/* 關閉所有通道（隔離 downstream） */
TCA9548_Status TCA9548_DisableAll(void);

/* 讀回目前通道設定（除錯用） */
TCA9548_Status TCA9548_GetChannel(uint8_t *mask);




#endif /* INC_I2C_TCA9548_H_ */
