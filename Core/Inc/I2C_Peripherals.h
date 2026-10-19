/**
  ******************************************************************************
  * @file    I2C_TCA9548.h
  * @brief   TCA9548 I2C多工模塊
  * Created on: 2026/10/13
  * @aurhor Deepseek and Ethan
  * 這個檔案只放相關的函式庫,至於巨集和相關參數都放在I2C_parameter.h
  ******************************************************************************
  */

#ifndef INC_I2C_PERIPHERALS_H_
#define INC_I2C_PERIPHERALS_H_

#include "stm32f4xx_hal.h"       // 核心HAL库
#include "stm32f4xx_hal_i2c.h"   // I2C外设支持
#include "stdbool.h"
#include "i2c.h"
#include "I2C_parameter.h"

uint16_t Compute_CRC(const uint8_t* data, uint16_t len);

//EEPROM
EEPROM_Status EEPROM_Init(void);
EEPROM_Status EEPROM_Write(uint16_t addr, const uint8_t* data, uint16_t size);
EEPROM_Status EEPROM_Read(uint16_t addr, uint8_t* data, uint16_t size);
//無CRC版本
EEPROM_Status EEPROM_Write_NoCRC (uint16_t addr, uint8_t* data, uint16_t size);
EEPROM_Status EEPROM_Read_NoCRC (uint16_t addr, uint8_t* data, uint16_t size);

void EEPROM_EraseAll(void);
void EEPROM_GetHealth(EEPROM_Health* health);
void EEPROM_ResetHealth(void);
bool EEPROM_IsReady(void);


//RTC
const char* I2C_Status_ToString(I2C_Status status);
//HAL_StatusTypeDef RTC_setTime(RTC_Time* time) ;
HAL_StatusTypeDef RTC_SetTime(RTC_Time* time);
HAL_StatusTypeDef RTC_GetTime(RTC_Time* time);
HAL_StatusTypeDef DS3231_GetTemp(DS3231_Temp* temp);



/**
  * @brief 将十进制数转换为BCD格式
  * @param val 要转换的十进制数 (0-99)
  * @retval 对应的BCD编码值
  * @warning 输入超过99将产生无效结果
  */
uint8_t dec_to_bcd(uint8_t val);
uint8_t bcd_to_dec(uint8_t val);

RTC_Time Parse_TimeString(const char *str);
//uint8_t calculate_day_of_week(uint16_t year, uint8_t month, uint8_t day) ;


/*
#if EEPROM_PWR_CTRL_ENABLE
void EEPROM_PowerCycle(void);
#endif
*/
#endif /* INC_I2C_PERIPHERALS_H_ */
