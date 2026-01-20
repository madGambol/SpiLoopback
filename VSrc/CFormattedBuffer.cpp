/*
 * CFormattedBuffer.cpp
 *
 *  Created on: November  3, 2021
 *      Author: C Arena
 */

#include "CFormattedBuffer.h"
#include "CSerialPrint.h"
#include "CHoldInterrupts.h"

#include <stdio.h>
#include <string.h>

CFormattedBuffer::CFormattedBuffer(void)
: mOffset( 0 ) 
{
    clear();
}

CFormattedBuffer::~CFormattedBuffer ()
{
}

void CFormattedBuffer::clear(void)
{
    memset( mMsgBuf, 0, sizeof(mMsgBuf) );
    mOffset = 0;
}

uint16_t CFormattedBuffer::getOffset(void) const
{
    return mOffset;
}

extern uint64_t ticks;

bool CFormattedBuffer::addTime (const char * pFormat)
{
    CHoldInterrupts now;
    bool bRetVal = false;

    do
    {
        if (pFormat == nullptr) break;

        if ( mOffset >= sizeof(mMsgBuf) ) break;

        char * pBuf     = &mMsgBuf[mOffset];

        size_t buflimit = sizeof(mMsgBuf) - mOffset;

        size_t outSize  = snprintf(pBuf, buflimit, pFormat, (uint32_t)ticks );

        if ((outSize + mOffset) >= sizeof(mMsgBuf)) break;

        bRetVal  = true;

        mOffset += outSize;

    } while (0);

    return bRetVal;
}

bool CFormattedBuffer::addStr( const char * pStr, const char * pFormat )
{
    CHoldInterrupts now;
    bool bRetVal = false;

    do
    {
        if (pFormat == nullptr) break;
    
        if ( mOffset >= sizeof(mMsgBuf) ) break;

        char * pBuf = &mMsgBuf[mOffset];

        size_t buflimit = sizeof(mMsgBuf) - mOffset;

        size_t outSize  = snprintf(pBuf, buflimit, pFormat, pStr );

        if ((outSize + mOffset) >= sizeof(mMsgBuf)) break;

        bRetVal = true;

        mOffset += outSize;

    } while (0);

    return bRetVal;
}

bool CFormattedBuffer::addUInt( uint32_t value, const char * pFormat )
{
    CHoldInterrupts now;
    bool bRetVal = false;

    do
    {
        if (pFormat == nullptr) break;

        if ( mOffset >= sizeof(mMsgBuf) ) break;

        char * pBuf     = &mMsgBuf[mOffset];

        size_t buflimit = sizeof(mMsgBuf) - mOffset;

        size_t outSize  = snprintf(pBuf, buflimit, pFormat, value );

        if ((outSize + mOffset) >= sizeof(mMsgBuf)) break;

        bRetVal  = true;

        mOffset += outSize;

    } while (0);

    return bRetVal;
}

bool CFormattedBuffer::print(void)
{
    bool bRetVal = false;

    bRetVal = putstring( mMsgBuf );

    uint32_t limit = sizeof(mMsgBuf);

    if ( mOffset < limit )
    {
        limit = mOffset;
    }

    memset( mMsgBuf, 0, limit );
    
    mOffset = 0;

    return bRetVal;
}

