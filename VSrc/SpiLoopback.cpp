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
#include "CSpiSlave.h"
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

void myReceiveCompleteCB( SPI_HandleTypeDef * pSpiDev,
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
CSpiSlave  gSpiSlave  ( &hspi3 );

CFormattedBuffer buffer;
CFormattedBuffer slaveBuffer; // used by the slave

uint8_t spiMasterBufIn [128] = {0,};
uint8_t spiMasterBufOut[128] = {0,};
uint8_t oldMasterBufOut[128] = {0,}; // this buffer saves the prior output for comparison

/////////////// Separate buffers for the Slave SPI device

uint8_t spiSlaveBufIn  [128] = {0,};
uint8_t spiSlaveBufOut [128] = {0,};

volatile bool gbTransmitComplete;
volatile bool gbReceiveComplete;

int sndRcvStatus;
int rcvSndStatus;

extern volatile bool     bDelay;
extern volatile uint32_t delay;

extern volatile uint32_t spiDelay;
extern volatile bool     bSpiDelay;

void sendITM( const char * pStr )
{
	do
	{
		if (!pStr) break;

		size_t len = strlen(pStr);

		size_t indx;

		for (indx = 0; indx < len; ++indx )
		{
			ITM_SendChar( (uint32_t)pStr[indx] ); // synchronous
		}
	} while(0);
}

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

bool bRcvSnd;
bool bSndRcv;

void MainMasterSlave(void)
{
	pSerial1 = CSerialPrint::getInstance ( &huart1 );

	if (!pSerial1)
	{
		while (1) {}
	}

	if (!pSerial1->init())
	{
		while (1) {}
	}

	buffer.addStr( "\f\r\n\r\n\r\nStarting up MainMasterSlave\r\n" );
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

	buffer.print();

	gSpiMaster.init();
	gSpiSlave. init();

	gSpiMaster.setCallback( &myTransmitCompleteCB );
	gSpiSlave.setCallback ( &myReceiveCompleteCB  );

	buffer.addStr( "gSpiMaster & gSpiSlave CB set up ");
	buffer.print();

	setUpBuf( spiMasterBufOut, 128 );                // pattern to send

	memset  ( spiMasterBufIn,  255, sizeof(spiMasterBufIn) ); // fill with 'ff' to see change

	uint32_t loopCount = 0;

	bDelay = false;
	delay  = 1000;     // one second

	memset( spiMasterBufOut,  0, sizeof(spiMasterBufOut) ); // fill

	while (1)
	{
		while (!bDelay) { /* wait a second here */ }

		memcpy( oldMasterBufOut, spiMasterBufOut, sizeof(oldMasterBufOut) ); // save old data
		memcpy( spiSlaveBufOut,  spiSlaveBufIn,   sizeof(spiSlaveBufOut)  ); // save old data

		memset( spiMasterBufOut,  0, sizeof(spiMasterBufOut) ); // fill
		memset( spiMasterBufIn,   0, sizeof(spiMasterBufIn)  ); // fill

		snprintf( (char *)spiMasterBufOut, sizeof(spiMasterBufOut), "Loop Count = %ld\n", loopCount++);

		buffer.addStr( (const char *)spiMasterBufOut );
		buffer.print();

		bDelay             = false;
		delay              = 1000;   // msec

		bSpiDelay          = false;
		spiDelay           = 100;    // timeout for spi transfer to complete

		gbTransmitComplete = false;

		gbReceiveComplete  = false;

		bRcvSnd = gSpiSlave. sendRcv( spiSlaveBufOut,  spiSlaveBufIn,  128, rcvSndStatus );

		bSndRcv = gSpiMaster.sendRcv( spiMasterBufOut, spiMasterBufIn, 128, sndRcvStatus );

		if (!bSndRcv)
		{
			buffer.addUInt( loopCount,"Master sendRcv call # %ld failed\r\n");
			buffer.print();
		}

		if (!bRcvSnd)
		{
			buffer.addUInt( loopCount,"Slave sendRcv call # %ld failed\r\n");
			buffer.print();
		}

		while (!bSpiDelay && !gbTransmitComplete && !gbReceiveComplete) {}

		if (gbTransmitComplete)
		{
			buffer.addUInt(loopCount, "Transmit %ld complete : " );

			if (memcmp( spiMasterBufIn, oldMasterBufOut, 128 ) == 0)
			{
				buffer.addStr( "Input & output match!", "%s\n\r" );
			}
			else
			{
				buffer.addStr( "Input & output DO NOT match!", "%s\n\r" );
			}

			buffer.print();
		}
		else if (spiDelay)
		{
			buffer.addStr("transmit timed out", "%s\n\r");
			buffer.print();
		}

		if (gbReceiveComplete)
		{
			buffer.addUInt(loopCount, "Receive %ld complete : " );

			if (memcmp( spiMasterBufOut, spiSlaveBufIn, 128 ) == 0)
			{
				buffer.addStr( "Master output & Slave input match!", "%s\n\r" );
			}
			else
			{
				buffer.addStr( "Master output & Slave input DO NOT match!", "%s\n\r" );
			}

			buffer.print();
		}

		gbTransmitComplete = false;
		gbReceiveComplete  = false;
	}
}

void myTransmitCompleteCB( SPI_HandleTypeDef * pSpiDev, const uint8_t * pBufIn, const uint8_t * pBufOut, uint16_t size)
{
	gbTransmitComplete = true;
}

void myReceiveCompleteCB( SPI_HandleTypeDef * pSpiDev, const uint8_t * pBufIn, const uint8_t * pBufOut, uint16_t size)
{
	gbReceiveComplete  = true;
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	CSpiMaster::transferComplete( hspi );
}
