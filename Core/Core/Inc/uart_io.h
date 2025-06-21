/*
 * uart_io.h
 *
 *  Created on: Feb 16, 2025
 *      Author: user
 */

#ifndef INC_UART_IO_H_
#define INC_UART_IO_H_

#pragma once
#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define BUFFER_SIZE 100


extern UART_HandleTypeDef huart2;  // CubeMX生成的UART2控制代碼

// 初始化UART輸入輸出
void uart_io_init(void);

// 檢查是否有數據可讀
bool uart_data_available(void);

// 自訂非阻塞回顯範例
void echo_received_data(void);

void clear_input_buffer() ;


#endif /* INC_UART_IO_H_ */
