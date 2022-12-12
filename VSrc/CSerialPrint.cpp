/*
 * CSerialPrint.cpp
 *
 *  Created on: Mar 14, 2021
 *      Author: chris
 */
#include <string.h>
#include <stdio.h>

#include "CSerialPrint.h"
#include "CHoldInterrupts.h"
#include "stm32f3xx_hal_uart.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;

volatile uint8_t        CSerialPrint::gInstanceIndx = 0;
CSerialPrint * CSerialPrint::gpInstance[] = {nullptr,};

//volatile uint32_t  CSerialPrint::mHalfCompleteCounter { 0 };
volatile uint32_t  CSerialPrint::mCompleteCounter     { 0 };

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
   CSerialPrint::transmitComplete( huart );
}

//void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
//{
//   CSerialPrint::transmitHalfComplete( huart );
//}

CSerialPrint::CSerialPrint( UART_HandleTypeDef * pUart )
: mpUart     ( pUart ),
  mbDmaBusy  ( false ),
  mInIndx    ( 0     ),
  mOutIndx   ( 0     ),
  mCount     ( 0     )
{
   if (gInstanceIndx < NUM_SERIAL_DEVICES)
   {
      gpInstance[gInstanceIndx] = this;
      ++gInstanceIndx;
   }

   memset( mBuf,    0, sizeof(mBuf)    );
   memset( mBufOut, 0, sizeof(mBufOut) );
}

CSerialPrint::~CSerialPrint()
{
}

bool CSerialPrint::init(void)
{
	bool bRetVal = false;

	HAL_StatusTypeDef status;

	if (mpUart->hdmatx)
	{
	   status = HAL_DMA_RegisterCallback( mpUart->hdmatx,
										  HAL_DMA_XFER_CPLT_CB_ID,
										  CSerialPrint::dmaOutputComplete
										);
	}
	else
	{
	  status = HAL_UART_RegisterCallback( mpUart,
										  HAL_UART_TX_COMPLETE_CB_ID,
										  CSerialPrint::transmitComplete
										);
	}

	if (status == HAL_OK)
	{
		bRetVal = true;
	}

	return bRetVal;
}

void CSerialPrint::sendData( const uint8_t * pBuf )
{
}

bool CSerialPrint::putChar( char ch )
{
   bool bRetVal = false;

   static uint8_t entryCount = 0;

   do
   {
      ++entryCount;
      if (entryCount > 1)
      {
         while (1) {}
      }

      if (entryCount)
      {
         --entryCount;
      }
      
      CHoldInterrupts now;

      if (mCount >= sizeof(mBuf)) break; // full, can't take more data

      ++mCount;

      mBuf[mInIndx] = ch;

      mInIndx = (mInIndx + 1) % sizeof(mBuf);

      bRetVal = true;

   } while(0);

   return bRetVal;
}

bool CSerialPrint::putString( const char *pStr )
{
   bool bRetVal = false;

   do
   {
      CHoldInterrupts now;

      if (!putStringNoEOL_(pStr)) break;

      if (!putStringNoEOL_("\r\n", 2)) break;

      if (mbDmaBusy) break;

      primePump();

      bRetVal = true;

   } while (0);

   return bRetVal;
}

bool CSerialPrint::putStringNoEOL_( const char * pStr, uint16_t strLen )
{
   bool bRetVal = false;

   do
   {
      if (!pStr) break;     // can't use this string

      CHoldInterrupts now;

      if (mCount >= sizeof(mBuf)) break; // full, can't take more data

      uint16_t indx;

      if (strLen == 0)
      {
          strLen = strlen( pStr );
      }

      for (indx = 0; indx < strLen; ++indx)
      {
         if (mCount >= sizeof(mBuf)) break;

         ++mCount;

         mBuf[mInIndx] = pStr[indx];

         mInIndx = (mInIndx + 1) % sizeof(mBuf);
      }

      bRetVal = indx == strLen;

   } while(0);

   return bRetVal;
}

bool CSerialPrint::putStringNoEOL( const char * pStr, uint16_t strLen )
{
   bool bRetVal = false;

   do
   {
      if (!pStr) break;     // can't use this string

      if (mCount >= sizeof(mBuf)) break; // full, can't take more data

      if (strLen == 0)
      {
          strLen = strlen( pStr );
      }

      bRetVal = putStringNoEOL_( pStr, strLen );

   } while(0);

   return bRetVal;
}

bool CSerialPrint::putCrLf( void )
{
   bool bRetVal = false;

   do
   {
      if (!putStringNoEOL("\r\n", 2)) break;

      if (mbDmaBusy) break;

      primePump();

      bRetVal = true;

   } while (0);

   return bRetVal;
}

CSerialPrint* CSerialPrint::getInstance( UART_HandleTypeDef * pUart )
{
   CSerialPrint* pRetVal = nullptr;

   if (pUart == &huart1)
   {
      static CSerialPrint serial1( pUart );

      pRetVal = &serial1;
   }
   else if (pUart == &huart2)
   {
      static CSerialPrint serial2( pUart );

      pRetVal = &serial2;
   }

   return pRetVal;
}

CSerialPrint & CSerialPrint::factory( void )
{
   while (!gpInstance[0]) {}

   return *gpInstance[0];
}

void CSerialPrint::transmitComplete( UART_HandleTypeDef * pUart )
{
   if (!pUart) return; // can't do anything with nullptr

   uint8_t indx;

   for (indx = 0; indx < gInstanceIndx; ++indx)
   {
      if (gpInstance[indx]->mpUart == pUart)
      {
         gpInstance[indx]->transmitComplete();
         break;
      }
   }
}

void CSerialPrint::dmaOutputComplete( DMA_HandleTypeDef * hdma )
{
   if (!hdma) return; // can't do anything with nullptr

   uint8_t indx;

   for (indx = 0; indx < gInstanceIndx; ++indx)
   {
      if (gpInstance[indx] && (gpInstance[indx]->mpUart) && (gpInstance[indx]->mpUart->hdmatx == hdma))
      {
         gpInstance[indx]->transmitComplete();
         break;
      }
   }
}

uint32_t myMin( int a, int b)
{
    return (uint32_t)((a)<(b)?a:b);
}

void CSerialPrint::transmitComplete()
{
   do
   {
      CHoldInterrupts now;

      ++mCompleteCounter;

      // indicate DMA not busy until we know otherwise
      mbDmaBusy = false;

      // check if there's more to send
      if (!mCount    ) break; // 0 --> no data pending, no DMA to set up

      // copy mCount chars from mOutIndx
      uint16_t indx;

      for (indx = 0; indx < mCount; ++indx)
      {
         uint16_t srcIndx = mOutIndx+indx;

         if (srcIndx >= sizeof(mBuf))
         {
            srcIndx -= sizeof(mBuf);
         }

         mBufOut[indx] = mBuf[srcIndx];
      }

      mbDmaBusy = true;   // do it now so that there is no race

      // send the data via DMA call to HAL
      HAL_StatusTypeDef status =  HAL_ERROR;
      
      if (mpUart->hdmatx) 
      { 
         status = HAL_UART_Transmit_DMA( mpUart, mBufOut, mCount );
      }
      else
      {
         status = HAL_UART_Transmit_IT(mpUart, mBufOut, mCount );
      }

      if (status == HAL_OK)
      {
         mOutIndx  = mInIndx;  // if equal, no data pending
         mCount    = 0;
      }
      else
      {
         mbDmaBusy = false;    // set false if the transmit fails
      }

   } while(0);
}

void CSerialPrint::primePump(void)
{
    do
    {
        if (mbDmaBusy) break;  // DMA is underway, the ISR will do the rest

        if (!mCount  ) break;  // sanity check: 0 to send, we're done

        CHoldInterrupts now;

        uint16_t indx;

        for ( indx = 0; indx < mCount; ++indx )
        {
           uint16_t srcIndx = mOutIndx+indx;

           if (srcIndx >= sizeof(mBuf))
           {
              srcIndx -= sizeof(mBuf);
           }

           mBufOut[indx] = mBuf[ srcIndx ];
        }

        mbDmaBusy = true;   // do it now so that there is no race between the
        //                  // HAL_UART_Transmit_DMA call and this variable

        // send the data via DMA call to HAL
        HAL_StatusTypeDef status =  HAL_ERROR;
        
        if (mpUart->hdmatx) 
        { 
           status = HAL_UART_Transmit_DMA( mpUart, mBufOut, mCount );
        }
        else
        {
           status = HAL_UART_Transmit_IT(mpUart, mBufOut, mCount );
        }

        if (status == HAL_OK)
        {
           mOutIndx  = mInIndx;  // if equal, no data pending
           mCount    = 0;
        }
        else
        {
           mbDmaBusy = false;    // set false if the transmit fails
        }

    } while(0);
}

//void CSerialPrint::transmitHalfComplete( UART_HandleTypeDef * pUart )
//{
//    ++mHalfCompleteCounter;
//}

#define DECR_VAL    1

#define DECREMENT(x) { x = ((x)>DECR_VAL)?((x)-DECR_VAL):0;}

#define SetWI(x,y)  { x = (y); }

#define DIGITS      7
#define HEX_DIGITS  4
#define RAMP_DIGITS 7

uint8_t digit[ DIGITS*2 ];

void _putchar_( char ch )
{
   static CSerialPrint & rSerial = CSerialPrint::factory();

   rSerial.putChar( ch );
}

bool putstringnlf( const char * pStr )
{
   static CSerialPrint & rSerial = CSerialPrint::factory();

   return rSerial.putStringNoEOL(pStr);
}

bool putstring( const char * pStr )
{
   static uint8_t entryCount = 0;

   static CSerialPrint & rSerial = CSerialPrint::factory();

   ++entryCount;

   if (entryCount > 1)
   {
      rSerial.putChar('?');
   }

   bool bRetValue = rSerial.putString( pStr );

   --entryCount;

   return bRetValue;
}

bool putCrLf(void)
{
   static CSerialPrint & rSerial = CSerialPrint::factory();

   return rSerial.putCrLf();
}

//
// AWP_128 scale
//
void print_pressure(int value)
{
    long int lvalue = value;
    unsigned char i;

    lvalue = ((lvalue * 25)+31) >> 5;

    if (lvalue < 0)
    {
        _putchar_('-');

        lvalue = -lvalue;
    }
    else
    {
      _putchar_(' ');
    }

    for (i=0; i < DIGITS; i++)
    {
        digit[i] = lvalue % 10 + '0';
        lvalue   = lvalue / 10;
    }

    for (i=0; i < DIGITS; i++)
    {
        if (i == (DIGITS-2))
        {
            _putchar_('.');
        }
        putchar(digit[DIGITS-i-1]);
    }
}

void print_comma(void)
{
    _putchar_(',');
}

void print_comma_blank(void)
{
   putstringnlf(", ");
}

void print_valuenlf(int value)
{
   char buf[16] = {0,};

   snprintf(buf, sizeof(buf), "%d", value);

   putstringnlf( buf );
}

void print_FloatValuenlf(float value)
{
   char buf[16] = {0,};

   snprintf(buf, sizeof(buf), "%2.2f", value);

   putstringnlf( buf );
}


void print_unsigned_value(unsigned int value)
{
    unsigned int lvalue;
    unsigned char i;

    lvalue = value;

    for (i=0; i < DIGITS; ++i )
    {
        digit[i] = lvalue % 10 + '0';
        lvalue   = lvalue / 10;
    }

    for (i=0; i < DIGITS; ++i )
    {
        putchar(digit[DIGITS-i-1]);
    }
}

void printFileLine( const char * pFn, unsigned int line)
{
    putstringnlf(pFn);
    putstringnlf(" : ");
    print_valuenlf( line );
    putCrLf();
}

void print_hex_unsigned_valuenlf(unsigned int value)
{
    unsigned int lvalue;
    signed char indx;
    signed char tmp;

    lvalue = value;

    for (indx=0; indx < HEX_DIGITS; ++indx )
    {
        tmp         = lvalue & 0x0f;

        digit[indx] = (tmp > 9)?(tmp-10+'A'):(tmp+'0');

        lvalue = lvalue >> 4;
    }

    putstringnlf("0x");
    putchar(digit[3]);
    putchar(digit[2]);
    putchar(digit[1]);
    putchar(digit[0]);
}

void printFileHexLine( const char * pFn, unsigned int line)
{
    putstringnlf(pFn);
    putstringnlf(" : ");
    print_hex_unsigned_valuenlf( line );
    putCrLf();
}

