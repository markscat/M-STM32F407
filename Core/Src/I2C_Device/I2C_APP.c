/*
 * I2C_APP.c
 *
 *  Created on: 2026年10月19日
 *      Author: Ethan
 */

#include "I2C_Device\I2C_Peripherals.h"
#include <stdio.h>
#include <I2C_Device\I2C_APP.h>
//#include "Ext_ADC\ads1115.h"

//#define ADC_1115

/* 外部依賴：UART ring buffer 處理（定義在 uart_io.c / Tools.c） */
extern void ProcessUartRingBuffer(void);
extern void HandleUartCommands(void);

/* 外部依賴：時間結構檢查（定義在 I2C_Peripherals.c 或 Tools.c） */
extern bool CheckTimeStruct(const RTC_Time* t);

/**
  * @brief  掃描 I2C 匯流排上的所有裝置，並初始化已知的 I2C 設備
  * @retval I2C_OK        成功
  * @retval I2C_ERR_INIT  初始化失敗（I2C_Init_Devices 回傳非 OK）
  * @note   本函式不包含延遲，呼叫端可依需求決定失敗後的行為。
  */
I2C_Status APP_I2C_Init(void)
{
    printf("\n*************Scan_I2C_Address********************\n");
    printf("Scan All I2C Device Address \n");
    //Scan_I2C_Address();

    bool scan_success = Scan_I2C_Address();

    // 判斷結果
    if (scan_success == false) {
        printf("\n[FATAL] 匯流排上沒有任何 I2C 設備，系統停止！\n");

        // 停下來，不要讓它去跑後面的 TCA9548_SelectByAddress 或是 Init
        while (1) {
            HAL_Delay(1000);
        }
    }
    printf("\n************************************************\n");

    printf("\n**************I2C init. Device *****************\n");

    I2C_Status status = I2C_Init_Devices();

    if (status != I2C_OK) {
        printf("[ERROR] I2C device init failed! status = %d\n", status);
        return status;
    }

    printf("I2C device init ok!\n\n");
    printf("\n************System Ready.********************2******\n");

    return I2C_OK;
}


/* ---- RTC 互動式設定（會阻塞等待 UART 輸入）---- */
HAL_StatusTypeDef APP_RTC_SetupInteractive(void){

	char input_str[MAX_INPUT_LEN + 1];
	char YandN;

	printf("need setup time ? [Y]/[N] \n");
	scanf(" %c", &YandN);
	if (YandN == 'Y' || YandN == 'y') {
		printf("\n input time :\n");
		scanf("%32s", input_str);
		printf("Received_time : %s\r\n\n", input_str);

		RTC_Time rtc_dt = Parse_TimeString(input_str);
		if (CheckTimeStruct(&rtc_dt)) {
			printf("set time... \n");
			HAL_StatusTypeDef st = RTC_SetTime(&rtc_dt);
			if (st == HAL_OK) {
				printf("時間設定成功: 20%02d-%02u-%02u(%02u) %02u:%02u:%02u\r\n",
						rtc_dt.year, rtc_dt.month, rtc_dt.date, rtc_dt.day,
						rtc_dt.hours, rtc_dt.minutes, rtc_dt.seconds);
			} else {
				printf("錯誤: 時間設定失敗 (HAL狀態碼: %d)\r\n", st);
			}
			return st;
		} else {
			printf("Invalid time format, skip setting.\n");
			return HAL_ERROR;
		}
	} else if (YandN == 'N' || YandN == 'n') {
		printf("show Time \n\n");
	} else {
		printf("No input, no time correction, display time\n\n");
	}
	return HAL_OK;
}

/* ---- RTC 時間 + 溫度顯示（單次，不迴圈）---- */
void APP_RTC_ShowTimeAndTemp(void){
	static const char* fracStr[] = {"00", "25", "50", "75"};
	RTC_Time rtc_dt = {0};
	DS3231_Temp temp = {0};

	HAL_StatusTypeDef st = RTC_GetTime(&rtc_dt);
	if (st == HAL_OK) {
		printf("現在時間: 20%02d-%02u-%02u(%02u) %02u:%02u:%02u\r\n",
				rtc_dt.year, rtc_dt.month, rtc_dt.date, rtc_dt.day,
				rtc_dt.hours, rtc_dt.minutes, rtc_dt.seconds);
	} else {
		printf("錯誤: 時間讀取失敗 (HAL狀態碼: %d)\r\n", st);
	}

	st = DS3231_GetTemp(&temp);
	if (st == HAL_OK) {
		printf("現在溫度: %d.%s °C\r\n", temp.Integer, fracStr[temp.Fraction]);
	}
}

/* ---- UART ring buffer 輪詢（單次，不迴圈）---- */
void APP_UART_Poll(void){
    ProcessUartRingBuffer();
    HandleUartCommands();
}

#ifdef ADC_1115

//#include "Ext_ADC/ads1115.h"

/* App 記住「目前設定」 */

static uint16_t s_mux        = ADS1115_MUX_AIN0;
static uint16_t s_pga        = ADS1115_PGA_ONE;
static uint16_t s_dataRate   = ADS1115_DATA_RATE_128;
static uint8_t  s_continuous = 0;   /* 0=單次, 1=連續 */

HAL_StatusTypeDef APP_ADS1115_Config(uint16_t mux, uint16_t pga,
                                     uint16_t dataRate, uint8_t continuous)
{
    s_mux        = mux;
    s_pga        = pga;
    s_dataRate   = dataRate;
    s_continuous = continuous;

//因為這邊又跑到初始化去了,為什麼需要初始化???
    //I2C_ADS1115_Status init_status = I2C_ADS1115_Init(&hi2c1, dataRate, pga);

    /* 初始化（設定 PGA / 取樣率） */
  /*  if (init_status != I2C_ADS1115_OK) {
    	// 這裡可以順便印出詳細錯誤，或者交給 Wrapper 去印
    	printf("[APP] ADS1115 Init failed! Code: %d\n", init_status);
    	return HAL_ERROR;
    }

*/
    ADS1115_UpdateVoltCoef(pga);


    /* 連續模式：立刻啟動轉換 */
    if (continuous) {
        return I2C_ADS1115_StartContinuous(mux, pga, dataRate);
    }
    return HAL_OK;

}

HAL_StatusTypeDef APP_ADS1115_Read(float *voltage)
{

    if (s_continuous) {
        return I2C_ADS1115_ReadContinuous(voltage);
    } else {
        return I2C_ADS1115_readOneShot(s_mux, voltage);
    }
}



#endif

#define PWM_Servo_motor

#ifdef PWM_Servo_motor
#include <stdlib.h> // 需要用到 abs() 絕對值函式

// 平滑轉動函式
// channel: 通道
// target_angle: 目標角度 (0~180)
// step_delay_ms: 每一步之間的延遲時間 (數字越大，轉越慢)
void PCA9685_MoveSlowly(uint8_t channel, uint8_t target_angle, uint16_t step_delay_ms) {
    // 靜態變數，用來記住每一顆舵機「目前」在哪個角度
    static int8_t current_angle[16] = {90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90};

    // 防呆
    if (channel > 15) return;
    if (target_angle > 180) target_angle = 180;

    int current = current_angle[channel];
    int target = target_angle;

    // 如果目標和現在一樣，就不用動了
    if (current == target) return;

    // 判斷要增加還是減少角度
    int step = (target > current) ? 1 : -1;

    // 開始一步一步轉
    while (current != target) {
        current += step;

        // 把角度換算成 Ticks (0度=102, 180度=512)
        uint16_t ticks = 102 + ((uint16_t)current * 410 / 180);

        // 送給 PCA9685
        PCA9685_SetPWM(channel, ticks);

        // 停頓一下，這就是控制「速度」的關鍵
        HAL_Delay(step_delay_ms);
    }

    // 更新記憶體中這顆舵機的最新角度
    current_angle[channel] = target;
}
#endif


/* ---- 時鐘顯示到 OLED ---- */

/**
 * @brief 將 RTC 時間格式化並顯示在 OLED 上
 * @param rtc_dt  RTC 時間結構體指標
 * @param temp    DS3231 溫度結構體指標（若為 NULL 則不顯示溫度）
 */
void APP_OLED_ShowClock(const RTC_Time *rtc_dt, const DS3231_Temp *temp)
{

#ifdef use_TCA9548
	//切換頻道到OLED
	TCA9548_SelectByAddress(0x3c);

#endif

    char line_buf[22];
    static const char* fracStr[] = {"00", "25", "50", "75"};
    static const char* weekStr[] = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};

    /* 清空螢幕緩衝區 */
    SH1106_Fill(SH1106_COLOR_BLACK);

    /* 第一行：日期  YYYY-MM-DD (Www) */
    snprintf(line_buf, sizeof(line_buf), "20%02d-%02d-%02d %s",
             rtc_dt->year, rtc_dt->month, rtc_dt->date,
             (rtc_dt->day >= 1 && rtc_dt->day <= 7) ? weekStr[rtc_dt->day - 1] : "---");

    SH1106_GotoXY(0, 0);

    SH1106_Puts(line_buf, &Font_7x10, SH1106_COLOR_WHITE);

    /* 第二行：時間  HH:MM:SS  （用 11x18 大字體讓時間更醒目） */
    snprintf(line_buf, sizeof(line_buf), "%02d:%02d:%02d",
             rtc_dt->hours, rtc_dt->minutes, rtc_dt->seconds);
    SH1106_GotoXY(0, 14);
    SH1106_Puts(line_buf, &Font_11x18, SH1106_COLOR_WHITE);

    /* 第三行：溫度（若有提供） */
    if (temp != NULL) {
        snprintf(line_buf, sizeof(line_buf), "Temp: %d.%s C",
                 temp->Integer, fracStr[temp->Fraction & 0x03]);
        SH1106_GotoXY(0, 40);
        SH1106_Puts(line_buf, &Font_7x10, SH1106_COLOR_WHITE);
    }

    /* 刷到螢幕 */
    SH1106_UpdateScreen();
}


/**
 * @brief 從 RTC 讀取時間並顯示到 OLED（含溫度）
 * @retval HAL_OK 成功；其他表示時間讀取失敗
 */

HAL_StatusTypeDef APP_OLED_UpdateClock(void)
{
    RTC_Time rtc_dt = {0};
    DS3231_Temp temp = {0};
    HAL_StatusTypeDef st;

    st = RTC_GetTime(&rtc_dt);
    if (st != HAL_OK) {
        return st;
    }

    /* 溫度讀取失敗不影響時間顯示，所以不檢查返回值 */
    DS3231_GetTemp(&temp);

    APP_OLED_ShowClock(&rtc_dt, &temp);
    return HAL_OK;
}



/**
 * @brief 將 RTC 時間格式化並顯示在 LCM 上
 * @param rtc_dt  RTC 時間結構體指標
 * @param temp    DS3231 溫度結構體指標（若為 NULL 則不顯示溫度）
 */

void APP_LCM_ShowClock(const RTC_Time *rtc_dt, const DS3231_Temp *temp) {

#ifdef use_TCA9548
    // 切換通道到 LCM (PCF8574 位址是 0x27)
    TCA9548_SelectByAddress(0x27);
#endif

    char line_buf[22];
    static const char* fracStr[] = {"00", "25", "50", "75"};
    static const char* weekStr[] = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};

    // --- 第一行：顯示日期與星期 ---
    // 格式：20YY-MM-DD Mon
    snprintf(line_buf, sizeof(line_buf), "20%02d-%02d-%02d %s",
             rtc_dt->year, rtc_dt->month, rtc_dt->date,
             (rtc_dt->day >= 1 && rtc_dt->day <= 7) ? weekStr[rtc_dt->day - 1] : "---");

    LCM_SetCursor(0, 0);   // 游標移到第一行開頭
    LCM_Print(line_buf);

    // --- 第二行：顯示時間與溫度 ---
    // 格式：HH:MM:SS 25.50°C
    if (temp != NULL) {
        snprintf(line_buf, sizeof(line_buf), "%02d:%02d:%02d %d.%sC",
                 rtc_dt->hours, rtc_dt->minutes, rtc_dt->seconds,
                 temp->Integer, fracStr[temp->Fraction]);
    } else {
        snprintf(line_buf, sizeof(line_buf), "%02d:%02d:%02d",
                 rtc_dt->hours, rtc_dt->minutes, rtc_dt->seconds);
    }

    LCM_SetCursor(1, 0);   // 游標移到第二行開頭
    LCM_Print(line_buf);
}


HAL_StatusTypeDef APP_LCM_UpdateClock(void){
    RTC_Time rtc_dt = {0};
    DS3231_Temp temp = {0};
    HAL_StatusTypeDef st;

    st = RTC_GetTime(&rtc_dt);
    if (st != HAL_OK) {
        return st;
    }

    /* 溫度讀取失敗不影響時間顯示，所以不檢查返回值 */
    DS3231_GetTemp(&temp);

    APP_LCM_ShowClock(&rtc_dt, &temp);
    return HAL_OK;
}
