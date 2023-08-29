/*
 * File:   CHoldInterrupts.cpp
 * Author: C. Arena
 *
 * Created on March 26, 2016, 11:07 AM
 */
#if  defined(STM32F303xC) || defined(STM32F303xE) || defined(STM32F303x8)
#include "stm32f3xx_hal.h"
#else
#define __get_PRIMASK(x) 0
#define __set_PRIMASK(x)
#endif

#include "CHoldInterrupts.h"

// See page 23 of PM0214 "STM32F3, STM32F4 and STM32L4 Series Cortex®-M4 programming manual"
// of April 2016, DocD022708 Rev 5 regarding the primask register
//
// __get_PRIMASK() returning 1 means that the processor is not interruptable
// __get_PRIMASK() returning 0 means that the processor IS interruptable
//

CHoldInterrupts::CHoldInterrupts()
:mPriMask( __get_PRIMASK() )
{
    if ((mPriMask & 1) == 0)
    {
        __set_PRIMASK( 1 );
    }
}

CHoldInterrupts::~CHoldInterrupts()
{
    __set_PRIMASK( mPriMask );
}

bool CHoldInterrupts::getPriMask (void)
{
    return __get_PRIMASK() & 1;
}

