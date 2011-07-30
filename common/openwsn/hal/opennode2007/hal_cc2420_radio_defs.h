/**************************************************************************************************
  Filename:       mac_radio_defs.h
  Revised:        $Date: 2007-09-11 18:59:21 -0700 (Tue, 11 Sep 2007) $
  Revision:       $Revision: 15378 $

  Description:    Describe the purpose and contents of the file.


  Copyright 2006-2007 Texas Instruments Incorporated. All rights reserved.

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
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
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
**************************************************************************************************/

#ifndef MAC_RADIO_DEFS_H
#define MAC_RADIO_DEFS_H


/* ------------------------------------------------------------------------------------------------
 *                                            Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "hal_mac_cfg.h"
#include "mac_dualchip.h"
#include "mac_dualchip_tx.h"
#include "mac_sfd.h"
#include "mac_mcu_timer.h"
#include "mac_spi.h"
#include "mac_spec.h"


/* ------------------------------------------------------------------------------------------------
 *                                    Target Specific Defines
 * ------------------------------------------------------------------------------------------------
 */

/* strobe command registers */
#define SNOP                        0x00
#define SXOSCON                     0x01
#define STXCAL                      0x02
#define SRXON                       0x03
#define STXON                       0x04
#define STXONCCA                    0x05
#define SRFOFF                      0x06
#define SXOSCOFF                    0x07
#define SFLUSHRX                    0x08
#define SFLUSHTX                    0x09
#define SACK                        0x0A
#define SACKPEND                    0x0B
#define SRXDEC                      0x0C
#define STXENC                      0x0D
#define SAES                        0x0E

/* configuration registers */
#define MAIN                        0x10
#define MDMCTRL0                    0x11
#define MDMCTRL1                    0x12
#define RSSI                        0x13
#define SYNCWORD                    0x14
#define TXCTRL                      0x15
#define RXCTRL0                     0x16
#define RXCTRL1                     0x17
#define FSCTRL                      0x18
#define SECCTRL0                    0x19
#define SECCTRL1                    0x1A
#define BATTMON                     0x1B
#define IOCFG0                      0x1C
#define IOCFG1                      0x1D
#define MANFIDL                     0x1E
#define MANFIDH                     0x1F
#define FSMTC                       0x20
#define MANAND                      0x21
#define MANOR                       0x22
#define AGCCTRL                     0x23
#define AGCTST0                     0x24
#define AGCTST1                     0x25
#define AGCTST2                     0x26
#define FSTST0                      0x27
#define FSTST1                      0x28
#define FSTST2                      0x29
#define FSTST3                      0x2A
#define RXBPFTST                    0x2B
#define FSMSTATE                    0x2C
#define ADCTST                      0x2D
#define DACTST                      0x2E
#define TOPTST                      0x2F
#define RESERVED                    0x30

/* FIFO registers */
#define TXFIFO                      0x3E
#define RXFIFO                      0x3F

/* RAM memory spaces */
#define RAM_TXFIFO                  0x000
#define RAM_RXFIFO                  0x080
#define RAM_KEY0                    0x100
#define RAM_RXNONCE_RXCTR           0x110
#define RAM_SABUF                   0x120
#define RAM_KEY1                    0x130
#define RAM_TXNONCE_TXCTR           0x140
#define RAM_CBCSTATE                0x150
#define RAM_IEEEADR                 0x160
#define RAM_PANID                   0x168
#define RAM_SHORTADR                0x16A

/* status byte */
#define XOSC16M_STABLE              (1 << 6)
#define TX_UNDERFLOWM               (1 << 5)
#define ENC_BUSY                    (1 << 4)
#define TX_ACTIVE                   (1 << 3)
#define PLL_LOCK                    (1 << 2)   /* datasheet name "LOCK" is too common */
#define RSSI_VALID                  (1 << 1)

/* FSCTRL */
#define FSCTRL_BASE_VALUE           0x4000
#define FSCTRL_FREQ_2405MHZ         357

/* MDMCTRL0 */
#define MDMCTRL0_RESET_VALUE        0x0AE2
#define PAN_COORDINATOR_BV          (1 << 12)
#define ADR_DECODE_BV               (1 << 11)

/* MDMCTRL1 */
#define MDMCTRL1_RESET_VALUE        0x0000
#define RX_MODE(x)                  ((x) << 0)
#define RX_MODE_INFINITE_RECEPTION  RX_MODE(2)
#define RX_MODE_NORMAL_OPERATION    RX_MODE(0)

/* TXCTRL */
#define TXCTRL_BASE_VALUE           0xA020  /* bit 5 "must be 1" */

/* RXCTRL1 */
#define RXCTRL1_RESET_VALUE         0x0A56
#define RXBPF_LOCUR_BV              (1 << 13) /* recommended setting */
#define RXCTRL1_INIT_VALUE          (RXCTRL1_RESET_VALUE | RXBPF_LOCUR_BV)

/* IOCFG0 */
#define IOCFG0_BASE_VALUE           0x0000
#define BCN_ACCEPT_BV               (1 << 11)
#define FIFOP_POLARITY_BV           (1 << 9)
#define FIFOP_THR_RESET_VALUE       64

/* IOCFG1 */
#define IOCFG1_RESET_VALUE          0x0000
#define CCAMUX_ADC_Q0               1
#define CCAMUX_XOSC16M_STABLE       24

/* MANAND */
#define MANAND_RESET_VALUE          0xFFFF
#define ADC_PD                      (1 << 3)
#define VGA_PD                      (1 << 2)
#define RXBPF_PD                    (1 << 1)

/* FSMSTATE */
#define FSM_FFCTRL_STATE_RX_INF     31        /* not documented in data sheet */

/* MANFIDL */
#define MANFIDL_RESET_VALUE         0x233D


/* ------------------------------------------------------------------------------------------------
 *                                    Unique Radio Defines
 * ------------------------------------------------------------------------------------------------
 */
#define MAC_RADIO_CC2420
#define MAC_RADIO_RXBUFF_CHIP_BUG


/* ------------------------------------------------------------------------------------------------
 *                                    Common Radio Defines
 * ------------------------------------------------------------------------------------------------
 */
#define MAC_RADIO_CHANNEL_DEFAULT               11
#define MAC_RADIO_TX_POWER_DEFAULT              0x7F
#define MAC_RADIO_TX_POWER_MAX_MINUS_DBM        25

#define MAC_RADIO_RECEIVER_SENSITIVITY_DBM      -95 /* dBm */
#define MAC_RADIO_RECEIVER_SATURATION_DBM       10  /* dBm */

#define MAC_RADIO_RSSI_OFFSET                   HAL_MAC_RSSI_OFFSET

#define MAC_RADIO_RX_TX_PROP_DELAY_MIN_USEC     3.076  /* usec */
#define MAC_RADIO_RX_TX_PROP_DELAY_MAX_USEC     3.284  /* usec */


/* ------------------------------------------------------------------------------------------------
 *                                    Common Radio Macros
 * ------------------------------------------------------------------------------------------------
 */
#define MAC_RADIO_MCU_INIT()                          macDualchipSystemInit()

#define MAC_RADIO_TURN_ON_POWER()                     macDualchipTurnOnRadioPower()
#define MAC_RADIO_TURN_OFF_POWER()                    macDualchipDisableVREG()
#define MAC_RADIO_TURN_ON_OSC()                       macDualchipTurnOnRadioOscillator()
#define MAC_RADIO_TURN_OFF_OSC()                      macSpiCmdStrobe(SXOSCOFF)

#define MAC_RADIO_RX_FIFO_HAS_OVERFLOWED()            (HAL_MAC_READ_FIFOP_PIN() && !HAL_MAC_READ_FIFO_PIN())
#define MAC_RADIO_RX_FIFO_IS_EMPTY()                  (!HAL_MAC_READ_FIFO_PIN() && !HAL_MAC_READ_FIFOP_PIN())

#define MAC_RADIO_RSSI_IS_VALID()                     (macSpiCmdStrobe(SNOP) & CC2420_RSSI_VALID_BM)

#define MAC_RADIO_SET_RX_THRESHOLD(x)                 macDualchipSetFifopThreshold((x)-1)
#define MAC_RADIO_RX_IS_AT_THRESHOLD()                HAL_MAC_READ_FIFOP_PIN()
#define MAC_RADIO_ENABLE_RX_THRESHOLD_INTERRUPT()     HAL_MAC_ENABLE_FIFOP_INT()
#define MAC_RADIO_DISABLE_RX_THRESHOLD_INTERRUPT()    HAL_MAC_DISABLE_FIFOP_INT()
#define MAC_RADIO_CLEAR_RX_THRESHOLD_INTERRUPT_FLAG() HAL_MAC_CLEAR_FIFOP_INT_FLAG()

#define MAC_RADIO_TX_ACK()                            macSpiCmdStrobe(SACK)
#define MAC_RADIO_TX_ACK_PEND()                       macSpiCmdStrobe(SACKPEND)
#define MAC_RADIO_RX_ON()                             macSpiCmdStrobe(SRXON)
#define MAC_RADIO_RXTX_OFF()                          macSpiCmdStrobe(SRFOFF)
#define MAC_RADIO_FLUSH_TX_FIFO()                     macSpiCmdStrobe(SFLUSHTX)
#define MAC_RADIO_FLUSH_RX_FIFO()                     st( macSpiCmdStrobe(SFLUSHRX); macSpiCmdStrobe(SFLUSHRX); )
#define MAC_RADIO_TX_IS_ACTIVE()                      (macSpiCmdStrobe(SNOP) & TX_ACTIVE)

#define MAC_RADIO_READ_RX_FIFO(pData,len)             macSpiReadFifo(RXFIFO, pData, (uint8)(len))
#define MAC_RADIO_WRITE_TX_FIFO(pData,len)            macSpiWriteFifo(TXFIFO, pData, (uint8)(len))

#define MAC_RADIO_SET_PAN_COORDINATOR(b)              st( if (b) { macDualchipOrMdmctrl0(PAN_COORDINATOR_BV); } else { macDualchipAndMdmctrl0(~PAN_COORDINATOR_BV); } )
#define MAC_RADIO_SET_CHANNEL(x)                      macSpiWriteReg(FSCTRL, (uint16)(FSCTRL_BASE_VALUE + (FSCTRL_FREQ_2405MHZ + 5 * ((x) - 11))))
#define MAC_RADIO_SET_TX_POWER(x)                     macSpiWriteReg(TXCTRL, (uint16)(TXCTRL_BASE_VALUE | (x)))

#define MAC_RADIO_SET_PAN_ID(x)                       macDualchipSetPanID(x)
#define MAC_RADIO_SET_SHORT_ADDR(x)                   macSpiWriteRamUint16(RAM_SHORTADR, x)
#define MAC_RADIO_SET_IEEE_ADDR(p)                    macSpiWriteRam(RAM_IEEEADR, p, 8)

#define MAC_RADIO_REQUEST_ACK_TX_DONE_CALLBACK()      macSfdRequestAckTxDoneCallback()
#define MAC_RADIO_CANCEL_ACK_TX_DONE_CALLBACK()       macSfdCancelAckTxDoneCallback()

#define MAC_RADIO_RANDOM_BYTE()                       macDualchipRandomByte()

#define MAC_RADIO_TX_RESET()                          macDualchipTxReset()
#define MAC_RADIO_TX_PREP_CSMA_UNSLOTTED()            /* nothing required */
#define MAC_RADIO_TX_PREP_CSMA_SLOTTED()              MAC_ASSERT_FORCED() /* not supported */
#define MAC_RADIO_TX_PREP_SLOTTED()                   MAC_ASSERT_FORCED() /* not supported */
#define MAC_RADIO_TX_GO_CSMA()                        macDualchipTxGoCsma()
#define MAC_RADIO_TX_GO_SLOTTED()                     MAC_ASSERT_FORCED() /* not supported */

#define MAC_RADIO_FORCE_TX_DONE_IF_PENDING()          macSfdForceTxDoneIfPending()

#define MAC_RADIO_TX_REQUEST_ACK_TIMEOUT_CALLBACK()   macDualchipTxRequestAckTimeoutCallback()
#define MAC_RADIO_TX_CANCEL_ACK_TIMEOUT_CALLBACK()    macDualchipTxCancelAckTimeoutCallback()

#define MAC_RADIO_TIMER_TICKS_PER_USEC()              HAL_MAC_TIMER_TICKS_PER_USEC /* may be fractional */
#define MAC_RADIO_TIMER_TICKS_PER_BACKOFF()           ((uint16) (HAL_MAC_TIMER_TICKS_PER_USEC * MAC_SPEC_USECS_PER_BACKOFF + 0.5))
#define MAC_RADIO_TIMER_TICKS_PER_SYMBOL()            ((uint16) (HAL_MAC_TIMER_TICKS_PER_USEC * MAC_SPEC_USECS_PER_SYMBOL + 0.5))

#define MAC_RADIO_TIMER_CAPTURE()                     HAL_MAC_TIMER_CAPTURE()
#define MAC_RADIO_TIMER_FORCE_DELAY(x)                MAC_ASSERT_FORCED() /* not supported */

#define MAC_RADIO_TIMER_SLEEP()                       HAL_MAC_TIMER_ROLLOVER_DISABLE_INTERRUPT()
#define MAC_RADIO_TIMER_WAKE_UP()                     HAL_MAC_TIMER_ROLLOVER_ENABLE_INTERRUPT()

#define MAC_RADIO_BACKOFF_COUNT()                     macMcuTimerBackoffCount
#define MAC_RADIO_BACKOFF_CAPTURE()                   macMcuTimerBackoffCount
#define MAC_RADIO_BACKOFF_SET_COUNT(x)                st( macMcuTimerBackoffCount = x; )
#define MAC_RADIO_BACKOFF_SET_COMPARE(x)              st( macMcuTimerBackoffCompare = x; )

#define MAC_RADIO_BACKOFF_COMPARE_CLEAR_INTERRUPT()   HAL_MAC_TIMER_ROLLOVER_CLEAR_INTERRUPT()
#define MAC_RADIO_BACKOFF_COMPARE_ENABLE_INTERRUPT()  HAL_MAC_TIMER_ROLLOVER_ENABLE_INTERRUPT()
#define MAC_RADIO_BACKOFF_COMPARE_DISABLE_INTERRUPT() HAL_MAC_TIMER_ROLLOVER_DISABLE_INTERRUPT()

#define MAC_RADIO_RECORD_MAX_RSSI_START()             macDualchipRecordMaxRssiStart()
#define MAC_RADIO_RECORD_MAX_RSSI_STOP()              macDualchipRecordMaxRssiStop()

#define MAC_RADIO_TURN_ON_RX_FRAME_FILTERING()        macDualchipOrMdmctrl0(ADR_DECODE_BV)
#define MAC_RADIO_TURN_OFF_RX_FRAME_FILTERING()       macDualchipAndMdmctrl0(~ADR_DECODE_BV)


/* ------------------------------------------------------------------------------------------------
 *                                    Common Radio Externs
 * ------------------------------------------------------------------------------------------------
 */
extern const CODE uint8 macRadioDefsTxPowerTable[];


/* ------------------------------------------------------------------------------------------------
 *                              Transmit Power Setting Configuration
 * ------------------------------------------------------------------------------------------------
 */

/*
 *  To use actual register values for setting power level, uncomment the following #define.
 *  In this case, the value passed to the set power function will be written directly to the
 *  LOWER BYTE of TXCTRL.  Characterized values for this register can be found in the datasheet
 *  in the "power settings" table.
 *
 *  NOTE!  The table gives the full value for TXCTRL, for example the register value
 *        for -7 dbm is 0xA0EF.  Only the lower byte is written so, in this case, use 0xEF for
 *        the regsiter value.
 */
//#define HAL_MAC_USE_REGISTER_POWER_VALUES


/**************************************************************************************************
 */
#endif
