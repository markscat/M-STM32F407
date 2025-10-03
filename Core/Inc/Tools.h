/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.h
  * @brief   This file contains all the function prototypes for
  *          the i2c.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __TOOLS_H
#define __TOOLS_H


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "main.h"
#include "I2C_Peripherals.h"
#include "ring_buffer.h" // << 也需要環形緩衝區的函數
#include <ctype.h>

//#include "stm32f4xx_hal.h"

//extern uint32_t us;

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

typedef struct {
    uint8_t century;  // 世紀部分 (e.g., 20)
    uint8_t year;     // 年份部分 (e.g., 25)
} YearComponents;


void Delay_us(uint32_t us);
uint8_t calculate_checksum(const char *data_str, size_t len);
void Process_Time_String(const char *time_str);
YearComponents split_year(uint16_t full_year);
uint16_t combine_year(YearComponents y);

char* ParseCommand(const char* command_line, int* command_id_ptr);
bool CheckTimeStruct (const RTC_Time* time_to_check );
void HandleUartCommands(void);
//掃描所有Ｉ２Ｃ設備
void Scan_I2C_Address(void);

//ring Buffer
void ProcessUartRingBuffer(void);
// 1. 命令緩衝區，存放從 UART 組裝好的一整行命令
extern char g_command_line_buffer[80];
extern uint16_t g_command_line_index ;
// 2. 通信旗標，【核心】ISR 和主迴圈之間的信號
extern volatile bool g_command_line_ready ;

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TOOLS_H__ */

