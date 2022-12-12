/*
 * SpiLoopback.cpp
 *
 *  Created on: December 12, 2022
 *      Author: C Arena
 */

#define UJSE_HAL_UART_REGISTER_CALLBACKS 1

#include "main.h"
#include "SpiLoopback.h"

extern UART_HandleTypeDef huart1;

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

CSerialPrint *pSerial  = nullptr;



void Main(void)
{

	while (1)
	{
		// main loop
	}
}
