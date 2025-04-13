/**
  ******************************************************************************
  * @file   uart_io.c
  * @brief   這個檔案主要是用來將__write()和__read()重新定向，讓Printf()和Scanf()可以往UART輸出
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  * 
  * 
  *     This program is free software: you can redistribute it and/or modify
  *     it under the terms of the GNU General Public License as published by
  *     the Free Software Foundation, either version 3 of the License, or
  *     any later version.
  *     
  *     This program is distributed in the hope that it will be useful,
  *     but WITHOUT ANY WARRANTY; without even the implied warranty of
  *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *     GNU General Public License for more details.
  *     
  *     You should have received a copy of the GNU General Public License
  *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
  *
  ******************************************************************************
  */



#include "uart_io.h"
#include "ring_buffer.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "gpio.h"


/**  @brief　接收中斷用臨時變數
 */
static volatile uint8_t rx_byte;
volatile uint8_t rxBuffer[1] = {0};  // 统一接收缓冲区

/**
 * @brief　TX_Buffer
 *
 */
#define TX_BUFFER_SIZE 128

/**
 * @brief　發送用狀態標誌
 *
 */
static volatile bool uart_tx_busy = false;     // 傳輸狀態標誌

/**
 * @brief　雙緩衝區
 *
 */
// 定義雙緩衝區
#define TX_BUFFER_SIZE 128
static uint8_t tx_buffer[2][TX_BUFFER_SIZE];  // 雙緩衝區
static volatile uint8_t active_buffer = 0;     // 當前使用的緩衝區索引

/**
 * *************************************************
 * @brief　初始化UART中斷接收
 * ************************************************
 *
 */
void uart_io_init(void) {
    ring_buffer_init();
    HAL_UART_Receive_IT(&huart2, (uint8_t*)rxBuffer, 1); // 显式类型转换
}

//--------------------------------------------------
// @brief　檢查是否有數據可讀
//--------------------------------------------------
bool uart_data_available(void) {
    return !rx_buf_is_empty();
}

/**
* *************************************************
 @brief 重定向printf的底層_write()
* *************************************************
*/

//#define FirstVerIsWork
//#define ThereVerInTest
//#define ThoreVerInTest
//#define signal_Buffer
#define Double_Buffer
//#define DMA_writer




#ifdef Double_Buffer
/**
 * Double_Buffer define 區塊
 * 利用非阻塞傳輸 + 雙緩衝區的方法,解決無法連續printf()的問題.<br/>
 * 前版必須使用setvbuf(stdout, NULL, _IONBF, 0);指令來取消 `stdout` 緩衝區，確保立即輸出<br/>
 * 但會出現以下的問題：<br/>
 * 1. 靜態緩衝區的競爭條件<br/>
 * 		即使禁用緩衝，若 _write() 在傳輸未完成時被再次調用（例如中斷或並行操作），tx_buffer 仍可能被覆蓋。解決方案：<br/>
 * 		-使用 雙緩衝區 或 佇列（Queue） 分離數據填充與傳輸過程。<br/>
 * 		-改用 DMA 傳輸 並搭配動態記憶體管理。<br/>
 * 2. 阻塞等待的效能問題<br/>
 * 		while(uart_tx_busy) 會阻塞主程式，可能影響即時性。改進方案：<br/>
 * 		-實現非阻塞式傳輸，將數據暫存至佇列，在 UART 閒置時逐步發送。<br/>
 * 		-使用中斷或 DMA 完成通知來驅動傳輸流程。<br/>
 * 3. 緩衝區溢出風險<br/>
 * 		tx_buffer 固定為 128 位元組，若單次 printf() 輸出過長，可能導致截斷。改進方案：
 * 		- 動態檢測數據長度，分次傳輸。
 * 		- 增大緩衝區並加入長度檢查。
 * */

//---------TX start-----------------------------------



void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        uart_tx_busy = false; // 標記發送完成
    }
}

// 改進的 _write 函式
int _write(int file, char *data, int len) {
    if (file != STDOUT_FILENO && file != STDERR_FILENO) {
        return -1;
    }
    // Step 1: 直接處理換行符，填充當前非活躍緩衝區
    uint8_t target_buffer = (active_buffer == 0) ? 1 : 0;  // 選擇非活躍緩衝區
    uint16_t processed_len = 0;

    for (int i = 0; i < len; i++) {
        if (data[i] == '\n') {
            if (processed_len + 2 > TX_BUFFER_SIZE) break;
            tx_buffer[target_buffer][processed_len++] = '\r';
            tx_buffer[target_buffer][processed_len++] = '\n';
        } else {
            if (processed_len + 1 > TX_BUFFER_SIZE) break;
            tx_buffer[target_buffer][processed_len++] = data[i];
        }
    }
    // Step 2: 等待前一次傳輸完成（非阻塞檢查，避免死等）
    uint32_t start = HAL_GetTick();
    while (uart_tx_busy) {
        if (HAL_GetTick() - start > 100) {
            return -1;  // 超時返回錯誤
        }
    }

    // Step 3: 啟動新傳輸並切換活躍緩衝區
    HAL_StatusTypeDef status = HAL_UART_Transmit_IT(&huart2, tx_buffer[target_buffer], processed_len);
    if (status == HAL_OK) {
        uart_tx_busy = true;
        active_buffer = target_buffer;  // 更新活躍緩衝區
        return len;  // 返回原始長度
    }
    return -1;
}
#endif


//---------TX end-----------------------------------


//---------RX start-----------------------------------
/**
//--------------------------------------------------
// 重定向scanf的底層_read()
//--------------------------------------------------
*/

//<0412 新增>

int _read(int file, char *ptr, int len) {
    HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin); // 调试用LED

    if (file == STDIN_FILENO) {
        int received = 0;
        uint32_t start = HAL_GetTick(); // 改用 HAL 的 tick 函数
        while (received < len) {
            if (!rx_buf_is_empty()) {
                *ptr = rx_buf_get();
                ptr++;
                received++;
                if (*(ptr-1) == '\n' || *(ptr-1) == '\r') break;
            }
            // 超时 5 秒（5000ms）
            if (HAL_GetTick() - start >= 5000) break;
        }
        return received;
    }
    return -1;
}



/**
//--------------------------------------------------
// 中斷回調函數（修正版）
//--------------------------------------------------
*/

#define BUFFER_SIZE 100
char inputBuffer[BUFFER_SIZE];
uint8_t RX_index = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        // 移除阻塞的 HAL_Delay，改用非阻塞操作

        char received_char = rxBuffer[0]; // 获取接收到的字符
        rx_buf_put(received_char); // 存入环形缓冲区


        // 边界检查，防止溢出
        if (RX_index < BUFFER_SIZE - 1) { // 保留一个位置给终止符
            if (received_char == '\r' || received_char == '\n') {
                inputBuffer[RX_index] = '\0'; // 终止字符串
                //printf("\r\nReceived: %s\r\n", inputBuffer); // 回显
                RX_index = 0; // 重置索引
            } else {
                inputBuffer[RX_index++] = received_char; // 存储字符
            }
        } else {
            // 缓冲区满，清空并提示错误
            RX_index = 0;
            //printf("\r\nError: Buffer overflow!\r\n");
        }

        // 重启接收中断
        HAL_UART_Receive_IT(&huart2, (uint8_t *)rxBuffer, 1);
    }
}
//</0412 新增>


/**
 *
 * @brief 清空緩衝區
 *
 * */
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}



#ifdef signal_Buffer
/**
 * signal_Buffer define 區塊
 * 基本上這段程式碼沒有太多問題;
 * BaudRate 越高,失誤的機率就更小
 *
 * 1. 波特率與傳輸時間的關係
 *	-低波特率（例如 9600）：每個字節傳輸時間較長（約 1.04ms/字節），容易導致 _write() 中的 while(uart_tx_busy) 等待時間過長，增加緩衝區被覆蓋的風險。
 *	- 高波特率（例如 115200）：傳輸速度快（約 0.087ms/字節），大幅減少等待時間，可能降低緩衝區衝突的機率。
 *
 * 2.程式碼的關鍵瓶頸
 * 	即使提高波特率，若以下問題未解決，仍可能出現相同問題：
 * 	 - 靜態緩衝區重用：若兩個 printf() 在 UART 傳輸完成前連續觸發 _write()，靜態 tx_buffer 會被覆蓋。
 * 	 - 阻塞等待的極限：while(uart_tx_busy) 的 100ms 超時在高負載場景下可能不足，導致數據丟失。
 *
 *
 * */


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        uart_tx_busy = false; // 標記發送完成
    }
}

int _write(int file, char *data, int len) {
    if (file == STDOUT_FILENO || file == STDERR_FILENO) {
    	  uint32_t start = HAL_GetTick();

        while (uart_tx_busy && (HAL_GetTick() - start < 100)); //  阻塞等待,最多等待 100ms

        static uint8_t tx_buffer[128]; // 靜態緩衝區避免失效
        uint16_t processed_len = 0;

        // 逐字符處理換行符
        for (int i = 0; i < len; i++) {
            if (data[i] == '\n') {
                if (processed_len + 2 > sizeof(tx_buffer)) break;
                tx_buffer[processed_len++] = '\r';
                tx_buffer[processed_len++] = '\n';
            } else {
                if (processed_len + 1 > sizeof(tx_buffer)) break;
                tx_buffer[processed_len++] = data[i];
            }
        }
        uart_tx_busy = true;
        HAL_UART_Transmit_IT(&huart2, tx_buffer, processed_len);
        return len; // 返回原始長度（符合標準庫預期）
    }
    return -1;
}
#endif


#ifdef ThoreVerInTest

int _write(int file, char *data, int len) {
    if (file == STDOUT_FILENO || file == STDERR_FILENO) {

        while (uart_tx_busy); // 阻塞等待

        static uint8_t tx_buffer[128]; // 靜態緩衝區避免失效
        uint16_t processed_len = 0;

        // 逐字符處理換行符
        for (int i = 0; i < len; i++) {
            if (data[i] == '\n') {
                if (processed_len + 2 > sizeof(tx_buffer)) break;
                tx_buffer[processed_len++] = '\r';
                tx_buffer[processed_len++] = '\n';
            } else {
                if (processed_len + 1 > sizeof(tx_buffer)) break;
                tx_buffer[processed_len++] = data[i];
            }
        }

        uart_tx_busy = true;
        HAL_UART_Transmit_IT(&huart2, tx_buffer, processed_len);
        return len; // 返回原始長度（符合標準庫預期）
    }
    return -1;
}
#endif

#ifdef ThereVerInTest

int _write(int file, char *data, int len) {
    if (file == STDOUT_FILENO || file == STDERR_FILENO) {
        while (uart_tx_busy); // 阻塞等待

        // 使用靜態緩衝區避免資料失效
        static char tx_buffer[128];
        int final_len = len;

        if (len > 0 && data[len-1] == '\n') {
            if (len + 1 > sizeof(tx_buffer)) {
                return -1; // 緩衝區不足
            }
            memcpy(tx_buffer, data, len);
            tx_buffer[len-1] = '\r';
            tx_buffer[len] = '\n';
            final_len = len + 1;
            data = tx_buffer;
        }

        uart_tx_busy = true;
        HAL_UART_Transmit_IT(&huart2, (uint8_t*)data, final_len);
        return len; // 返回原始長度（符合標準庫預期）
    }
    return -1;
}
#endif


#ifdef FirstVerIsWork

int _write(int file, char *data, int len) {
    if (file == STDOUT_FILENO || file == STDERR_FILENO) {
        while (uart_tx_busy); // 簡單阻塞等待
        uart_tx_busy = true;
        HAL_UART_Transmit_IT(&huart2, (uint8_t*)data, len);
        return len;
    }
    return -1;
}
#endif



#ifdef DMA_writer

// 定義 DMA 傳輸狀態標誌和緩衝區
static volatile bool uart_dma_busy = false;  // DMA 傳輸中標誌

// DMA 傳輸完成回調函式
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &huart2) {
        uart_dma_busy = false;  // 傳輸完成，釋放標誌
    }
}

// 改寫後的 _write 函式（僅修改傳輸部分）
int _write(int file, char *data, int len) {
    if (file == STDOUT_FILENO || file == STDERR_FILENO) {
        uint32_t start = HAL_GetTick();

        // 等待前一次 DMA 傳輸完成（最多等 100ms）
        while (uart_dma_busy && (HAL_GetTick() - start < 100)) {}

        // 處理換行符（與原程式碼相同）
        static uint8_t tx_processed[128];  // 改用靜態緩衝區避免 DMA 傳輸中數據被覆蓋
        uint16_t processed_len = 0;
        for (int i = 0; i < len; i++) {
            if (data[i] == '\n') {
                if (processed_len + 2 > sizeof(tx_processed)) break;
                tx_processed[processed_len++] = '\r';
                tx_processed[processed_len++] = '\n';
            } else {
                if (processed_len + 1 > sizeof(tx_processed)) break;
                tx_processed[processed_len++] = data[i];
            }
        }

        // 啟動 DMA 傳輸
        if (!uart_dma_busy) {
            memcpy(tx_buffer, tx_processed, processed_len);  // 複製到 DMA 緩衝區
            HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(&huart2, tx_buffer, processed_len);
            if (status == HAL_OK) {
                uart_dma_busy = true;
                return len;  // 返回原始長度
            }
        }
        return -1;  // 傳輸失敗或忙碌
    }
    return -1;
}
#endif





#ifdef V4_06
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

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
    	if()
       rx_buf_put(rx_byte); // 存儲到緩衝區
        printf("RX: %c\r\n",buff ); // Debug 印出接收資料
        HAL_UART_Receive_IT(huart, (uint8_t*)&rx_byte, 1); // 重新啟用接收
    }
}

#endif

