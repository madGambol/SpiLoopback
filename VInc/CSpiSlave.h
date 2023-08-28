#pragma once
/*
 * File:   CSpiSlave.h
 * Author: C. Arena at Hallowell EMC
 *
 * Created on April 8, 2016, 4:49 PM
 * Modified on January 1, 2023, 4:49 PM
 */

#include "main.h"
#include "CSpiMaster.h"

class CSpiSlave
{
    CSpiSlave(const CSpiSlave& orig);               // private to prevent use
    CSpiSlave & operator=( const CSpiSlave & rhs ); // private to prevent use

	SPI_HandleTypeDef * mpSpiDev;

	static CSpiSlave * gpSpiSlave;

	transmitCompleteCB * mpCB;

	uint8_t * mpBufOut;
	uint8_t * mpBufIn;
	uint16_t  mSndRcvSize;

public:

    CSpiSlave( SPI_HandleTypeDef * pSpi );

    //!\brief initialize the CSpiSlave. This should be called immediately after
    //!the CTOR is invoked.
    //!\return true if init succeeded in setting up DMA, false otherwise
    bool init(void);

    virtual ~CSpiSlave();

	/**
	 * \brief sendRcv expects the input and output buffers to persist: they should not be
	 *        allocated on the stack or reused for something else. Otherwise the results
	 *        are not defined.
	 * \param size is the size in bytes.
	 * \param sndRcvStatus is a status result set at the end of the send/receive setup.
	 * \return true if the operation returns HAL_OK, false otherwise
	 */
	bool sendRcv( uint8_t * pBufOut, uint8_t * pBufIn, uint16_t size, int & sndRcvStatus);

	bool transferComplete(void);

	bool dmaComplete(void);

	bool setCallback( transmitCompleteCB * pCB );

	static void transferComplete( SPI_HandleTypeDef * );

	static void dmaComplete     ( DMA_HandleTypeDef * );

private:


};
