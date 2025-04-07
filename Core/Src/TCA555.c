
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
 *
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
#include "TCA555.h"
#include "i2c.h"

//extern I2C_HandleTypeDef hi2c1;

/**
 *  @brief 初始化TCA9555
 *  1. 確認設備是否就緒
 *  2. 配置 TCA9555 的 16 個 GPIO 為輸出模式（默認全部輸入）
 */
void TCA9555_Init() {
    //** 檢查 I2C 是否就緒*/
    if (HAL_I2C_IsDeviceReady(&hi2c1, TCA9555_ADDR, 3, 100) != HAL_OK) {
        Error_Handler();  /** 設備未響應*/
    }

    // 配置 TCA9555 的 16 個 GPIO 為輸出模式（默認全部輸入）
    uint8_t config_cmd[3] = {0x06, 0x00, 0x00}; // 0x06 是配置暫存器地址
    if (HAL_I2C_Master_Transmit(&hi2c1, TCA9555_ADDR, config_cmd, 3, 100) != HAL_OK) {
        Error_Handler();
    }
}

/**
 *  @brief 寫入輸出
 *  @param data 設定輸出埠的數值
 *
 */
void TCA9555_WriteOutput(uint16_t data) {
    uint8_t output_cmd[3] = {
        0x02,  // 輸出暫存器地址（Port0 和 Port1）
        (uint8_t)(data & 0xFF),     // Port0 (低8位)
        (uint8_t)((data >> 8) & 0xFF) // Port1 (高8位)
    };
    HAL_I2C_Master_Transmit(&hi2c1, TCA9555_ADDR, output_cmd, 3, 100);
}

/**
 *  @brief 讀取輸入
 *  @param data 設定輸出埠的數值
 *
 */
uint16_t TCA9555_ReadInput()
 {
    uint8_t reg_addr = 0x00;  // 輸入暫存器地址（Port0）
    uint8_t input_data[2] = {0};

    // 寫入要讀取的暫存器地址
    HAL_I2C_Master_Transmit(&hi2c1, TCA9555_ADDR, &reg_addr, 1, 100);
    // 讀取 2 個位元組（Port0 和 Port1）
    HAL_I2C_Master_Receive(&hi2c1, TCA9555_ADDR, input_data, 2, 100);

    return (input_data[1] << 8) | input_data[0];
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
