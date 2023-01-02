/*
 * mySysTick.cpp
 *
 *  Created on: Dec 13, 2022
 *      Author: christopherarena
 */
#include "main.h"
#include "mySysTick.h"
#include "CFormattedBuffer.h"

extern CFormattedBuffer buffer;


volatile uint32_t delay  = 0;
volatile bool     bDelay = false;

volatile uint32_t spiDelay  = 0;
volatile bool     bSpiDelay = false;

uint32_t oneSec = 999;


void mySysTick(void)
{
	if (delay)
	{
		--delay;

		if (delay == 0)
		{
			bDelay = true;
		}
	}

	if (spiDelay)
	{
		--spiDelay;

		if (spiDelay == 0)
		{
			bSpiDelay = true;
		}
	}

	if (oneSec)
	{
		--oneSec;
	}
	else
	{
		oneSec = 999;
		HAL_GPIO_TogglePin( LD2_GPIO_Port, LD2_Pin );
	}
}
