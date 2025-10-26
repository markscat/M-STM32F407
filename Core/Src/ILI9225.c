/*
 * ILI9552.c
 *
 *  Created on: Oct 11, 2025
 *      Author: Ethan
 */

#include "ILI9225.h"
#include "main.h"
//#define ILI9225_CS_PORT   GPIOE
//#define ILI9225_CS_PIN    GPIO_PIN_9
//#define ILI9225_DC_PORT   GPIOE
//#define ILI9225_DC_PIN    GPIO_PIN_7
//#define ILI9225_RST_PORT  GPIOE
//#define ILI9225_RST_PIN   GPIO_PIN_8


// -------------------------------
// 基礎控制函數
// -------------------------------
void ILI9225_Init(void);                        // 初始化 LCD

// 硬體重置 LCD
void ILI9225_Reset(void){
    HAL_GPIO_WritePin(ILI9225_RST_PORT, ILI9225_RST_PIN, GPIO_PIN_RESET);
    ILI9225_DELAY(10); // 10ms
    HAL_GPIO_WritePin(ILI9225_RST_PORT, ILI9225_RST_PIN, GPIO_PIN_SET);
    ILI9225_DELAY(50); // 等待控制器穩定
}

/**寫入指令*/

void ILI9225_WriteCommand(uint16_t cmd){
    HAL_GPIO_WritePin(ILI9552_RS_GPIO_Port, ILI9552_RS_Pin, GPIO_PIN_RESET); // DC=0 → command
    HAL_GPIO_WritePin(ILI9552_CS_GPIO_Port, ILI9552_CS_Pin, GPIO_PIN_RESET); // CS low
    HAL_SPI_Transmit(&hspi1, (uint8_t*)&cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(ILI9225_CS_PORT, ILI9225_CS_PIN, GPIO_PIN_SET);   // CS high
}

/**
 * 寫入資料
 * */

void ILI9225_WriteData(uint16_t data){
	HAL_GPIO_WritePin(ILI9225_RS_PORT, ILI9225_RS_PIN, GPIO_PIN_SET);   // DC=1 → data
	HAL_GPIO_WritePin(ILI9225_CS_PORT, ILI9225_CS_PIN, GPIO_PIN_RESET); // CS low
	HAL_SPI_Transmit(&hspi1, (uint8_t*)&data, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(ILI9225_CS_PORT, ILI9225_CS_PIN, GPIO_PIN_SET);   // CS high


}
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
