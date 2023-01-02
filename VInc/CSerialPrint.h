#pragma once

/*
 * CSerialPrint.h
 *
 *  Created on: Mar 14, 2021
 *      Author: chris
 */

#include <cstdint>
#include "main.h"

enum { eSERBUFSIZE = 4096 };

class CSerialPrint
{
   UART_HandleTypeDef * mpUart;

   bool mbDmaBusy;

   enum { NUM_SERIAL_DEVICES = 3 };

   static CSerialPrint * gpInstance[ NUM_SERIAL_DEVICES ];
   static volatile uint8_t gInstanceIndx;

   uint8_t  mBuf   [eSERBUFSIZE];   // buffer assembling data to be sent
   uint8_t  mBufOut[eSERBUFSIZE];   // buffer being used for DMA

   volatile uint16_t mInIndx;       // index to the chars going into mBuf
   volatile uint16_t mOutIndx;      // index to the chars coming out of mBuf
   volatile uint16_t mCount;        // pending characters to go out

//   static volatile uint32_t mHalfCompleteCounter;
   static volatile uint32_t mCompleteCounter;

   void sendData( const uint8_t * pBuf );

   void transmitComplete();

   void primePump(void);

   // this method assumes that the CHoldInterrupts is in effect....
   bool putStringNoEOL_( const char * pStr, uint16_t strLen = 0);

public:

   CSerialPrint( UART_HandleTypeDef * pUart );

   virtual ~CSerialPrint();

   bool init(void);

   bool putChar( char ch );

   bool putString( const char * pStr );

   bool putStringNoEOL( const char * pStr, uint16_t strLen = 0 );

   bool putCrLf(void);

   static CSerialPrint * getInstance( UART_HandleTypeDef * pUart );

   static CSerialPrint & factory(void);

   static void transmitComplete    ( UART_HandleTypeDef * pUart );

   static void dmaOutputComplete   ( DMA_HandleTypeDef  * hdma  );
   static void DmaInputComplete    ( DMA_HandleTypeDef  * hdma  );
};

void _putchar_( char ch );

bool putstringnlf( const char * pStr );

bool putstring   ( const char * pStr );

bool putCrLf(void);

void print_pressure(int value);

void print_pressure(int value);

void print_comma(void);

void print_comma_blank(void);

void printFileLine( const char * pFn, unsigned int line);

void printFileHexLine( const char * pFn, unsigned int line);

void print_valuenlf(int value);

void print_FloatValuenlf(float value);

void print_unsigned_value(unsigned int value);

void print_hex_unsigned_valuenlf(unsigned int value);
