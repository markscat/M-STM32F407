/*
 * 程式名	　			:	ADS1115 相關驅動程式(ADS1115 Driver)
 * 更新人員(Modify by)	:	Ethan Yang
 * Date Modify			:	2025/10/03 (YYYY/MM/DD)
 * Last Modify			:
 *  * Description:		STM32F4 HAL-Based ADS1115 Library
 * References:
 * 	- https://www.ti.com/lit/gpn/ADS1113 [Datasheet]
 * 	- https://github.com/Squieler/ADS1115[source Code]
 *
 * 本程式主要是個人興趣索然，想要試試看ADS1115，在GitHUB上找到這個程式,所以想要測試看看.
 * 同時看看自己是否可以做一些修改，或是增加其他的功能。
 * 從這個版本看來，最多只有初始化，
 *
 *Vdd = 3.3V
 *|  PGA 設定	| FSR ±V | 每 bit 解析度 (mV/bit) 	| 實際最大可量測電壓  | 註解            			|
 *| -------- 	| ------ | --------------------- 	| -----------------  | ------------------------ |
 *|   2/3x   	| ±6.144 | 0.1875                	| 3.3                | 量程沒用到一半，解析度最粗	|
 *|   1x     	| ±4.096 | 0.125                 	| 3.3                | 解析度比 2/3x 高   		|
 *|   2x     	| ±2.048 | 0.0625                	| 3.3                | 適合低壓量測，解析度更好  	|
 *|   4x     	| ±1.024 | 0.03125               	| 1.024              | 適合 1V 以下訊號量測  	|
 *|   8x     	| ±0.512 | 0.015625              	| 0.512              | 適合小訊號精密量測     	|
 *|   16x    	| ±0.256 | 0.0078125				| 0.256              | 超精密量測小訊號     	 	|
 *
 *Vdd = 5V
 *|  PGA 設定	| FSR ±V | 每 bit 解析度 (mV/bit) 	| 實際最大可量測電壓  | 註解            			|
 *| -------- 	| ------ | --------------------- 	| -----------------  | ------------------------ |
 *|   2/3x   	| ±6.144 | 0.1875                	| 5                  | 量程沒用到一半，解析度最粗	|
 *|   1x     	| ±4.096 | 0.125                 	| 4.96               | 最大量程安全可量測 4.096V	|
 *|   2x     	| ±2.048 | 0.0625                	| 2.048              | 適合 2V 以下訊號量測  	|
 *|   4x     	| ±1.024 | 0.03125               	| 1.024              | 精密量測 1V 以下訊號  	|
 *|   8x     	| ±0.512 | 0.015625              	| 0.512              | 適合小訊號精密量測     	|
 *|   16x    	| ±0.256 | 0.0078125				| 0.256              | 超精密量測小訊號     	 	| *
 *
 *
 *
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
 *
 *	Bit15  Bit14-12   Bit11-9   Bit8   |  Bit7-5     Bit4       Bit3     Bit2        Bit1-0
 *  OS     MUX[2:0]   PGA[2:0]  MODE   |  DR[2:0]  COMP_MODE  COMP_POL  COMP_LAT  COMP_QUE[1:0]
 *
 *
 *  */


#include "stm32f4xx_hal.h" // Could be changed for a specific processor.
#include "string.h"
#include "stdio.h"
#include "Ext_ADC\ads1115.h"

/* Variables */
uint8_t ADS1115_devAddress = 0b1001000;	// 0x48, Addr pin to Gnd, 7 bit address, without R/W' bit.

I2C_HandleTypeDef ADS1115_I2C_Handler;	// HAL I2C handler store variable.

uint16_t ADS1115_dataRate = ADS1115_DATA_RATE_128; // Default
uint16_t ADS1115_pga = ADS1115_PGA_TWO; // Default
uint16_t ADS1115_port = ADS1115_MUX_AIN0; // Default


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

/* Function definitions. */
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

HAL_StatusTypeDef ADS1115_readSingleEnded(uint16_t muxPort, float *voltage) {

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
			*voltage = (float) (((int16_t) (ADS1115_rawValue[0] << 8) | ADS1115_rawValue[1]) * ADS1115_voltCoef);
			return HAL_OK;

		}

	}

	return HAL_ERROR;
}

HAL_StatusTypeDef ADS1115_readDifferential(uint16_t muxSetting, float *voltage) {

    // 設定 Config Register，高位元組
    ADS1115_config[0] = ADS1115_OS | muxSetting | ADS1115_pga | ADS1115_MODE;

    // 設定 Config Register，低位元組
    ADS1115_config[1] = ADS1115_dataRate | ADS1115_COMP_MODE |
                        ADS1115_COMP_POL | ADS1115_COMP_LAT | ADS1115_COMP_QUE;

    uint8_t waiting = 1;
    uint16_t cnt = 0;

    // 寫入設定，觸發轉換
    if (HAL_I2C_Mem_Write(&ADS1115_I2C_Handler,
                          (uint16_t)(ADS1115_devAddress << 1),
                          ADS1115_CONFIG_REG,
                          1, ADS1115_config, 2, ADS1115_TIMEOUT) == HAL_OK)
    {
        // 等待轉換完成
        while (waiting) {
            if (HAL_I2C_Mem_Read(&ADS1115_I2C_Handler,
                                 (uint16_t)(ADS1115_devAddress << 1),
                                 ADS1115_CONFIG_REG,
                                 1, ADS1115_config, 2, ADS1115_TIMEOUT) == HAL_OK)
            {
                if (ADS1115_config[0] & ADS1115_OS) waiting = 0;
            } else {
                return HAL_ERROR;
            }

            if (++cnt == 100) return HAL_TIMEOUT; // 避免死循環
        }

        // 讀取轉換結果
        if (HAL_I2C_Mem_Read(&ADS1115_I2C_Handler,
                             (uint16_t)(ADS1115_devAddress << 1),
                             ADS1115_CONVER_REG,
                             1, ADS1115_rawValue, 2, ADS1115_TIMEOUT) == HAL_OK)
        {
            // 注意：差動輸入會有正負號，所以用 int16_t
            int16_t raw = (int16_t)(ADS1115_rawValue[0] << 8 | ADS1115_rawValue[1]);

            *voltage = (float)raw * ADS1115_voltCoef; // 轉換成電壓

            return HAL_OK;
        }
    }

    return HAL_ERROR;
}

