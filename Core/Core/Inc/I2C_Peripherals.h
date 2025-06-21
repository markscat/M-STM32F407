/**
  ******************************************************************************
  * @file    eeprom_handler.h
  * @brief   AT24C32 EEPROM驱动模块
  * Created on: Mar 16, 2025
  * @aurhor Deepseek and Ethan
  ******************************************************************************
  */


#ifndef INC_I2C_PERIPHERALS_H_
#define INC_I2C_PERIPHERALS_H_

// eeprom_handler.h
#pragma once
#include "stm32f4xx_hal.h"       // 核心HAL库
#include "stm32f4xx_hal_i2c.h"   // I2C外设支持
#include "stdbool.h"
// 通过外部声明引用全局变量
extern I2C_HandleTypeDef hi2c1;  // I2C实例（在main.c中定义）

/* [共用 I2C 配置] 修改原 EEPROM 的 I2C 句柄为通用名称 */
// 原文件 eeprom_handler.h 中修改：
// extern I2C_HandleTypeDef* hi2c_eeprom; --> 修改为：
extern I2C_HandleTypeDef* hi2c_main;    // [MOD] 重命名为通用I2C句柄




// 专用配置宏（替代包含main.h）
/* 硬件配置 (根据实际电路修改) -------------------------------------------------*/
/**
 * @brief EEPROM位址
 * @def EEPROM_I2C_ADDR
 * EEPROM的位址由A0,A1,A2來決定
 *| A2 | A1 | A0 | 7-bit地址（二進制） | 16 進制 |
 *|--- |--- |--- | ------------------ |---------|
 *| 0  | 0  | 0  | `1010000`          |  0x50   |
 *| 0  | 0  | 1  | `1010001`          |  0x51   |
 *| 0  | 1  | 0  | `1010010`          |  0x52   |
 *| 0  | 1  | 1  | `1010011`          |  0x53   |
 *| 1  | 0  | 0  | `1010100`          |  0x54   |
 *| 1  | 0  | 1  | `1010101`          |  0x55   |
 *| 1  | 1  | 0  | `1010110`          |  0x56   |
 *| 1  | 1  | 1  | `1010111`          |  0x57   |
 *
 */
#define DS1307Backup

#define EEPROM_I2C_ADDR         0x50       // 设备7位地址
#define EEPROM_MAX_ADDR        	0x0FFF
#define I2C_HANDLE        		hi2c1      // 使用的I2C实例
#define EEPROM_PAGE_SIZE        32         // 设备页大小
#define EEPROM_PWR_CTRL_ENABLE  false      // 是否启用电源控制功能

#if EEPROM_PWR_CTRL_ENABLE
  #define EEPROM_PWR_GPIO        GPIOB
  #define EEPROM_PWR_PIN         GPIO_PIN_12
#endif


#define DS1307_I2C_ADDR    0x68        // DS1307 的 I2C 地址
#define DS1307_TIME_REG    0x00        // 时间寄存器起始地址
#define DS1307_CTRL_REG    0x07        // 控制寄存器地址

#ifdef DS1307Backup
#define DS1307_REG_SECOND 	0x00 	// 秒（BCD格式，bit7 = CH，1 表示停止振盪器）
#define DS1307_REG_MINUTE 	0x01	// 分（BCD格式）
#define DS1307_REG_HOUR  	0x02	// 時（BCD格式，支援12/24小時模式）
#define DS1307_REG_DOW    	0x03	// 星期（1=星期日 ~ 7=星期六）
#define DS1307_REG_DATE   	0x04	// 日（BCD格式）
#define DS1307_REG_MONTH  	0x05	// 月（BCD格式）
#define DS1307_REG_YEAR   	0x06	// 年（BCD格式，範圍 00~99）
#define DS1307_REG_CONTROL 	0x07	// 控制暫存器（可啟用方波輸出 SQW/OUT）

#define DS1307_REG_UTC_HR	0x08	// [自定義] UTC 小時偏移（例如 +8）
#define DS1307_REG_UTC_MIN	0x09	// [自定義] UTC 分鐘偏移（例如 +30）
#define DS1307_REG_CENT    	0x10	// [自定義] 世紀（例如 20 = 表示年份為 20xx）
#define DS1307_REG_RAM   	0x11	// [自定義] RAM 起始位置（DS1307有56 bytes SRAM）
#define DS1307_TIMEOUT		1000	// I2C timeout 時間（毫秒）
#endif

// 时间结构体定义（与DS1307寄存器对应）
typedef struct {
	uint16_t COMMAND_ID;
	uint8_t year;
    uint8_t month;
    uint8_t date;
    uint8_t hours;
    uint8_t minutes;
	uint8_t seconds;
    uint8_t day;
} DS1307_Time;


typedef struct {
	int year;
	int month;
	int day;
    int date;
	int hour;
	int minute;
	int second;
} DateAndTime;



/**
 *  @struct Point EEPROM_Status
 *  @brief I2C的錯誤碼
 * */

/* 错误代码定义 --------------------------------------------------------------*/
typedef enum {
  EEPROM_OK = 0,
  EEPROM_ERR_INIT,		//I2C initialization failed
  EEPROM_ERR_COMM,		//I2C communication failed
  EEPROM_ERR_CRC,		//I2C CRC failed
  EEPROM_ERR_ADDR,		//I2C Address failed
  EEPROM_ERR_POWER,  	//Power Control failed
  EEPROM_ERR_TIMEOUT,	//I2C communication Time out failed
  EEPROM_ERR_BUS_BUSY,	//I2C BUS BUSY
  EEPROM_ERR_Size		//over EEPROM Size
} EEPROM_Status;

typedef enum {
  I2C_OK = 0,
  I2C_ERR_INIT,		//I2C initialization failed
  I2C_ERR_COMM,		//I2C communication failed
  I2C_ERR_CRC,		//I2C CRC failed
  I2C_ERR_ADDR,		//I2C Address failed
  I2C_ERR_POWER,  	//Power Control failed
  I2C_ERR_TIMEOUT,	//I2C communication Time out failed
  I2C_ERR_BUS_BUSY,	//I2C BUS BUSY
  I2C_ERR_Size		//over EEPROM Size
} I2C_Status;



/* 健康状态结构体 ------------------------------------------------------------*/
typedef struct {
  uint32_t last_op_time;
  uint16_t write_count;
  uint16_t read_count;
  uint8_t  error_count;
  uint8_t  hw_status;
} EEPROM_Health;

/* 公开API -------------------------------------------------------------------*/
//EEPROM_Status EEPROM_Init(I2C_HandleTypeDef *hi2c);
//EEPROM_Status EEPROM_Write(uint16_t addr, uint8_t *data, uint16_t size);
//EEPROM_Status EEPROM_Read(uint16_t addr, uint8_t *data, uint16_t size);

uint16_t Compute_CRC(const uint8_t* data, uint16_t len);

//EEPROM
EEPROM_Status EEPROM_Init(void);
EEPROM_Status EEPROM_Write(uint16_t addr, const uint8_t* data, uint16_t size);
EEPROM_Status EEPROM_Read(uint16_t addr, uint8_t* data, uint16_t size);
void EEPROM_EraseAll(void);
void EEPROM_GetHealth(EEPROM_Health* health);
void EEPROM_ResetHealth(void);
bool EEPROM_IsReady(void);


//RTC DS1307
const char* I2C_Status_ToString(I2C_Status status);
HAL_StatusTypeDef DS1307_SetTime(DS1307_Time* time);
HAL_StatusTypeDef DS1307_GetTime(DS1307_Time* time);

/**
  * @brief 将十进制数转换为BCD格式
  * @param val 要转换的十进制数 (0-99)
  * @retval 对应的BCD编码值
  * @warning 输入超过99将产生无效结果
  */
uint8_t dec_to_bcd(uint8_t val);
uint8_t bcd_to_dec(uint8_t val);

DS1307_Time Parse_TimeString(const char *str);
uint8_t calculate_day_of_week(uint16_t year, uint8_t month, uint8_t day) ;


/*
#if EEPROM_PWR_CTRL_ENABLE
void EEPROM_PowerCycle(void);
#endif
*/
#endif /* INC_I2C_PERIPHERALS_H_ */
