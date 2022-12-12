/*
 * InterruptPriority.h
 *
 *  Created on: Oct 21, 2016
 *      Author: Chris
 */

#ifndef INC_INTERRUPTPRIORITY_H_
#define INC_INTERRUPTPRIORITY_H_

namespace PeripheralPriority
{
    typedef
    enum : uint8_t
    {
        eNO_PRI  = 0,
        eSYSTICK,      //
        eI2C1,         //
        eI2C2,         //
        eDMA1_CHAN1,   //
        eDMA1_CHAN2,   //
        eDMA1_CHAN3,   //
        eDMA1_CHAN4,   //
        eDMA1_CHAN5,   //
        eDMA1_CHAN6,   //
        eDMA1_CHAN7,   //
        eDMA2_CHAN1,   //
        eDMA2_CHAN2,   //
        eDMA2_CHAN3,   //
        eDMA2_CHAN4,   //
        eDMA2_CHAN5,   //
        eADC1,         //
        eADC2,         //
        eSERIAL1,      //
        eSERIAL2,      //
        eSERIAL3,      //
        eSPI1,         //
        eSPI2,         //
        eSPI3,         //
    } Type;
}

#endif /* INC_INTERRUPTPRIORITY_H_ */
