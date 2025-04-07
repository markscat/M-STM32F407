/*
 * error_handler.h
 *
 *  Created on: Mar 16, 2025
 *      Author: user
 */

#ifndef INC_ERROR_HANDLER_H_
#define INC_ERROR_HANDLER_H_

#pragma once
#include "eeprom_handler.h"
#include "gpio.h"



/* 公開API */
I2C_Status I2C_Init_Devices(void);
void Handle_Error(EEPROM_Status status);
void Enter_Failsafe_Mode(void);
EEPROM_Status EEPROM_Init(void);
EEPROM_Status EEPROM_Write(uint16_t addr, const uint8_t* data, uint16_t size);


#endif /* INC_ERROR_HANDLER_H_ */
