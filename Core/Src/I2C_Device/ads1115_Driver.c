/*
 * 程式名	　			    :	ADS1115 相關驅動程式(ADS1115 Driver)
 * 更新人員(Modify by)	:	Ethan Yang
 * Date Modify			:	2025/10/03 (YYYY/MM/DD)
 * Last Modify			:
 *
 * Description:		STM32F4 HAL-Based ADS1115 Library
 * References:
 * 	- https://www.ti.com/lit/gpn/ADS1113 [Datasheet]
 * 	- https://github.com/Squieler/ADS1115[source Code]
 *
 * 本程式主要是個人興趣索然，想要試試看ADS1115，在GitHUB上找到這個程式,所以想要測試看看.
 * 同時看看自己是否可以做一些修改，或是增加其他的功能。
 * 從這個版本看來，最多只有初始化，
 *
 * PGA = Programmable Gain Amplifier（可程式增益放大器）。
 * 決定PGA的方法：
 *
 *Vdd = 3.3V
 *|  PGA 設定	| FSR ±V | 每 bit 解析度 (mV/bit) 	| 實際最大可量測電壓  | 註解            			|
 *| -------- 	| ------ | --------------------- 	| -----------------  | -------------------- |
 *|   2/3x   	| ±6.144 | 0.1875                	| 3.3                | 量程沒用到一半，解析度最粗	|
 *|   1x     	| ±4.096 | 0.125                 	| 3.3                | 解析度比 2/3x 高   		|
 *|   2x     	| ±2.048 | 0.0625                	| 3.3                | 適合低壓量測，解析度更好  	|
 *|   4x     	| ±1.024 | 0.03125               	| 1.024              | 適合 1V 以下訊號量測  	|
 *|   8x     	| ±0.512 | 0.015625              	| 0.512              | 適合小訊號精密量測     	|
 *|   16x    	| ±0.256 | 0.0078125				| 0.256              | 超精密量測小訊號      	|
 *
 *Vdd = 5V
 *|  PGA 設定	| FSR ±V | 每 bit 解析度 (mV/bit) 	| 實際最大可量測電壓  | 註解            		|
 *| -------- 	| ------ | --------------------- 	| -------------  | -------------------- |
 *|   2/3x   	| ±6.144 | 0.1875                	| 5              | 量程沒用到一半，解析度最粗	|
 *|   1x     	| ±4.096 | 0.125                 	| 4.96           | 最大量程安全可量測 4.096V	|
 *|   2x     	| ±2.048 | 0.0625                	| 2.048          | 適合 2V 以下訊號量測  	|
 *|   4x     	| ±1.024 | 0.03125               	| 1.024          | 精密量測 1V 以下訊號  	|
 *|   8x     	| ±0.512 | 0.015625              	| 0.512          | 適合小訊號精密量測     	|
 *|   16x    	| ±0.256 | 0.0078125				| 0.256          | 超精密量測小訊號     	|
 *
 *
 *　config暫存器
 *　+------+--------------+--------------+-------+-----------+---------+--------+---------+--------------------+
 *	|Bit15 |   Bit14-12   |    Bit11-9   | Bit8  |  Bit7-5   |  Bit4   |  Bit3  | Bit2    |      Bit1-0        |
 *	+------+----+----+----+----+----+----+-------+---+---+---+---------+--------+---------+---------+----------+
 *  |OS    |MUX2|MUX1|MUX0|PGA2|PGA1|PGA0| MODE  |DR2|DR1|DR0|COMP_MODE|COMP_POL|COMP_LAT |COMP_QUE1|COMP_QUE0 |
 *	+------+----+----+----+----+----+----+-------+---+---+---+---------+--------+---------+---------+----------+
 *
 * ##版權：<br/>
 * GNU GENERAL PUBLIC LICENSE<br/>
 * Version 3, 29 June 2007<br/>
 * Copyright (C) [2025] [Ethan]<br/>
 * 本程式是一個自由軟體：您可以依照 **GNU 通用公共授權條款（GPL）** 發佈和/或修改，<br/>
 * GPL 版本 3 或（依您選擇）任何更新版本。<br/>
 * 本程式的發佈目的是希望它對您有幫助，但 **不提供任何擔保**，甚至不包含適銷性或特定用途適用性的默示擔保。<br/>
 * 請參閱 **GNU 通用公共授權條款** 以獲取更多詳細資訊。<br/>
 * 您應當已經收到一份 **GNU 通用公共授權條款** 副本。<br/>
 * 如果沒有，請參閱 <https://www.gnu.org/licenses/gpl-3.0.html>。<br/>
 *
 * 以下是原本的版宣告
 *
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

/* [Türkçe/Turkish]
 * Kullanım talimatı:
 *	1) Adress değişkenini güncelleyin:
 *		ADDR PIN --> GND ise 1001000
 *		ADDR PIN --> VDD ise 1001001
 *		ADDR PIN --> SDA ise 1001010
 *		ADDR PIN --> SCL ise 1001011
 *
 *	2) ADS1115_Init(...) fonks. ile I2C peripheral ve PGA ile Data Rate ayarlarını yapın. (HAL_OK veya HAL_ERROR)
 *	3) ADS1115_readSingleEnded(...) fonksiyonu ile single-shot okuma yapacağınız portu seçin ve float tipinde değişkeninizin adresini gönderin.
 *	4) Üçüncü adımdan sonra değişkeninizin içerisinde uygun katsayıyla çarpılmış gerilim değeri saklanacaktır.
 *
 *
 * 新版和原本的差異列表
 * =========================================================================================
 *  [修改對照表 (Modification Log)]
 * =========================================================================================
 *  項目 (Item)              原始版本 (Original)                 修改後 (Modified by Ethan)
 *  ---------------------------------------------------------------------------------------
 *  狀態回報型別 (Status)    HAL_StatusTypeDef                   I2C_ADS1115_Status
 *                           (只有 HAL_OK / HAL_ERROR)         (自訂精準錯誤碼，如 NACK/TIMEOUT)
 *  ---------------------------------------------------------------------------------------
 *  錯誤除錯能力 (Debug)     只回傳 HAL_ERROR，無法得知具體原因        精準區分 I2C 錯誤，並印出 Log
 *  ---------------------------------------------------------------------------------------
 *  裝置位址 (Address)       全域變數 ADS1115_devAddress          #define 常數 (節省記憶體)
 *  ---------------------------------------------------------------------------------------
 *  函式命名 (Function)      ADS1115_Init / readOneShont         I2C_ADS1115_Init / readOneShot
 *                           (原作者拼字錯誤)                     (修正拼字)
 *  ---------------------------------------------------------------------------------------
 *  PGA 防呆 (Default)       無 default 分支                     加入 default 回傳 ERR_INVALID_PGA
 *  ---------------------------------------------------------------------------------------
 *  相容性 (Compatibility)   無                                   保留 HAL 外皮 (Wrapper) 供舊程式碼使用
 *  ---------------------------------------------------------------------------------------
 *  讀取逾時 (Timeout)       回傳 HAL_ERROR                       回傳 ERR_CONV_TIMEOUT
 *  ---------------------------------------------------------------------------------------
 *  滿量程警告 (Saturation)  無，直接回傳數值                         加入 WARN_PGA_SATURATED 警告
 *  ---------------------------------------------------------------------------------------
 *  程式架構 (Architecture) 核心與外皮混在一起                       分離「核心(I2C_ADS1115_Status)」
 *                                                                與「外皮(HAL_StatusTypeDef)」
 * =========================================================================================
 *
 *
 * English versions
 *
 * List of differences between the new and original versions
 * =========================================================================================
 *  [Modification Log]
 * =========================================================================================
 *  Item                    Original Version                    Modified by Ethan
 *  ---------------------------------------------------------------------------------------
 *  Status Return Type      HAL_StatusTypeDef                   I2C_ADS1115_Status
 *                          (Only HAL_OK / HAL_ERROR)           (Custom error codes: NACK/TIMEOUT)
 *  ---------------------------------------------------------------------------------------
 *  Debug Capability        Only returns HAL_ERROR;             Accurately distinguishes I2C errors
 *                          unclear where the fault occurred    and prints detailed logs
 *  ---------------------------------------------------------------------------------------
 *  Device Address          Global variable ADS1115_devAddress #define constant (saves memory)
 *  ---------------------------------------------------------------------------------------
 *  Function Naming         ADS1115_Init / readOneShont         I2C_ADS1115_Init / readOneShot
 *                          (original author's typo)            (typo fixed)
 *  ---------------------------------------------------------------------------------------
 *  PGA Safety              No default branch;                  Added default returning
 *                          invalid input causes UB             ERR_INVALID_PGA
 *  ---------------------------------------------------------------------------------------
 *  Compatibility           None                                Preserved HAL wrapper for
 *                                                              legacy code compatibility
 *  ---------------------------------------------------------------------------------------
 *  Read Timeout            Returns HAL_ERROR                   Returns ERR_CONV_TIMEOUT
 *  ---------------------------------------------------------------------------------------
 *  Saturation Warning      None; returns raw value directly    Added WARN_PGA_SATURATED warning
 *  ---------------------------------------------------------------------------------------
 *  Architecture            Core and wrapper mixed together     Separated into "Core
 *                                                              (I2C_ADS1115_Status)" and
 *                                                              "Wrapper (HAL_StatusTypeDef)"
 * =========================================================================================
 */

#include "I2C_Device\ads1115.h"

extern I2C_HandleTypeDef hi2c1;


/* Variables */
//uint8_t ADS1115_devAddress = 0b1001000;	// 0x48, Addr pin to Gnd, 7 bit address, without R/W' bit.

I2C_HandleTypeDef ADS1115_I2C_Handler;	// HAL I2C handler store variable.

uint16_t ADS1115_dataRate = ADS1115_DATA_RATE_128; // Default
uint16_t ADS1115_pga = ADS1115_PGA_TWO; // Default
uint16_t ADS1115_port = ADS1115_MUX_AIN0; // Default
//1 xxx 100

/**
 * @param ADS1115_config[2]
 * 用途：暫存要寫入 ADS1115 配置暫存器的兩個位元組
 * ADS1115 的 CONFIG 寄存器 是 16 位元（2 個位元組）：
 * 高位元組（MSB）：控制單次/連續模式、通道選擇、PGA 放大器增益等
 * 低位元組（LSB）：控制資料速率、比較器設定等
 *
 * */
uint8_t ADS1115_config[2];

/**
 * @param ADS1115_rawValue[2]
 * 用途：暫存從 ADS1115 讀回來的 ADC 轉換結果
 * ADS1115 的 CONVERSION 寄存器 也是 16 位元（2 個位元組）：
 * 高位元組 → ADS1115_rawValue[0]
 * 低位元組 → ADS1115_rawValue[1]
 * 程式讀取後會組合成整數 ADC 值：
 * */
uint8_t ADS1115_rawValue[2];

float ADS1115_voltCoef;			// Voltage coefficient.

I2C_ADS1115_Status I2C_ADS1115_Init(I2C_HandleTypeDef *handler, uint16_t setDataRate, uint16_t setPGA){

#ifdef use_TCA9548

	uint8_t ch=0xFF;

	  for(uint8_t i= 0; i<g_scan_count ; i++){
		  if (g_scan_result[i].Peripheral_address == ADS1115_Address)	{
			  ch=g_scan_result[i].TCA9548_CH;
			  printf("ADS1115 ch = %d, Address =  0x%02X\n ",ch,g_scan_result[i].Peripheral_address);
	          break; // 找到了就可以跳出迴圈了
		  }
	  }
		  TCA9548_SelectChannel(ch);   // ← 切到 EEPROM 所在通道

#endif

    if (handler == NULL) {
        return I2C_ADS1115_ERR_INVALID_HANDLER; // 防呆
    }

	// Handler
	memcpy(&ADS1115_I2C_Handler, handler, sizeof(*handler));

	// Data rate and PGA configurations.
	ADS1115_dataRate = setDataRate;
	ADS1115_pga = setPGA;

	// Voltage coefficient update.
	switch (ADS1115_pga) {

	case ADS1115_PGA_TWOTHIRDS:
		ADS1115_voltCoef = 0.1875;
		break;

	case ADS1115_PGA_ONE:
		ADS1115_voltCoef = 0.125;
		break;

	case ADS1115_PGA_TWO:
		ADS1115_voltCoef = 0.0625;
		break;

	case ADS1115_PGA_FOUR:
		ADS1115_voltCoef = 0.03125;
		break;

	case ADS1115_PGA_EIGHT:
		ADS1115_voltCoef = 0.015625;
		break;

	case ADS1115_PGA_SIXTEEN:
		ADS1115_voltCoef = 0.0078125;
		break;
    default:
        return I2C_ADS1115_ERR_INVALID_PGA; // 抓到亂填的 PGA

		}
	// 檢查晶片
	HAL_StatusTypeDef hal_st = HAL_I2C_IsDeviceReady(&ADS1115_I2C_Handler, (uint16_t)(ADS1115_Address << 1),
													  5, ADS1115_TIMEOUT);

	if (hal_st == HAL_OK) {
	        return I2C_ADS1115_OK;
	    }
	    else if (hal_st == HAL_TIMEOUT) {
	        return I2C_ADS1115_ERR_I2C_TIMEOUT; // 匯流排超時 (可能 SCL/SDA 被拉死)
	    }
	    else if (hal_st == HAL_BUSY) {
	        return I2C_ADS1115_ERR_I2C_BUSY;    // 匯流排忙碌中
	    }
	    else {
	        // hal_st == HAL_ERROR，進一步抓出底層錯誤旗標
	        uint32_t i2c_err = HAL_I2C_GetError(&ADS1115_I2C_Handler);

	        if (i2c_err & HAL_I2C_ERROR_AF) {
	            return I2C_ADS1115_ERR_I2C_NACK;      // 最常見：晶片沒接、沒電、位址設錯
	        }
	        else if (i2c_err & HAL_I2C_ERROR_BERR) {
	            return I2C_ADS1115_ERR_I2C_BERR;      // 匯流排錯誤 (雜訊干擾)
	        }
	        else if (i2c_err & HAL_I2C_ERROR_ARLO) {
	            return I2C_ADS1115_ERR_I2C_ARBITRATION; // 多主機搶佔失敗
	        }
	        else {
	            return I2C_ADS1115_ERR_I2C_UNKNOWN;   // 其他未知的 HAL_ERROR
	        }
	    }

	/*if (HAL_I2C_IsDeviceReady(&ADS1115_I2C_Handler, (uint16_t) (ADS1115_devAddress << 1), 5, ADS1115_TIMEOUT) == HAL_OK) {
		return HAL_OK;
	} else {
		return HAL_ERROR;
	}*/
}


/* =========================================================
 * 2. 再寫外皮（Wrapper）
 * ========================================================= */

//外皮1 轉成回傳整數值
// 在 ads1115.c 裡面，自己定義的 init (回傳 int 就好)

#define ADS1115_Wrapper

#ifdef ADS1115_Wrapper

int ADS1115_DeviceInit_Wrapper(void) {
    // 假設你的 handler 是 &hi2c1
    //return (int)I2C_ADS1115_Init(&hi2c1, ADS1115_DATA_RATE_128, ADS1115_PGA_ONE);

    I2C_ADS1115_Status status = I2C_ADS1115_Init(&hi2c1, ADS1115_DATA_RATE_128, ADS1115_PGA_ONE);

    // 2. 根據狀態碼印出詳細報告
    switch (status) {
        case I2C_ADS1115_OK:
            return 0; // 成功回傳 0

        case I2C_ADS1115_ERR_INVALID_PGA:
            printf("[ADS1115] Error: Invalid PGA setting.\n");
            return (int)status;

        case I2C_ADS1115_ERR_INVALID_HANDLER:
            printf("[ADS1115] Error: I2C Handler is NULL.\n");
            return (int)status;

        case I2C_ADS1115_ERR_I2C_BUSY:
            printf("[ADS1115] Error: I2C Bus BUSY.\n");
            return (int)status;

        case I2C_ADS1115_ERR_I2C_TIMEOUT:
            printf("[ADS1115] Error: I2C TIMEOUT (Check wires).\n");
            return (int)status;

        case I2C_ADS1115_ERR_I2C_NACK:
            printf("[ADS1115] Error: I2C NACK (Check power/address).\n");
            return (int)status;

        case I2C_ADS1115_ERR_I2C_ARBITRATION:
            printf("[ADS1115] Error: I2C Arbitration lost.\n");
            return (int)status;

        case I2C_ADS1115_ERR_I2C_BERR:
            printf("[ADS1115] Error: I2C Bus Error (Noise).\n");
            return (int)status;

        case I2C_ADS1115_ERR_I2C_OVR:
            printf("[ADS1115] Error: I2C Overrun.\n");
            return (int)status;

        case I2C_ADS1115_ERR_I2C_UNKNOWN:
            printf("[ADS1115] Error: Unknown I2C Error.\n");
            return (int)status;

        default:
            printf("[ADS1115] Error: Unhandled code %d.\n", status);
            return (int)status;

    }
}
#else

//外皮2 轉成HAL_StatusTypeDef
HAL_StatusTypeDef ADS1115_Init(I2C_HandleTypeDef *handler, uint16_t setDataRate, uint16_t setPGA) {

    // 呼叫核心邏輯
    I2C_ADS1115_Status status = I2C_ADS1115_Init(handler, setDataRate, setPGA);

    // 2. 根據狀態碼印出詳細報告，並轉回 HAL 型別
    switch (status) {
        case I2C_ADS1115_OK:
            // 成功就默默通過，不印 Log 省 UART 頻寬
            return HAL_OK;

        /* --- 軟體與參數錯誤 --- */
        case I2C_ADS1115_ERR_NOT_INIT:
            printf("[ADS1115] Error: Driver not initialized.\n");
            return HAL_ERROR;
        case I2C_ADS1115_ERR_INVALID_PGA:
            printf("[ADS1115] Error: Invalid PGA setting (Check enum value).\n");
            return HAL_ERROR;
        case I2C_ADS1115_ERR_INVALID_MUX:
            printf("[ADS1115] Error: Invalid MUX channel.\n");
            return HAL_ERROR;
        case I2C_ADS1115_ERR_INVALID_RATE:
            printf("[ADS1115] Error: Invalid Data Rate.\n");
            return HAL_ERROR;
        case I2C_ADS1115_ERR_INVALID_HANDLER:
            printf("[ADS1115] Error: I2C Handler is NULL.\n");
            return HAL_ERROR;

        /* --- I2C 硬體與通訊錯誤 --- */
        case I2C_ADS1115_ERR_I2C_BUSY:
            printf("[ADS1115] Error: I2C Bus is BUSY.\n");
            return HAL_ERROR;
        case I2C_ADS1115_ERR_I2C_TIMEOUT:
            printf("[ADS1115] Error: I2C TIMEOUT (Check wires/pull-ups).\n");
            return HAL_ERROR;
        case I2C_ADS1115_ERR_I2C_NACK:
            printf("[ADS1115] Error: I2C NACK! Device not responding (Check power/address).\n");
            return HAL_ERROR;
        case I2C_ADS1115_ERR_I2C_ARBITRATION:
            printf("[ADS1115] Error: I2C Arbitration lost.\n");
            return HAL_ERROR;
        case I2C_ADS1115_ERR_I2C_BERR:
            printf("[ADS1115] Error: I2C Bus Error (Noise detected).\n");
            return HAL_ERROR;
        case I2C_ADS1115_ERR_I2C_OVR:
            printf("[ADS1115] Error: I2C Overrun/Underrun.\n");
            return HAL_ERROR;
        case I2C_ADS1115_ERR_I2C_UNKNOWN:
            printf("[ADS1115] Error: Unknown HAL I2C Error.\n");
            return HAL_ERROR;

        /* --- 暫存器與硬體行為錯誤 (Init 通常不會遇到，但保險起見留著) --- */
        case I2C_ADS1115_ERR_CFG_READ_FAIL:
            printf("[ADS1115] Error: Failed to read Config Register.\n");
            return HAL_ERROR;
        case I2C_ADS1115_ERR_CFG_VERIFY_FAIL:
            printf("[ADS1115] Error: Config Register readback mismatch.\n");
            return HAL_ERROR;
        case I2C_ADS1115_ERR_CONV_READ_FAIL:
            printf("[ADS1115] Error: Failed to read Conversion Register.\n");
            return HAL_ERROR;
        case I2C_ADS1115_ERR_CONV_TIMEOUT:
            printf("[ADS1115] Error: Conversion timeout (OS bit not set).\n");
            return HAL_ERROR;
        case I2C_ADS1115_ERR_CONV_NOT_READY:
            printf("[ADS1115] Error: Conversion data not ready.\n");
            return HAL_ERROR;

        /* --- 警告 (Init 階段通常不會觸發) --- */
        case I2C_ADS1115_WARN_PGA_SATURATED:
        case I2C_ADS1115_WARN_NEAR_RAIL:
        case I2C_ADS1115_WARN_NOISE_HIGH:
            // 警告在 Init 階段不應該發生，如果發生了代表底層邏輯有 Bug
            printf("[ADS1115] Warning triggered during Init (Logic Error).\n");
            return HAL_ERROR;

        /* --- 防呆：未來如果加了新錯誤碼忘了更新這裡 --- */
        default:
            printf("[ADS1115] Error: Unhandled status code %d.\n", status);
            return HAL_ERROR;
    }

}
#endif

I2C_ADS1115_Status I2C_ADS1115_readOneShot(uint16_t muxPort, float *voltage) {


    ADS1115_config[0] = ADS1115_OS | muxPort | ADS1115_pga | ADS1115_MODE;

    ADS1115_config[1] = ADS1115_dataRate | ADS1115_COMP_MODE | ADS1115_COMP_POL | ADS1115_COMP_LAT | ADS1115_COMP_QUE;
    uint8_t waiting = 1;
    uint16_t cnt = 0;

    // 1. 寫入 Config 啟動轉換
    if (HAL_I2C_Mem_Write(&ADS1115_I2C_Handler, (uint16_t)(ADS1115_Address << 1), ADS1115_CONFIG_REG, 1,
                          ADS1115_config, 2, ADS1115_TIMEOUT) != HAL_OK) {
        // 精準抓取寫入失敗的原因
        uint32_t err = HAL_I2C_GetError(&ADS1115_I2C_Handler);
        if (err & HAL_I2C_ERROR_AF) return I2C_ADS1115_ERR_I2C_NACK;
        if (err & HAL_I2C_ERROR_BERR) return I2C_ADS1115_ERR_I2C_BERR;
        return I2C_ADS1115_ERR_I2C_UNKNOWN;
    }

    // 2. 輪詢等待轉換完成 (檢查 OS bit)
    while (waiting) {
        if (HAL_I2C_Mem_Read(&ADS1115_I2C_Handler, (uint16_t)((ADS1115_Address << 1) | 0x1),
        		ADS1115_CONFIG_REG, 1, ADS1115_config, 2, ADS1115_TIMEOUT) == HAL_OK) {
            if (ADS1115_config[0] & ADS1115_OS) {
                waiting = 0; // 轉換完成
            }
        } else {
            return I2C_ADS1115_ERR_CFG_READ_FAIL; // 讀取 Config 失敗
        }

        if (++cnt == 100) {
            return I2C_ADS1115_ERR_CONV_TIMEOUT; // 等不到 OS bit，轉換超時
        }
    }

    // 3. 讀取轉換結果
    if (HAL_I2C_Mem_Read(&ADS1115_I2C_Handler, (uint16_t)((ADS1115_Address << 1) | 0x1),
    		ADS1115_CONVER_REG, 1, ADS1115_rawValue, 2, ADS1115_TIMEOUT) == HAL_OK) {

        // 組合 16-bit 數值
        /**
         * *voltage = (float)(   											 // 把兩個 uint8_t 組成一個 int16_t 原始 ADC 值
         *     (int16_t)(ADS1115_rawValue[0] << 8 | ADS1115_rawValue[1])     // 乘上電壓換算係數，轉換成毫伏 (mV)
         *      ADS1115_voltCoef);
         *
         *      - ADS1115_rawValue[0] << 8
         *        將高位元組左移 8 位，變成 16 位的高位部分
         *      - | ADS1115_rawValue[1]
         *        把低位元組與高位元組做 OR，組成完整的 16-bit ADC 值
         *      - (int16_t)(...)
         *        將組合後的無號數值轉成有號整數，因為 ADC 讀出來可能是負數（例如差分模式）
         *      - ADS1115_voltCoef
         *        每個 bit 對應多少電壓（mV/bit），把 ADC 原始值轉成實際電壓
         *      - (float)(...)
         *        強制轉型成浮點數，因為 voltage 參數是 float*
         *
         * */


        int16_t raw = (int16_t)((ADS1115_rawValue[0] << 8) | ADS1115_rawValue[1]);
        *voltage = (float)(raw * ADS1115_voltCoef);

        // 【選用】加上滿量程警告
        if (raw == 32767 || raw == -32768) {
            return I2C_ADS1115_WARN_PGA_SATURATED;
        }

        return I2C_ADS1115_OK;
    } else {
        return I2C_ADS1115_ERR_CONV_READ_FAIL; // 讀取轉換結果失敗
    }
}


HAL_StatusTypeDef ADS1115_readOneShot(uint16_t muxPort, float *voltage) {

    // 呼叫核心，注意這裡不要帶型別！
    I2C_ADS1115_Status status = I2C_ADS1115_readOneShot(muxPort, voltage);

    // 根據狀態回傳
    switch (status) {
        case I2C_ADS1115_OK:
            return HAL_OK;

        case I2C_ADS1115_WARN_PGA_SATURATED:
            // 如果是警告，可以印出訊息，但依然回傳 HAL_OK 讓程式繼續跑
            printf("[ADS1115] Warning: ADC Saturated! Voltage exceeds PGA range.\n");
            return HAL_OK;

        case I2C_ADS1115_ERR_CONV_TIMEOUT:
            printf("[ADS1115] Error: Conversion Timeout (OS bit not set).\n");
            return HAL_ERROR;

        case I2C_ADS1115_ERR_I2C_NACK:
            printf("[ADS1115] Error: I2C NACK during read.\n");
            return HAL_ERROR;

        case I2C_ADS1115_ERR_CFG_READ_FAIL:
        case I2C_ADS1115_ERR_CONV_READ_FAIL:
            printf("[ADS1115] Error: Failed to read register.\n");
            return HAL_ERROR;

        default:
            printf("[ADS1115] Error: Read failed with code %d.\n", status);
            return HAL_ERROR;
    }
}


I2C_ADS1115_Status I2C_ADS1115_StartContinuous(uint16_t muxPort, uint16_t pga, uint16_t dataRate ){

	// 記錄 PGA / DataRate
	    ADS1115_pga = pga;
	    ADS1115_dataRate = dataRate;

	    // 設定 Config Register
	    ADS1115_config[0] = ADS1115_OS | muxPort | ADS1115_pga | ADS1115_MODE_CONTINUOUS;
	    ADS1115_config[1] = ADS1115_dataRate | ADS1115_COMP_MODE | ADS1115_COMP_POL | ADS1115_COMP_LAT |
	    			 	 	ADS1115_COMP_QUE;
	    // 寫入 I2C
	    HAL_StatusTypeDef hal_st = HAL_I2C_Mem_Write(&ADS1115_I2C_Handler,
	                                                 (uint16_t)(ADS1115_Address << 1),
	                                                 ADS1115_CONFIG_REG, 1,
	                                                 ADS1115_config, 2,
	                                                 ADS1115_TIMEOUT);

	    if (hal_st == HAL_OK) {
	        return I2C_ADS1115_OK;
	    } else if (hal_st == HAL_TIMEOUT) {
	        return I2C_ADS1115_ERR_I2C_TIMEOUT;
	    } else {
	        uint32_t err = HAL_I2C_GetError(&ADS1115_I2C_Handler);
	        if (err & HAL_I2C_ERROR_AF) return I2C_ADS1115_ERR_I2C_NACK;
	        return I2C_ADS1115_ERR_I2C_UNKNOWN;
	    }

}


HAL_StatusTypeDef ADS1115_StartContinuous(uint16_t muxPort, uint16_t pga, uint16_t dataRate) {

	I2C_ADS1115_Status status = I2C_ADS1115_StartContinuous(muxPort, pga, dataRate);

	    if (status != I2C_ADS1115_OK) {
	        printf("[ADS1115] StartContinuous Failed: Code %d\n", status);
	        return HAL_ERROR;
	    }
	    return HAL_OK;

}


I2C_ADS1115_Status I2C_ADS1115_ReadContinuous(float *voltage){

	//return type defaults to 'int' [-Wimplicit-int]

    // 直接讀取轉換結果暫存器 (因為 ADC 正在背景持續轉換)
    HAL_StatusTypeDef hal_st = HAL_I2C_Mem_Read(&ADS1115_I2C_Handler,
                                                (uint16_t)(ADS1115_Address << 1) | 0x1,
                                                ADS1115_CONVER_REG, 1,
                                                ADS1115_rawValue, 2,
                                                ADS1115_TIMEOUT);

    if (hal_st != HAL_OK) {
        if (hal_st == HAL_TIMEOUT) return I2C_ADS1115_ERR_I2C_TIMEOUT;

        uint32_t err = HAL_I2C_GetError(&ADS1115_I2C_Handler);
        if (err & HAL_I2C_ERROR_AF) return I2C_ADS1115_ERR_I2C_NACK;
        return I2C_ADS1115_ERR_I2C_UNKNOWN;
    }

    // 組合 16-bit 數值並換算電壓
    int16_t raw = (int16_t)((ADS1115_rawValue[0] << 8) | ADS1115_rawValue[1]);
    *voltage = (float)(raw * ADS1115_voltCoef);

    // 【選用】偵測是否量測爆表
    if (raw == 32767 || raw == -32768) {
        return I2C_ADS1115_WARN_PGA_SATURATED;
    }

    return I2C_ADS1115_OK;
}

HAL_StatusTypeDef ADS1115_ReadContinuous(float *voltage){
	// 直接讀暫存器，因為 ADC 持續更新
	if(HAL_I2C_Mem_Read(&ADS1115_I2C_Handler,
			(uint16_t)(ADS1115_Address << 1),
			ADS1115_CONVER_REG, 1,
			ADS1115_rawValue, 2, ADS1115_TIMEOUT) != HAL_OK) {
		return HAL_ERROR;
	}
	// 計算電壓 (mV)
	*voltage = (float)(((int16_t)(ADS1115_rawValue[0] << 8) | ADS1115_rawValue[1]) * ADS1115_voltCoef);
	return HAL_OK;
}


/**
 * 以下為原始版本
 * 我這所以會改,是因為原始版本如果出錯了,只會說一句:HAL_Error而已
 * 錯在哪裡，發生什麼事了，什麼都不知道．看了就應隔
 * 其實只要留下用I2C_ADS1115_Status宣告的函數就好，至於皮？是因為我懶得改其他的程式碼而已．
 * 如果哪天有機會，就把其他的給改了．
 *
 *
 *
 *
 * */
/* Function definitions. */
/**
 *
 *

HAL_StatusTypeDef ADS1115_Init(I2C_HandleTypeDef *handler, uint16_t setDataRate, uint16_t setPGA) {

	// Handler
	memcpy(&ADS1115_I2C_Handler, handler, sizeof(*handler));

	// Data rate and PGA configurations.
	ADS1115_dataRate = setDataRate;
	ADS1115_pga = setPGA;


	// Voltage coefficient update.
	switch (ADS1115_pga) {

	case ADS1115_PGA_TWOTHIRDS:
		ADS1115_voltCoef = 0.1875;
		break;

	case ADS1115_PGA_ONE:
		ADS1115_voltCoef = 0.125;
		break;

	case ADS1115_PGA_TWO:
		ADS1115_voltCoef = 0.0625;
		break;

	case ADS1115_PGA_FOUR:
		ADS1115_voltCoef = 0.03125;
		break;

	case ADS1115_PGA_EIGHT:
		ADS1115_voltCoef = 0.015625;
		break;

	case ADS1115_PGA_SIXTEEN:
		ADS1115_voltCoef = 0.0078125;
		break;

		}

	if (HAL_I2C_IsDeviceReady(&ADS1115_I2C_Handler, (uint16_t) (ADS1115_devAddress << 1), 5, ADS1115_TIMEOUT) == HAL_OK) {
		return HAL_OK;
	} else {
		return HAL_ERROR;
	}

}

I2C_ADS1115_Status I2C_ADS1115_readOneShont(uint16_t muxPort, float *voltage){
	ADS1115_config[0] = ADS1115_OS | muxPort | ADS1115_pga | ADS1115_MODE;
		ADS1115_config[1] = ADS1115_dataRate | ADS1115_COMP_MODE | ADS1115_COMP_POL | ADS1115_COMP_LAT| ADS1115_COMP_QUE;
		uint8_t waiting=1;
		uint16_t cnt=0;

		if(HAL_I2C_Mem_Write(&ADS1115_I2C_Handler, (uint16_t) (ADS1115_devAddress << 1), ADS1115_CONFIG_REG, 1, ADS1115_config, 2, ADS1115_TIMEOUT) == HAL_OK)
		{
			while(waiting) //Checking Data Ready
			{
				if(HAL_I2C_Mem_Read(&ADS1115_I2C_Handler, (uint16_t) ((ADS1115_devAddress << 1) | 0x1), ADS1115_CONFIG_REG, 1, ADS1115_config, 2, ADS1115_TIMEOUT) == HAL_OK)
					{
					if(ADS1115_config[0] & ADS1115_OS) waiting=0;
					}
				else return HAL_ERROR;
				if(++cnt==100) return HAL_ERROR;
			}


			if(HAL_I2C_Mem_Read(&ADS1115_I2C_Handler, (uint16_t) ((ADS1115_devAddress << 1) | 0x1), ADS1115_CONVER_REG, 1, ADS1115_rawValue, 2, ADS1115_TIMEOUT) == HAL_OK)
			{
				*voltage = (float) (((int16_t) (ADS1115_rawValue[0] << 8) | ADS1115_rawValue[1]) * ADS1115_voltCoef);
				return HAL_OK;

			}

		}

		return HAL_ERROR;

}
HAL_StatusTypeDef ADS1115_StartContinuous(uint16_t muxPort, uint16_t pga, uint16_t dataRate) {
    // 記錄 PGA / DataRate
    ADS1115_pga = pga;
    ADS1115_dataRate = dataRate;

    // 設定 Config Register
    ADS1115_config[0] = ADS1115_OS | muxPort | ADS1115_pga | ADS1115_MODE_CONTINUOUS;
    ADS1115_config[1] = ADS1115_dataRate | ADS1115_COMP_MODE | ADS1115_COMP_POL | ADS1115_COMP_LAT | ADS1115_COMP_QUE;

    // 寫入 I2C 啟動連續模式
    return HAL_I2C_Mem_Write(&ADS1115_I2C_Handler, (uint16_t)(ADS1115_devAddress << 1),
                             ADS1115_CONFIG_REG, 1, ADS1115_config, 2, ADS1115_TIMEOUT);
}


HAL_StatusTypeDef ADS1115_ReadContinuous(float *voltage){
	// 直接讀暫存器，因為 ADC 持續更新
	if(HAL_I2C_Mem_Read(&ADS1115_I2C_Handler,
			(uint16_t)(ADS1115_devAddress << 1),
			ADS1115_CONVER_REG, 1,
			ADS1115_rawValue, 2, ADS1115_TIMEOUT) != HAL_OK) {
		return HAL_ERROR;
	}
	// 計算電壓 (mV)
	*voltage = (float)(((int16_t)(ADS1115_rawValue[0] << 8) | ADS1115_rawValue[1]) * ADS1115_voltCoef);
	return HAL_OK;
}*/


