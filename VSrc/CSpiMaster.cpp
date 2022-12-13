/*
 * CSpiMaster.cpp
 *
 *  Created on: Dec 12, 2022
 *      Author: christopherarena
 */

#include "CSpiMaster.h"
#include "CHoldInterrupts.h"

CSpiMaster * CSpiMaster::gpSpiMaster = nullptr;

CSpiMaster::CSpiMaster( SPI_HandleTypeDef * pSpi )
: mpSpiDev ( pSpi )
{
	if (gpSpiMaster) while(1) {} // should get attention

	gpSpiMaster = this;
}

CSpiMaster::~CSpiMaster()
{
	gpSpiMaster = nullptr;
}

bool CSpiMaster::init(void)
{
	bool bRetVal = false;

	HAL_StatusTypeDef status = HAL_ERROR;

	if (mpSpiDev->hdmatx && mpSpiDev->hdmarx )
	{
		status = HAL_DMA_RegisterCallback( mpSpiDev->hdmatx,
										   HAL_DMA_XFER_CPLT_CB_ID,
										   CSpiMaster::dmaComplete
										 );
	}
//	else
//	{
//		status = HAL_SPI_RegisterCallback( mpSpiDev,
//										   HAL_SPI_TX_COMPLETE_CB_ID,
//										   CSpiMaster::transferComplete
//									     );
//	}

	bRetVal = status == HAL_OK;

	return bRetVal;
}

bool CSpiMaster::sendRcv( uint8_t * pBufOut, uint8_t * pBufIn, uint16_t size, int & sndRcvStatus)
{
	bool bRetVal = false;

	do
	{
		if (!pBufOut) break;    // can't be null
		if (!pBufIn ) break;    // can't be null
		if (!size   ) break;    // must be > 0 bytes

		if (!mpSpiDev->hdmatx) break; // must be in DMA mode
		if (!mpSpiDev->hdmarx) break; // must be in DMA mode

		mpBufOut    = pBufOut;  // record the buffer addresses and size
		mpBufIn     = pBufIn;
		mSndRcvSize = size;

		HAL_StatusTypeDef status;

		status = HAL_SPI_TransmitReceive_DMA(mpSpiDev, pBufOut, pBufIn, size );

		bRetVal = status == HAL_OK;

	} while(0);

	return bRetVal;
}

bool CSpiMaster::transferComplete(void)
{
	bool bRetVal = false;

	do
	{
		if (!mpCB) break;

		(*mpCB)( mpSpiDev, mpBufIn, mpBufOut, mSndRcvSize );

		bRetVal = true;

	} while(0);

	return bRetVal;
}

bool CSpiMaster::dmaComplete(void)
{
	bool bRetVal = false;

	do
	{
		if (!mpCB) break;

		(*mpCB)( mpSpiDev, mpBufIn, mpBufOut, mSndRcvSize );

		bRetVal = true;

	} while(0);

	return bRetVal;
}


void CSpiMaster::transferComplete( SPI_HandleTypeDef * pSpi )
{
	do
	{
		if (!pSpi       ) break;
		if (!gpSpiMaster) break;

		if (gpSpiMaster->mpSpiDev == pSpi)
		{
			gpSpiMaster->transferComplete();
		}

	} while(0);
}

void CSpiMaster::dmaComplete     ( DMA_HandleTypeDef * pDma )
{
	do
	{
		if (!pDma) break;

		if (!gpSpiMaster) break;

		if (!gpSpiMaster->mpSpiDev) break;

		if ( gpSpiMaster->mpSpiDev->hdmatx != pDma) break;

		gpSpiMaster->dmaComplete();

	} while(0);
}

bool CSpiMaster::setCallback( transmitCompleteCB * pCB )
{
	bool bRetVal = false;

	do
	{
		if (mpCB) break;

		mpCB = pCB;

		bRetVal = true;

	} while(0);


	return bRetVal;
}

