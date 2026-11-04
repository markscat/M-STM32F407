
#ifndef ADS1115_H
#define ADS1115_H

/*
 * Library Name: 	ADS1115 STM32 Single-Ended, Single-Shot, PGA & Data Rate Enabled HAL Library
 * Written By:		Ahmet Batuhan Günaltay
 * Date Written:	02/04/2021 (DD/MM/YYYY)
 * Last Modified:	03/04/2021 (DD/MM/YYYY)
 * Description:		STM32F4 HAL-Based ADS1115 Library
 * References:
 * 	- https://www.ti.com/lit/gpn/ADS1113 [Datasheet]
 *
 * Copyright (C) 2021 - Ahmet Batuhan Günaltay
 *
	This software library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This software library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *  */


/*
 * Design by Ethan
 * 為什麼加入這個?
 * 理由很簡單,我覺得HAL_StatusTypeDef的錯誤碼太少了
 * 出了什麼事情根本不知道
 *
*/



#include "stm32f4xx_hal.h" // Could be changed for a specific processor.
#include "string.h"
#include "stdio.h"


/* Definitions */
#define ADS1115_OS (0b1 << 7) // Default



#define ADS1115_MUX_AIN0 (0b100 << 4)		// Analog input 1 0x40
#define ADS1115_MUX_AIN1 (0b101 << 4)		// Analog input 2 0x50
#define ADS1115_MUX_AIN2 (0b110 << 4)		// Analog input 3 0x60
#define ADS1115_MUX_AIN3 (0b111 << 4)		// Analog input 4 0x70

/*Differential Multiplexer Register */
#define ADS1115_MUX_DIFF_0_1   0x0000  // AIN0 - AIN1
#define ADS1115_MUX_DIFF_0_3   0x1000  // AIN0 - AIN3
#define ADS1115_MUX_DIFF_1_3   0x2000  // AIN1 - AIN3
#define ADS1115_MUX_DIFF_2_3   0x3000  // AIN2 - AIN3

#define ADS1115_PGA_TWOTHIRDS 	(0b000 << 1) 		// 2/3x Gain	-- 0.1875 mV by one bit		MAX: +- VDD + 0.3V  0x00
#define ADS1115_PGA_ONE			(0b001 << 1) 		// 1x Gain		-- 0.125 mV by one bit		MAX: +- VDD + 0.3V  0x02
#define ADS1115_PGA_TWO			(0b010 << 1) 		// 2x Gain		-- 0.0625 mV by one bit		MAX: +- 2.048 V		0x03
#define ADS1115_PGA_FOUR		(0b011 << 1) 		// 4x Gain		-- 0.03125 mV by one bit	MAX: +- 1.024 V		0x04
#define ADS1115_PGA_EIGHT		(0b100 << 1) 		// 8x Gain		-- 0.015625 mV by one bit	MAX: +- 0.512 V     0x05
#define ADS1115_PGA_SIXTEEN		(0b111 << 1) 		// 16x Gain		-- 0.0078125 mV by one bit	MAX: +- 0.256 V     0x06

#define ADS1115_MODE (0b1) // Default
/**/
#define ADS1115_MODE_CONTINUOUS 0x0  // 連續模式

#define ADS1115_DATA_RATE_8		(0b000 << 5)			// 8SPS
#define ADS1115_DATA_RATE_16	(0b001 << 5)			// 16SPS
#define ADS1115_DATA_RATE_32	(0b010 << 5)			// 32SPS
#define ADS1115_DATA_RATE_64	(0b011 << 5)			// 64SPS
#define ADS1115_DATA_RATE_128	(0b100 << 5)			// 128SPS
#define ADS1115_DATA_RATE_250	(0b101 << 5)			// 250SPS
#define ADS1115_DATA_RATE_475	(0b110 << 5)			// 475SPS
#define ADS1115_DATA_RATE_860	(0b111 << 5)			// 860SPS

#define ADS1115_COMP_MODE 	(0b0 << 4) // Default
#define ADS1115_COMP_POL 	(0b0 << 3) // Default
#define ADS1115_COMP_LAT 	(0b0 << 2) // Default
#define ADS1115_COMP_QUE 	(0b11)	   // Default

/* ADS1115 register configurations */
#define ADS1115_CONVER_REG 0x0
#define ADS1115_CONFIG_REG 0x1

/* TIMEOUT */
#define ADS1115_TIMEOUT 1 // Timeout for HAL I2C functions.



typedef enum {
    /* ==========================================
     * 成功
     * ========================================== */
	I2C_ADS1115_OK = 0,//redeclaration of enumerator 'ADS1115_OK'

    /* ==========================================
     * 軟體狀態與參數錯誤（還沒碰硬體就錯的）
     * ========================================== */
	I2C_ADS1115_ERR_NOT_INIT,           /* 沒呼叫 Init 就直接呼叫 Read */
	I2C_ADS1115_ERR_INVALID_PGA,        /* 傳入的 PGA 參數亂填 (超出 0~5) */
	I2C_ADS1115_ERR_INVALID_MUX,        /* 傳入的 MUX 通道亂填 (例如選了 AIN8) */
	I2C_ADS1115_ERR_INVALID_RATE,       /* 傳入的 Data Rate 亂填 */
	I2C_ADS1115_ERR_INVALID_HANDLER,    /* 傳入的 I2C_HandleTypeDef 是 NULL */

    /* ==========================================
     * I2C 匯流排層錯誤（HAL 回報的）
     * ========================================== */
	I2C_ADS1115_ERR_I2C_BUSY,           /* 匯流排被佔用 (HAL_BUSY) */
	I2C_ADS1115_ERR_I2C_TIMEOUT,        /* 匯流排超時 (HAL_TIMEOUT) */
	I2C_ADS1115_ERR_I2C_NACK,           /* 裝置沒回 ACK (通常是沒接好或位址錯，HAL_ERROR 且伴隨 AF 旗標) */
	I2C_ADS1115_ERR_I2C_ARBITRATION,    /* 多主機仲裁失敗 (HAL_ERROR 且伴隨 ARLO 旗標) */
	I2C_ADS1115_ERR_I2C_BERR,           /* 匯流排錯誤 (Bus Error, 通常是雜訊干擾) */
	I2C_ADS1115_ERR_I2C_OVR,            /* 溢位錯誤 (Overrun/Underrun) */
	I2C_ADS1115_ERR_I2C_UNKNOWN,        /* 其他未知的 HAL_ERROR */

    /* ==========================================
     * ADS1115 暫存器邏輯錯誤
     * ========================================== */
	I2C_ADS1115_ERR_CFG_READ_FAIL,      /* 寫入 Config 成功，但讀回 Config 失敗 */
	I2C_ADS1115_ERR_CFG_VERIFY_FAIL,    /* 寫入 Config 後，讀回來發現內容跟寫進去的不一樣 */
	I2C_ADS1115_ERR_CONV_READ_FAIL,     /* 轉換完成，但讀取 Conversion 暫存器失敗 */

    /* ==========================================
     * ADS1115 硬體行為錯誤（通訊 OK，但晶片不理你）
     * ========================================== */
	I2C_ADS1115_ERR_CONV_TIMEOUT,       /* 啟動轉換後，等不到 OS (Operational Status) bit 變為 1 */
	I2C_ADS1115_ERR_CONV_NOT_READY,     /* 嘗試讀取時，晶片回報資料還沒準備好 */

    /* ==========================================
//     * 物理量測警告（非通訊錯誤，但數值不可信）
     * ========================================== */
	I2C_ADS1115_WARN_PGA_SATURATED,     /* 量測值頂到 FSR 滿量程 (32767 或 -32768)，可能超壓 */
	I2C_ADS1115_WARN_NEAR_RAIL,         /* 量測值非常接近滿量程 (例如 > 95%)，建議換檔 */
	I2C_ADS1115_WARN_NOISE_HIGH          /* (選用) 連續讀取跳動過大，懷疑訊號有雜訊 */

} I2C_ADS1115_Status;



/* Function prototypes. */
HAL_StatusTypeDef ADS1115_Init(I2C_HandleTypeDef *handler, uint16_t setDataRate, uint16_t setPGA);
HAL_StatusTypeDef ADS1115_readOneShot(uint16_t muxPort, float *voltage);
HAL_StatusTypeDef ADS1115_StartContinuous(uint16_t muxPort, uint16_t pga, uint16_t dataRate);
HAL_StatusTypeDef ADS1115_ReadContinuous(float *voltage);

#endif
