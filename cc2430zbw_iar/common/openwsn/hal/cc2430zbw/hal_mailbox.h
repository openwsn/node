/**************************************************************************************************
  Filename:       hal_mailbox.h
  Revised:        $Date: 2007-11-07 11:54:17 -0800 (Wed, 07 Nov 2007) $
  Revision:       $Revision: 15885 $

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

#ifndef HAL_MAILBOX_H
#define HAL_MAILBOX_H

#define MBOX_SBL_SHELL     ((unsigned long)0x53544159)  // 'STAY' in SBL and run command shell
#define MBOX_SBL_GO_APP    ((unsigned long)0x4732474F)  // 'G2GO' good to go to App

// this is the mailbox value that tells the boot code to flash the downloaded image
// even though the operational image may be sane.
#define MBOX_OAD_ENABLE    ((unsigned long)0x454E424C)  // 'ENBL' enable downloaded code

/*
 * This is overlayed with the list of MAC timer callbacks for CC MAC. This
 * won't matter because it is read only after reset. In the boot code the
 * MAC stuff doesn't matter. In the application it is read in ZMain:main()
 * before the MAC init runs. Currently no one cares about what is read. We
 * can add an API if we need it. When the reset command executes it's a
 * don't-care when we write over the possible MAC stuff since we;'re
 * resetting anyway.
 */
typedef struct mbox_s {
  volatile unsigned long BootRead;
  volatile unsigned long AppRead;
} mboxMsg_t;

#ifdef __IAR_SYSTEMS_ICC__
__no_init mboxMsg_t mboxMsg @ 0xE000;
#endif

#endif
