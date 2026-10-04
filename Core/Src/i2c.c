/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
  *          本程式主要是用來初始化I2C的設備,
  * ＠note
  * 在 STM32Cube 的架構裡：
  * +-----------+-------------------------------+----------------------------+
  * | 層級		|				名稱				|			負責				 |
  * +-----------+-------------------------------+----------------------------+
  * | 應用層		| main.c						| 你的邏輯					 |
  * +-----------+-------------------------------+----------------------------+
  * | HAL 層		| stm32f4xx_hal_i2c.c			| I2C 周邊的通用邏輯			 |
  * +-----------+-------------------------------+----------------------------+
  * | MSP 層		| i2c.c 裡的 HAL_I2C_MspInit		| 這顆 MCU 的腳位、時鐘、中斷	 |
  * +-----------+-------------------------------+----------------------------+
  * | CMSIS 層	| stm32f4xx.h					| 暫存器定義					 |
  * +-----------+-------------------------------+----------------------------+
  *
  *
  * @brief HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
  * HAL 層：跟「周邊」有關
  *
  * I2C 的時脈要多少？100kHz 還是 400kHz？
  * 7-bit 還是 10-bit 定址？
  * 要不要 Dual Address？
  * 要不要 No Stretch？
  *
  * @brief MX_I2C1_Init()
 *
 *  MSP 層：跟「MCU 腳位 / 時鐘 / 中斷」有關
 *  MSP = MCU Support Package（微控制器支援包）。
 *
 *  I2C1 要用哪兩根腳？PB6/PB7 還是 PB8/PB9？
 *  這兩根腳要用哪個 AF？AF4 還是 AF9？
 *  要不要開 GPIOB 時鐘？
 *  要不要開 I2C1 時鐘？
 *  要不要開中斷？優先級多少？
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/
 *  */

void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;						//頻率
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;				//佔空比
  hi2c1.Init.OwnAddress1 = 0;							//
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;	//
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;	//
  hi2c1.Init.OwnAddress2 = 0;							//
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;	//
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;		//
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}


void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */
	  /** @brief  I2C1 GPIO Configuration
	   * PB6     ------> I2C1_SCL
	   * PB7     ------> I2C1_SDA
    */
  /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();

    /* I2C1 interrupt Init */
    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

    /* I2C1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
