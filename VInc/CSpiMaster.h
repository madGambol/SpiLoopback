#pragma once
/*
 * CSpiMaster.h
 *
 *  Created on: December 12, 2022
 *      Author: C Arena
 */

#include "main.h"
#include <cstdint>

typedef void (transmitCompleteCB)(SPI_HandleTypeDef *, const uint8_t * pBufIn, const uint8_t * pBufOut, uint16_t size);

class CSpiMaster
{
	SPI_HandleTypeDef * mpSpiDev;

	static CSpiMaster * gpSpiMaster;

	transmitCompleteCB * mpCB;

	uint8_t * mpBufOut;
	uint8_t * mpBufIn;
	uint16_t  mSndRcvSize;

public:

	CSpiMaster( SPI_HandleTypeDef * pSpi );

	virtual ~CSpiMaster();

	bool init(void);

	/**
	 * \brief sendRcv expects the input and output buffers to persist: they should not be
	 *        allocated on the stack or reused for something else. Otherwise the results
	 *        are not defined.
	 * \param size is the size in bytes.
	 * \param sndRcvStatus is a status result set at the end of the send/receive setup.
	 * \return true if the operation returns HAL_OK, false otherwise
	 */
	bool sendRcv( uint8_t * pBufOut, uint8_t * pBufIn, uint16_t size, int & sndRcvStatus);

	bool transferComplete(void);

	bool dmaComplete(void);

	bool setCallback( transmitCompleteCB * pCB );

	static void transferComplete( SPI_HandleTypeDef * );

	static void dmaComplete     ( DMA_HandleTypeDef * );


};

