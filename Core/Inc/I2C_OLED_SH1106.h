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

/**
 * @brief  Initializes the SH1106 display.
 * @param  hi2c: Pointer to the I2C handle.
 * @retval 1 on success, 0 on failure (device not found).
 */
uint8_t SH1106_Init(I2C_HandleTypeDef *hi2c);

uint8_t SH1106_OLED_Init();

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
