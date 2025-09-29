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
#include <I2C_OLED_fonts.h>
#include <I2C_OLED_SH1106.h>
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

//static void SH1106_WriteCommand(uint8_t command); // <--- [修改] 這是我們統一使用的內部命令發送函式
static SH1106_StatusTypeDef SH1106_WriteCommand(uint8_t command);

static void SH1106_WriteData(uint8_t* data, uint16_t size); // <--- [修改] 這是我們統一使用的內部數據發送函式




/* Private variable */

static uint8_t SH1106_Buffer[SH1106_WIDTH * SH1106_HEIGHT / 8]; // 顯示緩存區
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



#ifdef WriteCommand_without_HAL
static void SH1106_WriteCommand(uint8_t command) // <--- [修改] 這是我們統一使用的內部命令發送函式
{
    // 使用 HAL_I2C_Mem_Write 更穩定，它會發送 START -> 地址 -> 記憶體地址(控制字節) -> 數據 -> STOP
    HAL_I2C_Mem_Write(&hi2c1, SH1106_I2C_ADDR, 0x00, 1, &command, 1, HAL_MAX_DELAY);
}
#endif

/**
 * @brief  【統一】發送一塊數據
 */
static void SH1106_WriteData(uint8_t* data, uint16_t size) // <--- [修改] 這是我們統一使用的內部數據發送函式
{
    HAL_I2C_Mem_Write(&hi2c1, SH1106_I2C_ADDR, 0x40, 1, data, size, HAL_MAX_DELAY);
}
	
void SH1106_OLED_WriteCommand(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd}; // Control byte (0x00) + Command
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDRESS, buf, 2, HAL_MAX_DELAY);
}

void OLED_WriteData(uint8_t data) {
    uint8_t buf[2] = {0x40, data}; // Control byte (0x40) + Data
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDRESS, buf, 2, HAL_MAX_DELAY);
}
//</0805新增>

// --- 發送數據的輔助函式 (用來更新畫面) ---
void OLED_Write_Data(uint8_t* data, uint16_t size)
{
    uint8_t* temp_buf = malloc(size + 1);
    temp_buf[0] = 0x40; // 0x40 代表接下來是 Data
    memcpy(temp_buf+1, data, size);
    HAL_I2C_Master_Transmit(SH1106_I2C, SH1106_I2C_ADDR, temp_buf, size + 1, HAL_MAX_DELAY);
    free(temp_buf);
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



#if WriteCommand_without_HAL

uint8_t SH1106_OLED_Init(void) {
	printf("SH1106_OLED_Init \n");

	/* Check if LCD connected to I2C */
	if (HAL_I2C_IsDeviceReady(SH1106_I2C, SH1106_I2C_ADDR, 1, 20000) != HAL_OK) {
		/* Return false */
		return 0;
	}
    HAL_Delay(100);

    //SH1106_I2C_Write();

    SH1106_WriteCommand(OLED_DISPLAY_OFF);
    SH1106_WriteCommand(OLED_SET_CLK_DIV); // Set Display Clock Divide Ratio
    SH1106_WriteCommand(INIT_CLK_DIV);
    SH1106_WriteCommand(OLED_SET_MUX_RATIO); // Set Multiplex Ratio
    SH1106_WriteCommand(INIT_MUX_RATIO); // 1/64 duty
    SH1106_WriteCommand(OLED_SET_DISPLAY_OFFSET); // Set Display Offset
    SH1106_WriteCommand(OLED_SET_LOWER_COLUMN);
    SH1106_WriteCommand(0x40); // Set Start Line (0)
    SH1106_WriteCommand(0xAD); // Set DC-DC
    SH1106_WriteCommand(0x8B); // DC-DC ON
    SH1106_WriteCommand(0xA1); // Segment Remap (column 127 is mapped to SEG0)
    SH1106_WriteCommand(0xC8); // COM Output Scan Direction (remapped)
    SH1106_WriteCommand(0xDA); // Set COM Pins Hardware Configuration
    SH1106_WriteCommand(0x12);
    SH1106_WriteCommand(0x81); // Set Contrast Control
    SH1106_WriteCommand(0xAF); // 建議的對比度
    SH1106_WriteCommand(0xD9); // Set Pre-charge Period
    SH1106_WriteCommand(0x1F);
    SH1106_WriteCommand(0xDB); // Set VCOMH Deselect Level
    SH1106_WriteCommand(0x40);
    SH1106_WriteCommand(0xA4); // Entire Display ON from RAM
    SH1106_WriteCommand(0xA6); // Normal Display

    SH1106_WriteCommand(0xAF); // Display ON

    /*
    SH1106_WRITECOMMAND(OLED_DISPLAY_OFF);
    SH1106_WRITECOMMAND(0x02); // 這個是特殊用途，SH1106 Column Offset
    SH1106_WRITECOMMAND(OLED_SET_CONTRAST);
    SH1106_WRITECOMMAND(INIT_CONTRAST_VALUE);
    SH1106_WRITECOMMAND(OLED_SEG_REMAP_127);
    SH1106_WRITECOMMAND(OLED_NORMAL_DISPLAY);
    SH1106_WRITECOMMAND(OLED_SET_MUX_RATIO);
    SH1106_WRITECOMMAND(INIT_MUX_RATIO);
    SH1106_WRITECOMMAND(OLED_SET_DC_DC_MODE);
    SH1106_WRITECOMMAND(OLED_PUMP_ON);
    SH1106_WRITECOMMAND(OLED_SET_PUMP_VOLTAGE | INIT_PUMP_VOLTAGE);
    SH1106_WRITECOMMAND(OLED_SCAN_DIR_REMAP);
    SH1106_WRITECOMMAND(OLED_SET_DISPLAY_OFFSET);
    SH1106_WRITECOMMAND(INIT_DISPLAY_OFFSET);
    SH1106_WRITECOMMAND(OLED_SET_CLK_DIV);
    SH1106_WRITECOMMAND(INIT_CLK_DIV);
    SH1106_WRITECOMMAND(OLED_SET_PRECHARGE);
    SH1106_WRITECOMMAND(INIT_PRECHARGE);
    SH1106_WRITECOMMAND(OLED_SET_COM_CONFIG);
    SH1106_WRITECOMMAND(INIT_COM_CONFIG);
    SH1106_WRITECOMMAND(OLED_SET_VCOMH);
    SH1106_WRITECOMMAND(INIT_VCOMH);
    SH1106_WRITECOMMAND(OLED_DISPLAY_ON);
*/

    SH1106.CurrentX = 0;
    SH1106.CurrentY = 0;
    SH1106.Initialized = 1;

    printf("ready return\n");

    return 1;
}
#endif
#endif


/**
 * @brief 將緩存區內容刷新到螢幕
 */


void SH1106_UpdateScreen(void) {
    for (uint8_t page = 0; page < (SH1106_HEIGHT / 8); page++) {
        SH1106_WriteCommand(0xB0 + page);
        uint8_t col_addr = SH1106_COLUMN_OFFSET;
        SH1106_WriteCommand(0x00 | (col_addr & 0x0F));
        SH1106_WriteCommand(0x10 | ((col_addr >> 4) & 0x0F));
        SH1106_WriteData(&SH1106_Buffer[SH1106_WIDTH * page], SH1106_WIDTH);
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


// --- 發送命令的輔助函式 ---
void OLED_Write_CMD(uint8_t cmd)
{
    uint8_t data_to_send[2] = {0x00, cmd}; // 0x00 代表接下來是 Command
    HAL_I2C_Master_Transmit(SH1106_I2C, SH1106_I2C_ADDR, data_to_send, 2, HAL_MAX_DELAY);
}



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



//#define Legacy_Code
#ifdef Legacy_Code


void SH1106_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SH1106_COLOR_t c) {
	int16_t dx, dy, sx, sy, err, e2, i, tmp; 
	
	/* Check for overflow */
	if (x0 >= SH1106_WIDTH) {
		x0 = SH1106_WIDTH - 1;
	}
	if (x1 >= SH1106_WIDTH) {
		x1 = SH1106_WIDTH - 1;
	}
	if (y0 >= SH1106_HEIGHT) {
		y0 = SH1106_HEIGHT - 1;
	}
	if (y1 >= SH1106_HEIGHT) {
		y1 = SH1106_HEIGHT - 1;
	}
	
	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1); 
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1); 
	sx = (x0 < x1) ? 1 : -1; 
	sy = (y0 < y1) ? 1 : -1; 
	err = ((dx > dy) ? dx : -dy) / 2; 

	if (dx == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		/* Vertical line */
		for (i = y0; i <= y1; i++) {
			SH1106_DrawPixel(x0, i, c);
		}
		
		/* Return from function */
		return;
	}
	
	if (dy == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		/* Horizontal line */
		for (i = x0; i <= x1; i++) {
			SH1106_DrawPixel(i, y0, c);
		}
		
		/* Return from function */
		return;
	}
	
	while (1) {
		SH1106_DrawPixel(x0, y0, c);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err; 
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		} 
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		} 
	}
}

void SH1106_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SH1106_COLOR_t c) {
	/* Check input parameters */
	if (
		x >= SH1106_WIDTH ||
		y >= SH1106_HEIGHT
	) {
		/* Return error */
		return;
	}
	
	/* Check width and height */
	if ((x + w) >= SH1106_WIDTH) {
		w = SH1106_WIDTH - x;
	}
	if ((y + h) >= SH1106_HEIGHT) {
		h = SH1106_HEIGHT - y;
	}
	
	/* Draw 4 lines */
	SH1106_DrawLine(x, y, x + w, y, c);         /* Top line */
	SH1106_DrawLine(x, y + h, x + w, y + h, c); /* Bottom line */
	SH1106_DrawLine(x, y, x, y + h, c);         /* Left line */
	SH1106_DrawLine(x + w, y, x + w, y + h, c); /* Right line */
}

void SH1106_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SH1106_COLOR_t c) {
	uint8_t i;
	
	/* Check input parameters */
	if (
		x >= SH1106_WIDTH ||
		y >= SH1106_HEIGHT
	) {
		/* Return error */
		return;
	}
	
	/* Check width and height */
	if ((x + w) >= SH1106_WIDTH) {
		w = SH1106_WIDTH - x;
	}
	if ((y + h) >= SH1106_HEIGHT) {
		h = SH1106_HEIGHT - y;
	}
	
	/* Draw lines */
	for (i = 0; i <= h; i++) {
		/* Draw lines */
		SH1106_DrawLine(x, y + i, x + w, y + i, c);
	}
}

void SH1106_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SH1106_COLOR_t color) {
	/* Draw lines */
	SH1106_DrawLine(x1, y1, x2, y2, color);
	SH1106_DrawLine(x2, y2, x3, y3, color);
	SH1106_DrawLine(x3, y3, x1, y1, color);
}


void SH1106_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SH1106_COLOR_t color) {
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
	curpixel = 0;
	
	deltax = abs(x2 - x1);
	deltay = abs(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		SH1106_DrawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}

void SH1106_DrawCircle(int16_t x0, int16_t y0, int16_t r, SH1106_COLOR_t c) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    SH1106_DrawPixel(x0, y0 + r, c);
    SH1106_DrawPixel(x0, y0 - r, c);
    SH1106_DrawPixel(x0 + r, y0, c);
    SH1106_DrawPixel(x0 - r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        SH1106_DrawPixel(x0 + x, y0 + y, c);
        SH1106_DrawPixel(x0 - x, y0 + y, c);
        SH1106_DrawPixel(x0 + x, y0 - y, c);
        SH1106_DrawPixel(x0 - x, y0 - y, c);

        SH1106_DrawPixel(x0 + y, y0 + x, c);
        SH1106_DrawPixel(x0 - y, y0 + x, c);
        SH1106_DrawPixel(x0 + y, y0 - x, c);
        SH1106_DrawPixel(x0 - y, y0 - x, c);
    }
}

void SH1106_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SH1106_COLOR_t c) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    SH1106_DrawPixel(x0, y0 + r, c);
    SH1106_DrawPixel(x0, y0 - r, c);
    SH1106_DrawPixel(x0 + r, y0, c);
    SH1106_DrawPixel(x0 - r, y0, c);
    SH1106_DrawLine(x0 - r, y0, x0 + r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        SH1106_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, c);
        SH1106_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, c);

        SH1106_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, c);
        SH1106_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, c);
    }
}
 



#endif







#ifdef SH1106_DrawBitmap_AI
/**
 * @brief 绘制位图到 SH1106 显存（兼容水平扫描/MSB优先格式）
 * @param x       起始列 (0-127)
 * @param y       起始行 (0-63)
 * @param bitmap  位图数据指针（水平扫描，MSB优先）
 * @param w       位图宽度 (像素)
 * @param h       位图高度 (像素)
 * @param color   颜色 (1:白色, 0:黑色)
 * @note 数据格式：每字节8像素（MSB=左）, 每行字节数=ceil(w/8)
 */
void SH1106_DrawBitmap(int16_t x, int16_t y, const uint8_t* bitmap, int16_t w, int16_t h, uint16_t color) {
    // 边界检查
    if (x >= SH1106_WIDTH || y >= SH1106_HEIGHT) return;
    if (x + w < 0 || y + h < 0) return;

    int16_t byteWidth = (w + 7) / 8; // 每行字节数
    uint8_t byte = 0;

    for (int16_t j = 0; j < h; j++, y++) {
        if (y < 0 || y >= SH1106_HEIGHT) continue; // 跳过屏幕外的行

        for (int16_t i = 0; i < w; i++) {
            if (x + i < 0 || x + i >= SH1106_WIDTH) continue; // 跳过屏幕外的列

            // 每8像素读取新字节（MSB优先）
            if (i % 8 == 0) {
                byte = bitmap[j * byteWidth + i / 8];
            }
            // 绘制像素（若当前位为1）
            if (byte & (0x80 >> (i % 8))) {
                SH1106_DrawPixel(x + i, y, color);
            }
        }
    }
}
#endif




#ifdef dim_one
char SH1106_Putc(char ch, FontDef_t* Font, SH1106_COLOR_t color) {
	uint32_t i, b, j;

	// Check available space in LCD
	if (
		SH1106_WIDTH <= (SH1106.CurrentX + Font->FontWidth) ||
		SH1106_HEIGHT <= (SH1106.CurrentY + Font->FontHeight)
	) {
		//Error
		return 0;
	}

	// Go through font
	for (i = 0; i < Font->FontHeight; i++) {
		b = Font->data[(ch - 32) * Font->FontHeight + i];
		for (j = 0; j < Font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
				SH1106_DrawPixel(SH1106.CurrentX + j, (SH1106.CurrentY + i), (SH1106_COLOR_t) color);
			} else {
				SH1106_DrawPixel(SH1106.CurrentX + j, (SH1106.CurrentY + i), (SH1106_COLOR_t)!color);
			}
		}
	}

	// Increase pointer
	SH1106.CurrentX += Font->FontWidth;

	// Return character written
	return ch;
}


char SH1106_Puts(char* str, FontDef_t* Font, SH1106_COLOR_t color) {
	/* Write characters */
	while (*str) {
		/* Write character by character */
		if (SH1106_Putc(*str, Font, color) != *str) {
			/* Return error */
			return *str;
		}

		/* Increase string pointer */
		str++;
	}

	/* Everything OK, zero should be returned */
	return *str;
}
#endif

/*
  void SH1106_DrawImage(uint8_t *buffer, const uint8_t *image, uint8_t start_page) {

    for (uint8_t page = 0; page < (IMAGE_HEIGHT / 8); page++) {
        // 计算目标缓冲区的起始位置（每页128字节）
        uint8_t *dst = &buffer[(start_page + page) * 128];
        // 复制图像的一行（128像素 = 16字节）
        memcpy(dst, &image[page * 16], 16);
    }
}

	void SH1106_OLED_Clear(void) {
	    uint8_t zero[128] = {0};
	    for (uint8_t page = 0; page < 8; page++) {
	    	SH1106_OLED_SetPos(0, page);
	        HAL_I2C_Mem_Write(&hi2c1, SH1106_I2C_ADDR, OLED_DATA, 1, zero, 128, HAL_MAX_DELAY);
	    }
	}


void SH1106_OLED_SetPos(uint8_t x, uint8_t page) {
	x += OLED_COL_OFFSET;
    SH1106_OLED_WriteCommand(0xB0 + page);
    SH1106_OLED_WriteCommand(0x00 + (x & 0x0F));
    SH1106_OLED_WriteCommand(0x10 + ((x >> 4) & 0x0F));
}
void SH1106_UpdateScreen_org(void) {
	uint8_t page;

	for (page = 0; page < 8; page++) {

		SH1106_WRITECOMMAND(0xB0 + page);
		SH1106_WRITECOMMAND(0x02);
		SH1106_WRITECOMMAND(0x10);

		// Write multi data
		SH1106_I2C_WriteMulti(SH1106_I2C_ADDR,
							  0x40,
							  &SH1106_Buffer[SH1106_WIDTH * page],
							  SH1106_WIDTH);//<<這也是用到了這行
	}
}

void SH1106_DrawPixel(uint16_t x, uint16_t y, SH1106_COLOR_t color) {
	if (
		x >= SH1106_WIDTH ||
		y >= SH1106_HEIGHT
	) {
		// Error
		return;
	}

	//Check if pixels are inverted
	if (SH1106.Inverted) {
		color = (SH1106_COLOR_t)!color;
	}

	// Set color
	if (color == SH1106_COLOR_WHITE) {
		SH1106_Buffer[x + (y / 8) * SH1106_WIDTH] |= 1 << (y % 8);
	} else {
		SH1106_Buffer[x + (y / 8) * SH1106_WIDTH] &= ~(1 << (y % 8));
	}
}



void SH1106_OLED_UpdateScreen(uint8_t *buffer) {
    for (uint8_t page = 0; page < 8; page++) {
    	SH1106_OLED_SetPos(0, page);
    	HAL_I2C_Mem_Write(&hi2c1, SH1106_I2C_ADDR, OLED_DATA, 1, &buffer[page * 128], 128, HAL_MAX_DELAY);
    }
}

void SH1106_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color)
{

    int16_t byteWidth = (w + 7) / 8;
    uint8_t byte = 0;

    for(int16_t j=0; j<h; j++, y++)
    {
        for(int16_t i=0; i<w; i++)
        {
            if(i & 7)
            {
               byte <<= 1;
            }
            else
            {
               byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
            }
            if(byte & 0x80) SH1106_DrawPixel(x+i, y, color);
        }
    }
}


void SH1106_OLED_DrawPixel(uint8_t *buffer, uint8_t x, uint8_t y, uint8_t color) {
    if (x >= 128 || y >= 64) return;
    uint16_t byte_index = x + (y / 8) * 128;  // 计算字节位置
    uint8_t  bit_mask  = 1 << (y % 8);       // 计算位掩码


    if (color) {
        buffer[byte_index] |=  bit_mask;  // 设置像素为白色
    } else {
        buffer[byte_index] &= ~bit_mask;  // 设置像素为黑色
    }

void SH1106_UpdateScreen(void) {
    for (uint8_t page = 0; page < 8; page++) {
        SH1106_WriteCommand(0xB0 + page); // 設置 Page 位址

        // 設置 Column 位址，這是 SH1106 的關鍵
        uint8_t col_addr = SH1106_COLUMN_OFFSET;
        SH1106_WriteCommand(0x00 | (col_addr & 0x0F));        // 低 4 位
        SH1106_WriteCommand(0x10 | ((col_addr >> 4) & 0x0F)); // 高 4 位

        // 發送該 Page 的 128 字節數據
        SH1106_WriteData(&SH1106_Buffer[SH1106_WIDTH * page], SH1106_WIDTH);
    }
}
*/

