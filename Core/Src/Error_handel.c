/**
 *   @file EEPROM Error_handel.c
 *   @brief EEPROM 錯誤處理 ：
 *
 *  @date Mar 16, 2025
 *  @author  Deepseek and Ethan
 *
 *
 *
 * <br/>
 * GNU GENERAL PUBLIC LICENSE<br/>
 * Version 3, 29 June 2007<br/>
 * Copyright (C) [2025] [Ethan]<br/>
 * 本程式是一個自由軟體：您可以依照 **GNU 通用公共授權條款（GPL）** 發佈和/或修改，<br/>
 * GPL 版本 3 或（依您選擇）任何更新版本。<br/>
 * 本程式的發佈目的是希望它對您有幫助，但 **不提供任何擔保**，甚至不包含適銷性或特定用途適用性的默示擔保。<br/>
 * 請參閱 **GNU 通用公共授權條款** 以獲取更多詳細資訊。<br/>
 * 您應當已經收到一份 **GNU 通用公共授權條款** 副本。<br/>
 * 如果沒有，請參閱 <https://www.gnu.org/licenses/gpl-3.0.html>。<br/><br/>
 *
 * <br/>
 *     This program is free software: you can redistribute it and/or modify<br/>
 *     it under the terms of the GNU General Public License as published by<br/>
 *     the Free Software Foundation, either version 3 of the License, or<br/>
 *     any later version.<br/>
 *     <br/>
 *     This program is distributed in the hope that it will be useful,<br/>
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of<br/>
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the<br/>
 *     GNU General Public License for more details.<br/>
 *
 *     You should have received a copy of the GNU General Public License<br/>
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.<br/>
 *
 *
 */

#include "error_handler.h"
#include "data_recovery.h"
#include <stdio.h>
#include "string.h"
extern UART_HandleTypeDef huart2;  // 用於串口日誌


/**
  * @brief 綜合錯誤處理入口
  * @param status: 從EEPROM操作返回的錯誤碼
  */

void Handle_Error(EEPROM_Status status) {
  const char *errors[] = {
    "Operation successful",
    "Initialization failed",
    "Communication error",
    "CRC mismatch",
    "Invalid address"
  };

  char msg[60];
  snprintf(msg, sizeof(msg), "[EEPROM Error] %s\r\n", errors[status]);
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);

  if(status == EEPROM_ERR_CRC) {
    Enter_Failsafe_Mode();
  }
}


void Enter_Failsafe_Mode(void) {
	HAL_GPIO_TogglePin(LD4_GPIO_Port,LD4_Pin);
	HAL_Delay(0xFF);
  while(1) {
    HAL_UART_Transmit(&huart2, (uint8_t*)"In Failsafe Mode!\r\n", 19, 100);
    HAL_Delay(1000);
  }
}


