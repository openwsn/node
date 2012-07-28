/***********************************************************************************

  Filename:       hal_rf.c

  Description:    CC2430 radio interface.

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include "../../hal_comp_foundation.h"
#include "../../targets/srf04_soc/hal_board.h"
#include "../../targets/interface/hal_mcu_ti.h"
#include "../../targets/interface/hal_int.h"
#include "../../targets/interface/hal_assert_ti.h"
#include "../../utils/util.h"
#include "../../targets/interface/hal_rf.h"
#include "string.h"

/***********************************************************************************
* CONSTANTS AND DEFINES
*/

// Chip revision
#define REV_A                   0x01
#define REV_B                   0x02
#define REV_C                   0x03
#define REV_D                   0x04
#define REV_E                   0x05
#define REV_F                   0x06

#define CHIPREVISION            REV_E

// Security
#define PKT_LEN_MAX              127

// CC2430 RSSI Offset
#define RSSI_OFFSET               45
#define RSSI_OFFSET_LNA_HIGHGAIN  RSSI_OFFSET + 12
#define RSSI_OFFSET_LNA_LOWGAIN   RSSI_OFFSET + 6

// Output power programming (TXCTRLL)
#define PA_LEVEL_MASK           0x1F

// RF status flags
#define TX_ACTIVE_FLAG          0x10
#define FIFO_FLAG               0x08
#define FIFOP_FLAG              0x04
#define SFD_FLAG                0x02
#define CCA_FLAG                0x01

// Radio status states
#define TX_ACTIVE()             (RFSTATUS & TX_ACTIVE_FLAG)
#define FIFO()                  (RFSTATUS & FIFO_FLAG)
#define FIFOP()                 (RFSTATUS & FIFOP_FLAG)
#define SFD()                   (RFSTATUS & SFD_FLAG)
#define CCA()                   (RFSTATUS & CCA_FLAG)

// Various radio settings
#define ADR_DECODE              0x08
#define AUTO_CRC                0x20
#define AUTO_ACK                0x10
#define AUTO_TX2RX_OFF          0x08
#define RX2RX_TIME_OFF          0x04
#define ACCEPT_ACKPKT           0x01

// RF interrupt flags
#define IRQ_RREG_ON             0x80
#define IRQ_TXDONE              0x40
#define IRQ_FIFOP               0x20
#define IRQ_SFD                 0x10
#define IRQ_CCA                 0x08
#define IRQ_CSP_WT              0x04
#define IRQ_CSP_STOP            0x02
#define IRQ_CSP_INT             0x01

// RF status flags

// Selected strobes
#define ISRXON()                st(RFST = 0xE2;)
#define ISTXON()                st(RFST = 0xE3;)
#define ISTXONCCA()             st(RFST = 0xE4;)
#define ISRFOFF()               st(RFST = 0xE5;)
#define ISFLUSHRX()             st(RFST = 0xE6;)
#define ISFLUSHTX()             st(RFST = 0xE7;)

#define FLUSH_RX_FIFO()         st( ISFLUSHRX(); ISFLUSHRX(); )


// CC2590-CC2591 support
//
#ifdef INCLUDE_PA

/* Select CC2591 RX low gain mode */
#define HAL_PA_LNA_RX_LGM() st( uint8 i; P1_1= 0; for (i=0; i<8; i++) asm("NOP"); )


/* Select CC2591 RX high gain mode */
#define HAL_PA_LNA_RX_HGM() st( uint8 i; P1_1= 1; for (i=0; i<8; i++) asm("NOP"); )

#define HAL_RF_TXPOWER_0_DBM   0
#define HAL_RF_TXPOWER_13_DBM  1
#define HAL_RF_TXPOWER_15_DBM  2
#define HAL_RF_TXPOWER_18_DBM  3
#define HAL_RF_TXPOWER_19_DBM  4


#else // dummy macros when not using CC2591

#define HAL_PA_LNA_INIT()
#define HAL_PA_LNA_RX_LGM()
#define HAL_PA_LNA_RX_HGM()

#define HAL_RF_TXPOWER_MIN_4_DBM  0
#define HAL_RF_TXPOWER_0_DBM      1

#endif

/***********************************************************************************
* GLOBAL DATA
*/
#ifdef INCLUDE_PA
static const menuItem_t pPowerSettings[] =
{
  "0dBm", HAL_RF_TXPOWER_0_DBM,
  "13dBm", HAL_RF_TXPOWER_13_DBM,
  "15dBm", HAL_RF_TXPOWER_15_DBM,
  "18dBm", HAL_RF_TXPOWER_18_DBM,
  "19dBm", HAL_RF_TXPOWER_19_DBM
};
#else
static const menuItem_t pPowerSettings[] =
{
  "-4dBm", HAL_RF_TXPOWER_MIN_4_DBM,
  "0dBm", HAL_RF_TXPOWER_0_DBM
};
#endif

const menu_t powerMenu =
{
  pPowerSettings,
  N_ITEMS(pPowerSettings)
};

/***********************************************************************************
* LOCAL DATA
*/
static ISR_FUNC_PTR pfISR= NULL;
static uint8 rssiOffset = RSSI_OFFSET;


/***********************************************************************************
* LOCAL FUNCTIONS
*/
static void halPaLnaInit(void);

/***********************************************************************************
* GLOBAL FUNCTIONS
*/

/***********************************************************************************
* @fn      halRfInit
*
* @brief   Power up, sets default tuning settings, enables autoack, enables random
*          generator.
*
* @param   none
*
* @return  SUCCESS always (for interface compatibility)
*/
uint8 halRfInit(void)
{
    uint8 i;

    // turning on power to analog part of radio and waiting for voltage regulator.
    RFPWR = 0x04;
    while( RFPWR & 0x10 );

    // Setting for AUTO CRC and AUTOACK
    MDMCTRL0L |= (AUTO_CRC | AUTO_ACK);

    // Turning on AUTO_TX2RX
    FSMTC1 = ((FSMTC1 & (~AUTO_TX2RX_OFF & ~RX2RX_TIME_OFF))  | ACCEPT_ACKPKT);

    // Turning off abortRxOnSrxon.
    FSMTC1 &= ~0x20;

    // Set FIFOP threshold to maximum
    IOCFG0 = 0x7F;
    // tuning adjustments for optimal radio performance; details available in datasheet */
    RXCTRL0H = 0x32;
    RXCTRL0L = 0xF5;

    // Turning on receiver to get output from IF-ADC
    ISRXON();
    halMcuWaitUs(1);

    // Enable random generator
    ADCCON1 &= ~0x0C;

    for(i = 0 ; i < 32 ; i++)
    {
        RNDH = ADCTSTH;
        // Clock random generator
        ADCCON1 |= 0x04;
    }
    ISRFOFF();

    // Enable CC2591 with High Gain Mode
    halPaLnaInit();

    halRfEnableRxInterrupt();

    return SUCCESS;
}



/***********************************************************************************
* @fn      halRfGetChipId
*
* @brief   Get chip id
*
* @param   none
*
* @return  uint8 - result
*/
uint8 halRfGetChipId(void)
{
    return CHIPID;
}


/***********************************************************************************
* @fn      halRfGetChipVer
*
* @brief   Get chip version
*
* @param   none
*
* @return  uint8 - result
*/
uint8 halRfGetChipVer(void)
{
    return CHVER;
}

/***********************************************************************************
* @fn      halRfGetStatusByte
*
* @brief   Get chip status byte
*
* @param   none
*
* @return  uint8 - chip status byte
*/
uint8 halRfGetStatusByte(void)
{
    HAL_ASSERT(FALSE);
    return 0;
}



/***********************************************************************************
* @fn      halRfGetRandomByte
*
* @brief   Return random byte
*
* @param   none
*
* @return  uint8 - random byte
*/
uint8 halRfGetRandomByte(void)
{
    // Clock the random generator
    ADCCON1 |= 0x04;

    return RNDH;

}


/***********************************************************************************
* @fn      halRfGetRssiOffset
*
* @brief   Return RSSI Offset
*
* @param   none
*
* @return  uint8 - RSSI offset
*/
uint8 halRfGetRssiOffset(void)
{
  return rssiOffset;
}

/***********************************************************************************
* @fn      halRfSetChannel
*
* @brief   Set RF channel in the 2.4GHz band. The Channel must be in the range 11-26,
*          11= 2005 MHz, channel spacing 5 MHz.
*
* @param   channel - logical channel number
*
* @return  none
*/
void halRfSetChannel(uint8 channel)
{
    uint16 freqMHz;

    freqMHz= 2405 + ((channel - MIN_CHANNEL) * CHANNEL_SPACING); // Calculate frequency
    freqMHz -= (UINT32)2048;              // Subtract; datasheet sect 14.16

    FSCTRLL = LOBYTE(freqMHz);
    FSCTRLH &= ~0x03;
    FSCTRLH |= (HIBYTE(freqMHz) & 0x03);
}


/***********************************************************************************
* @fn      halRfSetShortAddr
*
* @brief   Write short address to chip
*
* @param   none
*
* @return  none
*/
void halRfSetShortAddr(uint16 shortAddr)
{
    SHORTADDRL= LOBYTE(shortAddr);
    SHORTADDRH= HIBYTE(shortAddr);
}


/***********************************************************************************
* @fn      halRfSetPanId
*
* @brief   Write PAN Id to chip
*
* @param   none
*
* @return  none
*/
void halRfSetPanId(uint16 panId)
{
    PANIDL= LOBYTE(panId);
    PANIDH= HIBYTE(panId);
}


/***********************************************************************************
* @fn      halRfSetPower
*
* @brief   Set TX output power
*
* @param   uint8 power - power level: TXPOWER_MIN_4_DBM, TXPOWER_0_DBM,
*                        TXPOWER_4_DBM
*
* @return  uint8 - SUCCESS or FAILED
*/
uint8 halRfSetTxPower(uint8 power)
{
    uint8 v;

    switch(power)
    {
#ifdef INCLUDE_PA
      case HAL_RF_TXPOWER_0_DBM:
        v = 0x06;
        break;
    case HAL_RF_TXPOWER_13_DBM:
        v = 0x13;
        break;
    case HAL_RF_TXPOWER_15_DBM:
        v = 0x17;
        break;
    case HAL_RF_TXPOWER_18_DBM:
        v = 0x5F;
        break;
    case HAL_RF_TXPOWER_19_DBM:
        v = 0xFF;
        break;
#else
    case HAL_RF_TXPOWER_0_DBM:
        v = 0x5F;
        break;
    case HAL_RF_TXPOWER_MIN_4_DBM:
        v = 0x17;
        break;
#endif
    default:
        return FAILED;
    }

    // Set TX power
    TXCTRLL&= ~PA_LEVEL_MASK;
    TXCTRLL|= v;

    return SUCCESS;
}


/***********************************************************************************
* @fn      halRfSetGain
*
* @brief   Set gain mode - only applicable for units with CC2590/91.
*
* @param   uint8 - gain mode
*
* @return  none
*/
void halRfSetGain(uint8 gainMode)
{
    if (gainMode==HAL_RF_GAIN_LOW) {
        HAL_PA_LNA_RX_LGM();
        rssiOffset = RSSI_OFFSET_LNA_LOWGAIN;
    } else {
        HAL_PA_LNA_RX_HGM();
        rssiOffset = RSSI_OFFSET_LNA_HIGHGAIN;
    }
}

/***********************************************************************************
* @fn      halRfWriteTxBuf
*
* @brief   Write to TX buffer
*
* @param   uint8* pData - buffer to write
*          uint8 length - number of bytes
*
* @return  none
*/
void halRfWriteTxBuf(uint8* pData, uint8 length)
{
    uint8 i;

    ISFLUSHTX();          // Making sure that the TX FIFO is empty.

    RFIF = ~IRQ_TXDONE;   // Clear TX done interrupt

    // Insert data
    for(i=0;i<length;i++){
        RFD = pData[i];
    }

}


/***********************************************************************************
* @fn      halRfAppendTxBuf
*
* @brief   Write to TX buffer
*
* @param   uint8* pData - buffer to write
*          uint8 length - number of bytes
*
* @return  none
*/
void halRfAppendTxBuf(uint8* pData, uint8 length)
{
    uint8 i;

    // Insert data
    for(i=0;i<length;i++){
        RFD = pData[i];
    }
}


/***********************************************************************************
* @fn      halRfReadRxBuf
*
* @brief   Read RX buffer
*
* @param   uint8* pData - data buffer. This must be allocated by caller.
*          uint8 length - number of bytes
*
* @return  none
*/
void halRfReadRxBuf(uint8* pData, uint8 length)
{
    while (length>0) {
        *pData++= RFD;
        length--;
    }
}



/***********************************************************************************
* @fn      halRfTransmit
*
* @brief   Transmit frame with Clear Channel Assessment.
*
* @param   none
*
* @return  uint8 - SUCCESS or FAILED
*/
uint8 halRfTransmit(void)
{
    uint8 status;

    ISTXON(); // Sending

    // Waiting for transmission to finish
    while(!(RFIF & IRQ_TXDONE) );

    RFIF = ~IRQ_TXDONE;
    status= SUCCESS;

    // TBD: use CCA
    return status;
}



/***********************************************************************************
* @fn      halRfReceiveOn
*
* @brief   Turn receiver on
*
* @param   none
*
* @return  none
*/
void halRfReceiveOn(void)
{
    FLUSH_RX_FIFO();
    ISRXON();
}

/***********************************************************************************
* @fn      halRfReceiveOff
*
* @brief   Turn receiver off
*
* @param   none
*
* @return  none
*/
void halRfReceiveOff(void)
{
    ISRFOFF();
    FLUSH_RX_FIFO();
}


/***********************************************************************************
* @fn      halRfDisableRxInterrupt
*
* @brief   Clear and disable RX interrupt.
*
* @param   none
*
* @return  none
*/
void halRfDisableRxInterrupt(void)
{
  // disable RX_FIFOP interrupt
  RFIM &= ~BV(5);
  // disable general RF interrupts
  IEN2 &= ~BV(0);
}


/***********************************************************************************
* @fn      halRfEnableRxInterrupt
*
* @brief   Enable RX interrupt.
*
* @param   none
*
* @return  none
*/
void halRfEnableRxInterrupt(void)
{
  // enable RX_FIFOP interrupt
  RFIM |= BV(5);
  // enable general RF interrupts
  IEN2 |= BV(0);
}


/***********************************************************************************
* @fn      halRfRxInterruptConfig
*
* @brief   Configure RX interrupt.
*
* @param   none
*
* @return  none
*/
void halRfRxInterruptConfig(ISR_FUNC_PTR pf)
{
    uint8 x;
    HAL_INT_LOCK(x);
    pfISR= pf;
    HAL_INT_UNLOCK(x);
}

/***********************************************************************************
* @fn      halRfWaitTransceiverReady
*
* @brief   Wait until the transciever is ready (SFD inactive).
*
* @param   none
*
* @return  none
*/
void halRfWaitTransceiverReady(void)
{
    while (RFSTATUS & (BV(1) | BV(4) ));
}

/************************************************************************************
 * @fn          macMcuRfIsr
 *
 * @brief       Interrupt service routine that handles FIFOP interrupts.
 *
 * @param       none
 *
 * @return      none
 */
HAL_ISR_FUNCTION( macMcuRfIsr, RF_VECTOR )
{
    uint8 rfim;
    uint8 x;

    HAL_INT_LOCK(x);

    rfim = RFIM;

    if ((RFIF & IRQ_FIFOP) & rfim)
    {
        (pfISR)();                  // Execute the custom ISR
        S1CON= 0;
        RFIF&= ~IRQ_FIFOP;
    }
    HAL_INT_UNLOCK(x);
}

/***********************************************************************************
* LOCAL FUNCTIONS
*/
static void halPaLnaInit(void)
{
#ifdef INCLUDE_PA  
    /* Initialize CC2591 to RX high gain mode */
    static uint8 fFirst= TRUE;
    
    if(fFirst) {
        uint8 i; 
        P1SEL&= ~0x02; 
        P1DIR|= 0x02; 
        P1_1= 1; 
        
        for (i=0; i<8; i++) { 
            asm("NOP");
        }
        fFirst = FALSE;
        rssiOffset = RSSI_OFFSET_LNA_HIGHGAIN;
    }
    
#else // do nothing
#endif
}



/***********************************************************************************
  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
***********************************************************************************/
