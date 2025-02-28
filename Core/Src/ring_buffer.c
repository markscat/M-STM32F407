/*
 * ring_buffer.c
 *
 *  Created on: Feb 16, 2025
 *      Author: user
 */
#include "ring_buffer.h"

volatile RingBuffer rx_buf = {0};

void ring_buffer_init(void) {
    rx_buf.head = 0;
    rx_buf.tail = 0;
}

bool rx_buf_is_empty(void) {
    return (rx_buf.head == rx_buf.tail);
}

bool rx_buf_is_full(void) {
    return ((rx_buf.head + 1) % RX_BUF_SIZE == rx_buf.tail);
}

void rx_buf_put(uint8_t data) {
    if (!rx_buf_is_full()) {
        rx_buf.buffer[rx_buf.head] = data;
        rx_buf.head = (rx_buf.head + 1) % RX_BUF_SIZE;
    }
}

uint8_t rx_buf_get(void) {
    uint8_t data = 0;
    if (!rx_buf_is_empty()) {
        data = rx_buf.buffer[rx_buf.tail];
        rx_buf.tail = (rx_buf.tail + 1) % RX_BUF_SIZE;
    }
    return data;
}

