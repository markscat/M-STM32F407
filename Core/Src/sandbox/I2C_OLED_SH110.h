/**
 * original author:  Tilen Majerle<tilen@majerle.eu>
 * modification for SH1106: Controllerstech (www.controllerstech.com)

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
#ifndef SH1106_H
#define SH1106_H

/* C++ detection */
#ifdef __cplusplus
extern C {
#endif

/**
 * This SH1106 LCD uses I2C for communication
 *
 * Library features functions for drawing lines, rectangles and circles.
 *
 * It also allows you to draw texts and characters using appropriate functions provided in library.
 *
 * Default pinout
 *
SH1106    |STM32F10x    |DESCRIPTION

VCC        |3.3V         |
GND        |GND          |
SCL        |PB6          |Serial clock line
SDA        |PB7          |Serial data line
 */

#include "main.h"

#include <I2C_OLED_fonts.h>

#include "stdlib.h"
#include "string.h"


/* 1.3 inch oled I2C address */

#define SH1106_I2C_ADDR         0x3C<<1
//#define SH1106_I2C_ADDR         0x78<<1



#ifndef SH1106_WIDTH
#define SH1106_WIDTH            128
#endif
/* SH1106 LCD height in pixels */
#ifndef SH1106_HEIGHT
#define SH1106_HEIGHT           64
#endif

/**
 * @brief  SH1106 color enumeration
 */
typedef enum {
	SH1106_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
	SH1106_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
} SH1106_COLOR_t;


/**
 * @brief  Initializes SH1106 LCD
 * @param  None
 * @retval Initialization status:
 *           - 0: LCD was not detected on I2C port
 *           - > 0: LCD initialized OK and ready to use
 */
uint8_t SH1106_Init(void);

/** 
 * @brief  Updates buffer from internal RAM to LCD
 * @note   This function must be called each time you do some changes to LCD, to update buffer from RAM to LCD
 * @param  None
 * @retval None
 */
void SH1106_UpdateScreen(void);

/**
 * @brief  Toggles pixels invertion inside internal RAM
 * @note   @ref SH1106_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  None
 * @retval None
 */
void SH1106_ToggleInvert(void);

/** 
 * @brief  Fills entire LCD with desired color
 * @note   @ref SH1106_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  Color: Color to be used for screen fill. This parameter can be a value of @ref SH1106_COLOR_t enumeration
 * @retval None
 */
void SH1106_Fill(SH1106_COLOR_t Color);

/**
 * @brief  Draws pixel at desired location
 * @note   @ref SH1106_UpdateScreen() must called after that in order to see updated LCD screen
 * @param  x: X location. This parameter can be a value between 0 and SH1106_WIDTH - 1
 * @param  y: Y location. This parameter can be a value between 0 and SH1106_HEIGHT - 1
 * @param  color: Color to be used for screen fill. This parameter can be a value of @ref SH1106_COLOR_t enumeration
 * @retval None
 */
void SH1106_DrawPixel(uint16_t x, uint16_t y, SH1106_COLOR_t color);

/**
 * @brief  Sets cursor pointer to desired location for strings
 * @param  x: X location. This parameter can be a value between 0 and SH1106_WIDTH - 1
 * @param  y: Y location. This parameter can be a value between 0 and SH1106_HEIGHT - 1
 * @retval None
 */
void SH1106_GotoXY(uint16_t x, uint16_t y);

/**
 * @brief  Puts character to internal RAM
 * @note   @ref SH1106_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  ch: Character to be written
 * @param  *Font: Pointer to @ref FontDef_t structure with used font
 * @param  color: Color used for drawing. This parameter can be a value of @ref SH1106_COLOR_t enumeration
 * @retval Character written
 */
char SH1106_Putc(char ch, FontDef_t* Font, SH1106_COLOR_t color);

/**
 * @brief  Puts string to internal RAM
 * @note   @ref SH1106_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  *str: String to be written
 * @param  *Font: Pointer to @ref FontDef_t structure with used font
 * @param  color: Color used for drawing. This parameter can be a value of @ref SH1106_COLOR_t enumeration
 * @retval Zero on success or character value when function failed
 */
char SH1106_Puts(char* str, FontDef_t* Font, SH1106_COLOR_t color);

/**
 * @brief  Draws line on LCD
 * @note   @ref SH1106_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x0: Line X start point. Valid input is 0 to SH1106_WIDTH - 1
 * @param  y0: Line Y start point. Valid input is 0 to SH1106_HEIGHT - 1
 * @param  x1: Line X end point. Valid input is 0 to SH1106_WIDTH - 1
 * @param  y1: Line Y end point. Valid input is 0 to SH1106_HEIGHT - 1
 * @param  c: Color to be used. This parameter can be a value of @ref SH1106_COLOR_t enumeration
 * @retval None
 */
void SH1106_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SH1106_COLOR_t c);

/**
 * @brief  Draws rectangle on LCD
 * @note   @ref SH1106_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: Top left X start point. Valid input is 0 to SH1106_WIDTH - 1
 * @param  y: Top left Y start point. Valid input is 0 to SH1106_HEIGHT - 1
 * @param  w: Rectangle width in units of pixels
 * @param  h: Rectangle height in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SH1106_COLOR_t enumeration
 * @retval None
 */
void SH1106_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SH1106_COLOR_t c);

/**
 * @brief  Draws filled rectangle on LCD
 * @note   @ref SH1106_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: Top left X start point. Valid input is 0 to SH1106_WIDTH - 1
 * @param  y: Top left Y start point. Valid input is 0 to SH1106_HEIGHT - 1
 * @param  w: Rectangle width in units of pixels
 * @param  h: Rectangle height in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SH1106_COLOR_t enumeration
 * @retval None
 */
void SH1106_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SH1106_COLOR_t c);

/**
 * @brief  Draws triangle on LCD
 * @note   @ref SH1106_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x1: First coordinate X location. Valid input is 0 to SH1106_WIDTH - 1
 * @param  y1: First coordinate Y location. Valid input is 0 to SH1106_HEIGHT - 1
 * @param  x2: Second coordinate X location. Valid input is 0 to SH1106_WIDTH - 1
 * @param  y2: Second coordinate Y location. Valid input is 0 to SH1106_HEIGHT - 1
 * @param  x3: Third coordinate X location. Valid input is 0 to SH1106_WIDTH - 1
 * @param  y3: Third coordinate Y location. Valid input is 0 to SH1106_HEIGHT - 1
 * @param  c: Color to be used. This parameter can be a value of @ref SH1106_COLOR_t enumeration
 * @retval None
 */
void SH1106_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SH1106_COLOR_t color);

/**
 * @brief  Draws circle to STM buffer
 * @note   @ref SH1106_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: X location for center of circle. Valid input is 0 to SH1106_WIDTH - 1
 * @param  y: Y location for center of circle. Valid input is 0 to SH1106_HEIGHT - 1
 * @param  r: Circle radius in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SH1106_COLOR_t enumeration
 * @retval None
 */
void SH1106_DrawCircle(int16_t x0, int16_t y0, int16_t r, SH1106_COLOR_t c);

/**
 * @brief  Draws filled circle to STM buffer
 * @note   @ref SH1106_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: X location for center of circle. Valid input is 0 to SH1106_WIDTH - 1
 * @param  y: Y location for center of circle. Valid input is 0 to SH1106_HEIGHT - 1
 * @param  r: Circle radius in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SH1106_COLOR_t enumeration
 * @retval None
 */
void SH1106_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SH1106_COLOR_t c);



#ifndef SH1106_I2C_TIMEOUT
#define SH1106_I2C_TIMEOUT					20000
#endif

/**
 * @brief  Initializes SH1106 LCD
 * @param  None
 * @retval Initialization status:
 *           - 0: LCD was not detected on I2C port
 *           - > 0: LCD initialized OK and ready to use
 */
void SH1106_I2C_Init();

/**
 * @brief  Writes single byte to slave
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  reg: register to write to
 * @param  data: data to be written
 * @retval None
 */
void SH1106_I2C_Write(uint8_t address, uint8_t reg, uint8_t data);

/**
 * @brief  Writes multi bytes to slave
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  reg: register to write to
 * @param  *data: pointer to data array to write it to slave
 * @param  count: how many bytes will be written
 * @retval None
 */
void SH1106_I2C_WriteMulti(uint8_t address, uint8_t reg, uint8_t *data, uint16_t count);

/**
 * @brief  Draws the Bitmap
 * @param  X:  X location to start the Drawing
 * @param  Y:  Y location to start the Drawing
 * @param  *bitmap : Pointer to the bitmap
 * @param  W : width of the image
 * @param  H : Height of the image
 * @param  color : 1-> white/blue, 0-> black
 */
void SH1106_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color);

// scroll the screen for fixed rows

void SH1106_ScrollRight(uint8_t start_row, uint8_t end_row);


void SH1106_ScrollLeft(uint8_t start_row, uint8_t end_row);


void SH1106_Scrolldiagright(uint8_t start_row, uint8_t end_row);


void SH1106_Scrolldiagleft(uint8_t start_row, uint8_t end_row);



void SH1106_Stopscroll(void);

void SH1106_ScrollDown(uint8_t start_row, uint8_t end_row);


// inverts the display i = 1->inverted, i = 0->normal

void SH1106_InvertDisplay (int i);






// clear the display

void SH1106_Clear (void);


/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
