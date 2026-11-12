/*
 * BPS_i2c.c
 *
 *	本程式為中間界面層,是為了要和底層的程式碼隔開,如此就算更換ＭＣＵ,只要改這邊就好
 *
 *
 *  Created on: 2026年11月12日
 *      Author: Ethan Yang
 */

#include "I2C_Device\BPS_i2c.h"
#include "i2c.h" // 拿 hi2c_main

HAL_StatusTypeDef BSP_I2C_Write(uint8_t dev_addr_7bit, const uint8_t *pData, uint16_t size) {

	//uint16_t final_addr = (dev_addr_7bit << 1);

    return HAL_I2C_Master_Transmit(hi2c_main, (dev_addr_7bit << 1), (uint8_t*)pData, size, 100);
}

HAL_StatusTypeDef BSP_I2C_IsReady(uint8_t dev_addr_7bit) {

    return HAL_I2C_IsDeviceReady(hi2c_main, (dev_addr_7bit << 1), 3, 100);
}


