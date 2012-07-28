/**************************************************************************************************
  Filename:       OSAL_Timers.c
  Revised:        $Date: 2007-10-28 18:43:04 -0700 (Sun, 28 Oct 2007) $
  Revision:       $Revision: 15800 $

  Description:    OSAL Timer definition and manipulation functions.


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

/*********************************************************************
 * INCLUDES
 */


#include "../osal_foundation.h"
#include "../include/ZComDef.h"
#include "OnBoard.h"
#include "../include/OSAL.h"
#include "OSAL_Timers.h"
//#include "hal_timer.h"


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

typedef struct
{
  void *next;
  UINT16 timeout;
  UINT16 event_flag;
  byte task_id;
} osalTimerRec_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

osalTimerRec_t *timerHead;
uint32 tmr_count;          // Amount of time per tick - in micro-sec
uint16 tmr_decr_time;      // Decr_Time for system timer
byte timerActive;          // Flag if hw timer active

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
// Milliseconds since last reboot
static uint32 osal_systemClock;

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */
osalTimerRec_t  *osalAddTimer( byte task_id, UINT16 event_flag, UINT16 timeout );
osalTimerRec_t *osalFindTimer( byte task_id, uint16 event_flag );
void osalDeleteTimer( osalTimerRec_t *rmTimer );
static void osalTimerUpdate( uint16 time );

void osal_timer_activate( byte turn_on );
void osal_timer_hw_setup( byte turn_on );
void osal_set_timer_interrupt( byte turn_on );
void osal_retune_timers( void );

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn      osalTimerInit
 *
 * @brief   Initialization for the OSAL Timer System.
 *
 * @param   none
 *
 * @return
 */
void osalTimerInit( void )
{
  // Initialize the rollover modulo
  tmr_count = TICK_TIME;
  tmr_decr_time = TIMER_DECR_TIME;

  // Initialize the system timer
  osal_timer_activate( false );
  timerActive = false;

  osal_systemClock = 0;
}

/*********************************************************************
 * @fn      osalAddTimer
 *
 * @brief   Add a timer to the timer list.
 *          Ints must be disabled.
 *
 * @param   task_id
 * @param   event_flag
 * @param   timeout
 *
 * @return  osalTimerRec_t * - pointer to newly created timer
 */
osalTimerRec_t * osalAddTimer( byte task_id, UINT16 event_flag, UINT16 timeout )
{
  osalTimerRec_t *newTimer;
  osalTimerRec_t *srchTimer;

  // Look for an existing timer first
  newTimer = osalFindTimer( task_id, event_flag );
  if ( newTimer )
  {
    // Timer is found - update it.
    newTimer->timeout = timeout;

    return ( newTimer );
  }
  else
  {
    // New Timer
    newTimer = osal_mem_alloc( sizeof( osalTimerRec_t ) );

    if ( newTimer )
    {
      // Fill in new timer
      newTimer->task_id = task_id;
      newTimer->event_flag = event_flag;
      newTimer->timeout = timeout;
      newTimer->next = (void *)NULL;

      // Does the timer list already exist
      if ( timerHead == NULL )
      {
        // Start task list
        timerHead = newTimer;
      }
      else
      {
        // Add it to the end of the timer list
        srchTimer = timerHead;

        // Stop at the last record
        while ( srchTimer->next )
          srchTimer = srchTimer->next;

        // Add to the list
        srchTimer->next = newTimer;
      }

      return ( newTimer );
    }
    else
      return ( (osalTimerRec_t *)NULL );
  }
}

/*********************************************************************
 * @fn      osalFindTimer
 *
 * @brief   Find a timer in a timer list.
 *          Ints must be disabled.
 *
 * @param   task_id
 * @param   event_flag
 *
 * @return  osalTimerRec_t *
 */
osalTimerRec_t *osalFindTimer( byte task_id, uint16 event_flag )
{
  osalTimerRec_t *srchTimer;

  // Head of the timer list
  srchTimer = timerHead;

  // Stop when found or at the end
  while ( srchTimer )
  {
    if ( srchTimer->event_flag == event_flag &&
         srchTimer->task_id == task_id )
      break;

    // Not this one, check another
    srchTimer = srchTimer->next;
  }

  return ( srchTimer );
}

/*********************************************************************
 * @fn      osalDeleteTimer
 *
 * @brief   Delete a timer from a timer list.
 *          Ints must be disabled.
 *
 * @param   table
 * @param   rmTimer
 *
 * @return  none
 */
void osalDeleteTimer( osalTimerRec_t *rmTimer )
{
  osalTimerRec_t *srchTimer;

  // Does the timer list really exist
  if ( (timerHead != NULL) && rmTimer )
  {
    // Add it to the end of the timer list
    srchTimer = timerHead;

    // First element?
    if ( srchTimer == rmTimer )
    {
      timerHead = rmTimer->next;
      osal_mem_free( rmTimer );
    }
    else
    {
      // Stop when found or at the end
      while ( srchTimer->next && srchTimer->next != rmTimer )
        srchTimer = srchTimer->next;

      // Found?
      if ( srchTimer->next == rmTimer )
      {
        // Fix pointers
        srchTimer->next = rmTimer->next;

        // Deallocate the timer struct memory
        osal_mem_free( rmTimer );
      }
    }
  }
}

/*********************************************************************
 * @fn      osal_start_timerEx
 *
 * @brief
 *
 *   This function is called to start a timer to expire in n mSecs.
 *   When the timer expires, the calling task will get the specified event.
 *
 * @param   byte taskID - task id to set timer for
 * @param   UINT16 event_id - event to be notified with
 * @param   UNINT16 timeout_value - in milliseconds.
 *
 * @return  ZSUCCESS, or NO_TIMER_AVAIL.
 */
byte osal_start_timerEx( byte taskID, UINT16 event_id, UINT16 timeout_value )
{
  halIntState_t intState;
  osalTimerRec_t *newTimer;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  // Add timer
  newTimer = osalAddTimer( taskID, event_id, timeout_value );
  if ( newTimer )
  {
#ifdef POWER_SAVING
    // Update timer registers
    osal_retune_timers();
    (void)timerActive;
#endif
    // Does the timer need to be started?
    if ( timerActive == FALSE )
    {
      osal_timer_activate( TRUE );
    }
  }

  HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.

  return ( (newTimer != NULL) ? ZSUCCESS : NO_TIMER_AVAIL );
}

/*********************************************************************
 * @fn      osal_stop_timerEx
 *
 * @brief
 *
 *   This function is called to stop a timer that has already been started.
 *   If ZSUCCESS, the function will cancel the timer and prevent the event
 *   associated with the timer from being set for the calling task.
 *
 * @param   byte task_id - task id of timer to stop
 * @param   UINT16 event_id - identifier of the timer that is to be stopped
 *
 * @return  ZSUCCESS or INVALID_EVENT_ID
 */
byte osal_stop_timerEx( byte task_id, UINT16 event_id )
{
  halIntState_t intState;
  osalTimerRec_t *foundTimer;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  // Find the timer to stop
  foundTimer = osalFindTimer( task_id, event_id );
  if ( foundTimer )
  {
    osalDeleteTimer( foundTimer );

#ifdef POWER_SAVING
    osal_retune_timers();
#endif
  }

  HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.

  return ( (foundTimer != NULL) ? ZSUCCESS : INVALID_EVENT_ID );
}

/*********************************************************************
 * @fn      osal_get_timeoutEx
 *
 * @brief
 *
 * @param   byte task_id - task id of timer to check
 * @param   UINT16 event_id - identifier of timer to be checked
 *
 * @return  Return the timer's tick count if found, zero otherwise.
 */
UINT16 osal_get_timeoutEx( byte task_id, UINT16 event_id )
{
  halIntState_t intState;
  uint16 rtrn = 0;
  osalTimerRec_t *tmr;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  tmr = osalFindTimer( task_id, event_id );

  if ( tmr )
  {
    rtrn = tmr->timeout;
  }

  HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.

  return rtrn;
}

/*********************************************************************
 * @fn      osal_timer_activate
 *
 * @brief
 *
 *   Turns the hardware timer on or off
 *
 * @param  byte turn_on - false - turn off, true - turn on
 *
 * @return  none
 */
void osal_timer_activate( byte turn_on )
{
  osal_timer_hw_setup( turn_on );
  timerActive = turn_on;
}

/*********************************************************************
 * @fn      osal_timer_num_active
 *
 * @brief
 *
 *   This function counts the number of active timers.
 *
 * @return  byte - number of timers
 */
byte osal_timer_num_active( void )
{
  halIntState_t intState;
  byte num_timers = 0;
  osalTimerRec_t *srchTimer;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  // Head of the timer list
  srchTimer = timerHead;

  // Count timers in the list
  while ( srchTimer != NULL )
  {
    num_timers++;
    srchTimer = srchTimer->next;
  }

  HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.

  return num_timers;
}

/*********************************************************************
 * @fn      osal_timer_hw_setup
 *
 * @brief
 *
 *   Setup the timer hardware.
 *
 * @param  byte turn_on
 *
 * @return  void
 */
void osal_timer_hw_setup( byte turn_on )
{
  if (turn_on)
  {
    HalTimerStart (OSAL_TIMER, tmr_count );
  }
  else
  {
    HalTimerStop (OSAL_TIMER);
  }
}

#if defined( POWER_SAVING )
/*********************************************************************
 * @fn      osal_sleep_timers
 *
 * @brief
 *
 *   This function will enable interrupts if timers are running.
 *
 * @param  none
 *
 * @return  none
 */
void osal_sleep_timers( void )
{
#ifndef TIMER_INT
  if ( osal_timer_num_active() )
    osal_set_timer_interrupt( TRUE );
#endif
}

/*********************************************************************
 * @fn      osal_unsleep_timers
 *
 * @brief
 *
 *   This function will disable interrupts if timers are running.
 *
 * @param  none
 *
 * @return  none
 */
void osal_unsleep_timers( void )
{
#ifndef TIMER_INT
  osal_set_timer_interrupt( FALSE );
#endif
}
#endif

/*********************************************************************
 * @fn      osal_set_timer_interrupt
 *
 * @brief
 *
 *   Setup the timer hardware interrupt.
 *
 * @param  byte turn_on
 *
 * @return  void
 *********************************************************************/
void osal_set_timer_interrupt( byte turn_on )
{
  // Enable or disable timer interrupts
  HalTimerInterruptEnable ( OSAL_TIMER, HAL_TIMER_CH_MODE_OUTPUT_COMPARE, turn_on);
}

/*********************************************************************
 * @fn      osalTimerUpdate
 *
 * @brief   Update the timer structures for a timer tick.
 *
 * @param   none
 *
 * @return  none
 *********************************************************************/
static void osalTimerUpdate( uint16 updateTime )
{
  halIntState_t intState;
  osalTimerRec_t *srchTimer;
  osalTimerRec_t *prevTimer;
  osalTimerRec_t *saveTimer;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  // Update the system time
  osal_systemClock += updateTime;

  // Look for open timer slot
  if ( timerHead != NULL )
  {
    // Add it to the end of the timer list
    srchTimer = timerHead;
    prevTimer = (void *)NULL;

    // Look for open timer slot
    while ( srchTimer )
    {
      // Decrease the correct amount of time
      if (srchTimer->timeout <= updateTime)
        srchTimer->timeout = 0;
      else
        srchTimer->timeout = srchTimer->timeout - updateTime;

      // When timeout, execute the task
      if ( srchTimer->timeout == 0 )
      {
        osal_set_event( srchTimer->task_id, srchTimer->event_flag );

        // Take out of list
        if ( prevTimer == NULL )
          timerHead = srchTimer->next;
        else
          prevTimer->next = srchTimer->next;

        // Next
        saveTimer = srchTimer->next;

        // Free memory
        osal_mem_free( srchTimer );

        srchTimer = saveTimer;
      }
      else
      {
        // Get next
        prevTimer = srchTimer;
        srchTimer = srchTimer->next;
      }
    }

#ifdef POWER_SAVING
    osal_retune_timers();
#endif
  }

  HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.
}

/*********************************************************************
 * @fn      osal_update_timers
 *
 * @brief   Update the timer structures for timer ticks.
 *
 * @param   none
 *
 * @return  none
 */
void osal_update_timers( void )
{
  osalTimerUpdate( tmr_decr_time );
}

#ifdef POWER_SAVING
/*********************************************************************
 * @fn      osal_adjust_timers
 *
 * @brief   Update the timer structures for elapsed ticks.
 *
 * @param   none
 *
 * @return  none
 *********************************************************************/
void osal_adjust_timers( void )
{
  uint16 eTime;

  if ( timerHead != NULL )
  {
    // Compute elapsed time (msec)
    eTime = TimerElapsed() /  TICK_COUNT;

    if ( eTime )
      osalTimerUpdate( eTime );
  }
}
#endif

#ifdef POWER_SAVING
/*********************************************************************
 * @fn      osal_retune_timers
 *
 * @brief
 *
 *   Adjust CPU sleep time to the lowest timeout value. If the timeout
 *   value is more then RETUNE_THRESHOLD, then the sleep time will be
 *   RETUNE_THRESHOLD.
 *
 * @param   none
 *
 * @return  none
 *********************************************************************/
void osal_retune_timers( void )
{
  halIntState_t intState;
  uint16 nextTimeout;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  // Next occuring timeout
  nextTimeout = osal_next_timeout();

  // Make sure timer counter can handle it
  if ( !nextTimeout || (nextTimeout > RETUNE_THRESHOLD) )
    nextTimeout = RETUNE_THRESHOLD;

  if (nextTimeout != tmr_decr_time)
  {
    // Stop the clock
    osal_timer_activate( FALSE );

    // Alter the rolling time
    tmr_decr_time = nextTimeout;
    tmr_count = (uint32)nextTimeout * TICK_TIME;

    // Restart the clock
    osal_timer_activate( TRUE );
  }

  HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.
}

/*********************************************************************
 * @fn      osal_next_timeout
 *
 * @brief
 *
 *   Search timer table to return the lowest timeout value. If the
 *   timer list is empty, then the returned timeout will be zero.
 *
 * @param   none
 *
 * @return  none
 *********************************************************************/
uint16 osal_next_timeout( void )
{
  uint16 nextTimeout;
  osalTimerRec_t *srchTimer;

  if ( timerHead != NULL )
  {
    // Head of the timer list
    srchTimer = timerHead;
    nextTimeout = OSAL_TIMERS_MAX_TIMEOUT;

    // Look for the next timeout timer
    while ( srchTimer != NULL )
    {
      if (srchTimer->timeout < nextTimeout)
      {
        nextTimeout = srchTimer->timeout;
      }
      // Check next timer
      srchTimer = srchTimer->next;
    }
  }
  else
  {
    // No timers
    nextTimeout = 0;
  }

  return ( nextTimeout );
}
#endif // POWER_SAVING

/*********************************************************************
 * @fn      osal_GetSystemClock()
 *
 * @brief   Read the local system clock.
 *
 * @param   none
 *
 * @return  local clock in milliseconds
 */
uint32 osal_GetSystemClock( void )
{
  return ( osal_systemClock );
}

/*********************************************************************
*********************************************************************/
