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
 *  	- 完成EEPROM AT24C32以及AT24C02的讀寫
 *  	- 完成OLED 螢幕開啟以及關閉的功能
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
 *
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
 *
 * I2C Address:
 * AT24C02				0x57
 * AT24C32				0x50
 * SH1106				0x3C
 * RTC(DS3231/ DS1307) 	0x68
 * ADS1115(ADC)			0x78
 *
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
#include "I2C_Peripherals.h"//ok

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/**
 * @defgroup Function_selete Function_selete
 * @brief 切換要執行的驅動程式,達到程式執行時的單純性
 * @{
 */

//#define TCA555
//#define uartPrintfloop
//#define EEPROM
//#define PrintfScanfTestCode
//#define RTCSet
//#define  RTC_Write_interr

//#define RTC_Write


//#define clear_EEPROM
//#define EEPROM_write
//#define EEPROM_read
#define time_setting
//#define ADS1115



#define MAX_INPUT_LEN 32 // 输入字符串最大长度
#define MAX_INPUT_LEN_MINIMAL 32
#define MAX_COMMAND_LEN_MAIN 32                         // UART 命令行的最大允許字符數 (不包括 '\0')
#define LINE_BUFFER_SIZE_MAIN (MAX_COMMAND_LEN_MAIN + 1) // <<<< 在此處定義！ 行緩衝區大小（+1 用於 '\0'）
//static uint8_t SH1106_Buffer_in_main[SH1106_WIDTH * SH1106_HEIGHT / 8];


bool CheckTimeStruct (const RTC_Time* time_to_check );


char line_buffer_main[LINE_BUFFER_SIZE_MAIN];
volatile uint16_t line_buffer_index_main = 0;

uint8_t rx_data;


#ifdef EEPROM
#include "error_handler.h"
#include <I2C_Peripherals.h>

/**
#define EEPROM_I2C_ADDR        0x50  // 7位地址 (A2A1A0接地)

#define EEPROM_PAGE_SIZE       32     // 每頁32字節
#define EEPROM_MAX_ADDR        0x0FFF // 最大地址 (4KB)
#define DATA_WITH_CRC_SIZE(data_size) (data_size + 2)  // 16-bit CRC
*/
#endif



/**  @brief 設定只執行TCA555的程式 */
#ifdef TCA555
#include "TCA555.h"
#endif
/**
 * @}
 */



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


// 來自 uart_io.c 的外部變數 (根據您的專案定義)
extern volatile uint16_t RxCounter;
extern volatile uint8_t RxFinishFlag;

/* USER CODE END PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
//使用者自訂「函式原型（Private Function Prototypes）



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




  printf("\n************************************************\n");
  Scan_I2C_Address();
  printf("\n************************************************\n");
  printf("\n");
  printf("\n**************I2C init. Device *****************\n");





#ifdef ADS1115

  // 初始化 ADS1115
      if (ADS1115_Init(&hi2c1, ADS1115_DATA_RATE_128, ADS1115_PGA_ONE) == HAL_OK) {
          printf("ADC ADS1115 init ok!\n");
          //while(1);
      }else{
    	  printf("ADC ADS1115 init fial \n");
      }

      /*
       * Vout = Vin*(R2/(R1+R2))
       * Vin = 5V
       * R2 = 2000 ohm
       * R3 = 3000 ohm
       *
       * */
      int32_t Vth = 1;


      printf("adc read ........... \n");

/**
 * ADS1115 程式應用
 * 	目前可以作到單端單次讀取,差動(AIN0-AIN1,AIN2-AIN3,AIN0-AIN3四種方式)單次讀取
 * 	單端連續讀取,差動連續讀取
 * 	Comparator mode 和 ALERT/RDY	還沒實作
 * */

      //決定單次還是連續
      //#define OneShont
      #define Continuous

      //決定是單端還是差動
      //#define singelend
      #define Different

	  #ifdef Continuous
      #ifdef singelend
      	  float voltage_A0;
      	  ADS1115_StartContinuous(ADS1115_MUX_AIN0,ADS1115_PGA_ONE,ADS1115_DATA_RATE_128);
      #endif

	#ifdef Different
      	  float voltage_diff_01;
      	  ADS1115_StartContinuous(ADS1115_MUX_DIFF_0_1,ADS1115_PGA_ONE,ADS1115_DATA_RATE_128);
	#endif

      while(1){
    	  #ifdef singelend
    	   if( ADS1115_ReadContinuous(&voltage_A0)== HAL_OK){
               printf("A0 voltage    = %.3f V\n", ((voltage_A0*Vth)/(1000)));
    	   }
    	  #endif

		  #ifdef Different
    	   if( ADS1115_ReadContinuous(&voltage_diff_01)== HAL_OK){
               printf("A0 voltage    = %.3f V\n", ((voltage_diff_01*Vth)/(1000)));
    	   }
		  #endif

           HAL_Delay(1000);
      }

#endif

#ifdef OneShont
      float voltage_A0,voltage_A1,voltage_diff_01;

      while(1) {
          if (ADS1115_readOneShont(ADS1115_MUX_AIN0, &voltage_A0) == HAL_OK) {
              printf("A0 voltage    = %.3f V\n", ((voltage_A0*Vth)/(1000)));
          } else {
              printf("Read failed\n");
          }

          if (ADS1115_readOneShont(ADS1115_MUX_AIN1, &voltage_A1) == HAL_OK) {
        	  printf("A1 voltage    = %.3f V\n", ((voltage_A1*Vth)/(1000)));
          } else {
        	  printf("Read failed\n");
          }
          if (ADS1115_readOneShont(ADS1115_MUX_DIFF_0_1, &voltage_diff_01) == HAL_OK) {
        	  printf("diff_1 voltage = %.3f V\n", ((voltage_diff_01*Vth)/(1000)));
        	  printf("A0-A1          = %.3f V\n",((voltage_A0-voltage_A1)/1000));
          } else {
        	  printf("Read failed\n");
          }
          printf("\n");

          HAL_Delay(1000);
      }
#endif

#endif


  I2C_Status status = I2C_Init_Devices();

    if (status != I2C_OK) {
      printf("[错误] I2C设备初始化失败! 错误码: %d\n", status);
      Error_Handler();  // 需自定义错误处理函数
      return status;
    }
    printf("I2C device init ok!yy \n\n");
    printf("\n************System Ready.**************************\n");


#define oled

#ifdef oled
    printf("init oled \n");

    SH1106_StatusTypeDef oled_status; // 创建一个变量来接收返回值
    oled_status = SH1106_Init();     // 调用初始化函数


#ifdef on_off_screen
     // --- 【关键】检查初始化结果 ---
     if (oled_status == SH1106_OK)
     {
       // --- 初始化成功 ---
       printf("OLED SH1106 Initialized successfully!\r\n");
       // 在这里可以执行一些开机画面或测试
       SH1106_Fill(SH1106_COLOR_WHITE); // 将屏幕填充为白色
       SH1106_UpdateScreen();          // 刷新屏幕
       HAL_Delay(1000);                // 等待1秒

       SH1106_Fill(SH1106_COLOR_BLACK); // 再填充为黑色
       SH1106_UpdateScreen();
       HAL_Delay(500);
     }
     else
     {
       // --- 初始化失败 ---
       printf("OLED SH1106 Initialization FAILED! Error Code: %d\r\n", oled_status);

       // 失败后可以采取一些措施，例如：
       // 1. 点亮一个红色 LED 指示错误
       //HAL_GPIO_WritePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin, GPIO_PIN_SET);

     }



    //uint8_t oled_buffer[128*64/8] = {0};  // SH1106 的完整缓冲区（128x64位）

    //memset(oled_buffer, 0x00, sizeof(oled_buffer)); // 全白
    SH1106_Fill(1);
    SH1106_UpdateScreen();

 	HAL_Delay(3000);
 	SH1106_Fill(0);
 	SH1106_UpdateScreen();


 	HAL_Delay(3000);
 	SH1106_Fill(1);
 	SH1106_UpdateScreen();


    while(1){};


    for (uint8_t x = 0; x < 128; x++) {
         for (uint8_t y = 0; y < 64; y++) {
         	//SH1106_OLED_DrawPixel(x, y, 1);
         	SH1106_DrawPixel(x,y,1);
            SH1106_UpdateScreen();

         }
     }
    //SH1106_UpdateScreen();
    //SH1106_UpdateScreen_org();
 	HAL_Delay(3000);
 	SH1106_Fill(1);
    while (1);

#endif


	#define HORSE_ANIM_WIDTH  128
	#define HORSE_ANIM_HEIGHT 64

	#define HORSE_ANIM_FRAME_COUNT (sizeof(horse_anim_frames) / sizeof(horse_anim_frames[0]))

    const unsigned char* horse_anim_frames[10] = {
            horse1, horse2, horse3, horse4, horse5,
            horse6, horse7, horse8, horse9, horse10
        };


    SH1106_Fill(SH1106_COLOR_BLACK);
	SH1106_UpdateScreen();


    if(oled_status == SH1106_OK){
    	while(1){
    		for(int i=1;i< HORSE_ANIM_FRAME_COUNT;i++){// **修正：索引從 0 到 FRAME_COUNT-1**
    			// 每繪製一幀前清屏，清除上一幀的殘影
    			// 由於 DrawBitmap 會在畫圖前判斷像素是否為黑，所以通常只會在背景是靜態的情況下需要手動清屏
                // 但對於動畫，通常是全屏重畫，所以每次清屏是正常的
                SH1106_Fill(SH1106_COLOR_BLACK);

                // 繪製當前幀的馬
                SH1106_DrawBitmap(0, 0, horse_anim_frames[i], HORSE_ANIM_WIDTH, HORSE_ANIM_HEIGHT, SH1106_COLOR_WHITE);

                // 更新螢幕，將緩衝區內容 (馬) 顯示出來
                SH1106_UpdateScreen();

                //HAL_Delay(100); // 調整這個值來控制速度，例如 50ms, 100ms, 200ms
    		}
		    //SH1106_Fill(SH1106_COLOR_BLACK);
			//SH1106_UpdateScreen();
    	}
    }else{
		printf("OLED SH1106 Initialization FAILED! Error Code: %d\r\n", oled_status);
    }



   /* const unsigned char* horse_anim[10] = {
        horse1, horse2, horse3, horse4, horse5,
        horse6, horse7, horse8, horse9, horse10
    };*/

    // horse_anim.h 中声明

    // 假设 horse1 是 128x32 的位图（水平扫描，MSB优先）
//    SH1106_DrawBitmap(0, 2, horse1, 128, 32, 1); // 在(0,2)位置绘制白色图像
//    SH1106_UpdateScreen();


    // 显示 128x32 水平扫描图像（居中）
  //  SH1106_DrawImageDirect(horse1, 0, 2, 128, 32, 0);
  //  SH1106_UpdateScreen();

#endif

//#define oled_test

#ifdef oled_test
    #define Fill_OLED_Display_at_1
    //#define Fill_OLED_Display_at_0

#ifdef squr
    for (uint8_t x = 50; x < 60; x++) {
        for (uint8_t y = 10; y < 20; y++) {
            SH1106_OLED_DrawPixel(oled_buffer, x, y, 1);
    	    SH1106_OLED_UpdateScreen(oled_buffer);

        }
    }


    while (1);
#endif


#ifdef Fill_OLED_Display_at_1

    //uint8_t oled_buffer[1024] = {0}; // 128 * 64 / 8

    SH1106_OLED_Init();
    SH1106_OLED_Clear();

    for (uint8_t x = 0; x < 128; x++) {
        for (uint8_t y = 0; y < 64; y++) {
        	SH1106_OLED_DrawPixel(oled_buffer, x, y, 1);
        }
    }
    SH1106_OLED_UpdateScreen(oled_buffer);
    //SH1106_UpdateScreen();

	HAL_Delay(1000);

#endif
#ifdef Fill_OLED_Display_at_0

    //uint8_t oled_buffer[1024] = {0}; // 128 * 64 / 8

    SH1106_OLED_Init();
    SH1106_OLED_Clear();

    for (uint8_t x = 0; x < 128; x++) {
        for (uint8_t y = 0; y < 64; y++) {
        	SH1106_OLED_DrawPixel(oled_buffer, x, y, 0);
        }
    }

    SH1106_OLED_UpdateScreen(oled_buffer);

    while(0){}

#endif

#endif


#ifdef time_setting

    //<讀取階段>
    HAL_StatusTypeDef DT_status;	//時間和日期的讀取狀態
    HAL_StatusTypeDef Temp_status;	//溫度的讀取狀態

    RTC_Time dt = {0};				//時間和日期
    DS3231_Temp Temp={0};			//RTC的溫度



    char input_str[MAX_INPUT_LEN + 1]; // +1 为终止符'\0'

    char YandN;



    printf("need setup time ? [Y]/[N] \n");
    scanf(" %c",&YandN);

    if(YandN =='Y' || YandN =='y'){
    	printf("\n input time :\n");
        scanf("%32s", input_str);
        printf("Received_time : %s\r\n\n",input_str);
        dt = Parse_TimeString(input_str);

        if (CheckTimeStruct(&dt)){
        	printf("set time... \n");
        	DT_status = RTC_SetTime(&dt);
        	if (DT_status == HAL_OK) {
        		printf("時間設定成功: 20%02d-%02u-%02u(%02u) %02u:%02u:%02u\r\n",
        				dt.year, dt.month, dt.date, dt.day,
						dt.hours , dt.minutes, dt.seconds);
        	} else {
        		printf("錯誤: 時間設定失敗 (HAL狀態碼: %d)\r\n", DT_status);
        	}

        }else{
        	printf("No calibration command, do not calibrate the time, display the time \n\n");
        }
    }else if(YandN =='N' || YandN =='n'){
    	printf("show Time \n\n");
    }else{
    	printf("No input, no time correction, display time\n\n");
    }

#define loop

    static const char*  fracStr[]={"00","25","50","75"};
#ifdef loop
    while(1){


        // 任務 1: 不斷地、非阻塞地從 UART 數據流中組裝命令
        ProcessUartRingBuffer();

        // 任務 2: 呼叫我們新的命令處理中心，讓它去檢查旗標並執行命令
        HandleUartCommands();

        // 讀取時間
    	DT_status = RTC_GetTime(&dt);

    	//顯示時間 as
    	if (DT_status == HAL_OK) {
    		printf("現在時間: 20%02d-%02u-%02u(%02u) %02u:%02u:%02u\r\n",
    				dt.year,dt.month,dt.date,dt.day,dt.hours,dt.minutes,dt.seconds);
    	} else {
    		printf("錯誤: 時間設定失敗 (HAL狀態碼: %d)\r\n", DT_status);
    	}

    	Temp_status = DS3231_GetTemp(&Temp);

    	if(Temp_status == HAL_OK)
    	{
    		printf("現在溫度: %d.%s °C\r\n", Temp.Integer,fracStr[Temp.Fraction]);

    		//printf("現在溫度: 20%02d \r\n",    		    				Temp.Integer, Temp.Fraction);
    	}

    	//memset(input_str, 0, sizeof(input_str)); // 清空輸入緩衝
    	HAL_Delay(1000);

    }
#endif //loop
#endif //time_setting



#ifdef RTC_Write



	    while(1){}

    if(dt.date<30 || dt.day<8 ){
     	printf("set time... /n");
     	DT_status = RTC_SetTime(&dt);

     	if (DT_status == HAL_OK) {
     		printf("時間設定成功: 20%02d-%02u-%02u(%02u) %02u:%02u:%02u\r\n",
     				dt.year, dt.month, dt.date, dt.day,
 					dt.hours , dt.minutes, dt.seconds);
     	} else {
     		printf("錯誤: 時間設定失敗 (HAL狀態碼: %d)\r\n", DT_status);
     	}
     	memset(input_str, 0, sizeof(input_str)); // 清空輸入緩衝
     }else{
     	printf("不需要校正時間:");
     	DT_status = RTC_GetTime(&dt);
     	if (DT_status == HAL_OK) {
     		printf("現在時間: %04u-%02u-%02u(%02u) %02u:%02u:%02u\r\n",
     				dt.year,dt.month,dt.date,dt.day,dt.hours,dt.minutes,dt.seconds);
     	} else {
     		printf("錯誤: 時間設定失敗 (HAL狀態碼: %d)\r\n", DT_status);
     	}
     }



    while(1){

    	DT_status = RTC_GetTime(&dt);
    	if (DT_status == HAL_OK) {
    		printf("現在時間: 20%02d-%02u-%02u(%02u) %02u:%02u:%02u\r\n",
    				dt.year,dt.month,dt.date,dt.day,dt.hours,dt.minutes,dt.seconds);
    	} else {
    		printf("錯誤: 時間設定失敗 (HAL狀態碼: %d)\r\n", DT_status);
    	}
    	        //memset(input_str, 0, sizeof(input_str)); // 清空輸入緩衝

    	       	HAL_Delay(1000);
       }
#endif

#ifdef RTCSet

    //RTC_Time dt={0};
    //HAL_StatusTypeDef uart_status;

    //宣告一個uart_common_id,讓每次uart中斷後,判斷是什麼樣的命令
	//int UART_common_id = 0;

    // 為了實現非阻塞的週期性任務，我們需要一個時間戳變數
        uint32_t last_periodic_task_tick = 0;


    while(1){
        ProcessUartRingBuffer();
        if (g_command_line_ready) {
        	// --- 只有在收到完整命令時，才進入此處理區 ---
        	printf("CMD RX: %s\n", g_command_line_buffer);
        	// 3. 解析命令，分離出 ID 和 Payload
        	int cmd_id;
        	const char* payload = ParseCommand(g_command_line_buffer, &cmd_id);
        	if (payload != NULL) { // 確保格式正確 (找到了逗號)
        		// 4. 使用從 UART 解析出的 cmd_id 進行分派
        		switch (cmd_id) {
        			case 1: // 設定時間
        				{
        					// A. 從 payload 解析時間，這需要一個【新的】時間變數
        					RTC_Time time_to_set = Parse_TimeString(payload);
        					// 檢查解析是否成功
        					if (time_to_set.year != 0xFF) {// B. 設定 RTC，並檢查返回值
        						HAL_StatusTypeDef status = RTC_SetTime(&time_to_set);
        						if (status == HAL_OK) {
        							printf("OK: Time set successfully.\n");
        							// =======================================================
        							//        ↓↓↓ 【新增的回讀驗證步驟】 ↓↓↓
        							// =======================================================
        							printf("Verifying time by reading back from RTC...\n");
        							RTC_Time verified_time;
        							HAL_Delay(5); // 給 RTC 一點點反應時間（可選，但有時有好處）
        							if (RTC_GetTime(&verified_time) == HAL_OK) {
        								printf("Verification successful. Current RTC Time: 20%02u-%02u-%02u %02u:%02u:%02u\n",
        										verified_time.year, verified_time.month, verified_time.date,
												verified_time.hours, verified_time.minutes, verified_time.seconds);
        							} else {
        								printf("WARN: Time set, but verification read failed!\n");
        							}
        							// =======================================================
        							//        ↑↑↑ 【新增的回讀驗證步驟】 ↑↑↑
        							// =======================================================
        						} else {
        							printf("ERR: Failed to set time (HAL Status: %d).\n", status);
        						}
        					} else {
        						printf("ERR: Invalid time data in payload for command 1.\n");
        					}
        				}
        				break;
        			case 2: // 獲取時間
        				{
        					// A. 宣告一個【新的】時間變數來存放讀取結果
        					RTC_Time current_time;
        					HAL_StatusTypeDef status = RTC_GetTime(&current_time);
        					if (status == HAL_OK) {
        						printf("OK: Current time is 20%02u-%02u-%02u %02u:%02u:%02u\n",
        								current_time.year, current_time.month, current_time.date,
										current_time.hours, current_time.minutes, current_time.seconds);
        					} else {
        						printf("ERR: Failed to get time (HAL Status: %d).\n", status);
        					}
        				}
        				break;
        				// 在這裡添加 case 3, 4, 5...
        				default: // 未知的命令 ID
        					printf("ERR: Unknown Command ID %d received.\n", cmd_id);
        					break;
        	                }
        	            } else { // 格式錯誤，連逗號都沒有
        	                printf("ERR: Invalid command format. Expected 'ID,Payload'.\n");
        	            }
        	// 5. 【非常重要】處理完畢，放下旗子，重置緩衝區，準備接收下一條命令
        	g_command_line_index = 0;
        	 memset(g_command_line_buffer, 0, sizeof(g_command_line_buffer)); // 推薦清空
        	g_command_line_ready = false;
        	// --- 命令處理區結束 ---
        }

        // 3. [低頻率執行] 在這裡處理其他週期性任務，而不是用 HAL_Delay() 阻塞所有事
        if (HAL_GetTick() - last_periodic_task_tick >= 1000) {
            last_periodic_task_tick = HAL_GetTick(); // 更新時間戳

            // 在這裡可以放你希望每秒執行一次的任務
            // 比如，打印一個心跳信息，或者更新一個不重要的顯示
            printf("Heartbeat: System is running.\n");
        }

        // 你的主迴圈現在可以飛速運行，既能即時響應命令，又能處理週期性任務

    	HAL_Delay(1000);
    }


#endif



#ifdef clear_EEPROM
    EEPROM_EraseAll();

#endif


#ifdef EEPROM_write
    uint8_t Write_data[MAX_INPUT_LEN]="Hello Stm32f407"; //crc = 0xB965

    //uint16_t W_data_len = strlen((char *)Write_data); // 实际长度=14（不含\0）

    printf("EEPROM Write ...... \n\n");
/*
    printf("原始数据 (Hex): ");
    for (int i = 0; i < W_data_len; i++) {
        printf("%02X ", Write_data[i]);
    }
    printf("\n");

	printf("read eeprom \n");
*/

    //EEPROM_Status W_status = EEPROM_Write(0x10, test_data, sizeof(test_data));

	EEPROM_Status W_status = EEPROM_Write_NoCRC(0x0, Write_data, strlen((char *)Write_data));

    HAL_Delay(5);

    if(W_status != EEPROM_OK) {
    	printf("Error \n");
      Handle_Error(status);
    }
#endif


#ifdef EEPROM_read
    uint8_t read_data[MAX_INPUT_LEN]={0};
    uint16_t R_data_len ;

/*
    for(int i = 0; i < MAX_INPUT_LEN ; i++) {
        printf("%02X ", read_data[i]);
        if((i+1) % 16 == 0){
        	printf("\n");  // 每16字節換行
        }
      }
      printf("\n\n");
*/


    EEPROM_Status E_status = EEPROM_Read_NoCRC(0x00, read_data,16);

    if(E_status == EEPROM_OK) {
        // 確保讀取的數據以 null 結尾
        read_data[strlen((char *)read_data)-1] = '\0';

        R_data_len = strlen((char *)read_data); // 实际长度=14（不含\0）

        printf("Read from EEPROM: %s\n", read_data);
        printf("原始数据 (Hex): ");
        for (int i = 0; i < R_data_len; i++) {
            printf("%02X ", read_data[i]);
        }
        printf("\n");

    } else {
        printf("EEPROM read failed with status: %d\n", status);
    }
/*
    printf("prog end......\n\n");
*/
    while(1)
    {

    }

#endif

#ifdef PrintfScanfTestCode

    char input_str[MAX_INPUT_LEN + 1]; // +1 为终止符'\0'

	    scanf("%32s", input_str);
	    printf("Received: %s\r\n",input_str);

	    DateAndTime result = Parse_TimeString(input_str);

	    printf("Parsed time: %04d-%02d-%02d %02d:%02d:%02d\n",
	               result.year, result.month, result.day,
	               result.hour, result.minute, result.second);

		   while(1){

		   }

#endif


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

    while(1){

   		   }
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
