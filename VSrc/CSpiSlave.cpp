/*
 * File:   CSpiSlave.cpp
 * Author: C. Arena
 *
 * Created on April 8, 2016, 4:49 PM
 */

#include "main.h"
#include "CSpiSlave.h"
#include <cstdint>
#include <string.h>
#include <stddef.h>

#define min(x,y) (((x)<(y))?(x):(y))

CSpiSlave * CSpiSlave::gpSpiSlave = nullptr;


bool CSpiSlave::init(void)
{
	bool bRetVal = false;

	HAL_StatusTypeDef status;

	status = HAL_DMA_RegisterCallback( mpSpiDev->hdmatx,
									   HAL_DMA_XFER_CPLT_CB_ID,
									   CSpiSlave::dmaComplete
									 );
	bRetVal = status == HAL_OK;

	return bRetVal;
}

CSpiSlave::CSpiSlave( SPI_HandleTypeDef * pSpi )
:
mpSpiDev( pSpi )
{
	if (gpSpiSlave) while (1) {}

	gpSpiSlave = this;
}


CSpiSlave::~CSpiSlave()
{
}

bool CSpiSlave::sendRcv( uint8_t *pBufOut,
		                 uint8_t *pBufIn,
						 uint16_t size,
						 int &sndRcvStatus
					   )
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

		status      = HAL_SPI_TransmitReceive_DMA(mpSpiDev, pBufOut, pBufIn, size );

		bRetVal     = status == HAL_OK;

	} while(0);

	return bRetVal;
}

bool CSpiSlave::transferComplete(void)
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

bool CSpiSlave::dmaComplete(void)
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


void CSpiSlave::transferComplete( SPI_HandleTypeDef * pSpi )
{
	do
	{
		if (!pSpi       ) break;
		if (!gpSpiSlave) break;

		if (gpSpiSlave->mpSpiDev != pSpi) break;

		gpSpiSlave->transferComplete();

	} while(0);
}

void CSpiSlave::dmaComplete     ( DMA_HandleTypeDef * pDma )
{
	do
	{
		if (!pDma) break;

		if (!gpSpiSlave) break;

		if (!gpSpiSlave->mpSpiDev) break;

		if ( gpSpiSlave->mpSpiDev->hdmatx != pDma) break;

		gpSpiSlave->dmaComplete();

	} while(0);
}

bool CSpiSlave::setCallback( transmitCompleteCB * pCB )
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

