
/**
 * @file TCA555.c
 * @brief TCA555 16位元擴充GPIO IC 驅動程式
 * @author Deepseek and Ethan
 * @date Feb 16, 2025
 * 
 * This program is mainly used to connect TCA555 and STM32F407; currently only the output has been tested, 
 *	and the input is to be tested.
 	
 *	Yes, that's right, this program was written by Deepseek, and I tested it.
 *	I'm a hardware engineer, not a software engineer, so what?
 *
 *	本程式主要為TCA555 和STM32F407連結用的;目前測試到的只有輸出,輸入待測試
 *  對,沒錯,這個程式是Deepseek寫，我來測試的。
 *  我是硬體工程師，又不是軟體，那又怎樣？
 *
 * 動作原理:
 * 先宣告一個十六位元的參數
 * uint16_t output_data = 0xFFFF;
 *
 * 將output_data分成高八位元和低八位元,每一個位元代表一個Port的輸出的數值
 * output_data每個位元對應的port如下表
 *
 *  +-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
 *　|    bit 15   |   bit 14    |    bit 13   |    bit 12   |   bit 11    |    bit 10   |     bit 9   |     bit 8   |
 *　+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
 *　|   Port 15   |   Port 14   |   Port 13   |   Port 12   |   Port 11   |   Port 10   |   Port 09   |   Port 08   |
 *　+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
 *　|    bit 7    |     bit 6   |     bit 5   |    bit 4    |     bit 3   |    bit 2    |    bit 1    |    bit 0    |
 *　+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
 *　|   Port 07   |   Port 06   |   Port 05   |   Port 04   |   Port 03   |   Port 02   |   Port 01   |   Port 00   |
 *  +-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
 *
 *TCA9555 的暫存器地圖（供你參考）：
	暫存器位址		功能					說明
     (Hex)
	 0x00		Input Port 0		讀取 P00 ~ P07 的狀態（唯讀）
	 0x01		Input Port 1		讀取 P10 ~ P17 的狀態（唯讀）
	 0x02		Output Port 0		設定 P00 ~ P07 的輸出電平
	 0x03		Output Port 1		設定 P10 ~ P17 的輸出電平
	 0x04		Polarity Inv 0		反相設定（預設 0x00）
	 0x05		Polarity Inv 1		反相設定（預設 0x00）
	 0x06		Config 0			設定 P00 ~ P07 是輸入還是輸出
	 0x07		Config 1			設定 P10 ~ P17 是輸入還是輸出
 *
 *
 *
 *
 *@note
 *  版權：
 * GNU GENERAL PUBLIC LICENSE
 * Version 3, 29 June 2007
 * Copyright (C) [2025] [Ethan]
 * 本程式是一個自由軟體：您可以依照 **GNU 通用公共授權條款（GPL）** 發佈和/或修改，
 * GPL 版本 3 或（依您選擇）任何更新版本。
 *
 * 本程式的發佈目的是希望它對您有幫助，但 **不提供任何擔保**，甚至不包含適銷性或特定用途適用性的默示擔保。
 * 請參閱 **GNU 通用公共授權條款** 以獲取更多詳細資訊。
 * 您應當已經收到一份 **GNU 通用公共授權條款** 副本。
 * 如果沒有，請參閱 <https://www.gnu.org/licenses/gpl-3.0.html>。

 *  Created on: Feb 28, 2025
 *      Author: user
 */
#include <I2C_Device\I2C_TCA555.h>
#include "i2c.h"

//extern I2C_HandleTypeDef hi2c1;

/**
 *  @brief 初始化TCA9555
 *  1. 確認設備是否就緒
 *  2. 配置 TCA9555 的 16 個 GPIO 為輸出模式（默認全部輸入）
 */
TCA9555_Status TCA9555_Init() {
    //** 檢查 I2C 是否就緒*/

#ifdef use_TCA9548

	if(TCA9548_SelectByAddress(TCA9555_ADDR) != HAL_OK){
		printf("TCA9555_I/O expander not exist, or Not connected to TCA9548");
	}else{
		TCA9548_ShowChAndAddress_SelectByAddress(TCA9555_ADDR,"TCA9555_I/O expander");
	}

#endif

	 if (BSP_I2C_IsReady(TCA9555_ADDR) != HAL_OK) {
	        return TCA9555_ERR_NOT_FOUND;
	    }
 	 //配置 16 個 GPIO 為輸出模式
	 uint8_t config_cmd[3] = {0x06, 0x00, 0x00};
	 if (BSP_I2C_Write(TCA9555_ADDR, config_cmd, 3) != HAL_OK) {
		 printf("TCA9555 init fial");
		 return TCA9555_ERR_WRITE_FAIL;
	 }

    return TCA9555_OK;


	 /*
    if (HAL_I2C_IsDeviceReady(&hi2c1, TCA9555_ADDR, 3, 100) != HAL_OK) {
        Error_Handler();  // 設備未響應
    }

    // 配置 TCA9555 的 16 個 GPIO 為輸出模式（默認全部輸入）
    uint8_t config_cmd[3] = {0x06, 0x00, 0x00}; // 0x06 是配置暫存器地址
    if (HAL_I2C_Master_Transmit(&hi2c1, TCA9555_ADDR, config_cmd, 3, 100) != HAL_OK) {
        Error_Handler();
    }*/
}



int TCA9555_DeviceInit_Wrapper(void) {
    // 假設你的 handler 是 &hi2c1
	    return (int)TCA9555_Init() ; // 假設 EEPROM_DeviceInit 回傳 I2C_Status，I2C_OK 通常是 0
	}

/**
 *  @brief 寫入輸出
 *  @param data 設定輸出埠的數值
 *
 */

#define with_BSP

// 假設你的 TCA9555_ADDR 已經定義為 7-bit 的 0x20
// 如果之前是定義 0x40，請改成 0x20


//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝設定io方向＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝


/**
 * @brief 動態設定 TCA9555 的 Port 0 和 Port 1 方向
 * @param p0_dir Port 0 (P00~P07) 的方向 (使用 TCA9555_DIR_OUTPUT 或 TCA9555_DIR_INPUT)
 * @param p1_dir Port 1 (P10~P17) 的方向
 */
void TCA9555_ConfigPorts(uint8_t p0_dir, uint8_t p1_dir) {

#ifdef use_TCA9548
    // 確保切換到 TCA9555 的通道
    if (TCA9548_SelectByAddress(TCA9555_ADDR) != HAL_OK) {
        printf("TCA9555 not found on MUX\n");
        return;
    }
#endif

    uint8_t config_cmd[3] = {
        0x06,   // 從 Port 0 的配置暫存器開始
        p0_dir, // Port 0 的方向
        p1_dir  // Port 1 的方向 (自動遞增寫入 0x07)
    };

    // 使用 BSP 寫入 (假設你已經改好 BSP)
    if (BSP_I2C_Write(TCA9555_ADDR, config_cmd, 3) != HAL_OK) {
        printf("TCA9555 ConfigPorts failed!\n");
    }
}

void TCA9555_SetPort0Dir(uint8_t dir) {
#ifdef use_TCA9548
    // 確保切換到 TCA9555 的通道
    if (TCA9548_SelectByAddress(TCA9555_ADDR) != HAL_OK) {
        printf("TCA9555 not found on MUX\n");
        return;
    }
#endif
    uint8_t cmd[2] = {0x06, dir};
    BSP_I2C_Write(TCA9555_ADDR, cmd, 2);
}

void TCA9555_SetPort1Dir(uint8_t dir) {

#ifdef use_TCA9548
    // 確保切換到 TCA9555 的通道
    if (TCA9548_SelectByAddress(TCA9555_ADDR) != HAL_OK) {
        printf("TCA9555 not found on MUX\n");
        return;
    }
#endif

    uint8_t cmd[2] = {0x07, dir};
    BSP_I2C_Write(TCA9555_ADDR, cmd, 2);
}


/**
 * @brief 獨立設定 TCA9555 某個腳位是輸入還是輸出 (不影響同 Port 的其他腳位)
 * @param pin 腳位編號 (0~15，0代表P00，8代表P10)
 * @param is_input 1 = 設為輸入，0 = 設為輸出
 *
 */
void TCA9555_SetPinDirection(uint8_t pin, uint8_t is_input) {
    if (pin > 15) return;

    uint8_t port = (pin >= 8) ? 1 : 0;
    uint8_t bit_pos = pin % 8;
    uint8_t reg_addr = 0x06 + port;
    //printf("Debug port= %d \n bit_pos = %d ,\n reg_addr =  0x%02X \n\n",port,bit_pos,reg_addr);
    // 確保切換到正確的通道
    #ifdef use_TCA9548
        if (TCA9548_SelectByAddress(TCA9555_ADDR) != HAL_OK) return;
    #endif

    // 1. 讀取目前的 Config 暫存器 (改用 MemRead)
    uint8_t current_config = 0;
    if (BSP_I2C_MemRead(TCA9555_ADDR, reg_addr, &current_config, 1) != HAL_OK) {
        printf("TCA9555 Read Config Failed\n");
        return;
    }

    // 2. 修改那個 bit
    if (is_input) {
        current_config |= (1 << bit_pos);
    } else {
        current_config &= ~(1 << bit_pos);
    }
    printf("\n Debug port= %d \n bit_pos = %d ,\n reg_addr =  0x%02X \n\n",port,bit_pos,reg_addr);
    // 3. 寫回 Config 暫存器
    uint8_t cmd[2] = {reg_addr, current_config};
    BSP_I2C_Write(TCA9555_ADDR, cmd, 2);

}


/**
 *  @brief 設定單一port的方向
 *  @param data 設定輸出埠的數值
 *
 */

void TCA9555_SetDirection(uint16_t direction) {
    uint8_t config_cmd[3] = {
        0x06,  // 配置暫存器地址
        (uint8_t)(direction & 0xFF),
        (uint8_t)((direction >> 8) & 0xFF)
    };
    HAL_I2C_Master_Transmit(&hi2c1, TCA9555_ADDR, config_cmd, 3, 100);
}

//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝設定io方向＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝寫入＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

#ifdef with_BSP

HAL_StatusTypeDef TCA9555_WriteOutput(uint8_t P0_data,uint8_t P1_data) {

#ifdef use_TCA9548

	if(TCA9548_SelectByAddress(TCA9555_ADDR) != HAL_OK){
		printf("TCA9555_I/O expander not exist, or Not connected to TCA9548");
	}

#endif

    uint8_t output_cmd[3] = {
        0x02,  // 輸出暫存器地址（從 Port0 開始，會自動遞增到 Port1）
        P0_data,       // Port0 (低8位)
        P1_data // Port1 (高8位)
    };

    // 改用 BSP 抽象層，並回傳狀態
    return BSP_I2C_Write(TCA9555_ADDR, output_cmd, 3);
}

#else
void TCA9555_WriteOutput(uint16_t data) {
    uint8_t output_cmd[3] = {
        0x02,  // 輸出暫存器地址（Port0 和 Port1）
        (uint8_t)(data & 0xFF),     // Port0 (低8位)
        (uint8_t)((data >> 8) & 0xFF) // Port1 (高8位)
    };
    HAL_I2C_Master_Transmit(&hi2c1, TCA9555_ADDR, output_cmd, 3, 100);
}
#endif

// 追蹤目前 16 個 Pin 的輸出狀態（避免每次寫入都蓋掉其他 Pin）
static uint16_t g_tca9555_output_cache = 0xFFFF;

/**
 * @brief 設定單一指定的 Pin 輸出 High 或 Low
 * @param pin 0 ~ 15
 * @param level 1 = High, 0 = Low
 */

void TCA9555_WritePin(uint8_t pin, uint8_t level) {
    if (pin > 15) return;

    // 1. 更新快取中的指定 bit
    if (level) {
        g_tca9555_output_cache |= (1 << pin);   // 設為 1 (High)
    } else {
        g_tca9555_output_cache &= ~(1 << pin);  // 設為 0 (Low)
    }

    // 2. 拆成 Port 0 (低 8 位) 與 Port 1 (高 8 位) 寫入晶片
    uint8_t p0 = (uint8_t)(g_tca9555_output_cache & 0xFF);
    uint8_t p1 = (uint8_t)((g_tca9555_output_cache >> 8) & 0xFF);

    TCA9555_WriteOutput(p0, p1);
}

//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝寫入＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝讀取＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝


/**
 *  @brief 讀取輸入
 *  @param data 設定輸出埠的數值
 *
 */
uint16_t TCA9555_ReadInput()
 {
    uint8_t reg_addr = 0x00;  // 輸入暫存器地址（Port0）
    uint8_t input_data[2] = {0};

#ifdef use_TCA9548
    // 確保讀取前有切到正確的 I2C 多工器通道
    if (TCA9548_SelectByAddress(TCA9555_ADDR) != HAL_OK) {
        printf("TCA9555 not found on MUX during ReadInput\n");
        return 0;
    }
#endif

    // 使用你專案中統一的 BSP 記憶體讀取函式，正確讀取 2 個位元組 (Port 0 和 Port 1)
        if (BSP_I2C_MemRead(TCA9555_ADDR, reg_addr, input_data, 2) != HAL_OK) {
            printf("TCA9555 ReadInput Failed\n");
            return 0;
        }
    /*
    // 寫入要讀取的暫存器地址
    HAL_I2C_Master_Transmit(&hi2c1, TCA9555_ADDR, &reg_addr, 1, 100);
    // 讀取 2 個位元組（Port0 和 Port1）
    HAL_I2C_Master_Receive(&hi2c1, TCA9555_ADDR, input_data, 2, 100);*/

    //return (input_data[1] << 8) | input_data[0];

   // 回傳 16-bit 狀態 (Port 1 在高位，Port 0 在低位)
       return ((uint16_t)input_data[1] << 8) | input_data[0];
}


/**
 * @brief 讀取單一指定的 Pin 狀態
 * @param pin 0 ~ 15
 * @return 0 或 1 (代表 Low 或 High)
 */
uint8_t TCA9555_ReadPin(uint8_t pin) {
    if (pin > 15) return 0;

    // 呼叫你原本寫好的 16-bit 讀取函數
    uint16_t all_pins = TCA9555_ReadInput();

    // 用遮罩把指定的那一顆 Pin 抓出來，並移到最右邊 (0 或 1)
    return (all_pins >> pin) & 0x01;
}
//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝讀取＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝



//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝寫入測試＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void Test_TCA9555_LED(void) {

    						//暫存器, P0  ,  P1
    uint8_t output_high[3] = {0x02, 0xFF, 0xFF}; // 全部輸出 High
    uint8_t output_low[3]  = {0x02, 0x00, 0x00}; // 全部輸出 Low

    printf("輸出 HIGH... 量 P00 應該是 5V\n");
    BSP_I2C_Write(TCA9555_ADDR, output_high , 3);
    HAL_Delay(10000);


    printf("輸出 LOW... 量 P00 應該是 0V\n");
    BSP_I2C_Write(TCA9555_ADDR, output_low, 3);
    HAL_Delay(10000);

    // 4. 再輸出 High 10秒 (量電壓應該是 5V)
    printf("輸出 HIGH... 量 P00 應該是 5V\n");
    BSP_I2C_Write(TCA9555_ADDR, output_high, 3);
    HAL_Delay(10000);

}

//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝寫入測試＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

