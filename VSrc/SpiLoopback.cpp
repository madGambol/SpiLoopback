/*
 * SpiLoopback.cpp
 *
 *  Created on: December 12, 2022
 *      Author: C Arena
 */

#define UJSE_HAL_UART_REGISTER_CALLBACKS 1

#include "main.h"
#include "SpiLoopback.h"
#include "CSerialPrint.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;

CSerialPrint *pSerial1  = nullptr;
CSerialPrint *pSerial2  = nullptr;


void Main(void)
{
	pSerial1        = CSerialPrint::getInstance ( &huart1 );

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

		pSerial1->putString  ( "\f\r\n\r\n\r\nStarting up MainCppLoop" );
		pSerial1->putString  ( "SPI Loopback test" );
		pSerial1->putString  ( "\r\n" );

	} while(0);

	while (1)
	{
		// main loop
	}
}
