/*
 * I2C_MCP4725.h
 *
 *  Created on: 2026年11月1日
 *      Author: Ethan
 */
#ifndef INC_I2C_MCP4725_H_
#define INC_I2C_MCP4725_H_

#include "stm32f4xx_hal.h"
#include "i2c.h"
#include "I2C_TCA9548.h"        // TCA9548 的 API


// MCP4725 預設 I2C 位址 (請依你模組上的 A0 接法確認，常見為 0x60 或 0x61)
#define MCP4725_I2C_ADDR    0x61

// MCP4725 指令
#define MCP4725_CMD_WRITE_DAC       0x40 // 寫入 DAC 暫存器 (不寫入 EEPROM)
#define MCP4725_CMD_WRITE_DAC_EEPROM 0x60 // 寫入 DAC 暫存器並存入 EEPROM (斷電記憶)

// 函式宣告
HAL_StatusTypeDef MCP4725_Init(void);
HAL_StatusTypeDef MCP4725_SetVoltage(float voltage, float vref);
HAL_StatusTypeDef MCP4725_SetRawValue(uint16_t value);
void mcp4725_sinWave_test(void);


#endif /* INC_I2C_MCP4725_H_ */
