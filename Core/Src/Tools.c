/**
  ******************************************************************************
  * @file    delay_us.c
  * @author  MCD Application Team
  * @brief   Delay function 
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) GUN
  *
  ******************************************************************************
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
**/
#include "Tools.h"
#include "uart_io.h"
#include <string.h>
#include <stdio.h>
#include <main.h>
#include "I2C_Peripherals.h"
extern volatile uint8_t RxBuffer[BUFFER_SIZE];

DS1307_Time RTC_DateTime;
DateAndTime BufferDateTime;



/* main.c 或 console.c */

/**
 * @brief 任務1: 行組裝器 (在 while(1) 中被不斷調用)
 *        從環形緩衝區讀取字節，組裝成一行後，舉起 g_command_line_ready 旗標。
 */
// 1. 命令緩衝區: 用來存放正在被組裝的命令字串
char g_command_line_buffer[80];
// 2. 索引: 記錄當前組裝到緩衝區的哪個位置了
uint16_t g_command_line_index = 0;
// 3. 旗標: 通知 main 迴圈「一行命令已經準備好了！」
volatile bool g_command_line_ready = false;



//確認輸入的時間參數正確與否


bool CheckTimeStruct (const DS1307_Time* time_to_check ){

	if(time_to_check->Command_ID != 1)
	{
		printf("is not time commend \n\n");
	    return false;
	}

	  // 使用 && (AND) 將所有條件串聯起來。
	    // 只有當 (條件1) AND (條件2) AND (條件3) ... 全部成立時，
	    // 整個表達式的結果才會是 true。
	    // 只要其中任何一個條件不成立，整個表達式立刻變為 false。

	    bool is_ok = (time_to_check->month >= 1 && time_to_check->month <= 12) &&
	            (time_to_check->date  >= 1 && time_to_check->date  <= 31) &&
	            (time_to_check->hours <= 23) && // 小時範圍 0-23
	            (time_to_check->minutes <= 59) && // 分鐘範圍 0-59
	            (time_to_check->seconds <= 59);   // 秒鐘範圍 0-59

	    if (is_ok) {
	           printf("Validation OK: All fields are valid.\n");
	       } else {
	           printf("Validation FAILED: At least one field is out of range.\n");
	       }

	       return is_ok;
}


/**
 * @brief  處理來自 UART 的異步命令。
 *
 * @note   此函數是事件驅動命令處理架構的核心。它應在主迴圈 (while(1)) 中
 *         被持續、快速地調用。
 *
 *         它的工作流程遵循一個清晰的「檢查-執行-重置」模型：
 *
 *         1. **檢查 (Check):**
 *            函數首先檢查全局旗標 `g_command_line_ready`。這個旗標由
 *            `ProcessUartRingBuffer()` 函數在成功組裝一行完整的 UART 命令後
 *            設定為 `true`。如果旗標為 `false`，表示沒有新命令，函數會立刻返回，
 *            幾乎不佔用任何 CPU 時間。
 *
 *         2. **執行 (Execute):**
 *            如果旗標為 `true`，說明 `g_command_line_buffer` 中有一條待處理的命令。
 *            函數會：
 *            a. 呼叫 `ParseCommand()` 將命令字串分解為 `Command_ID` 和 `Payload`。
 *            b. 使用 `switch` 語句，根據 `Command_ID` 將任務分派給對應的處理邏輯
 *               (case 1: 設定時間, case 2: 讀取時間等)。
 *            c. 在各自的 `case` 中，呼叫更底層的函數 (如 `Parse_TimeString`,
 *               `DS1307_SetTime`) 來完成具體的任務。
 *
 *         3. **重置 (Reset):**
 *            在命令被處理完畢後 (無論成功、失敗或未知命令)，函數會負責
 *            「清理現場」，為下一次命令的接收做準備。它會：
 *            a. 重置行緩衝區的索引 `g_command_line_index`。
 *            b. 清空行緩衝區 `g_command_line_buffer`。
 *            c. 最重要的是，將旗標 `g_command_line_ready` 重新設為 `false`，
 *               允許 `ProcessUartRingBuffer()` 開始組裝下一條新命令。
 *
 *         這種設計將「命令的接收/組裝」(由 ProcessUartRingBuffer 負責) 和
 *         「命令的解析/執行」(由本函數負責) 這兩個關注點完全分離，使得
 *         主迴圈的邏輯非常清晰，且整個系統是非阻塞的。
 */
void HandleUartCommands(void) {

    // 步驟 1: 檢查是否有一個完整的命令已經被 ProcessUartRingBuffer() 組裝好了
	// 預設 ProcessUartRingBuffer();都是正確的
	//但是這邊看不出來,

    if (g_command_line_ready) {

        // --- 命令處理流程開始 ---

        // 為了安全，打印收到的原始命令

        printf("CMD RX: %s\n", g_command_line_buffer);

        // 步驟 2: 【已修正】直接將完整的命令字串傳遞給 Parse_TimeString
        //          讓它一次性解析所有欄位，包括 Command_ID
        //DS130T7_Time
        DS1307_Time parsed_data= Parse_TimeString(g_command_line_buffer);
        //<< 出現incompatible types when initializing type 'int' using type 'DS1307_Time'的錯誤



            // 步驟 3: 使用 switch 結構，根據命令 ID 分派任務
            switch (parsed_data.Command_ID) {
                case 1: // 命令 1: 設定時間
                	{
                        if (CheckTimeStruct(&parsed_data)) {
                        	if (DS1307_SetTime(&parsed_data) == HAL_OK) {
                                printf("OK: Time set successfully.\n");
                            } else {
                                printf("ERR: Failed to set time via command.\n");
                            }
                        }
                    break;

                case 2: // 命令 2: 獲取時間
                    {
                    	// 【确认】重用 parsed_data 来接收 GetTime 的结果。
                    	// 这是可行的，因为在这个 case 中，它不再需要被用于其他目的。
                        if (DS1307_GetTime(&parsed_data) == HAL_OK) {
                            printf("OK: Current time is 20%02u-%02u-%02u %02u:%02u:%02u\n",
                            		parsed_data.year, parsed_data.month, parsed_data.date,
									parsed_data.hours, parsed_data.minutes, parsed_data.seconds);
                        } else {
                            printf("ERR: Failed to get time via command.\n");
                        }
                    }
                    break;

                    // 在這裡可以輕鬆添加 case 3, 4, ...

                    default: // 未知的命令 ID
                    	printf("ERR: Unknown Command ID %d received.\n", parsed_data.Command_ID);
                    break;
                    }
            }
    }

        // 步驟 4: 【非常重要】處理完畢，必須放下旗子，重置緩衝區，準備接收下一條命令
        g_command_line_index = 0;
        memset(g_command_line_buffer, 0, sizeof(g_command_line_buffer));
        g_command_line_ready = false;

        // --- 命令處理流程結束 ---

}



/**
 * @brief 從 UART 環形緩衝區處理字節流，嘗試組裝成一行完整的命令。
 *        此函數應在主迴圈中被反覆、快速地調用。
 */
void ProcessUartRingBuffer(void) {

    // --- 防禦性檢查 ---
    // 如果上一條命令已經準備好，但主迴圈還沒處理 (旗標還是 true)，
    // 那我們就先不組裝新的命令，避免覆蓋掉未處理的數據。
    if (g_command_line_ready) {
        return; // 立刻退出，等待主迴圈處理
    }

    // --- 核心邏輯：從環形緩衝區取數據 ---
    // 只要環形緩衝區裡還有字節，我們就一直處理它。
    while (!rx_buf_is_empty()) {

        // 從環形緩衝區安全地取出一個字節。
        // (注意：你的 ring_buffer 實現可能需要關中斷來保證線程安全)
        uint8_t byte = rx_buf_get();

        // --- 判斷讀取到的字節是什麼 ---

        // 情況 A：讀到的是行結束符（回車或換行）
        if (byte == '\r' || byte == '\n') {

            // 檢查之前是否已經有內容（避免處理一個空的行）
            if (g_command_line_index > 0) {

                // 這是一行命令的結束！
                // 1. 在緩衝區的當前位置加上字串結束符 '\0'
                g_command_line_buffer[g_command_line_index] = '\0';

                // 2. 【核心】舉起旗子，通知主迴圈！
                g_command_line_ready = true;

                // 3. 工作完成，立刻退出函數。
                //    索引 g_command_line_index 會在主迴圈處理完命令後被重置。
                return;
            }
            // 如果是空行 (index 為 0)，我們就忽略它，繼續下一個循環。

        }
        // 情況 B：讀到的是一個可打印的普通字符
        else if (isprint(byte)) {

            // 檢查我們的行緩衝區是否還有空間
            if (g_command_line_index < (sizeof(g_command_line_buffer) - 1)) {
                // 將這個字符存入緩衝區，並將索引後移一位
                g_command_line_buffer[g_command_line_index++] = byte;
            }
            // 如果緩衝區滿了，這裡會忽略掉後續的字符，防止溢出。

        }
        // 情況 C：其他字符（比如控制字符），我們暫時忽略。
    }

    // 如果程式碼執行到這裡，說明環形緩衝區已經被處理完了，
    // 但是還沒有遇到一個完整的行結束符。
    // 沒關係，函數靜靜地結束，等待下一次被主迴圈調用。
}




/**
 * @brief 計算給定字串的簡單異或校驗和。
 */
uint8_t calculate_checksum(const char *data_str, size_t len) {
    uint8_t checksum = 0;
    for (size_t i = 0; i < len; i++) {
        checksum ^= data_str[i];
    }
    return checksum;
}


#include <stdio.h>

// 假設 hi2c1 是你的 I2C Handle
// extern I2C_HandleTypeDef hi2c1;

void Scan_I2C_Address(void) {
    printf("--- Starting I2C Bus Scan ---\n");
    HAL_StatusTypeDef status;
    uint8_t found_count = 0;

    // I2C 的 7 位地址範圍是 1 到 127
    for (uint8_t i = 1; i < 128; i++) {
        // HAL 庫的函數需要一個左移一位的 8 位地址
        uint16_t device_address = (uint16_t)(i << 1);

        // 嘗試與該地址進行一次簡短的通訊，超時設定得很短
        // `Trials` 設為 1，`Timeout` 設為 10ms
        status = HAL_I2C_IsDeviceReady(&hi2c1, device_address, 1, 10);

        // 如果返回 HAL_OK，說明這個地址上有設備在回應
        if (status == HAL_OK) {
            printf("SUCCESS: Found device at I2C address 0x%02X\n", i);
            found_count++;
        }
    }

    if (found_count == 0) {
        printf("RESULT: No I2C devices found on the bus.\n");
    } else {
        printf("--- Scan Finished. Found %u device(s). ---\n", found_count);
    }
}





#ifdef on_use

/**
 * @brief 任務2: 命令解析器 (在 switch 前被調用)
 *        從 "ID,Payload" 格式中，分離出 ID 和 Payload。
 */
char* ParseCommand(const char* command_line, int* command_id_ptr) {
    const char* comma_pos = strchr(command_line, ',');
    if (comma_pos == NULL) { // 格式不對
        *command_id_ptr = 0; // 0 代表未知命令
        return NULL;
    }
    *command_id_ptr = atoi(command_line);
    return comma_pos + 1; // 返回指向 Payload 的指針
}




/**
 * 拆分年份
 * YearComponents y = split_year(2025); // 拆分為 {20, 25}
 * */
YearComponents split_year(uint16_t full_year) {
    YearComponents y;
    y.century = (uint8_t)(full_year / 100);
    y.year = (uint8_t)(full_year % 100);
    return y;
}

/**
 * 組合年份
 * //uint16_t reconstructed = combine_year(y); // 組合為 2025
 *
 * */
uint16_t combine_year(YearComponents y) {
    return (y.century * 100) + y.year;
}


/**
 * @brief 處理接收到的時間字串，解析、驗證並使用 DS1307_Time 結構體設置 DS1307。
 */
void Process_Time_String(const char *time_str)
{
    uint16_t p_year; // 解析出的年份 (完整)
    uint8_t p_month, p_day, p_hour, p_minute, p_second, p_weekday;
    uint16_t p_received_checksum_val; // 用於 sscanf

    // 使用本地緩衝區進行字串操作
    char local_buffer[BUFFER_SIZE];
    strncpy(local_buffer, time_str, BUFFER_SIZE - 1);
    local_buffer[BUFFER_SIZE - 1] = '\0'; // 確保以 null 結尾

    printf("處理命令: %s\r\n", local_buffer);

    // 查找最後一個逗號
    char *last_comma = strrchr(local_buffer, ',');
    if (last_comma == NULL) {
        printf("錯誤: 格式無效 (找不到校驗和逗號).\r\n");
        return;
    }

    // 計算數據部分的校驗和
    size_t data_len = last_comma - local_buffer;
    uint8_t calculated_checksum = calculate_checksum(local_buffer, data_len);

    // 解析包含校驗和的完整字串
    int parse_count = sscanf(local_buffer, "%hu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hu",
                                &p_year, &p_month, &p_day, &p_hour, &p_minute, &p_second, &p_weekday, &p_received_checksum_val);

    if (parse_count != 8)
    {
        printf("錯誤: 格式無效 (解析到 %d/8 項).\r\n", parse_count);
        return;
    }

    uint8_t received_checksum = (uint8_t)p_received_checksum_val; // 轉換解析出的校驗和

    printf("解析結果: 年=%u, 月=%u, 日=%u, 時=%u, 分=%u, 秒=%u, 周=%u, 接收校驗和=%u (計算值:%u)\r\n",
           p_year, p_month, p_day, p_hour, p_minute, p_second, p_weekday, received_checksum, calculated_checksum);

    // 1. 驗證校驗和
    if (calculated_checksum != received_checksum)
    {
        printf("錯誤: 校驗和不匹配!\r\n");
        return;
    }

    // 2. 驗證日期/時間範圍
    if (p_year < 2000 || p_year > 2099 ||
        p_month < 1 || p_month > 12 ||
        p_day < 1 || p_day > 31 ||
        p_hour > 23 || p_minute > 59 || p_second > 59 ||
        p_weekday < 1 || p_weekday > 7) // 假設星期為 1-7
    {
        printf("錯誤: 日期/時間值超出範圍.\r\n");
        return;
    }

    // 3. 驗證通過，準備 DS1307_Time 結構體並設置 DS1307 時間
    DS1307_Time timeToSet; // <-- 使用 DS1307_Time 結構體
    HAL_StatusTypeDef status;

    // 填充 timeToSet 結構體 (假設 DS1307_SetTime 內部處理 dec to bcd)
    timeToSet.seconds = p_second;
    timeToSet.minutes = p_minute;
    timeToSet.hours   = p_hour;
    timeToSet.day     = p_weekday; // 星期 (day of week)
    timeToSet.date    = p_day;     // 日期 (day of month)
    timeToSet.month   = p_month;
    timeToSet.year    = (uint8_t)(p_year % 100); // 年份 (後兩位)

    printf("校驗和正確. 正在設置 RTC 時間...\r\n");

    // 調用您 I2C_Peripherals.c 中的 DS1307_SetTime 函數
    status = DS1307_SetTime(&timeToSet); // <-- 傳遞結構體指針

    // 檢查寫入操作是否成功
    if (status == HAL_OK)
    {
        // 從設置的結構體中讀取值來打印確認信息
        printf("成功: DS1307 時間已更新為 20%02u-%02u-%02u (周:%u) %02u:%02u:%02u\r\n",
                timeToSet.year, timeToSet.month, timeToSet.date, timeToSet.day,
                timeToSet.hours, timeToSet.minutes, timeToSet.seconds);
    }
    else
    {
        printf("錯誤: 設置 DS1307 失敗. 狀態碼: %d. 請檢查 I2C 連接/地址.\r\n", status);
    }
}
#endif


//確認輸入的時間參數正確與否

 // --- 輔助函數的 #ifdef RTC_Write 結束 ---

void Delay_us(uint32_t us){
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload = SysTick -> LOAD +1;
	ticks = us*(SystemCoreClock/1000000);
	told= SysTick-> VAL;
	while(1){
		tnow = SysTick-> VAL;
		if(tnow !=told)
		{
			if(tnow<told)
			{
				tcnt+= told-tnow;
			}else{
				tcnt+= reload-tnow+told;
			}
			told=tnow;
			if(tcnt>=ticks)break;
		}
	}
}
