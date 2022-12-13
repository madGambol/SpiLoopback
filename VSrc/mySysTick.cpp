/*
 * mySysTick.cpp
 *
 *  Created on: Dec 13, 2022
 *      Author: christopherarena
 */

#include "mySysTick.h"
#include "CFormattedBuffer.h"

extern CFormattedBuffer buffer;


uint32_t delay  = 0;
bool     bDelay = false;

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
}
