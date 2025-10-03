/**
 * @file    sh1106.h
 * @author  Markscat (based on work by Tilen Majerle and Controllerstech)
 * @brief   Driver for SH1106 128x64 OLED display using I2C.
 *
 * @note    This driver uses an internal framebuffer.
 *          Call SH1106_UpdateScreen() to transfer the buffer to the display.
 */

#ifndef SH1106_H
#define SH1106_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h" // For HAL types like I2C_HandleTypeDef
#include <I2C_OLED_fonts.h>


//==============================================================================
// 1. Configuration Section
//==============================================================================

#define SH1106_I2C_ADDR         (0x3C << 1) // 7-bit address 0x3C left-shifted
#define OLED_ADDRESS	   		(0x3C << 1) // 7-bit address 0x3C left-shifted
#define SH1106_WIDTH            128
#define SH1106_HEIGHT           64

// IMPORTANT: SH1106 RAM is 132 pixels wide. The screen is mapped to a 128-pixel
// window inside this RAM. This offset defines the starting column.
// Common values are 0, 2, or 4. Adjust if your display is shifted.
#define SH1106_COLUMN_OFFSET    2

//==============================================================================
// 2. Public Types and Definitions
//==============================================================================

/**
 * @brief  Color enumeration for the monochrome display.
 */
typedef enum {
    SH1106_COLOR_BLACK = 0x00, // Pixel is off
    SH1106_COLOR_WHITE = 0x01, // Pixel is on
} SH1106_COLOR_t;

/**
 * @brief  Font definition structure.
 *         (Assuming you have a fonts.h file with this structure)
 */
/*
typedef struct {
    const uint8_t FontWidth;
    uint8_t FontHeight;
    const uint16_t *data;
} FontDef_t;
*/

//==============================================================================
// 3. Public Function Prototypes
//==============================================================================

// --- Core Functions ---

typedef enum {
    /* ---------------- 通用狀態碼 ---------------- */
    SH1106_OK = 0,                  // 操作成功
    SH1106_ERROR,                   // 未指定的通用錯誤
    SH1106_ERROR_BUSY,              // I2C 總線忙
    SH1106_ERROR_TIMEOUT,           // I2C 通訊超時
    SH1106_ERROR_NOT_INITIALIZED,   // 驅動未初始化

    /* ---------------- 初始化過程的詳細錯誤碼 ---------------- */
    SH1106_ERROR_INIT_DEV_NOT_FOUND,
    SH1106_ERROR_INIT_DISPLAY_OFF,
    SH1106_ERROR_INIT_SET_CLOCK,
    SH1106_ERROR_INIT_SET_MULTIPLEX,
    SH1106_ERROR_INIT_SET_OFFSET,
    SH1106_ERROR_INIT_SET_START_LINE,
    SH1106_ERROR_INIT_SET_DCDC,
    SH1106_ERROR_INIT_SEG_REMAP,
    SH1106_ERROR_INIT_COM_DIR,
    SH1106_ERROR_INIT_COM_PINS,
    SH1106_ERROR_INIT_SET_CONTRAST,
    SH1106_ERROR_INIT_SET_PRECHARGE,
    SH1106_ERROR_INIT_SET_VCOMH,
    SH1106_ERROR_INIT_DISPLAY_NORMAL,
    SH1106_ERROR_INIT_DISPLAY_ON,

    /* ---------------- 運行時函式的錯誤碼 ---------------- */
    SH1106_ERROR_UPDATE_SCREEN_PAGE0, // 可以在 UpdateScreen 中返回具體是哪一頁出錯
    SH1106_ERROR_UPDATE_SCREEN_PAGE1,
    // ...
    SH1106_ERROR_UPDATE_SCREEN_PAGE7,

} SH1106_StatusTypeDef;



#define SH1106_NORMALDISPLAY       0xA6
#define SH1106_INVERTDISPLAY       0xA7

//#define Ver_SSD1306
#define Ver_SH1106


#ifndef __SH1106_H__
#define __SH1106_H__


#define OLED_SET_DC_DC_MODE       0xAD
#define OLED_PUMP_ON              0x8B

// ===== 基本顯示控制 =====
#define OLED_DISPLAY_OFF        0xAE // 顯示關閉
#define OLED_DISPLAY_ON         0xAF // 顯示開啟
#define OLED_NORMAL_DISPLAY     0xA6 // 正常顯示模式
#define OLED_INVERT_DISPLAY     0xA7 // 反相顯示模式
#define OLED_NOP                0xE3 // 無操作（空指令）

// ===== 記憶體位址設定 =====
#define OLED_SET_LOWER_COLUMN   0x00 // 設定 Column 位址低位元 (0x00~0x0F)
#define OLED_SET_HIGHER_COLUMN  0x10 // 設定 Column 位址高位元 (0x10~0x1F)
#define OLED_SET_PAGE_ADDR      0xB0 // 設定 Page 位址 (0xB0~0xB7)

// ===== 滾動設定 =====
#define OLED_SCROLL_SETUP       0x26 // 水平滾動設定（後續需給方向與間隔）
#define OLED_SCROLL_OFF         0x2E // 停止滾動
#define OLED_SCROLL_ON          0x2F // 啟動滾動

// ===== 硬體設定 =====
#define OLED_SET_CONTRAST       0x81 // 對比度設定（後需給值 0x00~0xFF）
#define OLED_SEG_REMAP_0        0xA0 // 段驅動輸出正常順序 (column 0 → column max)
#define OLED_SEG_REMAP_127      0xA1 // 段驅動輸出反轉順序 (column max → column 0)
#define OLED_SET_MUX_RATIO      0xA8 // 多工比設定（後需給 0x1F~0x3F）
#define OLED_SET_DC_DC_MODE     0xAD // 設定內部 DC-DC 開關
#define OLED_DC_DC_OFF          0x8A
#define OLED_DC_DC_ON           0x8B

#define OLED_SET_DC_DC_MODE      0xAD
#define OLED_PUMP_ON            0x8B

#define OLED_SET_PUMP_VOLTAGE   0x30 // 設定電壓（後需給 0x00~0x03）

#define OLED_SCAN_DIR_NORMAL    0xC0 // COM 輸出掃描方向：正常（0 → max）
#define OLED_SCAN_DIR_REMAP     0xC8 // COM 輸出掃描方向：反轉（max → 0）
#define OLED_SET_DISPLAY_OFFSET 0xD3 // 顯示偏移設定（後需給 0x00~0x3F）
#define OLED_SET_CLK_DIV        0xD5 // 顯示時鐘分頻與振盪頻率
#define OLED_SET_PRECHARGE      0xD9 // 預充電期設定（後需給值）
#define OLED_SET_COM_CONFIG     0xDA // COM 硬體配置
#define OLED_SET_VCOMH          0xDB // VCOMH 電壓等級（後需給值）

// ===== 高級控制 =====
#define OLED_READ_MODIFY_WRITE  0xE0 // 讀-改-寫模式啟動
#define OLED_END                0xEE // 結束讀-改-寫模式

#endif

// 初始化參數設定
#define INIT_CONTRAST_VALUE    0xFF
#define INIT_MUX_RATIO         0x3F
#define INIT_PUMP_VOLTAGE      0x02  // 0~3 對應不同電壓
#define INIT_DISPLAY_OFFSET    0x00
#define INIT_CLK_DIV           0x80
#define INIT_PRECHARGE         0x1F
#define INIT_COM_CONFIG        0x12
#define INIT_VCOMH             0x40
#define OLED_DISPLAY_ALL_ON_RESUME 0xA4

//#define Mem_Write
#define OLED_Write_0805_



/**
 * @brief  Initializes the SH1106 display.
 * @param  hi2c: Pointer to the I2C handle.
 * @retval 1 on success, 0 on failure (device not found).
 */

//uint8_t SH1106_Init(I2C_HandleTypeDef *hi2c);

uint8_t SH1106_OLED_Init();

/**
 * @brief init with HAL*/
SH1106_StatusTypeDef SH1106_Init(void);


/**
 * @brief  Transfers the internal framebuffer to the OLED display.
 * @note   This function must be called to see any changes made by drawing functions.
 */
void SH1106_UpdateScreen(void);


// --- Drawing Primitives ---

/**
 * @brief  Fills the entire screen buffer with a specified color.
 * @param  color: Color to fill the screen with.
 */
void SH1106_Fill(SH1106_COLOR_t color);

/**
 * @brief  Draws a single pixel in the framebuffer.
 * @param  x, y: Pixel coordinates.
 * @param  color: Pixel color.
 */
void SH1106_DrawPixel(uint16_t x, uint16_t y, SH1106_COLOR_t color);

/**
 * @brief  Draws a bitmap image in the framebuffer.
 * @note   The bitmap data is expected to be in horizontal, MSB-first format.
 * @param  x, y: Top-left coordinates to start drawing.
 * @param  bitmap: Pointer to the bitmap data array.
 * @param  w, h: Width and height of the bitmap in pixels.
 */

void SH1106_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color);

//void SH1106_DrawBitmap(int16_t x, int16_t y, const uint8_t* bitmap, int16_t w, int16_t h);

/**
 * @brief  Draws a line between two points.
 */
void SH1106_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SH1106_COLOR_t color);

/**
 * @brief  Draws a rectangle outline.
 */
void SH1106_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SH1106_COLOR_t color);

/**
 * @brief  Draws a filled rectangle.
 */
void SH1106_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SH1106_COLOR_t color);

/**
 * @brief  Draws a circle outline.
 */
void SH1106_DrawCircle(int16_t x0, int16_t y0, int16_t r, SH1106_COLOR_t color);


// --- Text Functions ---

/**
 * @brief  Sets the cursor position for text drawing.
 * @param  x, y: Top-left coordinates for the next character.
 */
void SH1106_GotoXY(uint16_t x, uint16_t y);

/**
 * @brief  Draws a single character.
 * @param  ch: Character to draw.
 * @param  Font: Pointer to the font definition.
 * @param  color: Character color.
 * @retval The drawn character.
 */
char SH1106_Putc(char ch, FontDef_t* Font, SH1106_COLOR_t color);

/**
 * @brief  Draws a string.
 * @param  str: Pointer to the string.
 * @param  Font: Pointer to the font definition.
 * @param  color: String color.
 * @retval First character of the string.
 */
char SH1106_Puts(char* str, FontDef_t* Font, SH1106_COLOR_t color);


// --- Advanced Functions ---

/**
 * @brief  Inverts the entire display content (black becomes white and vice versa).
 * @note   This is a command sent to the display, it does not modify the framebuffer.
 * @param  inverted: 1 to invert, 0 for normal display.
 */
void SH1106_InvertDisplay(uint8_t inverted);

/**
 * @brief Turns the display panel on or off.
 * @param on: 1 to turn on, 0 to turn off.
 */
void SH1106_SetDisplayOn(uint8_t on);


#ifdef __cplusplus
}
#endif

#endif // SH1106_H
