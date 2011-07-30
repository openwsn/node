
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
***********************************************************************************/

/***********************************************************************************
  Filename:     hal_rf.h

  Description:  hal radio interface header file

***********************************************************************************/

#ifndef HAL_RF_H
#define HAL_RF_H

/***********************************************************************************
* INCLUDES
*/
#include <hal_types.h>
#include "hal_cc2520.h"

/***********************************************************************************
* TYPEDEFS
*/
// The chip status byte, returned by chip for all SPI accesses
typedef uint8 HAL_RF_STATUS;

/***********************************************************************************
* CONSTANTS AND DEFINES
*/

#define KEY_LENGTH              16
#define NONCE_LENGTH            16

// TX power
#define TXPOWER_MIN_4_DBM       0
#define TXPOWER_0_DBM           1
#define TXPOWER_4_DBM           2

/***********************************************************************************
* GLOBAL FUNCTIONS
*/
void halRfPowerUp(void);
void halRfResetChip(void);
HAL_RF_STATUS halRfInit(void);
HAL_RF_STATUS halRfWaitXoscStable(void);

uint8 halRfGetChipId(void);
uint8 halRfGetChipVer(void);
HAL_RF_STATUS halRfGetStatusByte(void);
uint8 halRfGetRandomByte(void);
uint8 halRfGetRssiOffset(void);

void halRfSetChannel(uint8 channel);
HAL_RF_STATUS halRfSetShortAddr(uint16 shortAddr);
HAL_RF_STATUS halRfSetPanId(uint16 PanId);
uint8 halRfSetPower(uint8 power);

HAL_RF_STATUS halRfRecvFrame(uint8* data, uint8 length);
HAL_RF_STATUS halRfWriteTxBuf(uint8* data, uint8 length);
HAL_RF_STATUS halRfReadRxBuf(uint8* data, uint8 length);
uint8 halRfTransmitCCA(void);

void halRfSecurityInit(uint8* key, uint8* nonceRx, uint8* nonceTx);
uint8 halRfRecvFrameUCCM(uint8* data, uint8 length, uint8 encrLength, uint8 authLength, uint8 m);
HAL_RF_STATUS halRfWriteTxBufCCM(uint8* data, uint8 length, uint8 encrLength, uint8 authLength, uint8 m);
void halRfIncNonceTx(void);

HAL_RF_STATUS halRfReceiveOn(void);
HAL_RF_STATUS halRfReceiveOff(void);

#endif
