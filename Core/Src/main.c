/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 @file           : main.c
 @brief          : Main program body
 ******************************************************************************
 * \mainpage
 *  # 各種關於STM32F407的測試
 *  目前的進度：
 *  	- 板上LED的操作
 *  	- printf和scanf的重寫
 *  	- TCA555,I2C擴充IO的寫入;但讀取有待測試
 *  	- TM1638的寫入;讀取有待測試
 *
 * # EEPROM的檔案結構
 * ├── Core<br/>
 * │   ├── Inc<br/>
 * │   │   ├── eeprom_handler.h    // EEPROM操作接口<br/>
 * │   │   ├── data_recovery.h     // 数据恢复机制<br/>
 * │   │   └── error_handler.h     // 错误处理系统<br/>
 * │   ├── Src<br/>
 * │   │   ├── eeprom_handler.c    // EEPROM底层驱动<br/>
 * │   │   ├── data_recovery.c     // 数据恢复实现<br/>
 * │   │   ├── error_handler.c     // 错误处理实现<br/>
 * │   │   └── main.c             // 主程序<br/>
 * #IO setup:<br/>
 * I2C1 GPIO Configuration<br/>
 *  PB6     ------> I2C1_SCL<br/>
 *	PB7     ------> I2C1_SDA<br/>
 * USART2 GPIO Configuration<br/>
 *	PA2     ------> USART2_TX<br/>
 * 	PA3     ------> USART2_RX<br/>
 * TM1638 GPIO Configuration<br/>
 *	PC4     ------> SEG_CLK<br/>
 * 	PC5     ------> SEG_DIO<br/>
 * 	PB0     ------> SEG_STB<br/>
 * ##版權：<br/>
 * GNU GENERAL PUBLIC LICENSE<br/>
 * Version 3, 29 June 2007<br/>
 * Copyright (C) [2025] [Ethan]<br/>
 * 本程式是一個自由軟體：您可以依照 **GNU 通用公共授權條款（GPL）** 發佈和/或修改，<br/>
 * GPL 版本 3 或（依您選擇）任何更新版本。<br/>
 * 本程式的發佈目的是希望它對您有幫助，但 **不提供任何擔保**，甚至不包含適銷性或特定用途適用性的默示擔保。<br/>
 * 請參閱 **GNU 通用公共授權條款** 以獲取更多詳細資訊。<br/>
 * 您應當已經收到一份 **GNU 通用公共授權條款** 副本。<br/>
 * 如果沒有，請參閱 <https://www.gnu.org/licenses/gpl-3.0.html>。<br/>
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "uart_io.h"
#include "ssd1306.h"
#include "Tools.h"
#include "error_handler.h"
#include "eeprom_handler.h"

/**
 * @defgroup Function_selete Function_selete
 * @brief 切換要執行的驅動程式,達到程式執行時的單純性
 * @{
 */

//#define TCA555
//#define uartPrintfloop
//#define EEPROM
//#define EEPROM_read
//#define EEPROM_write
#define MAX_INPUT_LEN 32 // 输入字符串最大长度
uint8_t rx_data;


#ifdef EEPROM
#include "error_handler.h"
#include "eeprom_handler.h"

/**
#define EEPROM_I2C_ADDR        0x50  // 7位地址 (A2A1A0接地)

#define EEPROM_PAGE_SIZE       32     // 每頁32字節
#define EEPROM_MAX_ADDR        0x0FFF // 最大地址 (4KB)
#define DATA_WITH_CRC_SIZE(data_size) (data_size + 2)  // 16-bit CRC
*/
#endif





//#define oled
/**  @brief 設定宣告OLED */
#ifdef oled
	#include "SH1106.h"
	#include "fonts.h"
	#include "bitmap.h"
	#include "horse_anim.h"
#endif


/**  @brief 設定只執行TCA555的程式 */
#ifdef TCA555
#include "TCA555.h"
#endif
/**
 * @}
 */
//確保 `Function_selete` 進入 `Function_selete` 群組

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//使用者自訂型別定義（Private Typedef）


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//使用者自訂 #define（Private Define）
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
//	使用者自訂巨集（Private Macro）


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//使用者自訂變數（Private Variables）

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
//使用者自訂「函式原型（Private Function Prototypes）

/*
 * @brife EEPROM函式宣告*/
/*
 HAL_StatusTypeDef EEPROM_Write(uint16_t memAddr, uint8_t *data, uint16_t size) ;
 HAL_StatusTypeDef EEPROM_WaitForWriteComplete(void) ;
 HAL_StatusTypeDef EEPROM_Read(uint16_t memAddr, uint8_t *data, uint16_t size);
 HAL_StatusTypeDef EEPROM_ReadWithCRC(uint16_t addr, uint8_t* data, uint16_t size);
 HAL_StatusTypeDef EEPROM_WriteWithCRC(uint16_t addr, uint8_t* data, uint16_t size);
*/
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//用來放「使用者自訂函式（Private User Code）」的實作


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /**
     * @defgroup Initialize Initialize
     * @brief Initialize all configured peripherals
     * @{
     */

    /**
     * @brief  GPIO初始化
     * param  無
     * @return 無
     * @note   這裡可以寫一些額外的備註
     * @warning 這裡可以寫警告資訊
     */
  MX_GPIO_Init();

  /**
     * @brief  SPI初始化
     * param  無
     * @return 無

     */
  MX_SPI1_Init();
  /**
      * @brief  USART初始化
      * param  無
      * @return 無
      */

  MX_USART2_UART_Init();
  /**
      * @brief  I2C初始化
      * I2C1 GPIO Configuration
 	 * PB6     ------> I2C1_SCL
 	 * PB7     ------> I2C1_SDA
      * param  無
      * @return 無
      *
 	 *
     */
  MX_I2C1_Init();

  //MX_I2C3_Init();
  /* USER CODE BEGIN 2 */
  uart_io_init(); // 初始化UART輸入輸出


  /**
    * @}
    */  //確保 `Initialize` 進入 `Initialize` 群組


  /* USER CODE BEGIN 2 */
  //uart_io_init(); // 初始化UART輸入輸出

  //

  I2C_Status status = I2C_Init_Devices();
    if (status != I2C_OK) {
      printf("[错误] I2C设备初始化失败! 错误码: %d\n", status);
      Error_Handler();  // 需自定义错误处理函数
      return status;
    }

    printf("I2C device init ok!yy \n\n");

    printf("System Ready. Enter a string:\r\n");



    char input_str[MAX_INPUT_LEN + 1]; // +1 为终止符'\0'



	    scanf("%32s", input_str);
	    printf("Received: %s\r\n",input_str);

		   printf("out for loop\n");

	   //char Year,Moth,Day,Hour,Minute,Second;

	   //uint8_t DayandTime=sizeof(input_str);

	   //int i;

		   for (int i = 0; input_str[i] != '\0'; i++) {
		  				   //printf("in for loop \n");
		  	              // 調用 printf 打印單個字符
		  	              // 由於 printf 已重定向到 UART，這會透過 _write 發送到 UART
		  				   printf("%c\n", input_str[i]);

		  	              // 可選：添加短暫延遲，以便在慢速終端上能看到逐字打印效果
		  	              // 注意：HAL_Delay 會阻塞主循環，僅用於演示或要求不高的地方
		  	              // HAL_Delay(50); // 延遲 50 毫秒
		  	          }
		  			   //break;

		   while(1){

		   }


    //scanf("%32s", input_str);
    //printf("Received: %s\r\n", input_str);


    //HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin); // 调试用LED


   // setvbuf(stdout, NULL, _IONBF, 0);
       // printf("UART Ready!\r\n");

       // HAL_UART_Receive_IT(&huart2, (uint8_t *)&rx_byte, 1);


    //char input_str[MAX_INPUT_LEN + 1]; // +1 为终止符'\0'
/*
    if (scanf("%32s", input_str) == 1) { // 限制输入长度防止溢出
                printf("Received: %s\r\n", input_str);
            } else {
                printf("Input error.\r\n");
                // 清空缓冲区残留数据
                while (getchar() != '\n');
            }

*/
    //printf("\n abcd \n");





#ifdef EEPROM
  printf("\n");

  printf("start......\n\n");

  if(EEPROM_Init() != EEPROM_OK) {
      Error_Handler();
    }

  printf("eeprom init ok......\n\n");


    uint8_t test_data[] = "STM32 EEPROM 5566";

    setvbuf(stdout, NULL, _IONBF, 0);
    //int S_test_data=10;
    //int S_test_data2=sizeof(test_data);

    //printf("arry size (bytes): %d\n",S_test_data ); // 輸出 10
    //printf("arry size (bytes): %d\n",S_test_data2 ); // 輸出 18

    //fflush(stdout); // 確保每個數值立即輸出
#endif




#ifdef ds1307_test
      // 初始化成功后的操作
      DS1307_Time current_time;
      status = DS1307_GetTime(&current_time);

        printf("当前RTC时间:\n");
        printf("--------------------------------\n");
        printf("日期: 20%02d-%02d-%02d\n",   // 假设年份为两位表示(00-99)
               current_time.year,
               current_time.month,
               current_time.date);
        printf("时间: %02d:%02d:%02d\n",
               current_time.hours,
               current_time.minutes,
               current_time.seconds);
        printf("星期: 周%d\n", current_time.day);  // 根据DS1307规范(1=周日,7=周六)
        printf("--------------------------------\n\n");


        while (1)
        {

        	printf("Time: %02d:%02d:%02d\n",
        	               current_time.hours,
        	               current_time.minutes,
        	               current_time.seconds);
        	        printf("星期: 周%d\n", current_time.day);  // 根据DS1307规范(1=周日,7=周六)
        	        printf("--------------------------------\n\n");
               	 HAL_Delay(1000);

        }

#endif



#ifdef EEPROM_write
    EEPROM_Status status = EEPROM_Write(0x200, test_data, sizeof(test_data));
    if(status != EEPROM_OK) {
      Handle_Error(status);
      //printf("write error status =%d\n",status);
    }//else{
    	//printf("write pass \n");
    //}
    //printf("prog end......\n\n");
#endif

#ifdef EEPROM_read

    uint8_t read_data[sizeof(test_data)];

	//printf("read data_1st =%s\n",read_data);



   status = EEPROM_Read(0x200, read_data, sizeof(read_data));

    printf("read data 2ed= %s\n",read_data);

    if(status == EEPROM_OK) {

    	//printf("read data = %s\n",read_data);

      if(memcmp(test_data, read_data, sizeof(test_data)) != 0) {
        //Handle_Data_Corruption();
      }
    }

    printf("prog end......\n\n");

#endif




#ifdef uartPrintfloop

    int i=0;
        while(i<1000){

        	 printf("USART2 Ready!%d\n",i);
        	 i++;
        	 HAL_Delay(1000);
        }

        // 發送 "ABCD" 驗證資料完整性
        //const uint8_t test_data[] = {0x41, 0x42, 0x43, 0x44};
       //  HAL_UART_Transmit(&huart2, test_data, sizeof(test_data), HAL_MAX_DELAY);


        // 發送混合換行符的測試字串
       // const char *test_str = "Line1\nLine2\r\nLine3\r";
        //HAL_UART_Transmit(&huart2, (uint8_t*)test_str, strlen(test_str), HAL_MAX_DELAY);
      /*
        int val = 0;
        printf("Test: ");
        scanf("%d", &val);
        clear_input_buffer();
        HAL_Delay(0xFF);
        printf("Value: %d\r\n", val);

      */
#endif

  #ifdef uartPrintf

  printf("USART2 Ready!\r\n");
  printf("Enter a number: ");
      int num = 0;
      scanf("%d", &num);
      printf("\n");

      HAL_Delay(0xFF);

      printf("You entered: %d\r\n", num);
      HAL_Delay(0xFFF);
#endif

#ifdef oled
  SH1106_Init();
    SH1106_DrawBitmap(2, 0, LOGO_BW_map, 128, 64, 1);  // 132x64, so leave 2 from both sides, draw from 0,0
    SH1106_UpdateScreen();



#endif


#ifdef TCA555

    /**
         * @brief  TCA9555 初始化
         * param  無
         * @return 無
         */
  TCA9555_Init();

  // 初始化 TCA9555 為輸出模式
  uint16_t output_data = 0xFFFF;
  //output_data=
  //|    bit 15   |   bit 14    |    bit 13   |    bit 12   |   bit 11    |    bit 10   |     bit 9   |     bit 8   |
  //|   Port 15   |   Port 14   |   Port 13   |   Port 12   |   Port 11   |   Port 10   |   Port 09   |   Port 08   |
  //
  //|    bit 7    |     bit 6   |     bit 5   |    bit 4    |     bit 3   |    bit 2    |    bit 1    |    bit 0    |
  //|   Port 07   |   Port 06   |   Port 05   |   Port 04   |   Port 03   |   Port 02   |   Port 01   |   Port 00   |




 //uint16 mask = TCA9555_GPIO_MASK(port, pin);
 ///int8_t position = 0;  // 當前點亮的位置（0~15）



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  TCA9555_WriteOutput(output_data);
	  output_data--;
	  HAL_Delay(500);
	 //printf("in while loop\n");

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
#endif

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
