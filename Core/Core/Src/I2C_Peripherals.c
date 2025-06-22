/*
 *   ******************************************************************************
 * @file eeprom_handler.c
 * @brief AT2432 讀寫程式：
 *
 *	- AT24C32是32Kbit的EEPROM，也就是4KByte，分页的话，每页32字节<br/>
 *
 *	私有函數結構：（公開函數宣告在eeprom_handler.h）<br/>
 *	EEPROM位置驗證<br/>
 *	static EEPROM_Status Validate_Address(uint16_t addr, uint16_t size);<br/>
 *	內部寫入：<br/>
 *	static EEPROM_Status Internal_Write(uint16_t addr, const uint8_t* data, uint16_t size);<br/>
 *	內部讀取<br/>
 *	static EEPROM_Status Internal_Read(uint16_t addr, uint8_t* data, uint16_t size);<br/>
 *	更新健康<br/>
 *	static void Update_Health(bool success);<br/>
 *	硬體重置<br/>
 *	static void Hardware_Reset(void);<br/>
 *
 *
 *  @date Mar 16, 2025<br/>
 *  @author  Deepseek and Ethan<br/>
 *
 *    ******************************************************************************
 */



#include <I2C_Peripherals.h>
#include <string.h>
#include "stdlib.h"
#include "stdbool.h"
#include "stdio.h"
#include "Tools.h"

#define EEPROM_SIZE 4096  // AT24C32容量
#define MAX_READ_LEN 32   // 推荐单次读取长度
#define MAX_EEPROM_WRITE_SIZE 32

#define DS1307_Driver

/* 私有变量 ------------------------------------------------------------------*/
static EEPROM_Status Validate_Address(uint16_t addr, uint16_t size);
static EEPROM_Status Internal_Write(uint16_t addr, const uint8_t* data, uint16_t size);
static EEPROM_Status Internal_Read(uint16_t addr, uint8_t* data, uint16_t size);
static void Update_Health(bool success);
static void Hardware_Reset(void);

static I2C_HandleTypeDef* hi2c_eeprom = &I2C_HANDLE;
static EEPROM_Health health_monitor = {0};
//static const uint16_t crc_poly = 0x1021;  // CRC-16-CCITTInternal_Write

/* [修改] 原EEPROM代码适配共用I2C ----------------------------------------------*/
// 原文件 eeprom_handler.c 中修改：
 //static I2C_HandleTypeDef* hi2c_eeprom = &EEPROM_I2C_HANDLE; //--> 修改为：
I2C_HandleTypeDef* hi2c_main = &I2C_HANDLE;  // [MOD] 统一I2C句柄名称


/* 私有函数原型 --------------------------------------------------------------*/


uint16_t received_crc;         		// 儲存從緩衝區提取的 CRC 值
uint16_t computed_crc;          	// 儲存即時計算的 CRC 值
extern uint8_t* buffer;         	// 外部宣告的數據緩衝區指標（需在其他檔案定義）
extern uint16_t size;           	// 外部宣告的數據長度（需在其他檔案定義）
static uint32_t error_counter = 0; 	// 錯誤計數器（僅當前檔案可見）
#define MAX_RETRIES 3           	// 最大容許錯誤次數
#define EEPROM_PAGE_SIZE 32   		// 定义页大小



//#define init_I2C_Driver
//#define DS1307_Driver
#define test2





/*====================初始化設備====================*/
/* 设备初始化配置结构体 */
typedef struct {
  uint8_t dev_addr;               // 设备I2C地址（7位格式）
  I2C_Status (*init_fn)(void);    // 设备专属初始化函数指针
  const char* dev_name;           // 设备名称（用于调试信息）
} I2C_DeviceConfig;

/*----------------------------- 设备初始化函数 -----------------------------*/
/**
  * @brief  EEPROM (AT24C32) 初始化
  * @retval I2C_Status 错误码
  */
static I2C_Status EEPROM_DeviceInit(void) {
  // 检测设备是否存在
  if (HAL_I2C_IsDeviceReady(&I2C_HANDLE, EEPROM_I2C_ADDR << 1, 3, 100) != HAL_OK) {
    return I2C_ERR_INIT;
  }
  // 可在此添加EEPROM特定初始化（例如写入默认配置）
  EEPROM_ResetHealth();
  return I2C_OK;
}

/**
  * @brief  DS1307 实时时钟初始化
  * @retval I2C_Status 错误码
  */
static I2C_Status DS1307_DeviceInit(void) {
  // 检测设备是否存在
  if (HAL_I2C_IsDeviceReady(&I2C_HANDLE, DS1307_I2C_ADDR << 1, 3, 100) != HAL_OK) {
    return I2C_ERR_INIT;
  }
  // 检查并启动时钟振荡器（清除CH位）
  uint8_t sec_reg;
  if (HAL_I2C_Mem_Read(&I2C_HANDLE, DS1307_I2C_ADDR << 1, DS1307_TIME_REG,
                      I2C_MEMADD_SIZE_8BIT, &sec_reg, 1, 100) != HAL_OK) {
    return I2C_ERR_COMM;
  }

  if (sec_reg & 0x80) {  // 如果时钟停止
    sec_reg &= ~0x80;    // 清除停止位
    if (HAL_I2C_Mem_Write(&I2C_HANDLE, DS1307_I2C_ADDR << 1, DS1307_TIME_REG,
                         I2C_MEMADD_SIZE_8BIT, &sec_reg, 1, 100) != HAL_OK) {
      return I2C_ERR_COMM;
    }
  }
  return I2C_OK;
}

/*----------------------------- 核心初始化函数 -----------------------------*/
/**
  * @brief  统一I2C设备初始化入口
  * @retval I2C_Status 错误码
  *
  */
I2C_Status I2C_Init_Devices(void) {
  // 设备配置表（按需扩展）
	/**
	 *
	 * 模块化设备管理
	 * 设备配置表 (dev_config[])：通过结构体数组定义连接的设备，包含：
	 * 	dev_addr: 设备的7位I2C地址
	 * 	init_fn: 设备专属初始化函数指针
	 * 	dev_name: 设备名称（用于调试）
	 * 	扩展性：未来新增设备只需在表中添加条目，例如添加温度传感器：
	 * <code>
	 * {
	 * .dev_addr  = 0x48,  // 假设为LM75温度传感器
	 * .init_fn   = LM75_DeviceInit,
	 * .dev_name  = "LM75 Temperature Sensor"
	 * }</code>
  *
	 * */
  static const I2C_DeviceConfig dev_config[] = {
    // EEPROM 设备
    {
      .dev_addr  = EEPROM_I2C_ADDR,  // 7位地址
      .init_fn   = EEPROM_DeviceInit,
      .dev_name  = "AT24C32 EEPROM"
    },
    // DS1307 设备
    {
      .dev_addr  = DS1307_I2C_ADDR,  // 7位地址
      .init_fn   = DS1307_DeviceInit,
      .dev_name  = "DS1307 RTC"
    },
    // 可在此添加更多设备...
  };
  //end I2C_DeviceConfig dev_config[]

  /**
   * 初始化流程分层
   * graph TD
   *  A[I2C_Init_Devices] --> B[初始化I2C硬件]
   *  B --> C{遍历设备表}
   *  C --> D[初始化EEPROM]
   *  C --> E[初始化DS1307]
   *  C --> F[...其他设备...]
   *  D --> G{成功?}
   *  G -->|是| H[继续下一个设备]
   *  G -->|否| I[返回错误]
   * */

  // 步骤1: 初始化I2C硬件总线
  if (HAL_I2C_Init(&I2C_HANDLE) != HAL_OK) {
    return I2C_ERR_INIT;
  }

  // 步骤2: 遍历所有设备进行初始化
  for (uint8_t i = 0; i < sizeof(dev_config)/sizeof(dev_config[0]); i++) {
    I2C_Status status = dev_config[i].init_fn();
    if (status != I2C_OK) {
      // 输出调试信息（需实现printf）
      printf("[I2C Init] Device %s initialization failed! Error: %d\n",
            dev_config[i].dev_name, status);
      return status; // 严格模式：遇到错误立即返回
    }
    printf("[I2C Init] %s initialized successfully.\n", dev_config[i].dev_name);
  }

  return I2C_OK;
}


const char* I2C_Status_ToString(I2C_Status status) {
  switch(status) {
    case I2C_OK:          	return "Success";
    case I2C_ERR_INIT:    	return "I2C hardware initialization failed";
    case I2C_ERR_COMM:    	return "Communication error";
    case I2C_ERR_CRC:	  	return"I2C CRC failed";
    case I2C_ERR_ADDR:	  	return "I2C Address failed";
    case I2C_ERR_TIMEOUT:	return "I2C communication Time out failed";
    case I2C_ERR_BUS_BUSY:	return "I2C BUS BUSY";
    case I2C_ERR_Size:		return "over EEPROM Size";
    // ...其他错误码描述
    default:              return "Unknown error";
  }
}

/*====================初始化段結束====================*/

/*====================RTC段====================*/


/* [新增] DS1307 时间写入函数 ---------------------------------------------------*/
/**
  * @brief 设置DS1307时间
  * @param time: 时间结构体指针
  * @retval HAL状态
  */
HAL_StatusTypeDef DS1307_SetTime(DS1307_Time* time) {
    uint8_t buffer[7];

    // 转换十进制到BCD格式
    time->COMMAND_ID = 1;
    buffer[0] = dec_to_bcd(time->seconds);//秒
    buffer[1] = dec_to_bcd(time->minutes);//分
    buffer[2] = dec_to_bcd(time->hours);//時
    buffer[3] = dec_to_bcd(time->day);//星期
    buffer[4] = dec_to_bcd(time->date);//日期
    buffer[5] = dec_to_bcd(time->month);//月
    buffer[6] = dec_to_bcd(time->year);//年

    // 写入时间寄存器
    return HAL_I2C_Mem_Write(hi2c_main, DS1307_I2C_ADDR << 1, DS1307_TIME_REG,
                            I2C_MEMADD_SIZE_8BIT, buffer, 7, 100);
}

/* [新增] DS1307 时间读取函数 ---------------------------------------------------*/
/**
  * @brief 读取DS1307时间
  * @param time: 时间结构体指针
  * @retval HAL状态
  */
HAL_StatusTypeDef DS1307_GetTime(DS1307_Time* time) {
    uint8_t buffer[7];
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Read(hi2c_main, DS1307_I2C_ADDR << 1, DS1307_TIME_REG,
                             I2C_MEMADD_SIZE_8BIT, buffer, 7, 100);
    if(status != HAL_OK) return status;

    //這個旗標代表這是RTC的旗標；在『從DS1307讀取資料』這個狀態之下，沒有任何意義
    //這個旗標主要是用來判斷當上位機用各種不同的連線方式,下達命令的時候,可以有一個旗標可以用來
    //說明『這次是用來做RTC校正時間的命令』;為了結構上的完整,所以才寫下這一行
    //time->COMMAND_ID = 1;

    // 转换BCD到十进制并处理停止位
    time->seconds = bcd_to_dec(buffer[0] & 0x7F); // 忽略停止位
    time->minutes = bcd_to_dec(buffer[1]);
    time->hours   = bcd_to_dec(buffer[2] & 0x3F); // 24小时模式
    time->day     = bcd_to_dec(buffer[3]);
    time->date    = bcd_to_dec(buffer[4]);
    time->month   = bcd_to_dec(buffer[5]);
    time->year    = bcd_to_dec(buffer[6]);

    return HAL_OK;
}


DS1307_Time Parse_TimeString(const char *str) {
    DS1307_Time DT = {0};

    // 示例字符串格式："1,2023,12,31,23,59,30"
    //int year, month, day, hour, minute, second;
    // 如果傳入的字串是空的，直接返回一個空的結構體，防止崩潰

    if (str == NULL || *str == '\0') {
        return DT;

	    }

    char *copy = strdup(str);  // 修改3：避免修改原始字串


    if (copy == NULL) {
        // 內存分配失敗
        return DT; // 內存分配失敗也返回空結構體
    }

    char *token = NULL;


    // 1. 解析 COMMAND_ID
    token = strtok(copy, ",");
    if (token == NULL) goto cleanup; // 如果第一個 token 就沒有，直接跳轉
    DT.COMMAND_ID = atoi(token);

    // 2. 解析 Year
    token = strtok(NULL, ","); // 【修正點】在這裡獲取下一個 token
    if (token == NULL) goto cleanup;
    DT.year = atoi(token) % 100;

    // 3. 解析 Month
    token = strtok(NULL, ",");
    if (token == NULL) goto cleanup;
    DT.month = atoi(token);

    //日
    token = strtok(NULL, ",");
    if((token = strtok(NULL, ",")) == NULL) goto cleanup;
    DT.date = atoi(token);  // 修正錯誤

    //時
    token = strtok(NULL, ",");
    if((token = strtok(NULL, ",")) == NULL) goto cleanup;
    DT.hours = atoi(token);

    //分
    token = strtok(NULL, ",");
    if((token = strtok(NULL, ",")) == NULL) goto cleanup;
    DT.minutes = atoi(token);

    //秒
    token = strtok(NULL, ",");
    if((token = strtok(NULL, ",")) == NULL) goto cleanup;
    DT.seconds = atoi(token);

    //星期
    token = strtok(NULL, ",");
    if((token = strtok(NULL, ",")) == NULL) goto cleanup;
    DT.day = atoi(token);

cleanup:
    free(copy);
    return DT;  // 修改4：回傳結構體
}
/* ====================RTC 函數結束=================*/

/* ====================[共用工具函数] BCD/十进制转换=================*/
// [MOD] 新增以下两个转换函数
uint8_t dec_to_bcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}
uint8_t bcd_to_dec(uint8_t val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}


/*====================EEPROM段====================*/

/**
  * @brief 初始化EEPROM模块
  * @retval 操作状态
  */
EEPROM_Status EEPROM_Init(void) {
  /* 电源控制初始化 */
#if EEPROM_PWR_CTRL_ENABLE
  HAL_GPIO_WritePin(EEPROM_PWR_GPIO, EEPROM_PWR_PIN, GPIO_PIN_SET);
  HAL_Delay(100);  // 等待电源稳定
#endif

  /* 设备就绪检查 */

  //if(HAL_I2C_IsDeviceReady(hi2c_eeprom, EEPROM_I2C_ADDR << 1, 3, 100) != HAL_OK) {
   // 原代码使用 hi2c_eeprom，现在统一改为 hi2c_main

  if(HAL_I2C_IsDeviceReady(hi2c_main, EEPROM_I2C_ADDR << 1, 3, 100) != HAL_OK) {
	  printf("I2C initialization failed \n");
    return EEPROM_ERR_INIT;
  }

  EEPROM_ResetHealth();
  	  printf("I2C Device is ready \n");
  return EEPROM_OK;
}

/**
  * @brief 写入数据到EEPROM
  * @param addr: 起始地址 (0x0000-0x0FFF)
  * @param data: 数据指针
  * @param size: 数据长度
  * @retval 操作状态
  */
EEPROM_Status EEPROM_Write(uint16_t addr, const uint8_t* data, uint16_t size) {
  /* 地址校验 */
  EEPROM_Status addr_status = Validate_Address(addr, size);
  if(addr_status != EEPROM_OK) {
    return addr_status;
  }else{
	  printf("I2C address communication pass\n");
  }

  /* 添加CRC校验码 */
  uint16_t crc = Compute_CRC(data, size); // <-- 计算 CRC
  printf("\nw_crc = %d \n",crc);

  uint8_t* buffer = (uint8_t*)malloc(size + 2); // <-- 分配空间：数据 + 2字节CRC


  if(!buffer) {
	  printf("I2C Write communication failed\n");
	  return EEPROM_ERR_COMM;
  }else{
	  printf("I2C Write communication pass\n");
  }

  memcpy(buffer, data, size); // 拷贝原始数据
  memcpy(buffer + size, &crc, 2); // 附加 CRC 到末尾


  /* 执行带错误恢复的写入 */

  EEPROM_Status status = Internal_Write(addr, buffer, size + 2);

  free(buffer);
   Update_Health(status == EEPROM_OK);

  return status;
}

/**
 * EEPROM_Read(uint16_t addr, uint8_t* data, uint16_t size)
 *
  * @brief 从EEPROM读取数据
  * @param addr: 起始地址
  * @param data: 数据缓存指针
  * @param size: 读取数据长度
  * @retval 操作状态
  */
EEPROM_Status EEPROM_Read (uint16_t addr, uint8_t* data, uint16_t size) {
  /* 地址校验 */
  EEPROM_Status addr_status = Validate_Address(addr, size);
  if(addr_status != EEPROM_OK) {
	  printf("Validate_Address pass");
    return addr_status;
  }

  /* 读取数据+CRC（嚴格限制長度） */
  uint8_t* buffer = (uint8_t*)malloc(size + 2);
  if(!buffer) {
    printf("Memory allocation failed\n");
    return EEPROM_ERR_COMM;
  }

  // 只讀取需要的部分（數據 + CRC）
  EEPROM_Status status = Internal_Read(addr, buffer, size + 2);

  if(status != EEPROM_OK) {
    free(buffer);
    return status;
  }
#define EEPROM_Read_debug

#ifdef EEPROM_Read_debug
  /* ========== 新增：打印原始數據 ==========*/
  printf("\n\n");
  printf("In EEPROM_Read......\n");
  printf("Raw Data from EEPROM (%d bytes):\n", size);
  for(int i = 0; i < size ; i++) {
    printf("%02X ", buffer[i]);
    if((i+1) % 16 == 0) printf("\n");  // 每16字節換行
  }
  printf("\n\n");
  printf("buffer = %s ",buffer);

  printf("\n\n");
#endif
   /* ========== 新增部分結束 ========== */

  /** CRC校验
   *
   * 從 buffer 中提取接收到的 CRC 值
   * buffer + size 指向數據結尾後的位置（CRC 存儲位置）
   * (uint16_t*) 強制轉換為 16-bit 指標，獲取兩個字節的 CRC 值
   *
   *  */
  /* CRC 驗證區塊 */
  // 安全複製 CRC 值（避免記憶體對齊問題）

  /* 提取 CRC（大端序） */
  uint16_t computed_crc = Compute_CRC(buffer, size);
  uint16_t received_crc = (buffer[size] << 8) | buffer[size - 1];  // 高位在前


  uint8_t test_data[] = "Hello"; // 简单数据
  uint16_t crc = Compute_CRC(test_data, (strlen((char*)test_data)-1));
  printf("CRC of 'Hello': 0x%04X\n", crc); // 预期: 0x34A2 (CCITT)
  printf("\n\n");


  printf("\n computed_crc......\n");
  printf("\n Computed CRC: 0x%04X \n Received CRC: 0x%04X\n", computed_crc, received_crc);
  printf("\n\n");


  /* 錯誤處理區塊 */
  if (computed_crc != received_crc) {
      // 輸出詳細錯誤資訊
      printf("[ERROR] CRC Mismatch! Computed: 0x%04X, Received: 0x%04X \n",
             computed_crc, received_crc);

      // 安全釋放緩衝區（需確保 buffer 是動態分配）
      if (buffer != NULL) {
          free(buffer);
          buffer = NULL;  // 避免懸空指標
      }

      // 錯誤次數累加與系統復位檢查
      error_counter++;
      if (error_counter > MAX_RETRIES) {
          printf("Fatal CRC Error! Triggering System Reset...\n");
          NVIC_SystemReset();  // 需確認已包含 CMSIS 或 HAL 庫
      }

      return EEPROM_ERR_CRC;  // 假設 EEPROM_ERR_CRC 已定義為錯誤碼
  }


  memcpy(data, buffer, size);
  free(buffer);

  Update_Health(true);
  return EEPROM_OK;
}

/* 私有函数实现 --------------------------------------------------------------*/

/**
  * @brief 计算CRC-16校验码
  */
uint16_t Compute_CRC(const uint8_t* data, uint16_t len) {
  uint16_t crc = 0xFFFF;
  uint16_t poly = 0x1021; // CCITT 多項式（確認是否與 EEPROM 一致）
  printf("data = ");
  for(uint16_t k=0; k<len; k++) {
	  printf("0x%X, ",(uint16_t)data[k]);
  }
  printf("\n\n");
  for(uint16_t i=0; i<len; i++) {
    //crc ^= (uint16_t)data[i] << 8; // 当前字节移至高8位
      crc ^= data[i];
    for(uint8_t j=0; j<8; j++) {
      crc = (crc & 0x0001) ? (crc >> 1) ^ poly : (crc >> 1);
    }
  }
  printf("Final CRC: 0x%04X\n", crc); // 仅打印最终结果
  return crc;
}

/**
  * @brief 带自动重试的底层写入
  */
static EEPROM_Status Internal_Write(uint16_t addr, const uint8_t* data, uint16_t size) {
  const uint8_t max_retries = 3;

  for(uint8_t retry=0; retry<max_retries; retry++) {
    HAL_StatusTypeDef hal_status = HAL_I2C_Mem_Write(hi2c_eeprom,EEPROM_I2C_ADDR << 1,addr,I2C_MEMADD_SIZE_16BIT,(uint8_t*)data,size,100);
    if(hal_status == HAL_OK) {
   	 HAL_Delay(5);
      /* 验证写入完成 */
      if(HAL_I2C_IsDeviceReady(hi2c_eeprom, EEPROM_I2C_ADDR << 1, 100, 100) == HAL_OK) {
    	  printf("EEPROM_OK...... \n");
        return EEPROM_OK;
      }
    }

    /* 错误恢复策略 */
    if(retry == 1) Hardware_Reset();
    HAL_Delay((retry+1)*10);
  }
  return EEPROM_ERR_COMM;
}

/**
  * @brief 带自动重试的底层读取
  */
static EEPROM_Status Internal_Read(uint16_t addr, uint8_t* data, uint16_t size) {

  const uint8_t max_retries = 3;

  for(uint8_t retry=0; retry<max_retries; retry++) {
    HAL_StatusTypeDef hal_status = HAL_I2C_Mem_Read(hi2c_eeprom,EEPROM_I2C_ADDR << 1,addr,I2C_MEMADD_SIZE_16BIT,data,size,100);
    if(hal_status == HAL_OK) {
    	printf("\n\n in Internal_Read...... \n EEPROM_Read ok ");
      return EEPROM_OK;
    }
    /* 错误恢复策略 */
    if(retry == 1) {Hardware_Reset();}
    HAL_Delay((retry+1)*10);
  }
  printf("\n\n in retry over 3 time's...... \n EEPROM_ERR_COMM");
  return EEPROM_ERR_COMM;
}

/**
  * @brief 硬件复位操作
  */
static void Hardware_Reset(void) {
  /* I2C总线复位 */
  HAL_I2C_DeInit(hi2c_eeprom);
  HAL_Delay(10);
  HAL_I2C_Init(hi2c_eeprom);

#if EEPROM_PWR_CTRL_ENABLE
  /* 电源循环复位 */
  HAL_GPIO_WritePin(EEPROM_PWR_GPIO, EEPROM_PWR_PIN, GPIO_PIN_RESET);
  HAL_Delay(50);
  HAL_GPIO_WritePin(EEPROM_PWR_GPIO, EEPROM_PWR_PIN, GPIO_PIN_SET);
  HAL_Delay(100);
#endif
}

/**
  * @brief 地址有效性驗證 (AT24C32專用)
  * @param addr  起始地址 (0x0000-0x0FFF)
  * @param size  數據長度 (單位: byte)
  * @retval EEPROM_Status 驗證結果
  */

static EEPROM_Status Validate_Address(uint16_t addr, uint16_t size) {
  const uint16_t max_addr = 0x0FFF;  // AT24C32最大地址
  const uint16_t PAGE_SIZE = 32;     // 頁寫入限制


  if(addr > max_addr){
	  printf("[ERROR] Address range 0x%04X-0x%04X out of bounds\n",
	                addr, addr + size - 1);
	  return EEPROM_ERR_ADDR;
  }
  if((addr + size) > max_addr)
  {
  	  printf("I2C Address failed\n");
  	  return EEPROM_ERR_ADDR;
   }

  // 檢查是否跨頁寫入 (僅在Write函數中需要，Read可選)
  if((addr % PAGE_SIZE) + size > PAGE_SIZE) {
      printf("[WARNING] Operation crosses page boundary (addr=0x%04X, size=%d)\n",
             addr, size);
      // 不返回錯誤，僅警告
  }

  return EEPROM_OK;
}

/**
  * @brief 更新设备健康状态
  */
static void Update_Health(bool success) {
  health_monitor.last_op_time = HAL_GetTick();
  health_monitor.error_count = success ? 0 : (health_monitor.error_count + 1);

  if(success) {
    health_monitor.hw_status |= 0x01;  // 标记最后一次操作成功
  } else {
    health_monitor.hw_status &= ~0x01;

    /* 连续错误超过阈值触发安全机制 */
    if(health_monitor.error_count >= 5) {
      NVIC_SystemReset();  // 触发系统复位
    }
  }
}



void EEPROM_EraseAll(void) {
    uint8_t blank[32];
    memset(blank, 0xFF, sizeof(blank)); // 填充0xFF

    for(uint16_t addr = 0; addr < EEPROM_SIZE; addr += 32) {
        HAL_I2C_Mem_Write(&hi2c1, 0xA0, addr, I2C_MEMADD_SIZE_16BIT, blank, 32, 100);
        HAL_Delay(5); // 必须延时！
    }
}


/* 公开辅助函数 --------------------------------------------------------------*/

void EEPROM_GetHealth(EEPROM_Health* health) {
  memcpy(health, &health_monitor, sizeof(EEPROM_Health));
}

void EEPROM_ResetHealth(void) {
  memset(&health_monitor, 0, sizeof(EEPROM_Health));
}

bool EEPROM_IsReady(void) {
  return (HAL_I2C_IsDeviceReady(hi2c_eeprom, EEPROM_I2C_ADDR << 1, 1, 100) == HAL_OK);
}

#if EEPROM_PWR_CTRL_ENABLE
void EEPROM_PowerCycle(void) {
  HAL_GPIO_WritePin(EEPROM_PWR_GPIO, EEPROM_PWR_PIN, GPIO_PIN_RESET);
  HAL_Delay(500);
  HAL_GPIO_WritePin(EEPROM_PWR_GPIO, EEPROM_PWR_PIN, GPIO_PIN_SET);
  HAL_Delay(100);
}
#endif

