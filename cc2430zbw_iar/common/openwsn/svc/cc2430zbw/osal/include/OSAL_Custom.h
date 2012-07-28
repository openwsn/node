/**************************************************************************************************
  Filename:       OSAL_Custom.h
  Revised:        $Date: 2007-10-28 18:43:04 -0700 (Sun, 28 Oct 2007) $
  Revision:       $Revision: 15800 $

  Description:    This file contains all the settings and other functions that
                  the user should set and change.


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

#ifndef OSAL_CUSTOM_H
#define OSAL_CUSTOM_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

 
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */
 
/*********************************************************************
 * FUNCTIONS
 */

/*
 * This function adds all the tasks to the task list.
 *  This is where to add new tasks.
 */
extern void osalAddTasks( void );

/*********************************************************************
$Log$
Revision 1.7  2003/12/17 19:49:44  rjessup
Added power manager and memory manager as a seperate module

Revision 1.6  2003/10/10 18:27:18  rjessup
bumped up the heap for windows

Revision 1.5  2003/10/09 00:47:19  rjessup
Fixed CW compiler warnings

Revision 1.4  2003/10/07 18:19:17  rjessup
Up'd the mem heap size

Revision 1.3  2003/10/06 22:57:19  rjessup
Changed the Heap size

Revision 1.2  2003/09/19 23:16:04  rjessup
Moved timer functions to OSAL_Timers and made them dynamic

Revision 1.1  2003/09/19 16:57:14  rjessup
Made the task definitions dynamic
Also, moved user changable items into OSAL_Custom

*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* OSAL_CUSTOM_H */
