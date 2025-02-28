/*
 * uart_io.c
 *
 *  Created on: Feb 16, 2025
 *      Author: user
 */

#include "uart_io.h"
#include "ring_buffer.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>

// 接收中斷用臨時變數
static volatile uint8_t rx_byte;

// 發送用狀態標誌
static volatile bool uart_tx_busy = false;

//--------------------------------------------------
// 初始化UART中斷接收
//--------------------------------------------------
void uart_io_init(void) {
    ring_buffer_init();
    HAL_UART_Receive_IT(&huart2, (uint8_t*)&rx_byte, 1); // 啟動接收中斷
}

//--------------------------------------------------
// 檢查是否有數據可讀
//--------------------------------------------------
bool uart_data_available(void) {
    return !rx_buf_is_empty();
}

//---------TX start---------------------------------
//--------------------------------------------------
// 重定向printf的底層_write()
//--------------------------------------------------
int _write(int file, char *data, int len) {
    if (file == STDOUT_FILENO || file == STDERR_FILENO) {
        while (uart_tx_busy); // 簡單阻塞等待
        uart_tx_busy = true;
        HAL_UART_Transmit_IT(&huart2, (uint8_t*)data, len);
        return len;
    }
    return -1;
}


//--------------------------------------------------
// 中斷回調函數
//--------------------------------------------------
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        uart_tx_busy = false; // 標記發送完成
    }
}

//---------TX end-----------------------------------


//--------------------------------------------------
// 重定向scanf的底層_read()
//--------------------------------------------------
int _read(int file, char *ptr, int len) {
    if (file == STDIN_FILENO) {
        int received = 0;
        clock_t start = clock();
        while (received < len) {
            if (!rx_buf_is_empty()) {
                *ptr = rx_buf_get();
                ptr++;
                received++;
                // 檢測到換行符或回車符則結束
                if (*(ptr-1) == '\n' || *(ptr-1) == '\r') break;
            }
            // 超時處理（5秒）
            if ((clock() - start) >= 5000) break;
        }
        return received;
    }
    return -1;
}

//--------------------------------------------------
// 中斷回調函數（修正版）
//--------------------------------------------------
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        rx_buf_put(rx_byte); // 存儲到緩衝區
        HAL_UART_Receive_IT(huart, (uint8_t*)&rx_byte, 1); // 重新啟用接收
    }
}



/*
//--------------------------------------------------
// 重定向scanf的底層_read()
//--------------------------------------------------
int _read(int file, char *ptr, int len) {
    if (file == STDIN_FILENO) {
        int received = 0;
        while (received < len && !rx_buf_is_empty()) {
            *ptr = rx_buf_get();
            ptr++;
            received++;
            if (*(ptr-1) == '\n') break; // 以換行符結束讀取
        }
        return received;
    }
    return -1;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        rx_buf_put(rx_byte); // 存儲數據到緩衝區
        HAL_UART_Receive_IT(huart, (uint8_t*)&rx_byte, 1); // 重新啟動接收
    }
}*/



//--------------------------------------------------
// 範例：回顯接收到的數據
//--------------------------------------------------
void echo_received_data(void) {
    if (uart_data_available()) {
        char c = rx_buf_get();
        printf("%c", c); // 回顯字符
    }
}
