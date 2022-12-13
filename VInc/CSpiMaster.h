#pragma once
/*
 * CSpiMaster.h
 *
 *  Created on: December 12, 2022
 *      Author: C Arena
 */

extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_tx;

class CSpiMaster
{
public:
	CSpiMaster();
	virtual ~CSpiMaster();


};

