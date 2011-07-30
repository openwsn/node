
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
  Filename:     hal_digio.c
    
  Description:  HAL digital IO functionality
    
***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_targetboard.h"
#include "hal_cpu.h"
#include "hal_digio.h"

/***********************************************************************************
* LOCAL VARIABLES
*/
static ISR_FUNC_PTR port1_isr_tbl[8] = {0};
static ISR_FUNC_PTR port2_isr_tbl[8] = {0};


/***********************************************************************************
* GLOBAL FUNCTIONS
*/

/***********************************************************************************
* @fn      halDigioConfig
*
* @brief   Configure the pin as specified by p.
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioConfig(const digioConfig* p)
{
/* todo
    register volatile uint8* dir;
    register volatile uint8* out;
    register const uint8 bitmask = p->pin_bm;
    
    // Sanity check
    if ((bitmask == 0) || (bitmask != (uint8)BV(p->pin)))
    {
        return(HAL_DIGIO_ERROR);
    }
    
    switch(p->port)
    {
    case 1: P1SEL &= ~bitmask; out = &P1OUT; dir = &P1DIR; break;
    case 2: P2SEL &= ~bitmask; out = &P2OUT; dir = &P2DIR; break;
    case 3: P3SEL &= ~bitmask; out = &P3OUT; dir = &P3DIR; break;
    case 4: P4SEL &= ~bitmask; out = &P4OUT; dir = &P4DIR; break;
    case 5: P5SEL &= ~bitmask; out = &P5OUT; dir = &P5DIR; break;
    case 6: P6SEL &= ~bitmask; out = &P6OUT; dir = &P6DIR; break;
    case 7: P7SEL &= ~bitmask; out = &P7OUT; dir = &P7DIR; break;
    case 8: P8SEL &= ~bitmask; out = &P8OUT; dir = &P8DIR; break;
    default: return(HAL_DIGIO_ERROR);
    }
    
    if (p->dir == HAL_DIGIO_OUTPUT)
    {
        if (p->initval == 1)
        {
            *out |= bitmask;
        }
        else
        {
            *out &= ~bitmask;
        }
        *dir |= bitmask;
    }
    else // input
    {
        *out &= ~bitmask;
        *dir &= ~bitmask;
    }
    return(HAL_DIGIO_OK);
*/
	return 0;
}


/***********************************************************************************
* @fn      halDigioSet
*
* @brief   Set output pin 
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioSet(const digioConfig* p)
{
/*	todo
    if (p->dir == HAL_DIGIO_OUTPUT)
    {
        switch (p->port)
        {
        case 1: P1OUT |= p->pin_bm; break;
        case 2: P2OUT |= p->pin_bm; break;
        case 3: P3OUT |= p->pin_bm; break;
        case 4: P4OUT |= p->pin_bm; break;
        case 5: P5OUT |= p->pin_bm; break;
        case 6: P6OUT |= p->pin_bm; break;
        case 7: P7OUT |= p->pin_bm; break;
        case 8: P8OUT |= p->pin_bm; break;
        default: return(HAL_DIGIO_ERROR);
        }
        return(HAL_DIGIO_OK);
    }
    return(HAL_DIGIO_ERROR);
*/
	return 0;
}


/***********************************************************************************
* @fn      halDigioClear
*
* @brief   Clear output pin 
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioClear(const digioConfig* p)
{
/*	todo 
    if (p->dir == HAL_DIGIO_OUTPUT)
    {
        switch (p->port)
        {
        case 1: P1OUT &= ~p->pin_bm; break;
        case 2: P2OUT &= ~p->pin_bm; break;
        case 3: P3OUT &= ~p->pin_bm; break;
        case 4: P4OUT &= ~p->pin_bm; break;
        case 5: P5OUT &= ~p->pin_bm; break;
        case 6: P6OUT &= ~p->pin_bm; break;
        case 7: P7OUT &= ~p->pin_bm; break;
        case 8: P8OUT &= ~p->pin_bm; break;
        default: return(HAL_DIGIO_ERROR);
        }
        return(HAL_DIGIO_OK);
    }
    return(HAL_DIGIO_ERROR);
*/
	return 0;
}


/***********************************************************************************
* @fn      halDigioToggle
*
* @brief   Toggle output pin 
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioToggle(const digioConfig* p)
{
/*	todo
    if (p->dir == HAL_DIGIO_OUTPUT)
    {
        switch (p->port)
        {
        case 1: P1OUT ^= p->pin_bm; break;
        case 2: P2OUT ^= p->pin_bm; break;
        case 3: P3OUT ^= p->pin_bm; break;
        case 4: P4OUT ^= p->pin_bm; break;
        case 5: P5OUT ^= p->pin_bm; break;
        case 6: P6OUT ^= p->pin_bm; break;
        case 7: P7OUT ^= p->pin_bm; break;
        case 8: P8OUT ^= p->pin_bm; break;
        default: return(HAL_DIGIO_ERROR);
        }
        return(HAL_DIGIO_OK);
    }
    return(HAL_DIGIO_ERROR);
*/
	return 0;
}


/***********************************************************************************
* @fn      halDigioGet
*
* @brief   Get value on input pin 
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioGet(const digioConfig* p)
{
/* todo
    if (p->dir == HAL_DIGIO_INPUT)
    {
        switch (p->port)
        {
        case 1: return (P1IN & p->pin_bm ? 1 : 0);
        case 2: return (P2IN & p->pin_bm ? 1 : 0);
        case 3: return (P3IN & p->pin_bm ? 1 : 0);
        case 4: return (P4IN & p->pin_bm ? 1 : 0);
        case 5: return (P5IN & p->pin_bm ? 1 : 0);
        case 6: return (P6IN & p->pin_bm ? 1 : 0);
        case 7: return (P7IN & p->pin_bm ? 1 : 0);
        case 8: return (P8IN & p->pin_bm ? 1 : 0);
        default: break;
        }
    }
    return(HAL_DIGIO_ERROR);
*/
	return 0;
}


/***********************************************************************************
* @fn      halDigioIntConnect
*
* @brief   Connect function to IO interrupt
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*          ISR_FUNC_PTR func - pointer to function
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioIntConnect(const digioConfig *p, ISR_FUNC_PTR func)
{
/* todo
    istate_t key;
    HAL_INT_LOCK(key);
    switch (p->port)
    {
    case 1: port1_isr_tbl[p->pin] = func; break;
    case 2: port2_isr_tbl[p->pin] = func; break;
    default: HAL_INT_UNLOCK(key); return(HAL_DIGIO_ERROR);
    }
    halDigioIntClear(p);
    HAL_INT_UNLOCK(key);
    return(HAL_DIGIO_OK);
*/
	return 0;
}


/***********************************************************************************
* @fn      halDigioIntEnable
*
* @brief   Enable interrupt on IO pin
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioIntEnable(const digioConfig *p)
{
/* todo
    switch (p->port)
    {
    case 1: P1IE |= p->pin_bm; break;
    case 2: P2IE |= p->pin_bm; break;
    default: return(HAL_DIGIO_ERROR);
    }
    return(HAL_DIGIO_OK);
*/
	return 0;
}


/***********************************************************************************
* @fn      halDigioIntDisable
*
* @brief   Disable interrupt on IO pin
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioIntDisable(const digioConfig *p)
{
/* todo
    switch (p->port)
    {
    case 1: P1IE &= ~p->pin_bm; break;
    case 2: P2IE &= ~p->pin_bm; break;
    default: return(HAL_DIGIO_ERROR);
    }
    return(HAL_DIGIO_OK);
*/
	return 0;
}


/***********************************************************************************
* @fn      halDigioIntDisable
*
* @brief   Clear interrupt flag
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioIntClear(const digioConfig *p)
{
/* todo
    switch (p->port)
    {
    case 1: P1IFG &= ~p->pin_bm; break;
    case 2: P2IFG &= ~p->pin_bm; break;
    default: return(HAL_DIGIO_ERROR);
    }
    return(HAL_DIGIO_OK);
*/
	return 0;
}


/***********************************************************************************
* @fn      halDigioIntSetEdge
*
* @brief   Set edge for IO interrupt
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*          edge - HAL_DIGIO_INT_FALLING_EDGE or HAL_DIGIO_INT_RISING_EDGE
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioIntSetEdge(const digioConfig *p, uint8 edge)
{
/* todo 
    switch(edge)
    {
    case HAL_DIGIO_INT_FALLING_EDGE:
        switch(p->port)
        {
        case 1: P1IES |= p->pin_bm; break;
        case 2: P2IES |= p->pin_bm; break;
        default: return(HAL_DIGIO_ERROR);
        }
        break;
        
    case HAL_DIGIO_INT_RISING_EDGE:
        switch(p->port)
        {
        case 1: P1IES &= ~p->pin_bm; break;
        case 2: P2IES &= ~p->pin_bm; break;
        default: return(HAL_DIGIO_ERROR);
        }
        break;
        
    default:
        return(HAL_DIGIO_ERROR);
    }
    return(HAL_DIGIO_OK);
*/
	return 0;
}


/***********************************************************************************
* @fn      port1_ISR
*
* @brief   ISR framework for P1 digio interrupt
*
* @param   none
*
* @return  none
*/

// Reference
// Migrating from PIC Microcontrollers to Cortex-M3 Application Note 234
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0234a/index.html

//#pragma vector=PORT1_VECTOR
//__interrupt void port1_ISR(void)
// todo 
// how can I assign the interrupt number?
#pragma arm section
void port1_ISR(void)
{
/*
    register uint8 i;
    if (P1IFG)
    {
        for (i = 0; i < 8; i++)
        {
            register const uint8 pinmask = 1 << i;
            if ((P1IFG & pinmask) && (P1IE & pinmask) && (port1_isr_tbl[i] != 0))
            {
                (*port1_isr_tbl[i])();
                P1IFG &= ~pinmask;
            }
        }
        __low_power_mode_off_on_exit();
    }
*/
}


/***********************************************************************************
* @fn      port2_ISR
*
* @brief   ISR framework for P2 digio interrupt
*
* @param   none
*
* @return  none
*/
//#pragma vector=PORT2_VECTOR
//__interrupt void port2_ISR(void)
// todo 
// how can I assign the interrupt number?
#pragma arm section
void port2_ISR(void)
{
/*
    register uint8 i;
    if (P2IFG)
    {
        for (i = 0; i < 8; i++)
        {
            register const uint8 pinmask = 1 << i;
            if ((P2IFG & pinmask) && (P2IE & pinmask) && (port2_isr_tbl[i] != 0))
            {
                (*port2_isr_tbl[i])();
                P2IFG &= ~pinmask;
            }
        }
        __low_power_mode_off_on_exit();
    }
*/
}

