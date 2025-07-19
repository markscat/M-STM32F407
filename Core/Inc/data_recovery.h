/*
 * data_recovery.h
 *
 *  Created on: Mar 16, 2025
 *      Author: user
 */

#ifndef INC_DATA_RECOVERY_H_
#define INC_DATA_RECOVERY_H_

#pragma once
#include <stdbool.h>
#include <I2C_Peripherals.h>

bool Handle_Data_Corruption(uint16_t addr, uint8_t* data, uint16_t size);
//static void Update_All_Backups(uint16_t main_addr, uint8_t* data, uint16_t size);

//bool Handle_Data_Corruption(uint16_t addr, uint8_t *data, uint16_t size);

#endif /* INC_DATA_RECOVERY_H_ */
