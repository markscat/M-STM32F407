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
#include <I2C_Peripherals.h>
extern volatile uint8_t RxBuffer[BUFFER_SIZE];

DS1307_Time RTC_DateTime;
DateAndTime BufferDateTime;


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

/**
 * @brief 將年分為『世紀』和『年份』
 *  
 * 
 * */

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
