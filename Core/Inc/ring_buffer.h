/*
 * ring_buffer.h
 *
 *  Created on: Feb 16, 2025
 *      Author: user
 */
#pragma once

#ifndef INC_RING_BUFFER_H_
#define INC_RING_BUFFER_H_

#include <stdbool.h>
#include <stdint.h>

#define RX_BUF_SIZE 128  // 接收緩衝區大小
#define RINGBUF_ERROR_OVERFLOW (1 << 0) // 溢位錯誤標誌
//extern volatile uint8_t rx_buf_error;  // 錯誤狀態標誌


/**
 *  @brief 緩衝區結構
 *  @param	head 存放緩衝區的第一筆資料<br/>
 *  @param	tail 存放緩衝區的最後一筆資料<br/>
 *  @param　緩衝區整體的結構<br/>
 *  @param　RX_BUF_SIZE緩衝區的大小
 */
typedef struct {
    volatile uint16_t head;
    volatile uint16_t tail;
    uint8_t buffer[RX_BUF_SIZE];
} RingBuffer;



/**
 *  @brief 初始化緩衝區
 */

void ring_buffer_init(void);

/**
 *  @brief 檢查是否為空
 */
bool rx_buf_is_empty(void);



/**
 *  @brief 檢查是否已滿
 */
bool rx_buf_is_full(void);


/**
 *  @brief 寫入一個字節
 */

void rx_buf_put(uint8_t data);

/**
 *  @brief 讀取一個字節
 */
uint8_t rx_buf_get(void);


#endif /* INC_RING_BUFFER_H_ */
