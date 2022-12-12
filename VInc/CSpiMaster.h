/* 
 * File:   CSpiMaster.h
 * Author: C. Arena at Hallowell EMC
 *
 * Created on April 24, 2016, 11:53 PM
 */

#ifndef CSPIMASTER_H
#define CSPIMASTER_H

#include <string>
#include "Envelope.h"
#include "CSpiBase.h"

class CDmaChannel;

typedef void (*DeliverMsgType)( void * pUser, const CSpiMessage::CEnvelope & incoming, CSpiMessage::CEnvelope & outgoing ); // call the synchronous method in CMessageHandler

class CSpiMaster: public CSpiBase
{
  CSpiMaster(const CSpiMaster& orig);
  CSpiMaster & operator=(const CSpiMaster & rhs);

 public:

  typedef CSpiBase::FPClckDivType FPClckDivType;

  CSpiMaster();

  CSpiMaster(SpiDuplexType duplex,
			 SpiDeviceType spiDev,
			 PinNumberType mosiPin,
			 PinNumberType misoPin,
			 PinNumberType spiclkPin,
			 PinNumberType spiNssPin,
			 PinNumberType attnPin  = eNO_PIN,
			 FPClckDivType clockDiv = eFPCLK_DIV4,
			 uint32_t priority      = eDEFAULT_PRIORITY
			);

  CSpiMaster(uint8_t mode, uint8_t bits, uint32_t speed, uint16_t delay, const char * pDevName );

  virtual ~CSpiMaster();

  CSpiMessage::CEnvelope & getOutgoingEnvelope(void);
  CSpiMessage::CEnvelope & getIncomingEnvelope(void);
  
  bool initiateTransfer(void);

  //!\brief enableDmaTransfer primes the DMA mechanism supporting the SPI
  //!device making it ready for the next transfer.
  //!\return true if the DMA is set up, false otherwise.
  bool enableDmaTransfer(void);
  bool deviceOpenedOK(void) const;

  void printSetup(void) const;

  /*   bool getXmitEnvelope(      CSpiMessage::CEnvelope * & pXmitEnv, size_t & sizeInWords ); */
  /*   bool getRecvEnvelope(const CSpiMessage::CEnvelope * & pRecvEnv, size_t & sizeInWords ); */

  static CSpiBase * getSpi(CSpiBase::SpiDeviceType spiDev);

 private:

    //!\brief interruptHandler is invoked for SPI related interrupts, but aren't
    //! used with DMA. This just sets up a debug print, not needed in the production
    //! version.
    //!\return void
    void interruptHandler(void);

    CInputBit   mAttnBit;

    int transfer(void);

    uint8_t     mMode;       //   =  0;
    uint8_t     mBits;       //   = 16;
    uint32_t    mSpeed;      //   =  6000000;
    uint16_t    mDelay;      //   =  2;

    std::string mDevName;
    int         mSpiDevFd;

  friend class CMessageHandler;
};

#endif /* CSPIMASTER_H */

