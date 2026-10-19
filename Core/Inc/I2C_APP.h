/*
 * APP_I2C_init.h
 *
 *  Created on: 2026年10月19日
 *      Author: user
 *
 *******************************************************************************
 * @file    APP_I2C_init.h
 * @brief   I2C 應用層初始化封裝
  ******************************************************************************
 */

#ifndef INC_I2C_APP_H_
#define INC_I2C_APP_H_


#include "I2C_parameter.h"   // 拿 I2C_Status

#define MAX_INPUT_LEN 32 // 输入字符串最大长度
#define MAX_INPUT_LEN_MINIMAL 32
#define MAX_COMMAND_LEN_MAIN 32                         // UART 命令行的最大允許字符數 (不包括 '\0')
#define LINE_BUFFER_SIZE_MAIN (MAX_COMMAND_LEN_MAIN + 1) // <<<< 在此處定義！ 行緩衝區大小（+1 用於 '\0'）
/**
  * @brief  掃描 I2C 匯流排 + 初始化所有已知 I2C 設備
  * @retval I2C_OK        成功
  * @retval 其他          失敗（透傳 I2C_Init_Devices 的錯誤碼）
  */
I2C_Status APP_I2C_Init(void);


/* ---- RTC 互動式設定（會阻塞等待 UART 輸入）---- */
HAL_StatusTypeDef APP_RTC_SetupInteractive(void);

/* ---- RTC 時間 + 溫度顯示（單次，不迴圈）---- */
void APP_RTC_ShowTimeAndTemp(void);

/* ---- UART ring buffer 輪詢（單次，不迴圈）---- */
void APP_UART_Poll(void);


#endif /* INC_I2C_APP_H_ */
