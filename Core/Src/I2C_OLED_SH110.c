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
#include <I2C_OLED_fonts.h>
#include <string.h>
#include <I2C_OLED_SH110.h>
#include <main.h>

extern I2C_HandleTypeDef hi2c1;

//<250803 新增>
#define OLED_COL_OFFSET 2
//</250803 新增>

#define SH1106_I2C &hi2c1

static uint8_t SH1106_Buffer[SH1106_WIDTH * SH1106_HEIGHT / 8];

/* Private SH1106 structure */
typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} SH1106_t;

/* Private variable */
static SH1106_t SH1106;

#define SH1106_NORMALDISPLAY       0xA6
#define SH1106_INVERTDISPLAY       0xA7

//<250803 新增>

static const uint8_t OLED_InitCmd[] = {
    0xAE,
    0xD5, 0x80,
    0xA8, 0x3F,
    0xD3, 0x00,
    0x40,
    0xAD, 0x8B,
    0xA1,
    0xC8,
    0xDA, 0x12,
    0x81, 0x7F,
    0xD9, 0x22,
    0xDB, 0x20,
    0xA4,
    0xA6,
    0xAF
};
//<0805新增,待測>
void OLED_WriteCommand_V2(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd}; // Control byte (0x00) + Command
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDRESS, buf, 2, HAL_MAX_DELAY);
}

void OLED_WriteData_V2(uint8_t data) {
    uint8_t buf[2] = {0x40, data}; // Control byte (0x40) + Data
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDRESS, buf, 2, HAL_MAX_DELAY);
}
//</0805新增,待測>


void SH1106_OLED_WriteCommand(uint8_t cmd) {
    HAL_I2C_Mem_Write(&hi2c1, SH1106_I2C_ADDR, OLED_CMD, 1, &cmd, 1, HAL_MAX_DELAY);
}

void SH1106_OLED_Init(void) {
    HAL_Delay(100);
    for (uint8_t i = 0; i < sizeof(OLED_InitCmd); i++) {
    	SH1106_OLED_WriteCommand(OLED_InitCmd[i]);
    }
}

void SH1106_OLED_SetPos(uint8_t x, uint8_t page) {
    x += OLED_COL_OFFSET;
    SH1106_OLED_WriteCommand(0xB0 + page);
    SH1106_OLED_WriteCommand(0x00 + (x & 0x0F));
    SH1106_OLED_WriteCommand(0x10 + ((x >> 4) & 0x0F));
}

void SH1106_OLED_Clear(void) {
    uint8_t zero[128] = {0};
    for (uint8_t page = 0; page < 8; page++) {
    	SH1106_OLED_SetPos(0, page);
        HAL_I2C_Mem_Write(&hi2c1, SH1106_I2C_ADDR, OLED_DATA, 1, zero, 128, HAL_MAX_DELAY);
    }
}

void SH1106_OLED_UpdateScreen(uint8_t *buffer) {
    for (uint8_t page = 0; page < 8; page++) {
    	SH1106_OLED_SetPos(0, page);
        HAL_I2C_Mem_Write(&hi2c1, SH1106_I2C_ADDR, OLED_DATA, 1, &buffer[page * 128], 128, HAL_MAX_DELAY);
    }
}

void SH1106_OLED_DrawPixel(uint8_t *buffer, uint8_t x, uint8_t y, uint8_t color) {
    if (x >= 128 || y >= 64) return;
    uint16_t byteIndex = x + (y / 8) * 128;
    if (color)
        buffer[byteIndex] |=  (1 << (y % 8));
    else
        buffer[byteIndex] &= ~(1 << (y % 8));
}


//</250803 新增>


uint8_t SH1106_Init(void) {

		HAL_Delay(100); // 等待電源穩定

		/* Check if LCD connected to I2C */
		if (HAL_I2C_IsDeviceReady(SH1106_I2C, SH1106_I2C_ADDR, 1, 20000) != HAL_OK) {
			printf("\n\n OLED Display init. fail \n\n");
			return 0;
		}

		// --- 標準水平定址模式 初始化序列 ---
	    SH1106_WRITECOMMAND(0xAE); // 關閉顯示

	    SH1106_WRITECOMMAND(0xD5); // 設置顯示時鐘分頻
	    SH1106_WRITECOMMAND(0x80); // 預設值

	    SH1106_WRITECOMMAND(0xA8); // 設置 Multiplex Ratio
	    SH1106_WRITECOMMAND(0x3F); // 適用於 64 行

	    SH1106_WRITECOMMAND(0xD3); // 設置顯示偏移
	    SH1106_WRITECOMMAND(0x00); // 無偏移

	    SH1106_WRITECOMMAND(0x40); // 設置顯示起始行 (0)

	    SH1106_WRITECOMMAND(0x8D); // --- 開啟電荷泵 ---
	    SH1106_WRITECOMMAND(0x14); // --- 啟用 ---

	    SH1106_WRITECOMMAND(0x20); // --- 設置記憶體定址模式 ---
	    SH1106_WRITECOMMAND(0x00); // --- 0x00 = 水平定址模式 ---

	    SH1106_WRITECOMMAND(0xA1); // 設置 Segment Re-map (A1 = 列位址 127 對應 SEG0)
	    SH1106_WRITECOMMAND(0xC8); // 設置 COM 掃描方向 (C8 = 從上到下)

	    SH1106_WRITECOMMAND(0xDA); // 設置 COM 引腳硬體配置
	    SH1106_WRITECOMMAND(0x12);

	    SH1106_WRITECOMMAND(0x81); // 設置對比度
	    SH1106_WRITECOMMAND(0xCF);

	    SH1106_WRITECOMMAND(0xD9); // 設置預充電週期
	    SH1106_WRITECOMMAND(0xF1);

	    SH1106_WRITECOMMAND(0xDB); // 設置 VCOMH
	    SH1106_WRITECOMMAND(0x40);

	    SH1106_WRITECOMMAND(0xA4); // 恢復到 RAM 內容顯示
	    SH1106_WRITECOMMAND(0xA6); // 正常顯示 (非反白)

	    SH1106_WRITECOMMAND(0xAF); // --- 開啟顯示 ---

		/* 初始化後清屏 */
		SH1106_Fill(SH1106_COLOR_BLACK);
		SH1106_UpdateScreen();

		/* 設定預設值 */
		SH1106.CurrentX = 0;
		SH1106.CurrentY = 0;
		SH1106.Initialized = 1;

		return 1;
	
}


uint8_t SH1106_Init_org(void) {

	/* Check if LCD connected to I2C */
	if (HAL_I2C_IsDeviceReady(SH1106_I2C, SH1106_I2C_ADDR, 1, 20000) != HAL_OK) {
		/* Return false */
		printf("\n\n OLED Display init. fail \n\n");
		return 0;
	}
	
	/* A little delay */
	uint32_t p = 2500;
	while(p>0)
		p--;
	
//#define SH1106_command
#define SSD1306_command
//#define oled


	  // Initialize the display
#ifdef SH1106_command
	printf("WRITECOMMAND=AE \n\n ");
	SH1106_WRITECOMMAND(0xAE); //display off
	printf("WRITECOMMAND=0x02 \n\n ");
	//SH1106_WRITECOMMAND(0xB0|0x00); //Set Page Start Address for Page Addressing Mode,0-7
	SH1106_WRITECOMMAND(0x02);
	printf("WRITECOMMAND=81 \n\n ");
	SH1106_WRITECOMMAND(0x81); //--set contrast control register
	printf("WRITECOMMAND =FF\n\n");
	SH1106_WRITECOMMAND(0xFF); // contrast value
	printf("WRITECOMMAND =A1\n\n");
	SH1106_WRITECOMMAND(0xA1); //--set segment re-map 0 to 127
	printf("WRITECOMMAND =A6\n\n");
	SH1106_WRITECOMMAND(0xA6); //--set normal display
	printf("WRITECOMMAND =A8\n\n");
	SH1106_WRITECOMMAND(0xA8); //--set multiplex ratio(1 to 64)
	printf("WRITECOMMAND =3F\n\n");
	SH1106_WRITECOMMAND(0x3F); // multiplex value
	printf("WRITECOMMAND =AD\n\n ");
	SH1106_WRITECOMMAND(0xAD); // Set Pump Mode
	printf("WRITECOMMAND =88\n\n");
	SH1106_WRITECOMMAND(0x8B); // Pump ON
	printf("WRITECOMMAND =30|20\n\n");
	SH1106_WRITECOMMAND(0x30|0x02); // Set Pump Voltage 8.0
	printf("WRITECOMMAND =C8\n\n");
	SH1106_WRITECOMMAND(0xC8); //Set COM Output Scan Direction
	printf("WRITECOMMAND =D3\n\n");
	SH1106_WRITECOMMAND(0xD3); //-set display offset
	printf("WRITECOMMAND =00\n\n");
	SH1106_WRITECOMMAND(0x00); //-not offset
	printf("WRITECOMMAND =D5\n\n");
	SH1106_WRITECOMMAND(0xD5); //--set display clock divide ratio/oscillator frequency
	printf("WRITECOMMAND =80\n\n");
	SH1106_WRITECOMMAND(0x80); //--set divide ratio
	printf("WRITECOMMAND =D9\n\n");
	SH1106_WRITECOMMAND(0xD9); //--set pre-charge period
	printf("WRITECOMMAND =1F\n\n");
	SH1106_WRITECOMMAND(0x1F); //
	printf("WRITECOMMAND =DA\n\n");
	SH1106_WRITECOMMAND(0xDA); //--set com pins hardware configuration
	printf("WRITECOMMAND =12\n\n");
	SH1106_WRITECOMMAND(0x12);
	printf("WRITECOMMAND =DB\n\n");
	SH1106_WRITECOMMAND(0xDB); //--set vcomh
	printf("WRITECOMMAND =40\n\n");
	SH1106_WRITECOMMAND(0x40); //
	printf("WRITECOMMAND =AF\n\n");
	SH1106_WRITECOMMAND(0xAF); //--turn on SH1106 panel
#endif


#ifdef SSD1306_command

    SH1106_WRITECOMMAND(0xAE); /*display off*/
    SH1106_WRITECOMMAND(0x02); /*set lower column address*/
    //SH1106_WRITECOMMAND(0x10); /*set higher column address*/
    SH1106_WRITECOMMAND(0x01); /*set higher column address*/
    SH1106_WRITECOMMAND(0x40); /*set display start line*/
    SH1106_WRITECOMMAND(0xB0); /*set page address*/
    SH1106_WRITECOMMAND(0x81); /*contract control*/
    SH1106_WRITECOMMAND(0xFF); /*128*/
    SH1106_WRITECOMMAND(0xA1); /*set segment remap*/
    SH1106_WRITECOMMAND(0xA6); /*normal / reverse*/
    SH1106_WRITECOMMAND(0xA8); /*multiplex ratio*/
    SH1106_WRITECOMMAND(0x3F); /*duty = 1/64*/
    SH1106_WRITECOMMAND(0xC8); /*Com scan direction*/
    SH1106_WRITECOMMAND(0xD3); /*set display offset*/
    SH1106_WRITECOMMAND(0x00);
    SH1106_WRITECOMMAND(0xD5); /*set osc division*/
    SH1106_WRITECOMMAND(0x80);
    SH1106_WRITECOMMAND(0xD9); /*set pre-charge period*/
    SH1106_WRITECOMMAND(0x1F);
    SH1106_WRITECOMMAND(0xDA); /*set COM pins*/
    SH1106_WRITECOMMAND(0x12);
    SH1106_WRITECOMMAND(0xDB); /*set vcomh*/
    SH1106_WRITECOMMAND(0x30);
    SH1106_WRITECOMMAND(0x8D); /*set charge pump enable*/
    SH1106_WRITECOMMAND(0x14); /*enabled*/
    SH1106_WRITECOMMAND(0xAF); /*display ON*/

#endif

#ifdef oled

    	SH1106_WRITECOMMAND(0xAE); //display off
    	SH1106_WRITECOMMAND(0x20);	//Set Memory Addressing Mode
    	SH1106_WRITECOMMAND(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    	SH1106_WRITECOMMAND(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
    	SH1106_WRITECOMMAND(0xc8);	//Set COM Output Scan Direction
    	SH1106_WRITECOMMAND(0x00); //---set low column address
    	SH1106_WRITECOMMAND(0x10); //---set high column address
    	SH1106_WRITECOMMAND(0x40); //--set start line address
    	SH1106_WRITECOMMAND(0x81); //--set contrast control register
    	SH1106_WRITECOMMAND(0xff); //亮度调节 0x00~0xff
    	SH1106_WRITECOMMAND(0xa1); //--set segment re-map 0 to 127
    	SH1106_WRITECOMMAND(0xa6); //--set normal display
    	SH1106_WRITECOMMAND(0xa8); //--set multiplex ratio(1 to 64)
    	SH1106_WRITECOMMAND(0x3F); //
    	SH1106_WRITECOMMAND(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    	SH1106_WRITECOMMAND(0xd3); //-set display offset
    	SH1106_WRITECOMMAND(0x00); //-not offset
    	SH1106_WRITECOMMAND(0xd5); //--set display clock divide ratio/oscillator frequency
    	SH1106_WRITECOMMAND(0xf0); //--set divide ratio
    	SH1106_WRITECOMMAND(0xd9); //--set pre-charge period
    	SH1106_WRITECOMMAND(0x22); //
    	SH1106_WRITECOMMAND(0xda); //--set com pins hardware configuration
    	SH1106_WRITECOMMAND(0x12);
    	SH1106_WRITECOMMAND(0xdb); //--set vcomh
    	SH1106_WRITECOMMAND(0x20); //0x20,0.77xVcc
    	SH1106_WRITECOMMAND(0x8d); //--set DC-DC enable
    	SH1106_WRITECOMMAND(0x14); //
    	SH1106_WRITECOMMAND(0xaf); //--turn on oled panel
#endif

	printf("write command is ok\n\n");

	/* Clear screen */
	SH1106_Fill(SH1106_COLOR_BLACK);

	/* Update screen */
	SH1106_UpdateScreen();
	

	/* Set default values */
	SH1106.CurrentX = 0;
	SH1106.CurrentY = 0;
	
	/* Initialized OK */
	SH1106.Initialized = 1;
	
	/* Return OK */
	return 1;
}

// --- 發送數據的輔助函式 (用來更新畫面) ---
void OLED_Write_Data(uint8_t* data, uint16_t size)
{
    uint8_t* temp_buf = malloc(size + 1);
    temp_buf[0] = 0x40; // 0x40 代表接下來是 Data
    memcpy(temp_buf+1, data, size);
    HAL_I2C_Master_Transmit(SH1106_I2C, SH1106_I2C_ADDR, temp_buf, size + 1, HAL_MAX_DELAY);
    free(temp_buf);
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

void SH1106_UpdateScreen(void) {
    // 1. 告訴晶片我們要更新的範圍
    SH1106_WRITECOMMAND(0x21); // 設置列位址
    SH1106_WRITECOMMAND(0);    // 起始列 0
    SH1106_WRITECOMMAND(127);  // 結束列 127

    SH1106_WRITECOMMAND(0x22); // 設置頁位址
    SH1106_WRITECOMMAND(0);    // 起始頁 0
    SH1106_WRITECOMMAND(7);    // 結束頁 7 (對應 64 行)

    // 2. 一次性將整個 1024 位元組的緩衝區數據，透過 I2C 發送出去
    SH1106_I2C_WriteMulti(SH1106_I2C_ADDR, 0x40, SH1106_Buffer, sizeof(SH1106_Buffer));
}




void SH1106_UpdateScreen_org(void) {
	uint8_t m;
	
	for (m = 0; m < 8; m++) {
		SH1106_WRITECOMMAND(0xB0 + m);
		SH1106_WRITECOMMAND(0x02);
		SH1106_WRITECOMMAND(0x10);
		
		/* Write multi data */
		SH1106_I2C_WriteMulti(SH1106_I2C_ADDR, 0x40, &SH1106_Buffer[SH1106_WIDTH * m], SH1106_WIDTH);
	}
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

void SH1106_Fill(SH1106_COLOR_t color) {
	/* Set memory */
	memset(SH1106_Buffer, (color == SH1106_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(SH1106_Buffer));
}

void SH1106_DrawPixel(uint16_t x, uint16_t y, SH1106_COLOR_t color) {
	if (
		x >= SH1106_WIDTH ||
		y >= SH1106_HEIGHT
	) {
		/* Error */
		return;
	}
	
	/* Check if pixels are inverted */
	if (SH1106.Inverted) {
		color = (SH1106_COLOR_t)!color;
	}
	
	/* Set color */
	if (color == SH1106_COLOR_WHITE) {
		SH1106_Buffer[x + (y / 8) * SH1106_WIDTH] |= 1 << (y % 8);
	} else {
		SH1106_Buffer[x + (y / 8) * SH1106_WIDTH] &= ~(1 << (y % 8));
	}
}

void SH1106_GotoXY(uint16_t x, uint16_t y) {
	/* Set write pointers */
	SH1106.CurrentX = x;
	SH1106.CurrentY = y;
}

char SH1106_Putc(char ch, FontDef_t* Font, SH1106_COLOR_t color) {
	uint32_t i, b, j;
	
	/* Check available space in LCD */
	if (
		SH1106_WIDTH <= (SH1106.CurrentX + Font->FontWidth) ||
		SH1106_HEIGHT <= (SH1106.CurrentY + Font->FontHeight)
	) {
		/* Error */
		return 0;
	}
	
	/* Go through font */
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
	
	/* Increase pointer */
	SH1106.CurrentX += Font->FontWidth;
	
	/* Return character written */
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
	
	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
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
 


void SH1106_Clear (void)
{
	SH1106_Fill (0);
    SH1106_UpdateScreen();
}
void SH1106_ON(void) {
	SH1106_WRITECOMMAND(0x8D);
	SH1106_WRITECOMMAND(0x14);
	SH1106_WRITECOMMAND(0xAF);
}
void SH1106_OFF(void) {
	SH1106_WRITECOMMAND(0x8D);
	SH1106_WRITECOMMAND(0x10);
	SH1106_WRITECOMMAND(0xAE);
}

void SH1106_I2C_WriteMulti(uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) {
uint8_t dt[256];
dt[0] = reg;
uint8_t i;
for(i = 0; i < count; i++)
dt[i+1] = data[i];
HAL_I2C_Master_Transmit(SH1106_I2C, address, dt, count+1, 10);
}


	void SH1106_I2C_Write(uint8_t address, uint8_t reg, uint8_t data) {
		uint8_t dt[2];
		dt[0] = reg;
		dt[1] = data;
		HAL_I2C_Master_Transmit(SH1106_I2C, address, dt, 2, 10);
	}

void SH1106_InvertDisplay (int i)
{
  if (i) SH1106_WRITECOMMAND (SH1106_INVERTDISPLAY);

  else SH1106_WRITECOMMAND (SH1106_NORMALDISPLAY);

}


void SH1106_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color)
{

    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
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
