/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>


//#define STDOUT_FILENO 1
//#define STDERR_FILENO 2
//#define STDIN_FILENO 0
#define TX_BUF_SIZE 128
//static uint8_t tx_buf[TX_BUF_SIZE];
//volatile uint8_t uart_tx_busy = 0;  // 0: 空閒, 1: 發送中
//uint8_t rx_byte;  // 用於接收單個字節

/* USER CODE END 0 */

UART_HandleTypeDef huart2;

/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/*
// 重定向_write()到USART2中斷發送
int _write(int file, char *data, int len) {
    if (file == STDOUT_FILENO || file == STDERR_FILENO) {
        // 等待直到前一次發送完成（可選阻塞或丟棄數據）
        while(uart_tx_busy);  // 阻塞等待（簡單但非即時）

        // 複製數據到緩衝區
        int send_len = (len < TX_BUF_SIZE) ? len : TX_BUF_SIZE;
        memcpy(tx_buf, data, send_len);

        // 啟動中斷發送
        uart_tx_busy = 1;
        HAL_UART_Transmit_IT(&huart2, tx_buf, send_len);
        return send_len;
    }
    return -1;
}

// USART發送完成中斷回調
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        uart_tx_busy = 0;  // 標記為空閒
    }
}


// --- 環形緩衝區定義 ---
#define RX_BUF_SIZE 128  // 緩衝區大小可自定義

typedef struct {
    uint8_t buffer[RX_BUF_SIZE];  // 數據存儲陣列
    volatile uint16_t head;       // 寫入位置索引（遞增）
    volatile uint16_t tail;       // 讀取位置索引（遞增）
} RingBuffer;

// 全局接收緩衝區實例化
volatile RingBuffer rx_buf = { .head = 0, .tail = 0 };

// --- 緩衝區操作函數 ---

// 檢查緩衝區是否為空
bool rx_buf_is_empty(void) {
    return (rx_buf.head == rx_buf.tail);
}

// 檢查緩衝區是否已滿
bool rx_buf_is_full(void) {
    return ((rx_buf.head + 1) % RX_BUF_SIZE == rx_buf.tail);
}

// 向緩衝區寫入一個字節
void rx_buf_put(uint8_t data) {
    if (!rx_buf_is_full()) {
        rx_buf.buffer[rx_buf.head] = data;
        rx_buf.head = (rx_buf.head + 1) % RX_BUF_SIZE;
    }
    // 可選：處理緩衝區滿的情況（如觸發錯誤標誌）
}

// 從緩衝區讀取一個字節
uint8_t rx_buf_get(void) {
    uint8_t data = 0;
    if (!rx_buf_is_empty()) {
        data = rx_buf.buffer[rx_buf.tail];
        rx_buf.tail = (rx_buf.tail + 1) % RX_BUF_SIZE;
    }
    return data;  // 若緩衝區為空，返回0
}

int _read(int file, char *ptr, int len) {
    if (file == STDIN_FILENO) {  // STDIN_FILENO=0
        int received = 0;
        while (received < len) {
            if (!rx_buf_is_empty()) {
                *ptr = rx_buf_get();  // 從緩衝區讀取字節
                ptr++;
                received++;

                // 若收到換行符（依協議自定義結束條件）
                if (*(ptr-1) == '\n' || *(ptr-1) == '\r') {
                    break;
                }
            }
        }
        return received;
    }
    return -1;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        rx_buf_put(rx_byte);               // 將接收到的字節存入緩衝區
        HAL_UART_Receive_IT(huart, (uint8_t*)&rx_byte, 1); // 重新啟用接收
    }
}

*/

/* USER CODE END 1 */
