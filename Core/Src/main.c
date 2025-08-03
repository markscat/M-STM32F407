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



#define MAX_INPUT_LEN 32 // 输入字符串最大长度
#define MAX_INPUT_LEN_MINIMAL 32
#define MAX_COMMAND_LEN_MAIN 32                         // UART 命令行的最大允許字符數 (不包括 '\0')
#define LINE_BUFFER_SIZE_MAIN (MAX_COMMAND_LEN_MAIN + 1) // <<<< 在此處定義！ 行緩衝區大小（+1 用於 '\0'）
//static uint8_t SH1106_Buffer_in_main[SH1106_WIDTH * SH1106_HEIGHT / 8];


bool CheckTimeStruct (const DS1307_Time* time_to_check );


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


//#define oled
/**  @brief 設定宣告OLED */
#ifdef oled
	#include <SH110_h>
	#include <I2C_OLED_fonts.h>
	#include "bitmap.h"
	#include <I2C_OLED_horse_anim.h>
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

  I2C_Status status = I2C_Init_Devices();

    if (status != I2C_OK) {
      printf("[错误] I2C设备初始化失败! 错误码: %d\n", status);
      Error_Handler();  // 需自定义错误处理函数
      return status;
    }

    printf("I2C device init ok!yy \n\n");

    printf("System Ready. Enter a string:\r\n");



    uint8_t oled_buffer[1024] = {0}; // 128 * 64 / 8

    SH1106_OLED_Init();
    SH1106_OLED_Clear();


    for (uint8_t x = 0; x < 128; x++) {
    	SH1106_OLED_DrawPixel(oled_buffer, x, x / 2, 1);
    	SH1106_OLED_DrawPixel(oled_buffer, x, 63 - x / 2, 1);
       }

    SH1106_OLED_UpdateScreen(oled_buffer);

    while(1){

    }



#ifdef I2C_ADDR_Scan
printf("\n\n");
	Scan_I2C_Address();

#endif

//#define SH1106
//#define BruteForceFill



#ifdef temp_sh1106
	  /* USER CODE BEGIN 2 */
	  printf("Attempting final initialization...\n");

	  // 呼叫我們全新的 Init 函式
	  SH1106_Init();

	  printf("Init sequence sent. Now filling screen with WHITE.\n");

	  // 將 STM32 的 RAM 緩衝區填滿白色
	  SH1106_Fill(SH1106_COLOR_WHITE);

	  // 呼叫我們全新的 UpdateScreen 函式，將緩衝區內容顯示出來
	  SH1106_UpdateScreen();

	  printf("Update command sent. Check the screen!\n");

#endif

 #ifdef BruteForceFill

	  SH1106_WRITECOMMAND(0x8D); // 開啟電荷泵
	  SH1106_WRITECOMMAND(0x14);
	  SH1106_WRITECOMMAND(0xAF); // 開啟顯示

	  // 步驟二：進入暴力填充測試
	  printf("Starting Brute-Force Fill Test...\n");

	  // --- 測試圖案 1: 全亮 (0xFF) ---
	      printf("Writing 0xFF to all pages...\n");
	      for (uint8_t page = 0; page < 8; page++) {
	          SH1106_WRITECOMMAND(0xB0 + page); // 設置頁
	          SH1106_WRITECOMMAND(0x02);        // 固定使用偏移 2 (最常見)
	          SH1106_WRITECOMMAND(0x10);

	          // 準備一個填滿 0xFF 的數據緩衝區
	          uint8_t page_data[128];
	          memset(page_data, 0xff, 128);

	          // 用最原始的 I2C 多位元組寫入函式發送
	          SH1106_I2C_WriteMulti(SH1106_I2C_ADDR, 0x40, page_data, 128);
	      }
	      HAL_Delay(2000); // 觀察 2 秒
#endif



#ifdef SH1106
	 printf("Initializing display...\n");
	  if (SH1106_Init() != 1) { // 這裡會使用您在 .c 檔中啟用的那組序列
	      printf("Init FAILED.\n");
	      while(1){}

	  } else {
	      printf("Init OK.\n");
	  }


	  // 在 RAM 的 buffer 中先畫好一個固定的測試圖案
	    //SH1106_Fill(SH1106_COLOR_BLACK);
	    // 畫一條從左上到右下的粗線，這樣比較容易看到
	    /*n
	  for(int i=0; i<SH1106_HEIGHT; i++) {
		  SH1106_DrawPixel(i*2, i, SH1106_COLOR_WHITE);
		  SH1106_DrawPixel(i*2+1, i, SH1106_COLOR_WHITE);
	    }

	  */
#endif

	   // uint8_t column_start_address = 0;






#ifdef BruteForceFill
while(1){



	  // 我們只嘗試更新第 4 頁 (螢幕正中央)
	    printf("Attempting to write to Page 4 ONLY...\n");
	    SH1106_WRITECOMMAND(0xB0 + 4);    // 明確指定頁面 4
	    SH1106_WRITECOMMAND(0x02);        // 列起始位址 低位
	    SH1106_WRITECOMMAND(0x10);        // 列起始位址 高位

	    // 準備一個可辨識的圖案 (全亮)
	    uint8_t page_data[128];
	    memset(page_data, 0x00, 128);

	    // 發送數據
	    SH1106_I2C_WriteMulti(SH1106_I2C_ADDR, 0x40, page_data, 128);

	    // 在這裡不做任何事，只持續觀察
	    HAL_Delay(1000); // 延遲一秒，避免 I2C 總線過於繁忙


/*
    // --- 測試圖案 1: 全亮 (0xFF) ---
    printf("Writing 0xFF to all pages...\n");
    for (uint8_t page = 0; page < 8; page++) {
        SH1106_WRITECOMMAND(0xB0 + page); // 設置頁
        SH1106_WRITECOMMAND(0x02);        // 固定使用偏移 2 (最常見)
        SH1106_WRITECOMMAND(0x10);

        // 準備一個填滿 0xFF 的數據緩衝區
        uint8_t page_data[128];
        memset(page_data, 0xFF, 128);

        // 用最原始的 I2C 多位元組寫入函式發送
        SH1106_I2C_WriteMulti(SH1106_I2C_ADDR, 0x40, page_data, 128);
    }
    HAL_Delay(2000); // 觀察 2 秒
*/
/*
    // --- 測試圖案 2: 全黑 (0x00) ---
    printf("Writing 0x00 to all pages...\n");
    for (uint8_t page = 0; page < 8; page++) {
        SH1106_WRITECOMMAND(0xB0 + page); // 設置頁
        SH1106_WRITECOMMAND(0x02);        // 固定使用偏移 2
        SH1106_WRITECOMMAND(0x10);

        // 準備一個填滿 0x00 的數據緩衝區
        uint8_t page_data[128];
        memset(page_data, 0x00, 128);

        // 用最原始的 I2C 多位元組寫入函式發送
        SH1106_I2C_WriteMulti(SH1106_I2C_ADDR, 0x40, page_data, 128);
    }
    HAL_Delay(2000); // 觀察 2 秒

    */
/*
    // --- 測試圖案 3: 交錯線條 (0xAA) ---
    printf("Writing 0xAA to all pages...\n");
    for (uint8_t page = 0; page < 8; page++) {
        SH1106_WRITECOMMAND(0xB0 + page); // 設置頁
        SH1106_WRITECOMMAND(0x02);        // 固定使用偏移 2
        SH1106_WRITECOMMAND(0x10);

        // 準備一個填滿 0xAA (二進制 10101010) 的數據緩衝區
        uint8_t page_data[128];
        memset(page_data, 0xAA, 128);

        // 用最原始的 I2C 多位元組寫入函式發送
        SH1106_I2C_WriteMulti(SH1106_I2C_ADDR, 0x40, page_data, 128);
    }
    HAL_Delay(2000); // 觀察 2 秒

*/
#endif


#ifdef  SH1106__

	printf("Testing Column Start Address: %d\n", column_start_address);

	    // 核心測試：用不同的起始位址來更新螢幕
	    for (uint8_t m = 0; m < 8; m++) {
	        SH1106_WRITECOMMAND(0xB0 + m);                  // 設置頁
	        SH1106_WRITECOMMAND(column_start_address & 0x0F); // 設置列起始位址 (低4位)
	        SH1106_WRITECOMMAND(0x10 | (column_start_address >> 4)); // 設置列起始位址 (高4位)
	        SH1106_I2C_WriteMulti(SH1106_I2C_ADDR, 0x40, &SH1106_Buffer_in_main[SH1106_WIDTH * m], SH1106_WIDTH);
	    }

	    // 每次測試完，增加位址，準備下一次測試
	    column_start_address++;
	    if (column_start_address > 131) { // SH1106 的內部記憶體寬度是 132
	        column_start_address = 0;
	    }

	    HAL_Delay(500); // 每半秒換一個位址
#endif


#ifdef SH1106_


	  // 步驟 2: 開啟顯示 (Display ON)
	  printf("Step 2: Sending Display ON (0xAF)...\n");
	  SH1106_WRITECOMMAND(0xAF);

	  // 此時螢幕應該是清晰的雪花屏或全亮
	  HAL_Delay(3000); // 等待 3 秒觀察

	  /********************* 關閉序列 *********************/
	  printf("--- Shutdown Sequence START ---\n");

	  // 步驟 3: 關閉顯示 (Display OFF)
	  // 如果點火成功，這一步應該能讓螢幕變黑
	  printf("Step 3: Sending Display OFF (0xAE)...\n");
	  SH1106_WRITECOMMAND(0xAE);

	  // 此時螢幕應該完全變黑
	  HAL_Delay(3000); // 等待 3 秒觀察

}
#endif


#ifdef time_setting

    //<讀取階段>
    DS1307_Time dt = {0};
	//HAL_StatusTypeDef DT_status;

    char input_str[MAX_INPUT_LEN + 1]; // +1 为终止符'\0'

    //分析input_str的資料,存到 dt
    //dt.year = 0xFF;



    char YandN;
    HAL_StatusTypeDef DT_status;

    printf("need setup time ? [Y]/[N] \n");
    scanf(" %c",&YandN);

    if(YandN =='Y' || YandN =='y'){
    	printf("\n input time :\n");
        scanf("%32s", input_str);
        printf("Received_time : %s\r\n\n",input_str);
        dt = Parse_TimeString(input_str);

        if (CheckTimeStruct(&dt)){
        	printf("set time... \n");
        	DT_status = DS1307_SetTime(&dt);
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



//#define loop

#ifdef loop
    while(1){


        // 任務 1: 不斷地、非阻塞地從 UART 數據流中組裝命令
        ProcessUartRingBuffer();

        // 任務 2: 呼叫我們新的命令處理中心，讓它去檢查旗標並執行命令
        HandleUartCommands();

        // 讀取時間
    	DT_status = DS1307_GetTime(&dt);

    	//顯示時間 as
    	if (DT_status == HAL_OK) {
    		printf("現在時間: 20%02d-%02u-%02u(%02u) %02u:%02u:%02u\r\n",
    				dt.year,dt.month,dt.date,dt.day,dt.hours,dt.minutes,dt.seconds);
    	} else {
    		printf("錯誤: 時間設定失敗 (HAL狀態碼: %d)\r\n", DT_status);
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
     	DT_status = DS1307_SetTime(&dt);

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
     	DT_status = DS1307_GetTime(&dt);
     	if (DT_status == HAL_OK) {
     		printf("現在時間: %04u-%02u-%02u(%02u) %02u:%02u:%02u\r\n",
     				dt.year,dt.month,dt.date,dt.day,dt.hours,dt.minutes,dt.seconds);
     	} else {
     		printf("錯誤: 時間設定失敗 (HAL狀態碼: %d)\r\n", DT_status);
     	}
     }



    while(1){

    	DT_status = DS1307_GetTime(&dt);
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

    //DS1307_Time dt={0};
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
        					DS1307_Time time_to_set = Parse_TimeString(payload);
        					// 檢查解析是否成功
        					if (time_to_set.year != 0xFF) {// B. 設定 RTC，並檢查返回值
        						HAL_StatusTypeDef status = DS1307_SetTime(&time_to_set);
        						if (status == HAL_OK) {
        							printf("OK: Time set successfully.\n");
        							// =======================================================
        							//        ↓↓↓ 【新增的回讀驗證步驟】 ↓↓↓
        							// =======================================================
        							printf("Verifying time by reading back from RTC...\n");
        							DS1307_Time verified_time;
        							HAL_Delay(5); // 給 RTC 一點點反應時間（可選，但有時有好處）
        							if (DS1307_GetTime(&verified_time) == HAL_OK) {
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
        					DS1307_Time current_time;
        					HAL_StatusTypeDef status = DS1307_GetTime(&current_time);
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
