/**
 * @file ring_buffer.c
 * @brief 環形緩衝區 ringbuff
 * @author Deepseek and Ethan 
 * @date Feb 16, 2025
 *
 * 數據緩衝<br/>
 *   用途：在數據生產者（如 UART 接收）和消費者（如數據處理邏輯）之間提供一個 臨時存儲區，避免數據因處理不及時而丟失。<br/> 
 *   場景：例如處理 UART 中斷接收的數據時，將數據暫存到緩衝區，主程序再從緩衝區讀取處理。<br/>
 *   <br/>   
 * 關鍵操作<br/>
 * rx_buf_put()：將數據寫入緩衝區頭部（head）。<br/>
 * rx_buf_get()：從緩衝區尾部（tail）讀取數據。<br/>
 * 循環機制：通過取模運算（% RX_BUF_SIZE）實現頭尾指針的循環移動。<br/>
 * 
 * (1) 串口數據接收<br/>
 * 中斷接收：在 UART 中斷中調用 rx_buf_put() 存儲接收到的字節。<br/>
 * 主程序處理：主循環中調用 rx_buf_get() 取出數據處理，避免阻塞中斷。<br/>
 * (2) 多任務數據傳遞<br/>
 * 在 RTOS 中，生產者任務寫入數據，消費者任務讀取數據，緩衝區作為共享資源（需添加互斥鎖保護）。<br/>
 
 * 優點與限制 <br/>
 * 優點：<br/>
 * 高效存取：O(1) 時間複雜度的讀寫操作。<br/>
 * 內存固定：靜態分配內存，避免動態分配的不確定性。<br/>
 * 線程安全：若僅在單一寫入者和單一讀取者場景下，無需加鎖（適用於中斷+主程序架構）。<br/>
 * 
 * 限制：<br/>
 * 固定大小：緩衝區大小 RX_BUF_SIZE 需預先定義，無法動態擴展。<br/>
 * 單一數據類型：僅支持 uint8_t 類型數據（可修改結構支持其他類型）。<br/>
 *
 *版權：
 *GNU GENERAL PUBLIC LICENSE
 * Version 3, 29 June 2007
 * Copyright (C) [2025] [Ethan]
 * 本程式是一個自由軟體：您可以依照 **GNU 通用公共授權條款（GPL）** 發佈和/或修改，
 * GPL 版本 3 或（依您選擇）任何更新版本。
 * 
 * 本程式的發佈目的是希望它對您有幫助，但 **不提供任何擔保**，甚至不包含適銷性或特定用途適用性的默示擔保。
 * 請參閱 **GNU 通用公共授權條款** 以獲取更多詳細資訊。
 * 您應當已經收到一份 **GNU 通用公共授權條款** 副本。
 * 如果沒有，請參閱 <https://www.gnu.org/licenses/gpl-3.0.html>。  
 */
#include "ring_buffer.h"
#include "stm32f4xx_hal.h"  // 确保包含 HAL 头文件



/**
 *  @brief 宣告緩衝區結構,並且將其結構的內容清空
 *  volatile 關鍵字：防止編譯器優化，確保緩衝區在 中斷服務程序（ISR） <br/>
 *  和主程序間的正確訪問（例如 UART 接收中斷即時寫入數據）。<br/>
 */


volatile RingBuffer rx_buf = {0};
volatile uint8_t rx_buf_error = 0;// 錯誤狀態標誌


/**
 *  @brief初始化函式
 *  初始化頭尾指針，表示緩衝區為空。
 */
void ring_buffer_init(void) {
    rx_buf.head = 0;
    rx_buf.tail = 0;
    rx_buf_error = 0; // 初始化錯誤標誌
}


 /**
 *  @brief判斷ring buff是否為空
 *  空條件：頭尾指針重合。
 */
bool rx_buf_is_empty(void) {
    return (rx_buf.head == rx_buf.tail);
}

/**
 *  @brief判斷ring buff是否為滿
 *  滿條件：頭指針的下一個位置是尾指針（犧牲一個存儲單元來區分空和滿狀態）。
 */

bool rx_buf_is_full(void) {
    return ((rx_buf.head + 1) % RX_BUF_SIZE == rx_buf.tail);
}

/**
 *  @brief 數據寫入
 *  僅在緩衝區未滿時寫入，避免數據覆蓋。
 */
//<V4.12>
//修改：添加臨界區保護與溢位標記
void rx_buf_put(uint8_t data) {
    uint32_t primask = __get_PRIMASK(); // 保存當前中斷狀態
    __disable_irq();                    // 禁用中斷（進入臨界區）

    if (!rx_buf_is_full()) {
        rx_buf.buffer[rx_buf.head] = data;
        rx_buf.head = (rx_buf.head + 1) % RX_BUF_SIZE;
    } else {
        rx_buf_error |= RINGBUF_ERROR_OVERFLOW; // 標記溢位錯誤
    }

    __set_PRIMASK(primask); // 恢復中斷狀態（退出臨界區）
}


/**
 *  @brief 數據讀取
 *  僅在緩衝區非空時讀取，避免讀取無效數據。
 */
// 修改：添加臨界區保護
uint8_t rx_buf_get(void) {
    uint8_t data = 0;
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    if (!rx_buf_is_empty()) {
        data = rx_buf.buffer[rx_buf.tail];
        rx_buf.tail = (rx_buf.tail + 1) % RX_BUF_SIZE;
    }

    __set_PRIMASK(primask);
    return data;
}

//</V4.12>

#if oldcode
void rx_buf_put(uint8_t data) {
/**
 * @todo
 * 1. 擴展數據類型：<br/>
 * 修改 buffer 數組類型以支持更大數據（如 uint32_t）。<br/>
 * 2. 動態大小調整：<br/>
 * 改用鏈表或動態數組實現靈活的緩衝區大小。<br/>
 * 2. 添加互斥鎖：<br/>
 * @code 
 * void rx_buf_put(uint8_t data) {
 * 		disable_interrupts(); // 進入臨界區
 * 		if (!rx_buf_is_full()) {
 *  		rx_buf.buffer[rx_buf.head] = data;
 *  		rx_buf.head = (rx_buf.head + 1) % RX_BUF_SIZE;
 *  		}
 *  	enable_interrupts();  // 離開臨界區
 * }
 * @endcode

 */
    if (!rx_buf_is_full()) {
        rx_buf.buffer[rx_buf.head] = data;
        rx_buf.head = (rx_buf.head + 1) % RX_BUF_SIZE;
    }
}


/**
 *  @brief 數據讀取
 *  僅在緩衝區非空時讀取，避免讀取無效數據。
 */
uint8_t rx_buf_get(void) {
    uint8_t data = 0;
    if (!rx_buf_is_empty()) {
        data = rx_buf.buffer[rx_buf.tail];
        rx_buf.tail = (rx_buf.tail + 1) % RX_BUF_SIZE;
    }
    return data;
}
#endif
