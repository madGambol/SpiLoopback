/*
 * SpiLoopback.cpp
 *
 *  Created on: December 12, 2022
 *      Author: C Arena
 */

#include "main.h"
#include "SpiLoopback.h"
#include "CSerialPrint.h"
#include "CSpiMaster.h"
#include "CFormattedBuffer.h"
#include "mySysTick.h"
#include <stdio.h>
#include <string.h>

CSerialPrint *pSerial1  = nullptr;
CSerialPrint *pSerial2  = nullptr;

void myTransmitCompleteCB( SPI_HandleTypeDef * pSpiDev, const uint8_t * pBufIn, const uint8_t * pBufOut, uint16_t size);

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;

extern SPI_HandleTypeDef hspi1;    // master
extern SPI_HandleTypeDef hspi3;    // slave

CSpiMaster gSpiMaster ( &hspi1 );

CFormattedBuffer buffer;

uint8_t bufIn[128] = {0,};
uint8_t bufOut[128] = {0,};

bool gbTransmitComplete;

int sndRcvStatus;

extern bool     bDelay;
extern uint32_t delay;

void setUpBuf( uint8_t * pBuf, uint16_t size )
{
	do
	{
		if (!pBuf) break;
		if (!size) break;

		uint16_t indx;

		for (indx = 0; indx < size; ++indx)
		{
			pBuf[indx] = ((uint8_t)indx) & 0x0ff; // just put the index there
		}

	} while(0);

}

bool gbBufferMatch = false;

void Main(void)
{
	pSerial1 = CSerialPrint::getInstance ( &huart1 );

	do
	{
		if (!pSerial1)
		{
			while (1) {}
		}

		if (!pSerial1->init())
		{
			while (1) {}
		}

		buffer.addStr( "\f\r\n\r\n\r\nStarting up MainCppLoop\r\n" );
		buffer.addStr( "SPI Loop back test\r\n" );
		buffer.addStr(  "\r\n");
		buffer.print();

		gSpiMaster.init();

		gSpiMaster.setCallback( &myTransmitCompleteCB );

		buffer.addStr( "gSpiMaster CB set up ");
		buffer.print();

		setUpBuf( bufOut, 128 );                // pattern to send
		memset  ( bufIn,  255, sizeof(bufIn) ); // fill with 'ff' to see change

		gbTransmitComplete = false;

		sndRcvStatus = HAL_ERROR;

		delay  = 1000; // one second
		bDelay = false;

		while (!bDelay) { /* wait a second here */ }

		delay  = 1000; // timeout for the transfer
		bDelay = false;

		bool bSndRcv = gSpiMaster.sendRcv(bufOut, bufIn, 128, sndRcvStatus);

		if (bSndRcv)
		{
			while (!bDelay && !gbTransmitComplete)
			{
				// spin our wheels
			}

			if (gbTransmitComplete)
			{
				buffer.addStr("Transmit complete.", "%s\n\r" );

				if (memcmp(bufIn, bufOut, 128) == 0)
				{
					buffer.addStr("Input & output match!", "%s\n\r" );

					gbBufferMatch = true;
				}
			}
			else if (bDelay)
			{
				buffer.addStr("timed out", "%s\n\r");
			}
			buffer.print();
		}

	} while(0);

	while (1)
	{
		// main loop
	}
}


void myTransmitCompleteCB( SPI_HandleTypeDef * pSpiDev, const uint8_t * pBufIn, const uint8_t * pBufOut, uint16_t size)
{
	gbTransmitComplete = true;
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	CSpiMaster::transferComplete( hspi );
}

