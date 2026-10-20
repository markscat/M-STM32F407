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


#define ADC_1115

#ifdef ADC_1115

#include "Ext_ADC\ads1115.h"

/* 設定 ADS1115：模式、通道、PGA、取樣率
 * 參數直接用 ads1115.h 的巨集：
 *   mux     : ADS1115_MUX_AIN0 / ADS1115_MUX_DIFF_0_1 ...
 *   pga     : ADS1115_PGA_ONE / ADS1115_PGA_TWO ...
 *   dataRate: ADS1115_DATA_RATE_128 ...
 *   continuous: 0 = 單次, 1 = 連續
 */

HAL_StatusTypeDef APP_ADS1115_Config(uint16_t mux, uint16_t pga,
                                     uint16_t dataRate, uint8_t continuous);

/* 讀取一次（依目前設定） */
HAL_StatusTypeDef APP_ADS1115_Read(float *voltage);

#endif
