#pragma once
/*
 * CSpiMaster.h
 *
 *  Created on: December 12, 2022
 *      Author: C Arena
 */

#include "main.h"
#include <cstdint>


class CSpiMaster
{
	SPI_HandleTypeDef * mpSpiDev;

	static CSpiMaster * gpSpiMaster;

public:

	CSpiMaster( SPI_HandleTypeDef * pSpi );

	virtual ~CSpiMaster();

	bool init(void);

	bool sendRcv( uint8_t * pBufOut, uint8_t * pBufIn, uint16_t size, int & sndRcvStatus);

	bool transferComplete(void);

	bool dmaComplete(void);

	static void transferComplete( SPI_HandleTypeDef * );

	static void dmaComplete     ( DMA_HandleTypeDef * );


};

