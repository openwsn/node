/**************************************************************************************************
  Filename:       hal_sleep.c
  Revised:        $Date: 2007-11-01 08:44:53 -0700 (Thu, 01 Nov 2007) $
  Revision:       $Revision: 15821 $

  Description:    This module contains the HAL power management procedures for the CC2430.


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

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "hal_types.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_sleep.h"
#include "hal_led.h"
#include "hal_key.h"
#include "mac_api.h"
#include "OSAL.h"
#include "OSAL_Timers.h"
#include "OSAL_Tasks.h"
#include "OSAL_PwrMgr.h"
#include "OnBoard.h"
#include "hal_drivers.h"

#if !defined (RTR_NWK) && defined (NWK_AUTO_POLL)
#include "nwk_globals.h"
#include "ZGlobals.h"
#endif

/* ------------------------------------------------------------------------------------------------
 *                                           Macros
 * ------------------------------------------------------------------------------------------------
 */

/* 32 kHz clock source select in CLKCON */
#if !defined (OSC32K_CRYSTAL_INSTALLED) || (defined (OSC32K_CRYSTAL_INSTALLED) && (OSC32K_CRYSTAL_INSTALLED == TRUE))
#define OSC_32KHZ                        0x00  /* external 32 KHz xosc */
#else
#define OSC_32KHZ                        0x80  /* internal 32 KHz rcosc */
#endif

/* POWER CONSERVATION DEFINITIONS
 * Sleep mode H/W definitions (enabled with POWER_SAVING compile option)
 */
#define CC2430_PM0            0  /* PM0, Clock oscillators on, voltage regulator on */
#define CC2430_PM1            1  /* PM1, 32.768 kHz oscillators on, voltage regulator on */
#define CC2430_PM2            2  /* PM2, 32.768 kHz oscillators on, voltage regulator off */
#define CC2430_PM3            3  /* PM3, All clock oscillators off, voltage regulator off */

/* HAL power management mode is set according to the power management state. The default
 * setting is HAL_SLEEP_OFF. The actual value is tailored to different HW platform. Both
 * HAL_SLEEP_TIMER and HAL_SLEEP_DEEP selections will:
 *   1. turn off the system clock, and
 *   2. halt the MCU.
 * HAL_SLEEP_TIMER can be woken up by sleep timer interrupt, I/O interrupt and reset.
 * HAL_SLEEP_DEEP can be woken up by I/O interrupt and reset.
 */
#define HAL_SLEEP_OFF         CC2430_PM0
#define HAL_SLEEP_TIMER       CC2430_PM2
#define HAL_SLEEP_DEEP        CC2430_PM3

/* MAX_SLEEP_TIME calculation:
 *   Sleep timer maximum duration = 0xFFFF7F / 32768 Hz = 511.996 seconds
 *   Round it to 510 seconds or 510000 ms
 */
#define MAX_SLEEP_TIME                   510000             /* maximum time to sleep allowed by ST */

/* minimum time to sleep, this macro is to:
 * 1. avoid thrashing in-and-out of sleep with short OSAL timer (~2ms)
 * 2. define minimum safe sleep period for different CC2430 revisions
 * AN044 - MINIMUM SLEEP PERIODS WITH PULL-DOWN RESISTOR
 */
#if !defined (PM_MIN_SLEEP_TIME)
#define PM_MIN_SLEEP_TIME                14                 /* default to minimum safe sleep time for CC2430 Rev B */
#endif

/* to disallow PM2/PM3 and use PM1 only set this value to false
 * AN044 - RESTRICT_USE_TO_PM1_ONLY
 */
#if !defined (PM1_ONLY)
#define PM1_ONLY                         FALSE              /* default to no AN044 - RESTRICT USE TO PM1 ONLY */
#endif

/* when a device is waken up be key interrupt, it
 * should stay in PM1 for PM2_HOLDOFF_TIME expecting
 * more key presses. When the timer is expired, the device
 * is allowed to go back to PM2 sleep.
 * AN044 - COMBINING POWER MODES
 */
#if !defined (PM2_HOLDOFF_TIME)
#define PM2_HOLDOFF_TIME                 0
#endif

/* Add code to set external interrupt line to output, drive the line to inactive level,
 * delay for 180us (30us if P1.0 or P1.1), set the line to input as close to PCON.IDLE = 1
 * as possible, and set the line tri-state. The following macro is using S1 key as an example.
 * User should tailor this macro for different interrupt line(s). On CC2430EB or CC2430DB boards,
 * the S1 key is mapped to P0.1. Thus 180us should be used for delays.
 * AN044 - DELAYING EXTERNAL INTERRUPTS
 */
#define EXTERNAL_INTERRUPT_DELAY()          st(P0DIR |= BV(1);    /* set P0.1 output */            \
                                               P0_1 = 1;          /* drive P0.1 inactive (high) */ \
                                               halSleepWait(180); /* delay 180us */                \
                                               P0DIR &= ~BV(1);   /* set P0.1 input */             \
                                               P0INP |= BV(1);)   /* set P0.1 tri-state */

/* This value is used to adjust the sleep timer compare value such that the sleep timer
 * compare takes into account the amount of processing time spent in function halSleep().
 * The first value is determined by measuring the number of sleep timer ticks it from
 * the beginning of the function to entering sleep mode.  The second value is determined
 * by measuring the number of sleep timer ticks from exit of sleep mode to the call to
 * osal_adjust_timers().
 */
#define HAL_SLEEP_ADJ_TICKS   (9 + 25)

#ifndef HAL_SLEEP_DEBUG_POWER_MODE
/* set CC2430 power mode; always use PM2 */
#define HAL_SLEEP_SET_POWER_MODE(mode)      st( if( CHVER <= REV_D )                    \
                                                {                                       \
                                                  HAL_SLEEP_SET_POWER_MODE_REV_D(mode); \
                                                }                                       \
                                                else                                    \
                                                {                                       \
                                                  HAL_SLEEP_SET_POWER_MODE_REV_E(mode); \
                                                }; )
#else
/* Debug: don't set power mode, just block until sleep timer interrupt */
#define HAL_SLEEP_SET_POWER_MODE(mode)      st( while(halSleepInt == FALSE); \
                                                halSleepInt = FALSE; )
#endif

/* for revision E, this sw workaround require additional code in all
 * ISR's that are used to wake up from PM.
 */
#define HAL_SLEEP_SET_POWER_MODE_REV_E(mode) st( SLEEP &= ~0x03;  /* clear mode bits */               \
                                                 SLEEP |= mode;   /* set mode bits   */               \
                                                 asm("NOP");                                          \
                                                 asm("NOP");                                          \
                                                 asm("NOP");                                          \
                                                 if( SLEEP & 0x03 )                                   \
                                                 {                                                    \
                                                   PCON |= 0x01;  /* enable mode */                   \
                                                   asm("NOP");    /* first instruction after sleep*/  \
                                                 }; )

/* for revision D and earlier */
#define HAL_SLEEP_SET_POWER_MODE_REV_D(mode)   st(SLEEP &= ~0x03;  /* clear mode bits */               \
                                                  SLEEP |= mode;   /* set mode bits   */               \
                                                  PCON |= 0x01;    /* enable mode     */               \
                                                  asm("NOP");)     /* first instruction after sleep*/

/* set main clock source to crystal (exit sleep) */
#define HAL_SLEEP_SET_MAIN_CLOCK_CRYSTAL()  st(SLEEP &= ~0x04;          /* turn on both oscs */ \
                                               while(!(SLEEP & 0x40));  /* wait for XOSC */     \
                                               asm("NOP");                                      \
                                               halSleepWait(63);        /* required for Rev B */\
                                               CLKCON = (0x00 | OSC_32KHZ);   /* 32MHx XOSC */  \
                                               while (CLKCON != (0x00 | OSC_32KHZ));            \
                                               SLEEP |= 0x04;)          /* turn off 16MHz RC */

/* set main clock source to RC oscillator (enter sleep) */
#define HAL_SLEEP_SET_MAIN_CLOCK_RC()       st(SLEEP &= ~0x04;          /* turn on both oscs */     \
                                               while(!(SLEEP & 0x20));  /* wait for RC osc */       \
                                               asm("NOP");                                          \
                                               CLKCON = (0x49 | OSC_32KHZ); /* select RC osc */     \
                                               /* wait for requested settings to take effect */     \
                                               while (CLKCON != (0x49 | OSC_32KHZ));                \
                                               SLEEP |= 0x04;)          /* turn off XOSC */

/* sleep and external interrupt port masks */
#define STIE_BV                             BV(5)
#define P0IE_BV                             BV(5)
#define P1IE_BV                             BV(4)
#define P2IE_BV                             BV(1)

/* sleep timer interrupt control */
#define HAL_SLEEP_TIMER_ENABLE_INT()        st(IEN0 |= STIE_BV;)     /* enable sleep timer interrupt */
#define HAL_SLEEP_TIMER_DISABLE_INT()       st(IEN0 &= ~STIE_BV;)    /* disable sleep timer interrupt */
#define HAL_SLEEP_TIMER_CLEAR_INT()         st(IRCON &= ~0x80;)      /* clear sleep interrupt flag */

/* backup interrupt enable registers before sleep */
#define HAL_SLEEP_IE_BACKUP_AND_DISABLE(ien0, ien1, ien2) st(ien0  = IEN0;    /* backup IEN0 register */ \
                                                             ien1  = IEN1;    /* backup IEN1 register */ \
                                                             ien2  = IEN2;    /* backup IEN2 register */ \
                                                             IEN0 &= STIE_BV; /* disable IEN0 except STIE */ \
                                                             IEN1 &= P0IE_BV; /* disable IEN1 except P0IE */ \
                                                             IEN2 &= (P1IE_BV|P2IE_BV);) /* disable IEN2 except P1IE, P2IE */

/* restore interrupt enable registers before sleep */
#define HAL_SLEEP_IE_RESTORE(ien0, ien1, ien2) st(IEN0 = ien0;   /* restore IEN0 register */ \
                                                  IEN1 = ien1;   /* restore IEN1 register */ \
                                                  IEN2 = ien2;)  /* restore IEN2 register */

/* Internal (MCU) Stack addresses. This is to check if the stack is exceeding the disappearing
 * RAM boundary of 0xF000. If the stack does exceed the boundary (unlikely), do not enter sleep
 * until the stack is back to normal.
 */
#define CSTK_PTR _Pragma("segment=\"XSP\"") __segment_begin("XSP")

/* convert msec to 320 usec units with round */
#define HAL_SLEEP_MS_TO_320US(ms)           (((((uint32) (ms)) * 100) + 31) / 32)

/* for optimized indexing of uint32's */
#if HAL_MCU_LITTLE_ENDIAN()
#define UINT32_NDX0   0
#define UINT32_NDX1   1
#define UINT32_NDX2   2
#define UINT32_NDX3   3
#else
#define UINT32_NDX0   3
#define UINT32_NDX1   2
#define UINT32_NDX2   1
#define UINT32_NDX3   0
#endif

/* ------------------------------------------------------------------------------------------------
 *                                        Local Variables
 * ------------------------------------------------------------------------------------------------
 */

/* HAL power management mode is set according to the power management state.
 */
static uint8 halPwrMgtMode = HAL_SLEEP_OFF;

/* stores the sleep timer count upon entering sleep */
static uint32 halSleepTimerStart;

/* stores the accumulated sleep time */
static uint32 halAccumulatedSleepTime;

/* stores the deepest level the device is allowed to sleep
 * See AN044 - COMBINING POWER MODES
 */
static uint8 halSleepLevel = CC2430_PM2;

#ifdef HAL_SLEEP_DEBUG_POWER_MODE
static bool halSleepInt = FALSE;
#endif

/* ------------------------------------------------------------------------------------------------
 *                                      Function Prototypes
 * ------------------------------------------------------------------------------------------------
 */

void halSleepSetTimer(uint32 timeout);
uint32 HalTimerElapsed( void );

/**************************************************************************************************
 * @fn          halSleep
 *
 * @brief       This function is called from the OSAL task loop using and existing OSAL
 *              interface.  It sets the low power mode of the MAC and the CC2430.
 *
 * input parameters
 *
 * @param       osal_timeout - Next OSAL timer timeout.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
void halSleep( uint16 osal_timeout )
{
  uint32        timeout;
  uint32        macTimeout;

  halAccumulatedSleepTime = 0;

  /* get next OSAL timer expiration converted to 320 usec units */
  timeout = HAL_SLEEP_MS_TO_320US(osal_timeout);
  if (timeout == 0)
  {
    timeout = MAC_PwrNextTimeout();
  }
  else
  {
    /* get next MAC timer expiration */
    macTimeout = MAC_PwrNextTimeout();

    /* get lesser of two timeouts */
    if ((macTimeout != 0) && (macTimeout < timeout))
    {
      timeout = macTimeout;
    }
  }

  /* HAL_SLEEP_PM2 is entered only if the timeout is zero and
   * the device is a stimulated device.
   */
  halPwrMgtMode = (timeout == 0) ? HAL_SLEEP_DEEP : HAL_SLEEP_TIMER;

  /* The sleep mode is also controlled by halSleepLevel which
   * defined the deepest level of sleep allowed. This is applied
   * to timer sleep only.
   */
  if ( timeout > 0 && halPwrMgtMode > halSleepLevel )
  {
    halPwrMgtMode = halSleepLevel;
  }

  /* Allow PM1 only.
   * AN044 - RESTRICT USE TO PM1 ONLY
   */
#if defined (PM1_ONLY) && (PM1_ONLY == TRUE)
    if (timeout > 0)
    {
      halPwrMgtMode = CC2430_PM1;
    }
    else
    {
      /* do not allow PM3 either */
      return;
    }
#endif

  /* DEEP sleep can only be entered when zgPollRate == 0.
   * This is to eliminate any possibility of entering PM3 between
   * two network timers.
   */
#if !defined (RTR_NWK) && defined (NWK_AUTO_POLL)
  if ((timeout > HAL_SLEEP_MS_TO_320US(PM_MIN_SLEEP_TIME)) ||
      (timeout == 0 && zgPollRate == 0))
#else
  if ((timeout > HAL_SLEEP_MS_TO_320US(PM_MIN_SLEEP_TIME)) ||
      (timeout == 0))
#endif
  {
    halIntState_t intState, ien0, ien1, ien2;
    HAL_ENTER_CRITICAL_SECTION(intState);

    /* always use "deep sleep" to turn off radio VREG on CC2430 */
    if (MAC_PwrOffReq(MAC_PWR_SLEEP_DEEP) == MAC_SUCCESS)
    {
      while ( (HAL_SLEEP_MS_TO_320US(halAccumulatedSleepTime) < timeout) || (timeout == 0) )
      {
        /* get peripherals ready for sleep */
        HalKeyEnterSleep();

#ifdef HAL_SLEEP_DEBUG_LED
        HAL_TURN_OFF_LED3();
#else
        /* use this to turn LEDs off during sleep */
        HalLedEnterSleep();
#endif

        /* set main clock source to RC oscillator for Rev B and Rev D */
        HAL_SLEEP_SET_MAIN_CLOCK_RC();

        /* enable sleep timer interrupt */
        if (timeout != 0)
        {
          if (timeout > HAL_SLEEP_MS_TO_320US( MAX_SLEEP_TIME ))
          {
            timeout -= HAL_SLEEP_MS_TO_320US( MAX_SLEEP_TIME );
            halSleepSetTimer(HAL_SLEEP_MS_TO_320US( MAX_SLEEP_TIME ));
          }
          else
          {
            /* set sleep timer */
            halSleepSetTimer(timeout);
          }

          /* set up sleep timer interrupt */
          HAL_SLEEP_TIMER_CLEAR_INT();
          HAL_SLEEP_TIMER_ENABLE_INT();
        }

#ifdef HAL_SLEEP_DEBUG_LED
        if (halPwrMgtMode == CC2430_PM1)
        {
          HAL_TURN_ON_LED1();
        }
        else
        {
          HAL_TURN_OFF_LED1();
        }
#endif

        /* save interrupt enable registers and disable all interrupts */
        HAL_SLEEP_IE_BACKUP_AND_DISABLE(ien0, ien1, ien2);

        /* This is to check if the stack is exceeding the disappearing
         * RAM boundary of 0xF000. If the stack does exceed the boundary
         * (unlikely), do not enter sleep until the stack is back to normal.
         */
        if ( ((uint16)(*( __idata uint16*)(CSTK_PTR)) >= 0xF000) )
        {
          HAL_EXIT_CRITICAL_SECTION(intState);

          /* AN044 - DELAYING EXTERNAL INTERRUPTS, do not relocate this line.
           * it has to stay as close to PCON.IDLE = 1 as possible.
           */
          EXTERNAL_INTERRUPT_DELAY();

          /* set CC2430 power mode */
          HAL_SLEEP_SET_POWER_MODE(halPwrMgtMode);
          /* wake up from sleep */

          HAL_ENTER_CRITICAL_SECTION(intState);
        }

        /* restore interrupt enable registers */
        HAL_SLEEP_IE_RESTORE(ien0, ien1, ien2);

        /* disable sleep timer interrupt */
        HAL_SLEEP_TIMER_DISABLE_INT();

        /* set main clock source to crystal for Rev B and Rev D only */
        HAL_SLEEP_SET_MAIN_CLOCK_CRYSTAL();

        /* Calculate timer elasped */
        halAccumulatedSleepTime += (HalTimerElapsed() / TICK_COUNT);

        /* deduct the sleep time for the next iteration */
        if ( osal_timeout > halAccumulatedSleepTime)
        {
          osal_timeout -= halAccumulatedSleepTime;
        }

        /* if the remaining time is less than the PM_MIN_SLEEP_TIME
         * burn the remaining time in a delay loop
         * AN044 - MINIMUM SLEEP PERIODS WITH PULL-DOWN RESISTOR
         */
        if ( osal_timeout < PM_MIN_SLEEP_TIME )
        {
          halSleepWait(osal_timeout*1000);
          halAccumulatedSleepTime += osal_timeout;
          osal_timeout = halAccumulatedSleepTime;
        }

#ifdef HAL_SLEEP_DEBUG_LED
        HAL_TURN_ON_LED3();
#else
        /* use this to turn LEDs back on after sleep */
        HalLedExitSleep();
#endif

        /* handle peripherals; exit loop if key presses */
        if ( HalKeyExitSleep() )
        {
#if defined (PM2_HOLDOFF_TIME) && (PM2_HOLDOFF_TIME > 0)
          /* The deepest sleep alowwed is PM1 until after the timer expired
           * AN044 - COMBINING POWER MODES
           */
          halSleepLevel = CC2430_PM1;
          osal_stop_timerEx (Hal_TaskID, HAL_SLEEP_TIMER_EVENT);
          osal_start_timerEx (Hal_TaskID, HAL_SLEEP_TIMER_EVENT, PM2_HOLDOFF_TIME);
#endif /* (PM2_HOLDOFF_TIME > 0) */
          break;
        }

        /* exit loop if no timer active */
        if ( timeout == 0 ) break;
      }

      /* power on the MAC; blocks until completion */
      MAC_PwrOnReq();

      /* adjust OSAL timers */
      osal_adjust_timers();

    }

    HAL_EXIT_CRITICAL_SECTION(intState);
  }
}

/**************************************************************************************************
 * @fn          halSleepSetTimer
 *
 * @brief       This function sets the CC2430 sleep timer compare value.  First it reads and
 *              stores the value of the sleep timer; this value is used later to update OSAL
 *              timers.  Then the timeout value is converted from 320 usec units to 32 kHz
 *              period units and the compare value is set to the timeout.
 *
 * input parameters
 *
 * @param       timeout - Timeout value in 320 usec units.  The sleep timer compare is set to
 *                        this value.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
void halSleepSetTimer(uint32 timeout)
{
  uint32 ticks;

  /* read the sleep timer; ST0 must be read first */
  ((uint8 *) &ticks)[UINT32_NDX0] = ST0;
  ((uint8 *) &ticks)[UINT32_NDX1] = ST1;
  ((uint8 *) &ticks)[UINT32_NDX2] = ST2;
  ((uint8 *) &ticks)[UINT32_NDX3] = 0;

  /* store value for later */
  halSleepTimerStart = ticks;


  /* Compute sleep timer compare value.  The ratio of 32 kHz ticks to 320 usec ticks
   * is 32768/3125 = 10.48576.  This is nearly 671/64 = 10.484375.
   */
  ticks += (timeout * 671) / 64;

  /* subtract the processing time spent in function halSleep() */
  ticks -= HAL_SLEEP_ADJ_TICKS;

  /* CC2430 Rev. B bug:  compare value must not be set higher than 0xFFFF7F */
  if((ticks & 0xFFFFFF) > 0xFFFF7F)
  {
    ticks = 0xFFFF7F;
  }

  /* set sleep timer compare; ST0 must be written last */
  ST2 = ((uint8 *) &ticks)[UINT32_NDX2];
  ST1 = ((uint8 *) &ticks)[UINT32_NDX1];
  ST0 = ((uint8 *) &ticks)[UINT32_NDX0];
}

/**************************************************************************************************
 * @fn          TimerElapsed
 *
 * @brief       Determine the number of OSAL timer ticks elapsed during sleep.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * None.
 *
 * @return      Number of timer ticks elapsed during sleep.
 **************************************************************************************************
 */
uint32 TimerElapsed( void )
{
  return ( halAccumulatedSleepTime );
}

/**************************************************************************************************
 * @fn          HalTimerElapsed
 *
 * @brief       Determine the number of OSAL timer ticks elapsed during sleep.  This function
 *              relies on OSAL macro TICK_COUNT to be set to 1; then ticks are calculated in
 *              units of msec.  (Setting TICK_COUNT to 1 avoids a costly uint32 divide.)
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * None.
 *
 * @return      Number of timer ticks elapsed during sleep.
 **************************************************************************************************
 */
uint32 HalTimerElapsed( void )
{
  uint32 ticks;

  /* read the sleep timer; ST0 must be read first */
  ((uint8 *) &ticks)[UINT32_NDX0] = ST0;
  ((uint8 *) &ticks)[UINT32_NDX1] = ST1;
  ((uint8 *) &ticks)[UINT32_NDX2] = ST2;

  /* set bit 24 to handle wraparound */
  ((uint8 *) &ticks)[UINT32_NDX3] = 0x01;

  /* calculate elapsed time */
  ticks -= halSleepTimerStart;

  /* add back the processing time spent in function halSleep() */
  ticks += HAL_SLEEP_ADJ_TICKS;

  /* mask off excess if no wraparound */
  ticks &= 0x00FFFFFF;

  /* Convert elapsed time in milliseconds with round.  1000/32768 = 125/4096 */
  return ( ((ticks * 125) + 4095) / 4096 );
}

/**************************************************************************************************
 * @fn          halSleepWait
 *
 * @brief       Perform a blocking wait.
 *
 * input parameters
 *
 * @param       duration - Duration of wait in microseconds.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
void halSleepWait(uint16 duration)
{
  while (duration--)
  {
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
  }
}

/**************************************************************************************************
 * @fn          halRestoreSleepLevel
 *
 * @brief       Restore the deepest timer sleep level.
 *
 * input parameters
 *
 * @param       None
 *
 * output parameters
 *
 *              None.
 *
 * @return      None.
 **************************************************************************************************
 */
void halRestoreSleepLevel( void )
{
  halSleepLevel = CC2430_PM2;
}

/**************************************************************************************************
 * @fn          halSleepTimerIsr
 *
 * @brief       Sleep timer ISR.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
HAL_ISR_FUNCTION(halSleepTimerIsr, ST_VECTOR)
{
  HAL_SLEEP_TIMER_CLEAR_INT();

  if( CHVER >= REV_E )
  {
    CLEAR_SLEEP_MODE();
  }
#ifdef HAL_SLEEP_DEBUG_POWER_MODE
  halSleepInt = TRUE;
#endif
}
