/*
 * SpiLoopback.cpp
 *
 *  Created on: December 12, 2022
 *      Author: C Arena
 *
 *      Connections for NUCLEO-F303RE
 *
 *      UART1:
 *              TX   PA2  CN10-35  CN9-2
 *              RX   PA3  CN10-37  CN9-1
 *
 *      UART2:
 *              TX   PB10 CN10-25  CN9-7
 *              RX   PB11 CN10-18  -na-
 *
 *      SPI1:
 *              SCLK PB3  CN10-31  CN9-4
 *              MISO PB4  CN10-27  CN9-6
 *              MOSI PB5  CN10-29  CN9-5
 *              NSS  PA15 CN07-32  CN8-3
 *
 *      SPI3:
 *              SCLK PC10 CN07-1   -na-
 *              MISO PC11 CN07-2   -na-
 *              MOSI PC12 CN07-3   -na-
 *              NSS  PA15 CN07-17  -na-
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

void myTransmitCompleteCB( SPI_HandleTypeDef * pSpiDev,
		                   const uint8_t * pBufIn,
						   const uint8_t * pBufOut,
						   uint16_t size
						 );

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

extern DMA_HandleTypeDef  hdma_usart1_rx;
extern DMA_HandleTypeDef  hdma_usart1_tx;
extern DMA_HandleTypeDef  hdma_usart2_rx;
extern DMA_HandleTypeDef  hdma_usart2_tx;

extern SPI_HandleTypeDef  hspi1;    // master
extern SPI_HandleTypeDef  hspi3;    // slave

CSpiMaster gSpiMaster ( &hspi1 );

CFormattedBuffer buffer;

uint8_t spiBufIn [128] = {0,};
uint8_t spiBufOut[128] = {0,};

bool gbTransmitComplete;

int sndRcvStatus;

extern volatile bool     bDelay;
extern volatile uint32_t delay;

void sendITM( const char * pStr )
{
	do
	{
		if (!pStr) break;

		size_t len = strlen(pStr);

		size_t indx;

		for (indx = 0; indx < len; ++indx )
		{
			ITM_SendChar( (uint32_t)pStr[indx] );
		}
	} while(0);
}

//bool sendRcvSpiData(const uint8_t * pOut, uint8_t * pIn, size_t size )
//{
//	bool bRetVal = false;
//
//	int status   = 0;
//
//	bRetVal = gSpiMaster.sendRcv( pOut, pIn, size, status );
//
//	return bRetVal;
//}

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

void MainCpp(void)
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

		sendITM("this is a test");

		uint32_t count = 0;

		for (int loop = 0; loop < 32; ++loop)
		{
			buffer.addInt(loop, "%04d : ");

			for (int indx = 0; indx < 32; ++indx)
			{
				buffer.addUInt( count++, "%4d, ");
			}

			buffer.print();
		}

		gSpiMaster.init();

		gSpiMaster.setCallback( &myTransmitCompleteCB );

		buffer.addStr( "gSpiMaster CB set up ");
		buffer.print();

		setUpBuf( spiBufOut, 128 );                // pattern to send

		memset  ( spiBufIn,  255, sizeof(spiBufIn) ); // fill with 'ff' to see change

		gbTransmitComplete = false;
		sndRcvStatus       = HAL_ERROR;

		bDelay = false;
		delay  = 1000; // one second

		while (!bDelay) { /* wait a second here */ }

		delay  = 1000; // timeout for the transfer
		bDelay = false;

		bool bSndRcv = gSpiMaster.sendRcv( spiBufOut, spiBufIn, 128, sndRcvStatus );

		if (!bSndRcv)
		{
			buffer.addStr( "transfer failed\r\n");
			buffer.print();
			break;
		}

		while (!bDelay && !gbTransmitComplete)
		{
			// spin our wheels
		}

		if (gbTransmitComplete)
		{
			gbTransmitComplete = false;

			buffer.addStr("Transmit complete.", "%s\n\r" );

			if (memcmp(spiBufIn, spiBufOut, 128) == 0)
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

	} while(0);

	uint32_t loopCount = 0;

	bDelay = true;
	delay  = 0;     // will use one second

	while (1)
	{
		while (!bDelay) { /* wait a second here */ }

		memset  ( spiBufOut,  0, sizeof(spiBufOut) ); // fill
		memset  ( spiBufIn,   0, sizeof(spiBufIn)  ); // fill

		snprintf( (char *)spiBufOut, sizeof(spiBufOut), "Loop Count = %ld\n", loopCount++);

		buffer.addStr( (const char *)spiBufOut );
		buffer.print();

		bDelay = false;
		delay  = 1000;

		gSpiMaster.sendRcv( spiBufOut, spiBufIn, 128, sndRcvStatus );

		while (!bDelay && !gbTransmitComplete) {}

		if (gbTransmitComplete)
		{
			buffer.addStr("Transmit complete.", "%s\n\r" );
			buffer.print();

			if (memcmp(spiBufIn, spiBufOut, 128) == 0)
			{
				buffer.addStr( "Input & output match!", "%s\n\r" );
				buffer.print();
			}
		}
		else if (bDelay)
		{
			buffer.addStr("timed out", "%s\n\r");
			buffer.print();
		}

		gbTransmitComplete = false;
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
