/*
 * ILI9552.h
 *
 *  Created on: Oct 11, 2025
 *      Author: Ethan
 */

#ifndef INC_ILI9225_H_
#define INC_ILI9225_H_

#include <stdint.h>   // for uint8_t, uint16_t
#include "spi.h"


// ======================================================
// ILI9225 LCD Controller Register Definitions
// ======================================================

// Driver output control
#define ILI9225_DRIVER_OUTPUT_CTRL         0x01
#define ILI9225_LCD_AC_DRIVING_CTRL        0x02
#define ILI9225_ENTRY_MODE                 0x03
#define ILI9225_DISPLAY_CTRL1              0x07
#define ILI9225_BLANK_PERIOD_CTRL1         0x08
#define ILI9225_FRAME_CYCLE_CTRL           0x0B
#define ILI9225_INTERFACE_CTRL             0x0C
#define ILI9225_OSC_CTRL                   0x0F

// Power control
#define ILI9225_POWER_CTRL1                0x10
#define ILI9225_POWER_CTRL2                0x11
#define ILI9225_POWER_CTRL3                0x12
#define ILI9225_POWER_CTRL4                0x13
#define ILI9225_POWER_CTRL5                0x14

// GRAM (Display RAM)
#define ILI9225_GRAM_ADDR_SET1             0x20
#define ILI9225_GRAM_ADDR_SET2             0x21
#define ILI9225_GRAM_DATA_REG              0x22

// Gate and partial display
#define ILI9225_GATE_SCAN_CTRL             0x30
#define ILI9225_VERTICAL_SCROLL_CTRL1      0x31
#define ILI9225_VERTICAL_SCROLL_CTRL2      0x32
#define ILI9225_VERTICAL_SCROLL_CTRL3      0x33
#define ILI9225_PARTIAL_DRIVING_POS1       0x34
#define ILI9225_PARTIAL_DRIVING_POS2       0x35
#define ILI9225_HORIZONTAL_WINDOW_ADDR1    0x36
#define ILI9225_HORIZONTAL_WINDOW_ADDR2    0x37
#define ILI9225_VERTICAL_WINDOW_ADDR1      0x38
#define ILI9225_VERTICAL_WINDOW_ADDR2      0x39

// Gamma control
#define ILI9225_GAMMA_CTRL1                0x50
#define ILI9225_GAMMA_CTRL2                0x51
#define ILI9225_GAMMA_CTRL3                0x52
#define ILI9225_GAMMA_CTRL4                0x53
#define ILI9225_GAMMA_CTRL5                0x54
#define ILI9225_GAMMA_CTRL6                0x55
#define ILI9225_GAMMA_CTRL7                0x56
#define ILI9225_GAMMA_CTRL8                0x57
#define ILI9225_GAMMA_CTRL9                0x58
#define ILI9225_GAMMA_CTRL10               0x59


// -------------------------------
// 基礎控制函數
// -------------------------------
void ILI9225_Init(void);                        // 初始化 LCD
void ILI9225_Reset(void);                       // 硬體重置 LCD
void ILI9225_WriteCommand(uint16_t cmd);        // 寫入指令
void ILI9225_WriteData(uint16_t data);          // 寫入資料
void ILI9225_WriteRegister(uint16_t reg, uint16_t data);  // 寫入一組暫存器資料
uint16_t ILI9225_ReadData(void);                // （可選）讀取資料

// -------------------------------
// 顯示設定
// -------------------------------
void ILI9225_SetOrientation(uint8_t mode);      // 設定畫面方向（0~3）
void ILI9225_SetWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);  // 定義繪圖區域
void ILI9225_SetCursor(uint16_t x, uint16_t y); // 設定目前游標位置
void ILI9225_Sleep(bool enable);                // 進入/退出睡眠模式
void ILI9225_DisplayOn(void);                   // 開啟顯示
void ILI9225_DisplayOff(void);                  // 關閉顯示

// -------------------------------
// 繪圖功能
// -------------------------------
void ILI9225_DrawPixel(uint16_t x, uint16_t y, uint16_t color);  // 畫單一像素
void ILI9225_FillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color); // 填滿矩形
void ILI9225_Clear(uint16_t color);            // 清屏
void ILI9225_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color); // 畫線
void ILI9225_DrawCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);               // 畫圓
void ILI9225_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color); // 畫框

// -------------------------------
// 字型與文字（可選擴充）
// -------------------------------
void ILI9225_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg);
void ILI9225_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg);

// -------------------------------
// SPI介面相關 (底層)
// -------------------------------
void ILI9225_Select(void);     // CS = 0
void ILI9225_Unselect(void);   // CS = 1
void ILI9225_DC_Command(void); // RS/DC = 0
void ILI9225_DC_Data(void);    // RS/DC = 1
void ILI9225_SPI_Write(uint8_t data);  // 實際傳輸一個 byte





#endif // _ILI9225_REGS_H_

