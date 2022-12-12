/*
 * CSpiBase.h
 *
 *  Created on: Aug 9, 2016
 *      Author: chris
 */

#ifndef INC_CSPIBASE_H_
#define INC_CSPIBASE_H_

#include "VTypes.h"
#include "COCOutBit.h"
#include "CInputBit.h"
#include "MicroControllerTypes.h"
#include "Envelope.h"
#include "CPrintSetup.h"
#include "InterruptPriority.h"

#if defined(STM32F303xC) || defined(STM32F303xE)
#include "CDmaDevice.h"
#include "stm32f303xc.h"
#include "stm32f3xx.h"
#else

struct GPIO_TypeDef;
struct DMA_TypeDef;
struct SPI_TypeDef;

typedef enum {} IRQn_Type;

#endif

class CSpiMaster;
class CSpiSlave;
class CDmaChannel;

typedef void (*DeliverMsgType)( void * pUser, const CSpiMessage::CEnvelope & incoming, CSpiMessage::CEnvelope & outgoing ); // call the synchronous method in CMessageHandler

extern "C"
{
    void SPI1_IRQHandler(void);
    void SPI2_IRQHandler(void);
    void SPI3_IRQHandler(void);
};

class CSpiBase : public CPrintSetup
{
    bool setMasterSlave(void);

    PeripheralPriority::Type getInterruptPriority(void) const;

public:

    typedef enum
    {
        eNOMASTERSLAVE = 0, eMASTER = 0x55, eSLAVE = 0xcc
    } MasterSlaveType;

    static const char * getMasterSlaveString( MasterSlaveType ms );

    typedef enum
    {
        eSPI1 = 0, eSPI2, eSPI3, eSPI_LAST
    } SpiDeviceType;  // SPI devices in the part

    static const char * getSpiDeviceString( SpiDeviceType spiDev );

    typedef enum
    {
        eFULL_DUPLEX = 0, eHALF_DUPLEX_XMT, eHALF_DUPLEX_RCV, eDUPLEX_LAST
    } SpiDuplexType;

    static const char * getDuplexString( SpiDuplexType dup );

    typedef enum
    {
        eFPCLK_DIV2   = 0,
        eFPCLK_DIV4   = 1,
        eFPCLK_DIV8   = 2,
        eFPCLK_DIV16  = 3,
        eFPCLK_DIV32  = 4,
        eFPCLK_DIV64  = 5,
        eFPCLK_DIV128 = 6,
        eFPCLK_DIV256 = 7,
    } FPClckDivType;

    enum
    {
        eDEFAULT_PRIORITY = 4
    };

    CSpiBase();

    CSpiBase(MasterSlaveType select,
             SpiDuplexType   duplex,
             SpiDeviceType   spiDev,
             PinNumberType   mosiPin,
             PinNumberType   misoPin,
             PinNumberType   spiclkPin,
             PinNumberType   spiNssPin,
             PinNumberType   attnPin,
             FPClckDivType   clkDivisor = eFPCLK_DIV4,
             bool            bDmaEnable = true
            );

    virtual ~CSpiBase();

    //!\brief this method reports the state of the SpiSlave device.
    //!Note that any interrupt has the potential for interfering with the operation
    //!of the non-DMA configuration.
    //!\return true if DMA is enabled.
    bool dmaEnabled(void) const;

    //!\brief displayReason displays the reason for a failure or success.
    //!\return void
    void displayReason(void);

    //!\brief setMessageCB is called by the consumer of message packets. The
    //!caller will receive a callback through the msgDeliveryDB function provided
    //!\param msgDeliveryCB is the call back delivering message
    //!\return void
    void setMessageCB(DeliverMsgType msgDeliveryCB, void * pUser);

    virtual CSpiMessage::CEnvelope & getOutgoingEnvelope(void) = 0;
    virtual CSpiMessage::CEnvelope & getIncomingEnvelope(void) = 0;

    //!\brief initiateTransfer starts the DMA or non-DMA transfer of data
    //!\return true if the operation was started, false otherwise.
    virtual bool initiateTransfer(void) = 0;

    //!\brief interruptHandler is invoked for SPI related interrupts, but aren't
    //! used with DMA. This just sets up a debug print, not needed in the production
    //! version.
    //!\return void
    virtual void interruptHandler(void) = 0;

    //!\brief setupOK returns the status of the spiSlave CTOR processing.
    //!\return true if the setup was OK, false otherwise.
    bool setupOK(void) const;

    //void getBufferPointers(const uint16_t * & pRecv, uint16_t * & pXmit  );

    void printSetup(void) const;

    static const char * getMasterSlaveStr( const MasterSlaveType & ms );

protected:

    MasterSlaveType mMasterSlave;
    SpiDuplexType   mSpiDuplexEnum;

    SpiDeviceType   mSpiDeviceEnum;

    SPI_TypeDef   * mpSpi;

    bool mbSpiOK;

    PinNumberType  mMosiPin;
    PinNumberType  mMisoPin;
    PinNumberType  mSpiclkPin;
    PinNumberType  mSpiNssPin;
    PinNumberType  mAttnPin;
    FPClckDivType  mClockDiv;

    CInputBit      mMosiBit;
    CInputBit      mSpiClkBit;
    CInputBit      mSpiNssBit;

    COCOutBit      mMisoBit;

    CDmaChannel  * mpRxChannel;
    CDmaChannel  * mpTxChannel;

    typedef PeripheralPriority::Type PriorityType;

    PriorityType   mInterruptPrioritySpi;   /// SPI device
    PriorityType   mInterruptPriorityDma;   /// DMA channels
    IRQn_Type      mIrq;

    CSpiMessage::CEnvelope mRcvEnvelope;
    CSpiMessage::CEnvelope mXmtEnvelope;

    const char   * mpReason;

    DeliverMsgType mpDeliverMsgCB;
    void         * mpUser;

    bool           mbDmaEnabled;

    void printSpiPins(void) const;

    static CSpiBase * gpSpiBase[eSPI_LAST];

    void printCallBackData(void) const;

    bool enableClocks(void);

    bool setSpiNVIC(void);

    bool clearSpiNVIC(void);

    friend void SPI1_IRQHandler(void);
    friend void SPI2_IRQHandler(void);
    friend void SPI3_IRQHandler(void);

};

uint32_t getSpiIrqCounter(void);

#endif /* INC_CSPIBASE_H_ */
