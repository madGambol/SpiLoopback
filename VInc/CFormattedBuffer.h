#pragma once
/*
 * CFormattedBuffer.h
 *
 *  Created on: November 3, 2021
 *      Author: C Arena
 */

#include <stdint.h>
#include <cstdint>
#include <cstddef>

enum { eMSGBUFSIZE = 2048 };


class CFormattedBuffer
{
    char     mMsgBuf[eMSGBUFSIZE];
    uint16_t mOffset;

public:

    CFormattedBuffer();

    virtual ~CFormattedBuffer();

    bool addTime( const char * pFormat = "%ld" );

    bool addStr( const char * pStr, const char * pFormat = "%s" );

    bool addUInt ( uint32_t value, const char * pFormat = "%lu" );

    uint16_t getOffset(void) const;

    void clear(void);

    bool print(void);
};



