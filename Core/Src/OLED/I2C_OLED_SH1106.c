/**
 * original author:  Tilen Majerle<tilen@majerle.eu>
 * modification for SH1106: ControllersTech (www.controllerstech.com)

   ----------------------------------------------------------------------
   	Copyright (C) Alexander Lutsai, 2016
    Copyright (C) Tilen Majerle, 2015

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------
 */
#include <string.h>
#include <main.h>
//#include <OLED\I2C_OLED_fonts.h>
#include <OLED\Font\I2C_OLED_fonts.h>

#include <OLED\I2C_OLED_SH1106.h>
#include "I2C_Device\I2C_TCA9548.h"
#include <stdlib.h> // <--- 【添加這一行】
#include "log.h"
extern I2C_HandleTypeDef hi2c1;

// --- 內部 (static) 變數 ---

/* Private SH1106 structure */

typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} SH1106_t;



//<250803 新增>
#define OLED_COL_OFFSET 2
//</250803 新增>

#define SH1106_I2C &hi2c1
static uint8_t SH1106_Buffer[SH1106_WIDTH * SH1106_HEIGHT / 8];
static SH1106_StatusTypeDef SH1106_WriteCommand(uint8_t command);

//static void SH1106_WriteData(uint8_t* data, uint16_t size); // <--- [修改] 這是我們統一使用的內部數據發送函式
SH1106_StatusTypeDef OLED_WriteData(uint8_t *data, uint16_t size);

/* Private variable */
static SH1106_t SH1106; // 狀態變數 (CurrentX, CurrentY 等)



/**
 * @brief  【已实现】将 HAL_StatusTypeDef 转换为 SH1106_StatusTypeDef
 * @param  hal_status: 从 HAL 库函数返回的状态码
 * @retval 对应的 SH1106 状态码
 */
static SH1106_StatusTypeDef SH1106_Translate_HAL_Status(HAL_StatusTypeDef hal_status) {
    switch(hal_status) {
        case HAL_OK:
            return SH1106_OK;

        case HAL_BUSY:
            return SH1106_ERROR_BUSY;

        case HAL_TIMEOUT:
            return SH1106_ERROR_TIMEOUT;

        case HAL_ERROR:
        default:
            return SH1106_ERROR;
    }
}

/**
 * @brief  【統一】發送一個命令字節
 */

static SH1106_StatusTypeDef SH1106_WriteCommand(uint8_t command) {
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1, SH1106_I2C_ADDR, 0x00, 1, &command, 1, 100);

    if (status != HAL_OK) {
        // 在返回错误之前，先打印详细的日志！
        LOG_ERROR("I2C WriteCommand failed for CMD=0x%02X, HAL_Status=%d", command, status);
        return SH1106_Translate_HAL_Status(status); // 假设有这个转换函数
    }
    return SH1106_OK;
}



/**
 * @brief  【統一】發送一塊數據
 */

// 傳送資料（Data，可傳整個 buffer）
SH1106_StatusTypeDef OLED_WriteData(uint8_t *data, uint16_t size) {
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(SH1106_I2C, SH1106_I2C_ADDR, 0x40, 1, data, size, HAL_MAX_DELAY);
    return SH1106_Translate_HAL_Status(status);
}

// --- 發送命令的輔助函式 ---
void OLED_Write_CMD(uint8_t cmd)
{
    uint8_t data_to_send[2] = {0x00, cmd}; // 0x00 代表接下來是 Command
    HAL_I2C_Master_Transmit(SH1106_I2C, SH1106_I2C_ADDR, data_to_send, 2, HAL_MAX_DELAY);
}

void SH1106_ShowRawBuffer(const uint8_t* buffer) {
    uint8_t temp[132];  // 非 const

    for (uint8_t page = 0; page < 8; page++) {
        memcpy(temp, &buffer[page * 132], 132);
        SH1106_WriteCommand(0xB0 + page);
        SH1106_WriteCommand(0x00 | (2 & 0x0F)); // column offset = 2
        SH1106_WriteCommand(0x10 | ((2 >> 4) & 0x0F));

        //HAL_I2C_Mem_Write(&hi2c1, SH1106_I2C_ADDR, 0x40, 1, &buffer[page * 132], 132, HAL_MAX_DELAY);
        HAL_I2C_Mem_Write(&hi2c1, SH1106_I2C_ADDR, 0x40, 1, temp, 132, HAL_MAX_DELAY);

    }
}

/**
 * OLED 驅動 IC 在初始化時，必須照 datasheet 的上電順序，尤其是：
 * Display OFF
 * Clock / Multiplex / Offset / Segment remap / COM scan
 * Contrast / Precharge / VCOM
 * Charge Pump
 * Display ON
 *
 *
 *+---------------------+
 *|       Start         |
 *+----------+----------+
 *         |
 *         v
*+---------------------+
*|  Display OFF (0xAE) |
*+----------+----------+
*           |
*           v
*+-----------------------------+
*|  Set Charge Pump (0x8D)     |
*|  Enable Charge Pump (0x14)  |
*+----------+------------------+
*           |
*           v
*+-----------------------------+
*|  Set Contrast (0x81) + val  |
*+----------+------------------+
*           |
*           v
*+-----------------------------+
*|  Set Multiplex Ratio (0xA8) |
*+----------+------------------+
*           |
*           v
*+-----------------------------+
*|  Set Display Offset (0xD3)  |
*+----------+------------------+
*           |
*           v
*+-------------------------------+
*|  Set Display Clock (0xD5)      |
*+----------+--------------------+
*           |
*           v
*+-------------------------------+
*|  Set Pre-charge Period (0xD9)  |
*+----------+--------------------+
*           |
*           v
*+----------------------------+
*|  Set VCOM Detect (0xDB)    |
*+----------+-----------------+
*           |
*           v
*+----------------------------+
*|  Set Segment Re-map (0xA0/1)|
*+----------+-----------------+
*           |
*           v
*+--------------------------------+
*| Set COM Scan Direction (0xC0/8)|
*+----------+---------------------+
*           |
*           v
*+-----------------------+
*|  Set Start Line (0x40)|
*+----------+------------+
*           |
*           v
*+---------------------+
*|  Clear Display RAM  |
*+----------+----------+
*           |
*           v
*+---------------------+
*|  Display ON (0xAF)  |
*+----------+----------+
*           |
*           v
*+---------------------+
*|        End          |
*+---------------------+
*
 *
 * **/
#ifdef Ver_SH1106

// ... (宏定义部分不变) ...

SH1106_StatusTypeDef SH1106_Init(void) {

    if (HAL_I2C_IsDeviceReady(&hi2c1, SH1106_I2C_ADDR, 1, 200) != HAL_OK) {
        LOG_ERROR("Device not found at I2C address 0x%02X", SH1106_I2C_ADDR);
        return SH1106_ERROR_INIT_DEV_NOT_FOUND;
    }

    HAL_Delay(100);
    LOG_INFO("SH1106 Device found. Sending init sequence...");

    SH1106_WriteCommand(OLED_DISPLAY_OFF);

    // --- 1. 核心时钟和布局配置 ---
    SH1106_WriteCommand(OLED_SET_CLK_DIV);
    SH1106_WriteCommand(INIT_CLK_DIV);

    SH1106_WriteCommand(OLED_SET_MUX_RATIO);
    SH1106_WriteCommand(INIT_MUX_RATIO);

    SH1106_WriteCommand(OLED_SET_DISPLAY_OFFSET);
    SH1106_WriteCommand(INIT_DISPLAY_OFFSET);

    //SH1106_WriteCommand(OLED_SET_START_LINE | 0x00); // 明确设置起始行为 0

    // --- 2. SH1106 专属配置 (关键) ---
    // 设置 Column 起始地址为 2，以在 132 宽的 RAM 中居中显示 128 宽的图像
    // 这等同于 SH1106_COLUMN_OFFSET 的作用，但在 Init 中设置更彻底
    SH1106_WriteCommand(0x02); // Set Lower Column Address
    SH1106_WriteCommand(0x10); // Set Higher Column Address

    // --- 3. 硬件扫描方向 ---
    SH1106_WriteCommand(OLED_SEG_REMAP_127); // Column 129 mapped to SEG0
    SH1106_WriteCommand(OLED_SCAN_DIR_REMAP); // Scan from COM[N-1] to COM0

    SH1106_WriteCommand(OLED_SET_COM_CONFIG);
    SH1106_WriteCommand(INIT_COM_CONFIG);

    // --- 4. 电压和亮度配置 ---
    SH1106_WriteCommand(OLED_SET_DC_DC_MODE);
    SH1106_WriteCommand(OLED_DC_DC_ON); // 打开内置电荷泵

    SH1106_WriteCommand(OLED_SET_CONTRAST);
    SH1106_WriteCommand(INIT_CONTRAST_VALUE);

    SH1106_WriteCommand(OLED_SET_PRECHARGE);
    SH1106_WriteCommand(INIT_PRECHARGE);

    SH1106_WriteCommand(OLED_SET_VCOMH);
    SH1106_WriteCommand(INIT_VCOMH);

    SH1106_WriteCommand(OLED_SET_PUMP_VOLTAGE | INIT_PUMP_VOLTAGE); // 明确设置电荷泵电压

    // --- 5. 最终显示模式设置 ---
    SH1106_WriteCommand(OLED_DISPLAY_ALL_ON_RESUME); // 显示 RAM 内容
    SH1106_WriteCommand(OLED_NORMAL_DISPLAY);       // 正常模式

    SH1106_WriteCommand(OLED_DISPLAY_ON);           // 开显示

    // ... (软件状态初始化和清屏) ...
    SH1106.Initialized = 1;
    SH1106_Fill(SH1106_COLOR_BLACK);
    SH1106_UpdateScreen();

    LOG_INFO("Init sequence sent successfully.");
    return SH1106_OK;
}
#endif

/**
 * 這個函數主教是作一個包裝(wrapper)
 * 因為在做函數初始化的時候,型別和其他的設備型別不同,所以必須做個包裝
 *
 * */
I2C_Status SH1106_DeviceInit(void)
{
#ifdef use_TCA9548

	uint8_t ch=0xFF;

		  for(uint8_t i= 0; i<g_scan_count ; i++){
			  if (g_scan_result[i].Peripheral_address == 0x3C)	{
				  ch=g_scan_result[i].TCA9548_CH;
				  printf("SH1106 OLED ch = %d, Address =  0x%02X\n ",ch,g_scan_result[i].Peripheral_address);
		          break; // 找到了就可以跳出迴圈了
			  }
		  }
			  TCA9548_SelectChannel(ch);   // ← 切到 SH1106 所在通道

#endif
    SH1106_StatusTypeDef st = SH1106_Init();
    if (st == SH1106_OK) {
        return I2C_OK;
    }else{
    	printf("[I2C Init] SH1106_Init failed: %d\n", st);
        return I2C_ERR_INIT;
    }
}

int SH1106_DeviceInit_Wrapper(void){
printf("\n in SH1106_DeviceInit_Wrapper \n\n");
#ifdef use_TCA9548

	if(TCA9548_SelectByAddress(0x3C) != HAL_OK){

			printf("SH1106_OLED not exist, or Not connected to TCA9548");
		}else{
			TCA9548_ShowChAndAddress_SelectByAddress(0x3C,"SH1106_OLED");
		}

/*
	uint8_t ch=0xFF;
	for(uint8_t i= 0; i<g_scan_count ; i++){
		if (g_scan_result[i].Peripheral_address == 0x3C){
			ch=g_scan_result[i].TCA9548_CH;
			printf("SH1106 OLED ch = %d, Address =  0x%02X\n ",ch,g_scan_result[i].Peripheral_address);
			break; // 找到了就可以跳出迴圈了
			}
	}

	TCA9548_SelectChannel(ch);   // ← 切到 SH1106 所在通道
	printf("\nDebug:TCA9548 setup ch ok  \n\n");
*/
#endif
	//printf("\nDebug:SH1106_Init  \n\n");

	SH1106_StatusTypeDef st = SH1106_Init();
	if (st == SH1106_OK) {
		return I2C_OK;
	}else{
		printf("[I2C Init] SH1106_Init failed: %d\n", st);
		return I2C_ERR_INIT;
	}
}

/**
 * @brief 將緩存區內容刷新到螢幕
 */


void SH1106_UpdateScreen(void) {
    for (uint8_t page = 0; page < (SH1106_HEIGHT / 8); page++) {
        SH1106_WriteCommand(0xB0 + page);
        uint8_t col_addr = SH1106_COLUMN_OFFSET;
        SH1106_WriteCommand(0x00 | (col_addr & 0x0F));
        SH1106_WriteCommand(0x10 | ((col_addr >> 4) & 0x0F));
        OLED_WriteData(&SH1106_Buffer[SH1106_WIDTH * page], SH1106_WIDTH);
    }
}


#ifdef Ver_SSD1306
void SSD1306_UpdateScreen(void) {
	    // 1. 告訴晶片我們要更新的範圍
	SH1106_WriteCommand(0x21); // 設置列位址
	SH1106_WriteCommand(0);    // 起始列 0
	SH1106_WriteCommand(127);  // 結束列 127

	SH1106_WriteCommand(0x22); // 設置頁位址
    SH1106_WriteCommand(0);    // 起始頁 0
    SH1106_WriteCommand(7);    // 結束頁 7 (對應 64 行)

    // 2. 一次性將整個 1024 位元組的緩衝區數據，透過 I2C 發送出去
    SH1106_I2C_WriteMulti(SH1106_I2C_ADDR, 0x40, SH1106_Buffer, sizeof(SH1106_Buffer));

}
#endif



/**
 * @brief 用指定顏色填充整個緩存區
 */
void SH1106_Fill(SH1106_COLOR_t color) {
	/* Set memory */
	memset(SH1106_Buffer, (color == SH1106_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(SH1106_Buffer));
}

/**
 * @brief 在緩存區中畫一個點
 */
void SH1106_DrawPixel(uint16_t x, uint16_t y, SH1106_COLOR_t color) {
    if (x >= SH1106_WIDTH || y >= SH1106_HEIGHT) {
        return; // 超出邊界
    }

    if (color == SH1106_COLOR_WHITE) {
        SH1106_Buffer[x + (y / 8) * SH1106_WIDTH] |= (1 << (y % 8));
    } else {
         SH1106_Buffer[x + (y / 8) * SH1106_WIDTH] &= ~(1 << (y % 8));
    }
}


/**
 * @brief 【統一】繪製點陣圖，這個版本最可靠
 *
 * @brief SH1106_DrawImageDirect 通用图像绘制函数
 * @param image    图像数据指针
 * @param x        起始列 (0-127)
 * @param Y        / 直接使用Y坐标（0-63）
 * @param width    图像宽度 (像素)
 * @param height   图像高度 (像素)
 * @param is_vertical_format 数据格式: 0=水平扫描, 1=垂直扫描
 *
 */
void SH1106_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color)
{
    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    for (int16_t j = 0; j < h; j++, y++) // 外層迴圈：遍歷每一行 (row)
    	{
        for (int16_t i = 0; i < w; i++) // 內層迴圈：遍歷該行的每一個像素 (pixel)
        	{
            // 【關鍵邏輯 1】
            if (i % 8 == 0) {
            	// 只在需要的時候（每 8 個像素）從記憶體讀取一次新的 byte
                byte = bitmap[j * byteWidth + i / 8];
            }
            // 【關鍵邏輯 2】
            if (byte & (0x80 >> (i % 8))) {
            	// 使用位元遮罩檢查當前像素對應的 bit 是否為 1
                // 0x80 >> 0 -> 1000 0000 (檢查第0個像素)
                // 0x80 >> 1 -> 0100 0000 (檢查第1個像素)
                // ...
                // 0x80 >> 7 -> 0000 0001 (檢查第7個像素)
                SH1106_DrawPixel(x + i, y, (SH1106_COLOR_t)color);
            }
        }
    }
}



/**
 * @brief 移動文字游標
 */
void SH1106_GotoXY(uint16_t x, uint16_t y) {
    SH1106.CurrentX = x;
    SH1106.CurrentY = y;
}

/**
 * @brief 繪製一個字元
 */
char SH1106_Putc(char ch, FontDef_t* Font, SH1106_COLOR_t color) {
    uint32_t i, b, j;

    if (SH1106_WIDTH <= (SH1106.CurrentX + Font->FontWidth) ||
        SH1106_HEIGHT <= (SH1106.CurrentY + Font->FontHeight)) {
        return 0; // 空間不足
    }

    // 遍歷字元的每一個像素
    for (i = 0; i < Font->FontHeight; i++) {
        b = Font->data[(ch - 32) * Font->FontHeight + i];
        for (j = 0; j < Font->FontWidth; j++) {
            if ((b << j) & 0x8000) {
                SH1106_DrawPixel(SH1106.CurrentX + j, (SH1106.CurrentY + i), color);
            } else {
                // 背景透明，不畫黑色
                // SH1106_DrawPixel(SH1106.CurrentX + j, (SH1106.CurrentY + i), !color);
            }
        }
    }

    SH1106.CurrentX += Font->FontWidth; // 移動游標
    return ch;
}

/**
 * @brief 繪製字串
 */
char SH1106_Puts(char* str, FontDef_t* Font, SH1106_COLOR_t color) {
    while (*str) {
        if (SH1106_Putc(*str, Font, color) != *str) {
            return *str; // 出錯
        }
        str++;
    }
    return *str;
}
/**
 * @brief 繪製字串（支援 \n 自動換行）
 * @param str 要印出的字串
 * @param Font 字型結構體指標 (例如 &Font_7x10)
 * @param color 顏色
 */
char SH1106_Puts_line_break(char* str, FontDef_t* Font, SH1106_COLOR_t color) {
    uint16_t startX = SH1106.CurrentX; // 記錄起始 X 座標，方便換行後對齊

    while (*str) {
        if (*str == '\n') {
            SH1106.CurrentX = startX;                     // 換行時回到當初的起始 X 座標
            SH1106.CurrentY += Font->FontHeight + 2;      // 往下移一個字型高度（額外加 2 像素當行距）
        } else {
            if (SH1106_Putc(*str, Font, color) != *str) {
                return *str; // 空間不足或繪製出錯
            }
        }
        str++;
    }
    return *str;
}

void SH1106_DrawImageDirect(
    const uint8_t *image,  // 移除非必要的buffer参数
    uint8_t x,
    uint8_t y,             // 直接使用Y坐标（0-63）
    uint16_t width,
    uint16_t height,
    uint8_t is_vertical
) {

    uint8_t page = y / 8;
    if (x >= SH1106_WIDTH || page >= 8) return;

    // 内部直接操作SH1106_Buffer
    if (is_vertical) {
        // 垂直扫描处理（直接使用静态缓冲区）
        for (uint8_t p = 0; p < (height / 8); p++) {
            for (uint8_t col = 0; col < width; col++) {
                uint8_t byte = 0;
                for (uint8_t bit = 0; bit < 8; bit++) {
                    uint16_t src_idx = col * height + p * 8 + bit;
                    if (image[src_idx / 8] & (1 << (src_idx % 8))) {
                        byte |= (1 << bit);
                    }
                }
                SH1106_Buffer[(page + p) * SH1106_WIDTH + x + col] = byte;
            }
        }
    } else {
        // 水平扫描处理（直接memcpy）
        for (uint8_t p = 0; p < (height / 8); p++) {
            memcpy(
                &SH1106_Buffer[(page + p) * SH1106_WIDTH + x],
                &image[p * (width / 8)],
                width / 8
            );
        }
    }
}


/*
void RunDisplayTest(void)
{
    // 1. 清除螢幕緩衝區
    SH1106_Fill(SH1106_COLOR_BLACK);

    // 2. 在左上角畫一個 10x10 的方塊
    SH1106_DrawFilledRectangle(0, 0, 10, 10, SH1106_COLOR_WHITE);

    // 3. 在右下角畫一個 10x10 的方塊
    SH1106_DrawFilledRectangle(SH1106_WIDTH - 10, SH1106_HEIGHT - 10, 10, 10, SH1106_COLOR_WHITE);

    // 4. 在螢幕頂部寫字
    //SH1106_GotoXY(15, 5); // 設定文字起始位置
    //SH1106_Puts("TEST OK", &Font_7x10, SH1106_COLOR_WHITE);

    // 5. 將緩衝區內容更新到螢幕
    SH1106_UpdateScreen();
}
*/



void SH1106_ToggleInvert(void) {
	uint16_t i;

	/* Toggle invert */
	SH1106.Inverted = !SH1106.Inverted;

	/* Do memory toggle */
	for (i = 0; i < sizeof(SH1106_Buffer); i++) {
		SH1106_Buffer[i] = ~SH1106_Buffer[i];
	}
}

void SH1106_I2C_Write(uint8_t address, uint8_t reg, uint8_t data) {
	uint8_t dt[2];
	dt[0] = reg;
	dt[1] = data;
	HAL_I2C_Master_Transmit(SH1106_I2C, address, dt, 2, 10);
}

void SH1106_Clear (void)
{
	SH1106_Fill (0);
    SH1106_UpdateScreen();
}
void SH1106_ON(void) {
	SH1106_WriteCommand(0x8D);
	SH1106_WriteCommand(0x14);
	SH1106_WriteCommand(0xAF);
}
void SH1106_OFF(void) {
	SH1106_WriteCommand(0x8D);
	SH1106_WriteCommand(0x10);
	SH1106_WriteCommand(0xAE);
}

void SH1106_I2C_WriteMulti(uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) {
	uint8_t dt[256];
	dt[0] = reg;
	uint8_t i;

	for(i = 0; i < count; i++)
		{
			dt[i+1] = data[i];
			//printf("data[%d]= %d \n",i,data[i]);//<<陷入無窮迴圈,跳不出去
			//printf("i =%d \n",i);
		}

	//printf("for loop leave");
	HAL_I2C_Master_Transmit(SH1106_I2C, address, dt, count+1, 10);


}


void SH1106_InvertDisplay (uint8_t inverted )
{
  if (inverted) SH1106_WriteCommand (SH1106_INVERTDISPLAY);

  else SH1106_WriteCommand (SH1106_NORMALDISPLAY);

}

/**
 * @brief  Reads status byte from the SH1106
 * @param  None
 * @retval Status byte. 0xFF if read error.
 */
uint8_t SH1106_ReadStatus(void) {
    uint8_t status_byte;

    // 使用 STM32 HAL 函式庫從 OLED 讀取一個位元組的資料
    // HAL_I2C_Master_Receive 會發送設備位址並請求數據
    // 注意：這裡不需要先發送 "register address"，因為狀態暫存器是預設的讀取目標
    if (HAL_I2C_Master_Receive(SH1106_I2C, SH1106_I2C_ADDR, &status_byte, 1, HAL_MAX_DELAY) == HAL_OK) {
        return status_byte;
    } else {
        // 如果 I2C 讀取失敗 (例如，設備沒有回應 ACK)，返回 0xFF 作為錯誤碼
        return 0xFF;
    }
}

