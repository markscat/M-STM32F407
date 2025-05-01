/*
 * @file TM1638.c
 * @author TPruteanuVlad
 * @date Jan 19, 2025
 *
 *
 */


#include "TPruteanuVlad_TM1638.h"

#include "main.h"
//#include "gpio.h"
//#include <stdlib.h>
#include <string.h>
#include <math.h>

void tm1638_SDOhigh(TM1638 *tm)
{
    HAL_GPIO_WritePin(tm->dio_port, tm->dio_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(tm->dio_port, tm->dio_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(tm->dio_port, tm->dio_pin, GPIO_PIN_SET);
}
void tm1638_SDOlow(TM1638 *tm)
{
    HAL_GPIO_WritePin(tm->dio_port, tm->dio_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(tm->dio_port, tm->dio_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(tm->dio_port, tm->dio_pin, GPIO_PIN_RESET);
}

void tm1638_STBhigh(TM1638 *tm)
{
    HAL_GPIO_WritePin(tm->stb_port, tm->stb_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(tm->stb_port, tm->stb_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(tm->stb_port, tm->stb_pin, GPIO_PIN_SET);
}

void tm1638_STBlow(TM1638 *tm)
{
    HAL_GPIO_WritePin(tm->stb_port, tm->stb_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(tm->stb_port, tm->stb_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(tm->stb_port, tm->stb_pin, GPIO_PIN_RESET);
}

void tm1638_CLKhigh(TM1638 *tm)
{
    HAL_GPIO_WritePin(tm->clk_port, tm->clk_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(tm->clk_port, tm->clk_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(tm->clk_port, tm->clk_pin, GPIO_PIN_SET);
}
void tm1638_CLKlow(TM1638 *tm)
{
    HAL_GPIO_WritePin(tm->clk_port, tm->clk_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(tm->clk_port, tm->clk_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(tm->clk_port, tm->clk_pin, GPIO_PIN_RESET);
}

void tm1638_InitPins(TM1638 *tm)
{
    tm1638_CLKhigh(tm);
    tm1638_STBhigh(tm);
    HAL_GPIO_WritePin(tm->vcc_port, tm->vcc_pin, GPIO_PIN_SET);
}
// send signal to start transmission of data
void tm1638_StartPacket(TM1638 *tm)
{
    tm1638_CLKhigh(tm);
    tm1638_STBhigh(tm);
    tm1638_STBlow(tm);
    tm1638_CLKlow(tm);
}
// send signal to end transmission of data
void tm1638_EndPacket(TM1638 *tm)
{
    tm1638_CLKlow(tm);
    tm1638_STBlow(tm);
    tm1638_CLKhigh(tm);
    tm1638_STBhigh(tm);
}

// send signal necessary to confirm transmission of data when using fix address mode

void tm1638_Confirm(TM1638 *tm)
{
    tm1638_STBlow(tm);
    tm1638_SDOlow(tm);
    tm1638_STBhigh(tm);
    tm1638_SDOhigh(tm);
    tm1638_STBlow(tm);
    tm1638_SDOlow(tm);
}

void tm1638_SendData(TM1638 *tm, uint8_t Data)
{
    uint8_t ByteData[8] = {0};
    // convert data to bit array
    for (uint8_t j = 0; j < 8; j++)
    {
        ByteData[j] = (Data & (0x01 << j)) && 1;
    }
    // send bit array
    for (int8_t j = 0; j < 8; j++)
    {
        tm1638_CLKlow(tm);
        if (ByteData[j] == GPIO_PIN_SET)
        {
            tm1638_SDOhigh(tm);
        }
        else
        {
            tm1638_SDOlow(tm);
        }
        tm1638_CLKhigh(tm);
    }
}
void tm1638_DisplayClear(TM1638 *tm)
{
    tm1638_InitPins(tm);
    tm1638_StartPacket(tm);
    tm1638_SendData(tm, DATA_SET);
    tm1638_Confirm(tm);
    tm1638_SendData(tm, 0xc0);
    for (uint8_t i = 0; i < 16; i++)
    {
        tm1638_SendData(tm, 0x00);
    }
    tm1638_Confirm(tm);
    tm1638_SendData(tm, DISPLAY_OFF);
    tm1638_EndPacket(tm);
}


/**
 * @param brightness must be an integer between 0 and 7
 */
void tm1638_TurnOn(TM1638 *tm, uint8_t brightness)
{
    if (brightness > 0 && brightness < 8)
    {
        brightness = brightness | DISPLAY_ON;
        tm1638_SendData(tm, brightness);
    }
    else
    {
        tm1638_DisplayTxt(tm, "Error 1");
        return;
    }
}
/**
 * @param c must have a maximum length of 8(+ 8 maximum dots) and must contain only valid characters
 */
void tm1638_DisplayTxt(TM1638 *tm, char *c)
{

    tm1638_DisplayClear(tm);
    tm1638_DisplayClear(tm);
    tm1638_DisplayClear(tm);
    char padded[8] = "        ";
    uint8_t dot[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t p = 0;
    for (int8_t i = strlen(c) - 1; i >= 0; i--)
    {
        if (p == 8)
        {
            tm1638_DisplayTxt(tm, "Error 2");
            return;
        }

        if (c[i] == '.')
            dot[p] = 1;
        else
            padded[p++] = c[i];
    }
    for (int8_t i = 7; i >= 0; i--)
    {
        int hex;
        hex = chr_to_hex(padded[i]);
        if (hex >= 0)
        {
            if (dot[i])
            {
                tm1638_Seg(tm, 7 - i + 1, hex + 0x80);
            }
            else
            {
                tm1638_Seg(tm, 7 - i + 1, hex);
            }
        }
        else
        {
            tm1638_DisplayTxt(tm, "Error 7");
            return;
        }
    }

    tm1638_TurnOn(tm, 1);
}

void tm1638_Led(TM1638 *tm, int position, int on)
{
    if (!(position >= 1 && position <= 8))
    {
        tm1638_DisplayTxt(tm, "Error 3");
        return;
    }
    if (!(on == 0 || on == 1))
    {
        tm1638_DisplayTxt(tm, "Error 4");
        return;
    }
    tm1638_InitPins(tm);
    tm1638_StartPacket(tm);
    tm1638_SendData(tm, DATA_SET);
    tm1638_Confirm(tm);
    tm1638_SendData(tm, 0xc0 + 0x01 * 2 * position - 0x01);
    tm1638_CLKhigh(tm);
    tm1638_SDOhigh(tm);
    tm1638_SDOlow(tm);
    tm1638_SendData(tm, on);
    tm1638_Confirm(tm);
    tm1638_TurnOn(tm, 1);
    tm1638_EndPacket(tm);
}
void tm1638_Seg(TM1638 *tm, int position, int data)
{
    if (!(position >= 1 && position <= 8))
    {
        tm1638_DisplayTxt(tm, "Error 5");
        return;
    }
    if (!(data >= 0x00 && data <= 0xff))
    {
        tm1638_DisplayTxt(tm, "Error 6");
        return;
    }
    tm1638_InitPins(tm);
    tm1638_StartPacket(tm);
    tm1638_SendData(tm, DATA_SET);
    tm1638_Confirm(tm);
    tm1638_SendData(tm, 0xc0 + 0x01 * 2 * (position - 1));
    tm1638_CLKhigh(tm);
    tm1638_SDOhigh(tm);
    tm1638_SDOlow(tm);
    tm1638_SendData(tm, data);
    tm1638_Confirm(tm);
    tm1638_TurnOn(tm, 1);
    tm1638_EndPacket(tm);
}


uint8_t tm1638_ReadKey(TM1638 *tm)
{
	//tm->stb_port
	//uint32_t DIO_Pin;
	//GPIO_TypeDef DIO_GPIO_Port;
	//GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    HAL_GPIO_WritePin(tm->vcc_port, tm->vcc_pin, 1);

    int a[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    // S1... 1 S2... 9 S3... 17 S4... 25 S5... 5 S6... 13 S7... 21 S8... 29
    int ok = 1;
    int key;
    while (1)
    {
        if (ok)
        {
            tm1638_StartPacket(tm);
            tm1638_SendData(tm, DATA_READ);

            GPIO_InitStruct.Pin = tm->dio_pin;
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;

            HAL_GPIO_Init(tm->dio_port, &GPIO_InitStruct);
            for (int i = 0; i < 4; i++)
            {
                tm1638_CLKhigh(tm);
                for (int j = 0; j < 8; j++)
                {
                    tm1638_CLKhigh(tm);
                    HAL_Delay(1);
                    a[i * 8 + j] = HAL_GPIO_ReadPin(tm->dio_port, tm->dio_pin);
                    if (a[i * 8 + j] && j != 0)
                        ok = 0;
                    tm1638_CLKlow(tm);
                }
            }
            tm1638_CLKhigh(tm);
            tm1638_STBhigh(tm);
        }
        else
        {
            break;
        }
        GPIO_InitStruct.Pin = tm->dio_pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        HAL_GPIO_Init(tm->dio_port , &GPIO_InitStruct);
    }
    key = a[1] * 1 + a[9] * 2 + a[17] * 3 + a[25] * 4 + a[5] * 5 + a[13] * 6 + a[21] * 7 + a[29] * 8;
    return key;
}


void tm1638_Draw(TM1638 *tm)
{
    int pos = 0;
    int current = 0x00;
    int key = 0;
    tm1638_DisplayClear(tm);
    tm1638_DisplayClear(tm);
    tm1638_DisplayClear(tm);
    tm1638_DisplayClear(tm);
    HAL_GPIO_WritePin(tm->vcc_port, tm->vcc_pin, 1);
    while (1)
    {
        key = tm1638_ReadKey(tm);
        if (key == 1)
        {
            if (pos)
            {
                tm1638_Led(tm, pos, 0);
            }
            pos = pos % 8 + 1;
            tm1638_Led(tm, pos, 1);
            current = 0x00;
            HAL_Delay(150);
        }
        else
        {
            current = current ^ ((int)(pow(0x02, key - 2)));
            tm1638_Seg(tm, pos, current);
            HAL_Delay(150);
        }
    }
}

int8_t chr_to_hex(char c)
{
    switch (c)
    {
    case '0':
        return 0x3f;
    case '1':
        return 0x06;
    case '2':
        return 0x5b;
    case '3':
        return 0x4f;
    case '4':
        return 0x66;
    case '5':
        return 0x6d;
    case '6':
        return 0x7d;
    case '7':
        return 0x07;
    case '8':
        return 0x7f;
    case '9':
        return 0x6f;
    case ' ':
        return 0x00;
    case 'A':
        return 0x77;
    case 'B':
        return 0x7f;
    case 'C':
        return 0x39;
    case 'E':
        return 0x79;
    case 'F':
        return 0x71;
    case 'H':
        return 0x76;
    case 'I':
        return 0x06;
    case 'J':
        return 0x0e;
    case 'L':
        return 0x38;
    case 'O':
        return 0x3f;
    case 'P':
        return 0x73;
    case 'S':
        return 0x6d;
    case 'U':
        return 0x3e;
    case '_':
        return 0x08;
    case '-':
        return 0x40;
    case 'a':
        return 0x5f;
    case 'b':
        return 0x7c;
    case 'c':
        return 0x58;
    case 'd':
        return 0x5e;
    case 'h':
        return 0x74;
    case 'i':
        return 0x04;
    case 'n':
        return 0x54;
    case 'o':
        return 0x5c;
    case 'r':
        return 0x50;
    case 't':
        return 0x78;
    case 'u':
        return 0x1c;
    case 'y':
        return 0x6e;
    }
    return -0x01;
}

//#endif



