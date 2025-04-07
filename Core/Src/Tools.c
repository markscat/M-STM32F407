/**
  ******************************************************************************
  * @file    delay_us.c
  * @author  MCD Application Team
  * @brief   Delay function 
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) GUN
  *
  ******************************************************************************
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
**/
#include "Tools.h"


void Delay_us(uint32_t us){
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload = SysTick -> LOAD +1;
	ticks = us*(SystemCoreClock/1000000);
	told= SysTick-> VAL;
	while(1){
		tnow = SysTick-> VAL;
		if(tnow !=told)
		{
			if(tnow<told)
			{
				tcnt+= told-tnow;
			}else{
				tcnt+= reload-tnow+told;
			}
			told=tnow;
			if(tcnt>=ticks)break;
		}
	}
}
