
/*
 * TCA555.c
 * 
 * This program is mainly used to connect TCA555 and STM32F407; currently only the output has been tested, 
 *	and the input is to be tested.
 	
 *	Yes, that's right, this program was written by Deepseek, and I tested it.
 *	I'm a hardware engineer, not a software engineer, so what?
 *
 *	本程式主要為TCA555 和STM32F407連結用的;目前測試到的只有輸出,輸入待測試
 *  對,沒錯,這個程式是Deepseek寫，我來測試的。
 *  我是硬體工程師，又不是軟體，那又怎樣？


 *  Created on: Feb 28, 2025
 *      Author: user
 */



void TCA9555_Init() {
    // 檢查 I2C 是否就緒
    if (HAL_I2C_IsDeviceReady(&hi2c1, TCA9555_ADDR, 3, 100) != HAL_OK) {
        Error_Handler();  // 設備未響應
    }

    // 配置 TCA9555 的 16 個 GPIO 為輸出模式（默認全部輸入）
    uint8_t config_cmd[3] = {0x06, 0x00, 0x00}; // 0x06 是配置暫存器地址
    if (HAL_I2C_Master_Transmit(&hi2c1, TCA9555_ADDR, config_cmd, 3, 100) != HAL_OK) {
        Error_Handler();
    }
}

void TCA9555_WriteOutput(uint16_t data) {
    uint8_t output_cmd[3] = {
        0x02,  // 輸出暫存器地址（Port0 和 Port1）
        (uint8_t)(data & 0xFF),     // Port0 (低8位)
        (uint8_t)((data >> 8) & 0xFF) // Port1 (高8位)
    };
    HAL_I2C_Master_Transmit(&hi2c1, TCA9555_ADDR, output_cmd, 3, 100);
}

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

void TCA9555_SetDirection(uint16_t direction) {
    uint8_t config_cmd[3] = {
        0x06,  // 配置暫存器地址
        (uint8_t)(direction & 0xFF),
        (uint8_t)((direction >> 8) & 0xFF)
    };
    HAL_I2C_Master_Transmit(&hi2c1, TCA9555_ADDR, config_cmd, 3, 100);
}
