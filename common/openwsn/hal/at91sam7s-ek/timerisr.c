 
//  ***************************************************************************** 
//          timerisr.c 
//  
//     Timer 0 Interrupt Service Routine 
// 
//  entered when Timer0 RC compare interrupt asserts (200 msec period) 
//     blinks LED2 (pin PA2) 
// 
//    Author:  James P Lynch     May 12, 2007 
//  *****************************************************************************  
 
#include "AT91SAM7S256.h" 
#include "board.h" 
 
unsigned long  tickcount = 0;         // global variable counts interrupts 
 
 
void Timer0IrqHandler (void) { 
          
  volatile AT91PS_TC    pTC = AT91C_BASE_TC0;    // pointer to timer channel 0 register structure 
  volatile AT91PS_PIO   pPIO = AT91C_BASE_PIOA;    // pointer to PIO register structure 
 unsigned int     dummy;       // temporary 
 
 dummy = pTC->TC_SR;          // read TC0 Status Register to clear interrupt  
 tickcount++;            // increment the tick count 
 
  if  ((pPIO->PIO_ODSR & LED2) == LED2) 
  pPIO->PIO_CODR = LED2;        // turn LED2 (DS2) on   
 else 
  pPIO->PIO_SODR = LED2;        // turn LED2 (DS2) off 
} 