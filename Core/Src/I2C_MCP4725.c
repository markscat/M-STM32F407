/*
 * I2C_MCP4725.C
 *
 *  Created on: 2026年11月1日
 *      Author: Ethan
 */


#include "I2C_MCP4725.h"

/**
 * @brief 初始化 MCP4725 (確認設備在位)
 */
	HAL_StatusTypeDef MCP4725_Init(void) {
		// 檢查設備是否在匯流排上
		if (HAL_I2C_IsDeviceReady(hi2c_main, MCP4725_I2C_ADDR << 1, 3, 100) != HAL_OK) {
			return HAL_ERROR;
		}

	    // 強制讓輸出歸零 (安全狀態)
	    // 這樣就算之前 EEPROM 記著 3.3V，現在也會被強制拉回 0V
	    MCP4725_SetRawValue(0);

		return HAL_OK;
	}

/**
 * @brief 直接寫入 12-bit 原始數值 (0 ~ 4095)
 * @param value 0~4095，對應 0V ~ VCC
 */
HAL_StatusTypeDef MCP4725_SetRawValue(uint16_t value) {
    if (value > 4095) value = 4095; // 限制在 12-bit 範圍內

    uint8_t buffer[3];
    // 第一 Byte：指令。0x40 代表「快速寫入 DAC，不寫入 EEPROM」
    // 如果要用斷電記憶，把 0x40 改成 0x60 即可
    buffer[0] = MCP4725_CMD_WRITE_DAC;

    // 第二 Byte：DAC 數值的高 8 位 (Bit 11 ~ Bit 4)
    buffer[1] = (value >> 4) & 0xFF;

    // 第三 Byte：DAC 數值的低 4 位 (Bit 3 ~ Bit 0)，放在最高的 4 個 bit
    buffer[2] = (value << 4) & 0xF0;

    // 發送資料 (MCP4725 不需要暫存器位址，直接寫入)
    return HAL_I2C_Master_Transmit(hi2c_main, MCP4725_I2C_ADDR << 1, buffer, 3, 100);
}

/**
 * @brief 根據目標電壓，計算並輸出對應的數值
 * @param voltage 目標電壓 (例如 1.5V)
 * @param vref    參考電壓 (MCP4725 的 VCC，例如 3.3V 或 5.0V)
 */
HAL_StatusTypeDef MCP4725_SetVoltage(float voltage, float vref) {
    // 防呆：電壓不能超過參考電壓
    if (voltage >= vref) {
        return MCP4725_SetRawValue(4095);
    }
    if (voltage <= 0.0f) {
        return MCP4725_SetRawValue(0);
    }

    // 計算公式： Value = (Voltage / Vref) * 4095
    uint16_t value = (uint16_t)((voltage / vref) * 4095.0f);

    return MCP4725_SetRawValue(value);
}
