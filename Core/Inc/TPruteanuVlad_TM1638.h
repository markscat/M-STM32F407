/*
 * TPruteanuVlad_TM1638.h
 *
 *  Created on: Jan 19, 2025
 *      Author: user
 */

#ifndef INC_TPRUTEANUVLAD_TM1638_H_
#define INC_TPRUTEANUVLAD_TM1638_H_



#include "main.h"


#define DATA_SET 0x40
#define DATA_READ 0x42
#define DISPLAY_ON 0x88
#define DISPLAY_OFF 0x80

typedef struct
{
    GPIO_TypeDef *clk_port;
    GPIO_TypeDef *dio_port;
    GPIO_TypeDef *stb_port;
    GPIO_TypeDef *vcc_port;
    uint32_t clk_pin, dio_pin, stb_pin, vcc_pin;
    int key;

} TM1638;

void tm1638_STBhigh(TM1638 *tm);
void tm1638_STBlow(TM1638 *tm);
void tm1638_CLKhigh(TM1638 *tm);
void tm1638_CLKlow(TM1638 *tm);
void tm1638_SDOhigh(TM1638 *tm);
void tm1638_SDOlow(TM1638 *tm);
void tm1638_SendData(TM1638 *tm, uint8_t Data);
void tm1638_StartPacket(TM1638 *tm);
void tm1638_EndPacket(TM1638 *tm);
void tm1638_Confirm(TM1638 *tm);
void tm1638_InitPins(TM1638 *tm);
void tm1638_DisplayClear(TM1638 *tm);
void tm1638_TurnOn(TM1638 *tm, uint8_t brightness);
void tm1638_Led(TM1638 *tm, int position, int on);
void tm1638_Seg(TM1638 *tm, int position, int data);
void tm1638_DisplayTxt(TM1638 *tm, char *c);
void tm1638_Draw(TM1638 *tm);
int8_t chr_to_hex(char c);


#endif /* INC_TPRUTEANUVLAD_TM1638_H_ */
