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

typedef struct {
    uint8_t buffer[RX_BUF_SIZE];
    volatile uint16_t head;  // 寫入位置
    volatile uint16_t tail;  // 讀取位置
} RingBuffer;

// 初始化緩衝區
void ring_buffer_init(void);

// 檢查是否為空
bool rx_buf_is_empty(void);

// 檢查是否已滿
bool rx_buf_is_full(void);

// 寫入一個字節
void rx_buf_put(uint8_t data);

// 讀取一個字節
uint8_t rx_buf_get(void);


#endif /* INC_RING_BUFFER_H_ */
