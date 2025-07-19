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
#define RTC_Read
#define RTC_Write
//#define  RTC_Write_interr

//#define clear_EEPROM
//#define EEPROM_write
//#define EEPROM_read



#define MAX_INPUT_LEN 32 // 输入字符串最大长度
#define MAX_INPUT_LEN_MINIMAL 32
#define MAX_COMMAND_LEN_MAIN 32                         // UART 命令行的最大允許字符數 (不包括 '\0')
#define LINE_BUFFER_SIZE_MAIN (MAX_COMMAND_LEN_MAIN + 1) // <<<< 在此處定義！ 行緩衝區大小（+1 用於 '\0'）
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



#ifdef clear_EEPROM
    EEPROM_EraseAll();

#endif


#ifdef EEPROM_write
    uint8_t Write_data[MAX_INPUT_LEN]="Hello Stm32f407"; //crc = 0xB965

    uint16_t W_data_len = strlen((char *)Write_data); // 实际长度=14（不含\0）


    printf("原始数据 (Hex): ");
    for (int i = 0; i < W_data_len; i++) {
        printf("%02X ", Write_data[i]);
    }
    printf("\n");

	printf("read eeprom \n");


    //EEPROM_Status W_status = EEPROM_Write(0x10, test_data, sizeof(test_data));

	EEPROM_Status W_status = EEPROM_Write(0x0, Write_data, strlen((char *)Write_data));

    HAL_Delay(5);

    if(W_status != EEPROM_OK) {
    	printf("Error \n");
      Handle_Error(status);
    }
#endif


#ifdef EEPROM_read
    uint8_t read_data[MAX_INPUT_LEN]={0};
    uint16_t R_data_len ;

    for(int i = 0; i < MAX_INPUT_LEN ; i++) {
        printf("%02X ", read_data[i]);
        if((i+1) % 16 == 0){
        	printf("\n");  // 每16字節換行
        }
      }
      printf("\n\n");

    EEPROM_Status E_status = EEPROM_Read(0x00, read_data,16);


    //printf("read data 2ed= %s\n",read_data);

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

    while(1)
    {

    }
*/
#endif


#ifdef RTC_Write_interr
    // =================================================================================
    // == UART 輸入處理的“基石”變數 ==
    // 這些變數是 UART 接收和行命令處理的基礎，必須存在。
    // =================================================================================
    //uint8_t uart_single_byte_rx_buffer[1];        		// 用於 HAL_UART_Receive_IT 函數的單字節緩衝區

    #define MAX_COMMAND_LEN       32               		// UART 命令行的最大長度 (例如時間字符串)
    #define LINE_BUFFER_SIZE    (MAX_COMMAND_LEN + 1)	// 行緩衝區大小（+1 用於 '\0'）

    char line_buffer[LINE_BUFFER_SIZE];         		// <<<< 基石1: 存儲從 UART 組裝好的一行命令

    volatile uint16_t line_buffer_index = 0;   			// <<<< 基石2: 當前行緩衝區中字符的索引
                                               	   	   	//         (如果ISR直接操作，用volatile)
    volatile bool uart_line_ready = false;       		// <<<< 基石3: 標誌位，true表示line_buffer中有一條完整命令待處理
                                               	   	   	 //         (ISR設置，主循環清除，用volatile)

    // RTC 操作狀態
    HAL_StatusTypeDef global_DT_status; // 用於存儲DS1307操作的返回狀態

    while (1) { // <<<< 主循環開始 >>>>

        // --------------------------------------------------------------------
        // 步驟 1: 檢查是否有新的 UART 命令準備好
        // 這部分邏輯會更新 line_buffer, line_buffer_index,
        // 並可能設置一個像 uart_line_ready 的標誌。
        // 如果你直接在 ISR 中填充 line_buffer (不推薦)，
        // 那麼 line_buffer 和 line_buffer_index 可能會被 ISR 直接修改。
        // 為了簡化，我們假設當 line_buffer 準備好時，
        // 你的 if (line_buffer_index > 0 && ...) 條件會自然為真。
        // --------------------------------------------------------------------
        // 例如，如果使用環形緩衝區和 uart_line_ready 標誌：
        // check_uart_and_assemble_line_if_needed(); // 這個函數會填充 line_buffer 並設置 uart_line_ready

        // --------------------------------------------------------------------
        // 步驟 2: 如果命令準備好了，則執行你的處理邏輯
        // --------------------------------------------------------------------
        // (你提供的、我們已修正的程式碼片段就放在這裡)

        if (uart_line_ready) { // <<<< 修改點A: 使用 uart_line_ready 作為命令就緒的判斷條件
        	DS1307_Time time_data_from_uart_cmd = {0}; // 局部變數存儲解析結果
        	time_data_from_uart_cmd.year = 0xFF;       // 初始化為解析失敗

        	printf("Processing UART input: \"%s\"\r\n", line_buffer); // line_buffer 包含完整命令
        	time_data_from_uart_cmd = Parse_TimeString(line_buffer);

        	if (time_data_from_uart_cmd.year != 0xFF) { // 時間解析成功
        		printf("Debug: time_data_from_uart_cmd.year after parsing = %d\r\n", time_data_from_uart_cmd.year);

        		printf("Attempting to set time via UART...\r\n");

        		global_DT_status = DS1307_SetTime(&time_data_from_uart_cmd); // <<<< 修改點B: 使用 global_DT_status

        		if (global_DT_status == HAL_OK) {

        			printf("時間設定成功 (UART): 20%02u-%02u-%02u(%02u) %02u:%02u:%02u\r\n",
        					time_data_from_uart_cmd.year,
							time_data_from_uart_cmd.month,
							time_data_from_uart_cmd.date,
							time_data_from_uart_cmd.day,
							time_data_from_uart_cmd.hours,
							time_data_from_uart_cmd.minutes,
							time_data_from_uart_cmd.seconds);
        		} else {
        	                    printf("錯誤: 時間設定失敗 (UART) (HAL狀態碼: %d)\r\n", global_DT_status);
        	                }
        	            } else if (strcmp(line_buffer, "get_time") == 0) {

        	                DS1307_Time rtc_time_for_get_cmd_local = {0}; // 局部變數

        	                global_DT_status = DS1307_GetTime(&rtc_time_for_get_cmd_local); // <<<< 修改點B: 使用 global_DT_status

        	                if (global_DT_status == HAL_OK) {
        	                	printf("時間設定成功 (UART): 20%02u-%02u-%02u(%02u) %02u:%02u:%02u\r\n",
        	                	        					time_data_from_uart_cmd.year,
        	                								time_data_from_uart_cmd.month,
        	                								time_data_from_uart_cmd.date,
        	                								time_data_from_uart_cmd.day,
        	                								time_data_from_uart_cmd.hours,
        	                								time_data_from_uart_cmd.minutes,
        	                								time_data_from_uart_cmd.seconds);
        	                } else {
        	                    printf("錯誤: 獲取時間失敗 (UART cmd) (HAL狀態碼: %d)\r\n", global_DT_status);
        	                }
        	            } else {
        	                if (strlen(line_buffer) > 0) {
        	                     printf("UART: Invalid command or time string format: \"%s\"\r\n", line_buffer);
        	                }
        	            }

        	            // **重要**: 處理完命令後，重置 line_buffer, line_buffer_index, 和 uart_line_ready
        	            memset(line_buffer, 0, sizeof(line_buffer));
        	            line_buffer_index = 0;
        	            uart_line_ready = false; // <<<< 修改點C: 清除行就緒標誌
        }

        // --------------------------------------------------------------------
        // 步驟 3: 執行其他循環任務 (例如，定期打印時間，閃爍LED等)
        // --------------------------------------------------------------------

        DS1307_Time rtc_display_time;       // 用於步驟3中定期顯示的時間
        HAL_StatusTypeDef rtc_display_status; // 用於步驟3中獲取時間的狀態
        static uint32_t last_display_tick = 0;  // 用於步驟3的計時 (靜態變數)


        if (HAL_GetTick() - last_display_tick >= 1000) {// 每 1000 毫秒執行一次
        	last_display_tick = HAL_GetTick(); 	// 更新上次執行時間

        	rtc_display_status = DS1307_GetTime(&rtc_display_time); // 獲取當前時間
        	if (rtc_display_status == HAL_OK) {
        		printf("当前RTC时间 (定期显示):\n"); // <<<< 來自你原始碼的打印邏輯
        		printf("--------------------------------\n");
        		printf("時間設定成功 (UART): 20%02u-%02u-%02u(%02u) %02u:%02u:%02u\r\n",
        				rtc_display_time.year,
						rtc_display_time.month,
						rtc_display_time.date,
						rtc_display_time.day,
						rtc_display_time.hours,
						rtc_display_time.minutes,
						rtc_display_time.seconds);
        		printf("--------------------------------\n\n");

        	} else {
        		printf("錯誤: (定期顯示) 獲取時間失敗 (HAL狀態碼: %d)\r\n", rtc_display_status);
        	}
        } // <<<< 步驟3 的 if 結束 >>>>

        // HAL_Delay(1); // 可選，給低優先級任務一點時間，但會增加命令響應延遲
    } // <<<< 主循環結束 >>>>


#endif

#ifdef RTC_Write

	#define SetupTime
    //#define showtime

     //<讀取階段>
    DS1307_Time dt = {0};
    //bool valid_time_provided_by_user = false; // << 標誌用戶是否通過輸入提供了有效時間
	HAL_StatusTypeDef DT_status;


	#ifdef SetupTime
    //char input_str[MAX_INPUT_LEN + 1]; // +1 为终止符'\0'
    char line_buffer_minimal[MAX_INPUT_LEN + 1]; // << 用於從環形緩衝區讀取一次
    uint16_t line_buffer_idx_minimal = 0;

    // 將 dt.year 初始化為一個無效值，表示尚未從用戶獲取有效時間
    // 如果 Parse_TimeString 失敗，dt.year 應該保持或被設為此無效值
    //dt.year = 0xFF;


    printf("input time (YY-MM-DD-W-hh-mm-ss, e.g., 23-08-15-2-10-30-00) or press Enter to skip:\n"); // 提供格式範例
    //</0608 修改>
    // --- 嘗試從環形緩衝區讀取一次命令 ---
    // 給用戶一點時間通過 UART 發送命令
    HAL_Delay(5000); // << 等待5秒，允許 UART 數據進入環形緩衝區
                     // 注意：這仍然是一個阻塞操作，但比無限等待 scanf 要好一點

    memset(line_buffer_minimal, 0, sizeof(line_buffer_minimal));
    line_buffer_idx_minimal = 0;


    /// @param  primask_minimal：
    ///聲明一個 uint32_t 類型的變數 primask_minimal，用於稍後保存中斷狀態，
    /// 以便在訪問共享資源（環形緩衝區）時創建臨界區。
    uint32_t primask_minimal;


    // 開始一個 while 循環，這個循環會持續執行，直到滿足某些跳出條件。
    // 循環條件是：line_buffer_idx_minimal（當前行緩衝區的索引）小於 MAX_INPUT_LEN_MINIMAL（行緩衝區的最大允許長度）。
    // 這是為了防止行緩衝區溢出。
    while (line_buffer_idx_minimal < MAX_INPUT_LEN) {
        uint8_t byte_from_ring_m;   // 用於存儲從環形緩衝區讀取出來的單個字節。
        bool got_byte_m = false;    // 布爾標誌，用於標記本次循環是否成功從環形緩衝區讀取到了字節。

        // --- 進入臨界区：準備讀取環形緩衝區 ---
        // 環形緩衝區通常由中斷服務程序（ISR，如 UART 接收中斷）寫入，
        // 並由主程序讀取。為了避免在讀取過程中數據被中斷修改導致不一致，
        // 需要暫時禁用中斷。
        primask_minimal = __get_PRIMASK(); // 保存當前的全局中斷使能狀態 (PRIMASK 寄存器)。
        __disable_irq();                   // 禁用所有可屏蔽中斷。

        // 檢查環形緩衝區是否為空。
        // rx_buf_is_empty() 是一個假設存在的函數，它會返回 true 如果環形緩衝區是空的。
        if (!rx_buf_is_empty()) {
            // 如果環形緩衝區不為空，則從中讀取一個字節。
            // rx_buf_get() 是一個假設存在的函數，它會從環形緩衝區的尾部取出一個字節。
            byte_from_ring_m = rx_buf_get();
            got_byte_m = true; // 標記成功讀取到一個字節。
        }
        // --- 退出臨界区：環形緩衝區讀取操作完成 ---
        __set_PRIMASK(primask_minimal); // 恢復之前保存的中斷使能狀態。

        // 判斷是否真的從環形緩衝區讀取到了數據。
        if (got_byte_m) {
            // 如果成功讀取到一個字節 (byte_from_ring_m)。

            // 檢查讀取到的字節是否是行結束符（回車符 '\r' 或換行符 '\n'）。
            if (byte_from_ring_m == '\r' || byte_from_ring_m == '\n') {
                // 如果是行結束符：
                // 檢查當前行緩衝區中是否已經有數據（line_buffer_idx_minimal > 0）。
                // 這樣做是為了處理可能連續出現的換行符，或者忽略僅有換行符的空行（如果第一個字符就是換行）。
                if (line_buffer_idx_minimal > 0) {
                    // 如果行緩衝區中已有數據，則認為一行已經完整結束。
                    break; // 跳出当前的 while 循環，停止讀取更多字符。
                }
                // 如果 line_buffer_idx_minimal 為 0（即這是行的第一個字符且是換行符），
                // 則循環會繼續，實際上是忽略了這個開頭的空行。
                // （根據需求，這裡也可以選擇直接 break 或者做其他處理）。
            } else if (isprint(byte_from_ring_m)) { // 檢查讀取到的字節是否是可打印字符。
                                                    // isprint() 函數來自 <ctype.h>。
                // 如果是可打印字符：
                // 將該字符存儲到行緩衝區 line_buffer_minimal 的當前索引位置。
                line_buffer_minimal[line_buffer_idx_minimal] = byte_from_ring_m;
                // 然後將索引 line_buffer_idx_minimal 遞增，指向下一個空閒位置。
                line_buffer_idx_minimal++;
            }
            // else {
            //   // 如果字符既不是行結束符，也不是可打印字符（例如其他控制字符），
            //   // 這段簡化的程式碼會忽略它，循環繼續。
            //   // 根據需求，這裡也可以添加對特定控制字符（如退格）的處理。
            // }
        } else {
            // 如果 got_byte_m 為 false，表示環形緩衝區已經空了，沒有更多數據可以讀取。
            break; // 跳出当前的 while 循環。
        }
    } // while 循環結束

    // 在循環結束後（無論是因為讀到行尾、緩衝區滿，還是環形緩衝區空了），
    // 在 line_buffer_minimal 的當前索引位置（line_buffer_idx_minimal）添加字符串結束符 '\0'。
    // 這確保了 line_buffer_minimal 總是一個有效的 C 語言字符串。
    // 即使 while 循環因為 line_buffer_idx_minimal == MAX_INPUT_LEN_MINIMAL 而退出，
    // 這裡也會在 line_buffer_minimal[MAX_INPUT_LEN_MINIMAL]（即數組的最後一個有效位置的前一個，如果MAX_INPUT_LEN_MINIMAL是大小-1）
    // 或 line_buffer_minimal[實際長度] 放置 '\0'。
    // 嚴格來說，如果循環是因為 line_buffer_idx_minimal 達到 MAX_INPUT_LEN_MINIMAL 而跳出，
    // 且 MAX_INPUT_LEN_MINIMAL 是緩衝區大小減1（為'\0'留位），那麼這裡的索引是正確的。
    // 如果 MAX_INPUT_LEN_MINIMAL 是緩衝區實際能存儲字符的最大個數，
    // 那麼 line_buffer_minimal 數組的大小應為 MAX_INPUT_LEN_MINIMAL + 1。
    line_buffer_minimal[line_buffer_idx_minimal] = '\0';



    //<0615修改>

    bool user_provided_valid_time_for_setup = false; // << 新增：明確的標誌

    // --- 判斷是否從 UART 獲取到有效輸入 ---
     if (line_buffer_idx_minimal > 0) { // 如果從 UART 讀到了東西
         printf("Received from UART: \"%s\"\r\n", line_buffer_minimal);
         DS1307_Time parsed_dt_from_uart_setup  = Parse_TimeString(line_buffer_minimal);

         if (parsed_dt_from_uart_setup.year  != 0xFF) { // 解析成功
             //dt = parsed_dt_uart;
             dt = parsed_dt_from_uart_setup;
             user_provided_valid_time_for_setup = true ;
             printf("Time parsed from UART.\r\n"); // << 將解析成功的時間賦給 dt
         } else {
             printf("Invalid time format from UART. Skipping UART input.\r\n");
             // dt.year 保持 0xFF，後續邏輯會認為不需要校正或使用默認
         }
     } else {
         printf("No input from UART, or input was empty. Skipping UART input.\r\n");
         // dt.year 保持 0xFF
     }



     // --- 寫入階段：基於 user_provided_valid_time_for_setup 決定 ---

     //if (memcmp(&dt, &(DS1307_Time){0}, sizeof(dt)) == 0) {
    if (user_provided_valid_time_for_setup) { // <<<< 關鍵修改：使用新的布爾標誌判斷
        printf("Setting RTC with time provided by user during Setup...\r\n");

    	 DT_status = DS1307_SetTime(&dt);

    	 printf("set time");
    	 if (DT_status == HAL_OK) {
    		 printf("時間設定成功: 20%02d-%02u-%02u(%02u) %02u:%02u:%02u\r\n",
    					dt.year, dt.month, dt.date, dt.day, dt.hours , dt.minutes, dt.seconds);
    	 } else {
    		 printf("錯誤: 時間設定失敗 (HAL狀態碼: %d)\r\n", DT_status);
    	 }

    	 //memset(input_str, 0, sizeof(input_str)); // 清空輸入緩衝
    	}else{
    		// 如果 dt.year 仍然是 0xFF，表示 UART 沒有提供有效時間，
    		// 並且程序也沒有預設有效時間。
    		// 這裡可以選擇：
    		// 1. 報錯並提示必須校時 (但這又回到了如何輸入的問題)
    		// 2. 使用一個硬編碼的默認時間（不推薦長期）
    		// 3. 什麼都不做，讓 RTC 保持其當前（可能無效的）時間
    		printf("不需要校正時間:");
     }

     // 總是在最後獲取並打印一次時間
 	DT_status = DS1307_GetTime(&dt); // 重新獲取時間，看看最終結果
 	if (DT_status == HAL_OK) {
 		printf("當前 RTC 時間 (校正後或原始): 20%02u-%02u-%02u(%02u) %02u:%02u:%02u\r\n",
 				dt.year,dt.month,dt.date,dt.day,dt.hours,dt.minutes,dt.seconds);
 	} else {
 		printf("錯誤: 獲取最終 RTC 時間失敗 (HAL狀態碼: %d)\r\n", DT_status);
 	}


     //</0608 修改>

 	#endif

#ifdef showtime
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

#endif




#ifdef RTC_Read
      // 初始化成功后的操作
    //

 	volatile bool uart_command_ready_main = false;
 	static uint32_t last_showtime_tick = 0;

    while(1){

        // --- 步驟 1 (行組裝): 從環形緩衝區讀取字節並組裝到 line_buffer_main ---
        // <<< 修改：這個 if 條件保持，但其內部邏輯可能需要調整為能一次處理多個字節，或者讓主循環快速迭代 >>>
    	if (!uart_command_ready_main) { // 只有當上一條命令處理完畢才組裝新命令
            // <<< 新增：內部循環以盡快處理環形緩衝區中的所有可用字節 >>>
    		while (!uart_command_ready_main && !rx_buf_is_empty()) { // 會持續讀取直到一行完成或緩衝區空

    			uint8_t byte_from_ring;
    			bool got_byte_in_inner_loop = false;

    			uint32_t primask = __get_PRIMASK();
    			__disable_irq();

    			if (!rx_buf_is_empty()) { // 使用你 ring_buffer.c 的函數
    				byte_from_ring = rx_buf_get(); // 使用你 ring_buffer.c 的函數
    				got_byte_in_inner_loop = true;
    			}
    			__set_PRIMASK(primask);

    			if (got_byte_in_inner_loop) {
    				if (byte_from_ring == '\r' || byte_from_ring == '\n'){

    					if (line_buffer_index_main > 0){ // 有內容才算一行
    						line_buffer_main[line_buffer_index_main] = '\0'; // 結束字符串
    						uart_command_ready_main = true; // << 命令準備好了
                            break; // << 新增：一行組裝完成，跳出內部while
    					}
    				} else if (isprint(byte_from_ring)) { // 需要 #include <ctype.h>
    					if (line_buffer_index_main < LINE_BUFFER_SIZE_MAIN - 1) {
    						line_buffer_main[line_buffer_index_main++] = byte_from_ring;
    					} else { // 行緩衝區滿
    						line_buffer_main[LINE_BUFFER_SIZE_MAIN - 1] = '\0';
 	            			uart_command_ready_main = true;
 	            			printf("Warning: UART line_buffer_main overflow, command truncated.\r\n");
                            break; // << 新增：行緩衝區滿，跳出內部while

 	            		}
    				} else if (byte_from_ring == '\b' || byte_from_ring == 0x7F) { // 退格
 	            		if (line_buffer_index_main > 0) {
 	            			line_buffer_index_main--;
 	            		}
    				}
    				// 新增：如果行緩衝區滿了但還沒遇到換行符，也強制結束
    				if (line_buffer_index_main >= LINE_BUFFER_SIZE_MAIN - 1 && !uart_command_ready_main) {
    					line_buffer_main[LINE_BUFFER_SIZE_MAIN - 1] = '\0';
    					uart_command_ready_main = true;
    					printf("Warning: UART line_buffer_main full, command forced.\r\n");
    					break; // 跳出內部 while
    				}
    			} else {
                    break; // 環形緩衝區空了，跳出內部 while
                }// <<< 內部 while 結束 >>>
    		}

    		// --- 新增：步驟2 (命令處理): 如果一行命令準備好了，則處理它 ---
    		// <<< 修改：將此 if 塊移到行組裝的 if 塊之外，使其獨立判斷 >>>


    		if (uart_command_ready_main) {
    			printf("UART CMD RX (in while(1)): \"%s\"\r\n", line_buffer_main);
    			DS1307_Time parsed_time_cmd_loop = {0}; // 局部變數
    			parsed_time_cmd_loop.year = 0xFF; // 標記解析失敗

    			if (strcmp(line_buffer_main, "get_time") == 0) {
    				printf("Executing 'get_time' command (in while(1))...\r\n");
    				DT_status = DS1307_GetTime(&dt); // << 使用你已有的 dt 和 DT_status
    				if (DT_status == HAL_OK) {
    					printf("Current RTC Time: 20%02u-%02u-%02u(%02u) %02u:%02u:%02u\r\n",
    							dt.year, dt.month, dt.date, dt.day,
								dt.hours, dt.minutes, dt.seconds);
    				} else {
    					printf("Error executing 'get_time': Failed to read RTC (HAL: %d)\r\n", DT_status);
    				}
    			} else { // 嘗試解析為時間設置命令
    				parsed_time_cmd_loop = Parse_TimeString(line_buffer_main);
    				if (parsed_time_cmd_loop.year != 0xFF) { // 時間成功被解析
    					printf("Parsed valid time from UART (in while(1)). Setting RTC...\r\n");
    					dt = parsed_time_cmd_loop; // << 更新你已有的 dt
    					DT_status = DS1307_SetTime(&dt); // << 使用你已有的 DT_status
    					if (DT_status == HAL_OK) {
    						printf("RTC time set successfully (in while(1)): 20%02u-%02u-%02u(%02u) %02u:%02u:%02u\r\n",
    								dt.year, dt.month, dt.date, dt.day,
									dt.hours, dt.minutes, dt.seconds);
    					} else {
    						printf("Error setting RTC with parsed time (in while(1)) (HAL: %d)\r\n", DT_status);
    					}
    				} else { // 解析失敗且不是已知命令 "get_time"
    					if (strlen(line_buffer_main) > 0) {
    						printf("UART (in while(1)): Invalid command or time string format: \"%s\"\r\n", line_buffer_main);
    						printf("Usage: YY-MM-DD-W-hh-mm-ss  OR  get_time\r\n");
    					}
    				}
    			}
    	     		// 重置以便接收下一條命令
    	     		memset(line_buffer_main, 0, sizeof(line_buffer_main));
    	     		line_buffer_index_main = 0;
    	     		uart_command_ready_main = false;
    	     	}
    	     	        // --- 命令處理邏輯結束 ---


    	     	 if (HAL_GetTick() - last_showtime_tick >= 1000) { // 每 1000 毫秒執行一次
    	     		 last_showtime_tick = HAL_GetTick(); // 更新上次執行時間
    	     		 DT_status = DS1307_GetTime(&dt); // << 你原有的獲取時間

    	     		 if (DT_status == HAL_OK) {
    	     			 printf("Showtime: 20%02u-%02u-%02u(%02u) %02u:%02u:%02u\r\n",
    	     					 dt.year,dt.month,dt.date,dt.day,dt.hours,dt.minutes,dt.seconds);
    	    	     	} else {
    	    	     		printf("錯誤 (Showtime): 時間獲取失敗 (HAL狀態碼: %d)\r\n", DT_status);
    	    	     	}
    	     	 }
    	}
    }
	#endif


    //<0608版本,可用>
#ifdef readtime_0608
 	DS1307_Time current_time;
    while (1)
    {
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
               	 HAL_Delay(1000);
        }

#endif
    //</0608版本,可用>



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
