/* 
 * File:   CSpiMaster.cpp
 * Author: C. Arena at Hallowell EMC
 * 
 * Created on April 24, 2016, 11:53 PM
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <byteswap.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/time.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "CSpiMaster.h"

//#include <linux/types.h>
//#include <linux/spi/spidev.h>
#include "crcpy.h"

#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std; 

static void pabort(const char *s)
{
    perror(s);
    abort();
}

//<editor-fold defaultstate="collapsed" desc="// byte ordering functions">
typedef struct _swap2Bytes
{
    union
    {
        uint16_t word;
        uint8_t bytes[2];
    };
} swap2Bytes;

typedef struct _swap4Bytes
{
    union
    {
        uint32_t word;
        uint16_t words[2];
        uint8_t bytes[4];
    };
} swap4Bytes;

uint16_t toVOrder(uint16_t val)
{
    swap2Bytes tmp;

    tmp.word = val;

    swap(tmp.bytes[0], tmp.bytes[1]);

    return tmp.word;
}

uint32_t toVOrder(uint32_t val)
{
    swap4Bytes tmp;

    tmp.word = val;

    swap(tmp.bytes[0], tmp.bytes[3]);
    swap(tmp.bytes[1], tmp.bytes[2]);

    return tmp.word;
}
//</editor-fold>

CSpiMaster::CSpiMaster(uint8_t mode, uint8_t bits, uint32_t speed, uint16_t delay, const char * pDevName)
  :
  CSpiBase( eMASTER, eFULL_DUPLEX, eSPI1, eNO_PIN,  eNO_PIN, eNO_PIN, eNO_PIN, eNO_PIN ),
  mMode    ( mode     ),
  mBits    ( bits     ),
  mSpeed   ( speed    ),
  mDelay   ( delay    ),
  mDevName ( pDevName ),
  mSpiDevFd( open(mDevName.c_str(), O_RDWR) )
{
  do
  {
    mXmtEnvelope.clear();
    mRcvEnvelope.clear();

    if (mSpiDevFd < 0) break;

    int retVal = ioctl(mSpiDevFd, SPI_IOC_WR_MODE, &mMode);

    if (retVal == -1)
    {
      pabort("can't set spi mode");
      printf("mode = 0x%02x\n\r", int(mMode));
      break;
    }
    /*
     * bits per word
     */
    //        retVal = ioctl(mSpiDevFd, SPI_IOC_WR_BITS_PER_WORD, &mBits);
    //
    //        if (retVal == -1)
    //        {
    //            stringstream str;
    //            str << "can't set bits per word : " << int(mBits);
    //            pabort( str.str().c_str() );
    //            break;
    //        }

    retVal = ioctl(mSpiDevFd, SPI_IOC_RD_BITS_PER_WORD, &mBits);

    if (retVal == -1)
    {
      pabort("can't get bits per word");
      break;
    }
        
    cout << "Got " << int(mBits) << " bits per word." << endl;

    /*
     * max speed Hz
     */
    retVal = ioctl(mSpiDevFd, SPI_IOC_WR_MAX_SPEED_HZ, &mSpeed);

    printf("retVal for SPI_IOC_WR_MAX_SPEED_HZ is %d\n", retVal);

    if (retVal == -1)
    {
      pabort("can't set max speed Hz");
      break;
    }

    retVal = ioctl(mSpiDevFd, SPI_IOC_RD_MAX_SPEED_HZ, &mSpeed);

    printf("retVal for SPI_IOC_RD_MAX_SPEED_HZ is %d\n", retVal);

    if (retVal == -1)
    {
      pabort("can't get max speed Hz");
      break;
    }

    gpSpiBase[ mSpiDeviceEnum ] = this;

  } while (0);
}

CSpiMaster::~CSpiMaster()
{
}

// void swapBytes( CSpiMessage::CEnvelope & env )
// {
//   uint16_t * pU16 = (uint16_t *)&env;
 
//   //size_t size = sizeof(CSpiMessage::CEnvelope)/sizeof(uint16_t);

//   uint16_t i;

//   for (i = 0; i < eMAX_MSG_SIZE_WORDS ; i++)
//   {
//     pU16[i] = bswap_16( pU16[i] );
//   }
  
//   struct TwoWords
//   {
//       union
//       {
//           uint32_t u32;
//           uint16_t u16[2];
//       };
//   };
  
//   TwoWords _2Words;
  
//   _2Words.u32 = bswap_32( env.mEnv.msgChecksum );
  
//   uint16_t tmp    = _2Words.u16[0];
  
//   _2Words.u16[0]  = _2Words.u16[1];
//   _2Words.u16[1]  = tmp;

//   env.msgChecksum = _2Words.u32;
// }

bool CSpiMaster::deviceOpenedOK(void) const
{
    return mSpiDevFd >= 0;
}

CSpiMessage::CEnvelope & CSpiMaster::getOutgoingEnvelope(void)
{
    return mXmtEnvelope;
}

CSpiMessage::CEnvelope & CSpiMaster::getIncomingEnvelope(void)
{
    return mRcvEnvelope;
}

bool CSpiMaster::initiateTransfer(void)
{
    return false;
}

void CSpiMaster::interruptHandler(void)
{
    
}

int CSpiMaster::transfer(void)
{
    struct spi_ioc_transfer iocTransfer;

    //cout << "Using " << int(mBits) << " bits per word" << endl;

    memset(&iocTransfer, 0, sizeof (iocTransfer)); // clear the struct
    
    //cout << "Transmit ";
    //dumpEnvelope( mTxEnvelope );
        
    //cout << "mTxEnvelope: ";
    //dumpEnvelope( mTxEnvelope );

    //cout << "mTxEnvelope(after swap): ";
    //dumpEnvelope( mTxEnvelope );

    iocTransfer.tx_buf        = (unsigned long) mXmtEnvelope.getBuffer();
    iocTransfer.rx_buf        = (unsigned long) mRcvEnvelope.getBuffer();
    iocTransfer.len           = eMSG_BUFFER_SIZE_WORDS*(16/mBits);
    iocTransfer.delay_usecs   = mDelay;
    //tr.interbyte_usecs      = 2;
    iocTransfer.speed_hz      = mSpeed;
    iocTransfer.bits_per_word = mBits;
    iocTransfer.cs_change     = 1;

    int ret = ioctl(mSpiDevFd, SPI_IOC_MESSAGE(1), &iocTransfer); // actually perform the transfer

    //uint16_t bufSizeInBytes = CSpiMessage::CEnvelope::getBufferSizeBytes();

    //mRcvEnvelope.copySwapBytes( (const uint8_t *)mRcvEnvelope.getBuffer(), bufSizeInBytes );
    //mRcvEnvelope.copySwapCrc  ( mRcvEnvelope.getCrc() );

//    if ( mRcvEnvelope.checkCrc() )
//    {
//      cout << "Master Receive CRC OK:\n";
//    }
//    else
//    {
//      cout << "Master Receive CRC BAD:\n";
//    }

    //dumpEnvelope( mRcvEnvelope );
        
//    if (ret < 1)
//    {
//        //pabort("can't send spi message");
//    }
    
    //cout << "CSpiMaster::transfer called" << endl;
    
    return ret;
}

void CSpiMaster::printSetup(void) const
{
}

CSpiBase * CSpiMaster::getSpi(CSpiBase::SpiDeviceType spiDev)
{
  if ((eSPI1 <= spiDev) && (spiDev < eSPI_LAST) )
  {
    return gpSpiBase[ spiDev ];
  }
  else
  {
    return 0;
  }
}

// bool CSpiMaster::getXmitBuffer(uint16_t  * & pXmitBuf, size_t & size)
// {
//   pXmitBuf    = mTxEnvelope.getBuffer();
//   size        = eMAX_MSG_SIZE_WORDS;
    
//   return true;
// }

// bool CSpiMaster::getRecvBuffer(const uint16_t  * & pRcvBuf, size_t & size)
// {
//   pRcvBuf     = mRxEnvelope.getBuffer();
//   size        = eMAX_MSG_SIZE_WORDS;
    
//   return true;
// }

// bool CSpiMaster::getXmitEnvelope( CSpiMessage::CEnvelope * & pXmitEnv, size_t & sizeInWords )
// {
//   pXmitEnv    = &mTxEnvelope;
//   sizeInWords = eMAX_MSG_SIZE_WORDS;
//   return true;
// }

// bool CSpiMaster::getRecvEnvelope( const CSpiMessage::CEnvelope * & pRecvEnv, size_t & sizeInWords )
// {
//   pRecvEnv    = &mRxEnvelope;
//   sizeInWords = eMAX_MSG_SIZE_WORDS;
    
//   return true;
// }


