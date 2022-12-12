/*
 * CSpiBase.cpp
 *
 *  Created on: Aug 9, 2016
 *      Author: chris
 */

#include "CSpiBase.h"
#include "CSpiMaster.h"
#include "SerialUtil.h"
#include "InterruptPriority.h"

#include <new>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

#define min(x,y) (((x)<(y))?(x):(y))

CSpiBase  * CSpiBase::gpSpiBase [CSpiBase::eSPI_LAST] = {0, 0, 0};
//CSpiMaster * CSpiBase::gpSpiMaster[CSpiBase::eSPI_LAST] = {0, 0, 0};

CSpiBase::CSpiBase()
:
mMasterSlave  ( CSpiBase::eNOMASTERSLAVE ),
mSpiDuplexEnum( CSpiBase::eDUPLEX_LAST   ),
mSpiDeviceEnum( eSPI_LAST ),
mpSpi         ( 0         ),
mbSpiOK       ( false     ),
mMosiPin      ( eNO_PIN   ),
mMisoPin      ( eNO_PIN   ),
mSpiclkPin    ( eNO_PIN   ),
mSpiNssPin    ( eNO_PIN   ),
mAttnPin      ( eNO_PIN   ),
mpRxChannel   ( 0         ),
mpTxChannel   ( 0         ),
mInterruptPrioritySpi( PeripheralPriority::eNO_PRI ),
mInterruptPriorityDma( PeripheralPriority::eNO_PRI ),
mpReason      ( 0         ),
mpDeliverMsgCB( 0         ),
mpUser        ( 0         )
{
    // This one is a placeholder for the actual object
}

CSpiBase::CSpiBase(CSpiBase::MasterSlaveType select,
                   SpiDuplexType duplex,
                   SpiDeviceType spiDev,
                   PinNumberType mosiPin,
                   PinNumberType misoPin,
                   PinNumberType spiclkPin,
                   PinNumberType spiNssPin,
                   PinNumberType attnPin,
                   FPClckDivType clkDivisor,
                   bool          bDmaEnable
                  )
:
mMasterSlave  ( select     ),
mSpiDuplexEnum( duplex     ),
mSpiDeviceEnum( spiDev     ),
mpSpi         ( 0          ),
mbSpiOK       ( false      ),
mMosiPin      ( mosiPin    ),
mMisoPin      ( misoPin    ),
mSpiclkPin    ( spiclkPin  ),
mSpiNssPin    ( spiNssPin  ),
mAttnPin      ( attnPin    ),
mClockDiv     ( clkDivisor ),
mpRxChannel   ( 0          ),
mpTxChannel   ( 0          ),
mInterruptPrioritySpi( PeripheralPriority::eNO_PRI ),
mInterruptPriorityDma( PeripheralPriority::eNO_PRI ),
mpReason      ( 0          ),
mpDeliverMsgCB( 0          ),
mpUser        ( 0          ),
mbDmaEnabled  ( bDmaEnable )
{
  // other stuff ...
}

CSpiBase::~CSpiBase()
{
}

void CSpiBase::printSetup(void) const
{
}
