/**************************************************************************************************
  Filename:       ZComDef.h
  Revised:        $Date: 2007-10-28 18:43:04 -0700 (Sun, 28 Oct 2007) $
  Revision:       $Revision: 15800 $

  Description:    Type definitions and macros.


  Copyright 2004-2007 Texas Instruments Incorporated. All rights reserved.

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
**************************************************************************************************/

#ifndef ZCOMDEF_H
#define ZCOMDEF_H

#ifdef __cplusplus
extern "C"
{
#endif


/*********************************************************************
 * INCLUDES
 */

/* HAL */
#include "../osal_foundation.h"  
// #include "hal_types.h"
// #include "hal_defs.h"

/*********************************************************************
 * CONSTANTS
 */

#ifndef false
  #define false 0
#endif

#ifndef true
  #define true 1
#endif

#ifndef CONST
  #define CONST const
#endif

#ifndef GENERIC
  #define GENERIC
#endif

#ifndef ROOT
  #define ROOT
#endif

#ifndef REFLECTOR
  #if defined ( COORDINATOR_BINDING )
    #define REFLECTOR
  #endif
#endif

/*** Return Values ***/
#define ZSUCCESS                  0
#define INVALID_TASK              1
#define MSG_BUFFER_NOT_AVAIL      2
#define INVALID_MSG_POINTER       3
#define INVALID_LEN               4
#define INVALID_SENDING_TASK      5
#define INVALID_DESTINATION_TASK  6
#define INVALID_EVENT_ID          7
#define INVALID_TIMEOUT_VALUE     8
#define INVALID_INTERRUPT_ID      9
#define INVALID_ISR_PTR           10
#define INVALID_TASK_TABLE        11
#define NO_TIMER_AVAIL            12
#define INVALID_MSG_LEN           13
#define NOT_READY                 14
#define NV_ITEM_UNINIT            15
#define NV_OPER_FAILED            16
#define INVALID_MEM_SIZE          17
#define NV_BAD_ITEM_LEN           18

/*** Component IDs ***/
#define COMPID_OSAL               0
#define COMPID_MTEL               1
#define COMPID_MTSPCI             2
#define COMPID_NWK                3
#define COMPID_NWKIF              4
#define COMPID_MACCB              5
#define COMPID_MAC                6
#define COMPID_APP                7
#define COMPID_TEST               8

#define COMPID_RTG                9
#define COMPID_DATA               11

/* Temp CompIDs for testing */
#define COMPID_TEST_NWK_STARTUP           20
#define COMPID_TEST_SCAN_CONFIRM          21
#define COMPID_TEST_ASSOC_CONFIRM         22
#define COMPID_TEST_REMOTE_DATA_CONFIRM   23

// OSAL NV item IDs
#define ZCD_NV_EXTADDR                    0x0001
#define ZCD_NV_BOOTCOUNTER                0x0002
#define ZCD_NV_STARTUP_OPTION             0x0003
#define ZCD_NV_START_DELAY                0x0004

// NWK Layer NV item IDs
#define ZCD_NV_NIB                        0x0021
#define ZCD_NV_DEVICE_LIST                0x0022
#define ZCD_NV_ADDRMGR                    0x0023
#define ZCD_NV_POLL_RATE                  0x0024
#define ZCD_NV_QUEUED_POLL_RATE           0x0025
#define ZCD_NV_RESPONSE_POLL_RATE         0x0026
#define ZCD_NV_REJOIN_POLL_RATE           0x0027
#define ZCD_NV_DATA_RETRIES               0x0028
#define ZCD_NV_POLL_FAILURE_RETRIES       0x0029
#define ZCD_NV_STACK_PROFILE              0x002A
#define ZCD_NV_INDIRECT_MSG_TIMEOUT       0x002B
#define ZCD_NV_ROUTE_EXPIRY_TIME          0x002C
#define ZCD_NV_EXTENDED_PAN_ID            0x002D
#define ZCD_NV_BCAST_RETRIES              0x002E
#define ZCD_NV_PASSIVE_ACK_TIMEOUT        0x002F
#define ZCD_NV_BCAST_DELIVERY_TIME        0x0030
#define ZCD_NV_NWK_MODE                   0x0031
#define ZCD_NV_CONCENTRATOR_ENABLE        0x0032
#define ZCD_NV_CONCENTRATOR_DISCOVERY     0x0033
#define ZCD_NV_CONCENTRATOR_RADIUS        0x0034
#define ZCD_NV_MAX_SOURCE_ROUTE           0x0035

// APS Layer NV item IDs
#define ZCD_NV_BINDING_TABLE              0x0041
#define ZCD_NV_GROUP_TABLE                0x0042
#define ZCD_NV_APS_FRAME_RETRIES          0x0043
#define ZCD_NV_APS_ACK_WAIT_DURATION      0x0044
#define ZCD_NV_APS_ACK_WAIT_MULTIPLIER    0x0045
#define ZCD_NV_BINDING_TIME               0x0046

// Security NV Item IDs
#define ZCD_NV_SECURITY_LEVEL             0x0061
#define ZCD_NV_PRECFGKEY                  0x0062
#define ZCD_NV_PRECFGKEYS_ENABLE          0x0063

// ZDO NV Item IDs
#define ZCD_NV_USERDESC                   0x0081
#define ZCD_NV_NWKKEY                     0x0082
#define ZCD_NV_PANID                      0x0083
#define ZCD_NV_CHANLIST                   0x0084
#define ZCD_NV_LEAVE_CTRL                 0x0085
#define ZCD_NV_SCAN_DURATION              0x0086
#define ZCD_NV_LOGICAL_TYPE               0x0087

// ZCL NV item IDs
#define ZCD_NV_SCENE_TABLE                0x0091

// Non-standard NV item IDs
#define ZCD_NV_SAPI_ENDPOINT              0x00A1

// NV Items Reserved for applications (user applications)
// 0x0201 ?0x0FFF

// ZCD_NV_STARTUP_OPTION values
//   These are bit weighted - you can OR these together.
//   Setting one of these bits will set their associated NV items
//   to code initialized values.
#define ZCD_STARTOPT_DEFAULT_CONFIG_STATE  0x01
#define ZCD_STARTOPT_DEFAULT_NETWORK_STATE 0x02
#define ZCD_STARTOPT_AUTO_START            0x04
#define ZCD_STARTOPT_CLEAR_CONFIG   ZCD_STARTOPT_DEFAULT_CONFIG_STATE
#define ZCD_STARTOPT_CLEAR_STATE  ZCD_STARTOPT_DEFAULT_NETWORK_STATE
/*********************************************************************
 * TYPEDEFS
 */

/*** Data Types ***/
typedef uint8   byte;
typedef uint16  UINT16;
typedef int16   INT16;
typedef int32   int24;
typedef uint32  uint24;

enum
{
  AddrNotPresent = 0,
  AddrGroup = 1,
  Addr16Bit = 2,
  Addr64Bit = 3,
  AddrBroadcast = 15
};

#define Z_EXTADDR_LEN   8

typedef byte ZLongAddr_t[Z_EXTADDR_LEN];

typedef struct
{
  union
  {
    uint16      shortAddr;
    ZLongAddr_t extAddr;
  } addr;
  byte addrMode;
} zAddrType_t;

#define ZSuccess                    0x00
#define ZFailure                    0x01
#define ZInvalidParameter           0x02

// ZStack status values
#define ZMemError                   0x10
#define ZBufferFull                 0x11
#define ZUnsupportedMode            0x12
#define ZMacMemError                0x13

#define ZSapiInProgress             0x20
#define ZSapiTimeout                0x21
#define ZSapiInit                   0x22

	// APS status values
#define ZApsFail                    0xb1
#define ZApsTableFull               0xb2
#define ZApsIllegalRequest          0xb3
#define ZApsInvalidBinding          0xb4
#define ZApsUnsupportedAttrib       0xb5
#define ZApsNotSupported            0xb6
#define ZApsNoAck                   0xb7
#define ZApsDuplicateEntry          0xb8
#define ZApsNoBoundDevice           0xb9
	
	// Security status values
#define ZSecNoKey                   0xa1
#define ZSecOldFrmCount             0xa2
#define ZSecMaxFrmCount             0xa3
#define ZSecCcmFail                 0xa4

	// NWK status values
#define ZNwkInvalidParam            0xc1
#define ZNwkInvalidRequest          0xc2
#define ZNwkNotPermitted            0xc3
#define ZNwkStartupFailure          0xc4
#define ZNwkAlreadyPresent          0xc5
#define ZNwkSyncFailure             0xc6
#define ZNwkTableFull               0xc7
#define ZNwkUnknownDevice           0xc8
#define ZNwkUnsupportedAttribute    0xc9
#define ZNwkNoNetworks              0xca
#define ZNwkLeaveUnconfirmed        0xcb
#define ZNwkNoAck                   0xcc  // not in spec
#define ZNwkNoRoute                 0xcd

	// MAC status values
#define ZMacSuccess                 0x00
#define ZMacBeaconLoss              0xe0
#define ZMacChannelAccessFailure    0xe1
#define ZMacDenied                  0xe2
#define ZMacDisableTrxFailure       0xe3
#define ZMacFailedSecurityCheck     0xe4
#define ZMacFrameTooLong            0xe5
#define ZMacInvalidGTS              0xe6
#define ZMacInvalidHandle           0xe7
#define ZMacInvalidParameter        0xe8
#define ZMacNoACK                   0xe9
#define ZMacNoBeacon                0xea
#define ZMacNoData                  0xeb
#define ZMacNoShortAddr             0xec
#define ZMacOutOfCap                0xed
#define ZMacPANIDConflict           0xee
#define ZMacRealignment             0xef
#define ZMacTransactionExpired      0xf0
#define ZMacTransactionOverFlow     0xf1
#define ZMacTxActive                0xf2
#define ZMacUnAvailableKey          0xf3
#define ZMacUnsupportedAttribute    0xf4
#define ZMacUnsupported             0xf5

typedef byte ZStatus_t;

typedef struct
{
  uint8  txCost;       // counter of transmission success/failures
  uint8  rxCost;       // average of received rssi values
  uint8  inKeySeqNum;  // security key sequence number
  uint32 inFrmCntr;    // security frame counter..
} linkInfo_t;

/*********************************************************************
 * Global System Events
 */

#define SYS_EVENT_MSG               0x8000  // A message is waiting event

/*********************************************************************
 * Global System Messages
 */

#define SPI_INCOMING_ZTOOL_PORT   0x21    // Raw data from ZTool Port (not implemented)
#define SPI_INCOMING_ZAPP_DATA    0x22    // Raw data from the ZAPP port (see serialApp.c)
#define MT_SYS_APP_MSG            0x23    // Raw data from an MT Sys message
#define MT_SYS_APP_RSP_MSG        0x24    // Raw data output for an MT Sys message

#define AF_DATA_CONFIRM_CMD       0xFD    // Data confirmation
#define AF_INCOMING_MSG_CMD       0x1A    // Incoming MSG type message
#define AF_INCOMING_KVP_CMD       0x1B    // Incoming KVP type message
#define AF_INCOMING_GRP_KVP_CMD   0x1C    // Incoming Group KVP type message

#define KEY_CHANGE                0xC0    // Key Events

#define ZDO_NEW_DSTADDR           0xD0    // ZDO has received a new DstAddr for this app
#define ZDO_STATE_CHANGE          0xD1    // ZDO has changed the device's network state
#define ZDO_MATCH_DESC_RSP_SENT   0xD2    // ZDO match descriptor response was sent
#define ZDO_CB_MSG                0xD3    // ZDO incoming message callback

// OSAL System Message IDs/Events Reserved for applications (user applications)
// 0xE0 ?0xFF

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ZCOMDEF_H */
