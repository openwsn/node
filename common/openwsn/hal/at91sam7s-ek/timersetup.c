Çë²Î¿¼Ô­ÎÄ

// ***************************************************************************** 
//          timersetup.c 
// 
//     Purpose:  Set up the 16-bit Timer/Counter 
// 
//     We will use Timer Channel 0 to develop a 50 msec interrupt. 
// 
//     The AT91SAM7S-EK board has a 18,432,000 hz crystal oscillator. 
// 
//     MAINCK  = 18432000 hz 
//     PLLCK = (MAINCK / DIV) * (MUL + 1) = 18432000/14 * (72 + 1)  
//     PLLCLK = 1316571 * 73 = 96109683 hz 
// 
//     MCK = PLLCLK / 2 = 96109683 / 2 = 48054841 hz 
// 
//     TIMER_CLOCK5 = MCK / 1024  = 48054841 / 1024  =  46928 hz 
// 
//     TIMER_CLOCK5 Period = 1 / 46928  =  21.309239686 microseconds 
// 
//     A little algebra:  .050 sec = count * 21.3092396896*10**-6 
//                        count =  .050 / 21.3092396896*10**-6 
//                        count =  2346 
// 
// 
//     Therefore:  set Timer Channel 0 register RC to 9835 
//                 turn on capture mode WAVE = 0 
//                 enable the clock  CLKEN = 1 
//                 select TIMER_CLOCK5  TCCLKS = 100 
//                 clock is NOT inverted CLKI = 0 
//                 enable RC compare CPCTRG = 1 
//                 enable RC compare interrupt  CPCS = 1 
//                 disable all the other timer 0 interrupts 
// 
// Author:  James P Lynch  May 12, 2007 
// ***************************************************************************** 
 
 
/********************************************************** 
                  Header files 
 **********************************************************/ 
#include "AT91SAM7S256.h" 
#include "board.h" 
 
 
 
 
void TimerSetup(void) { 
  
  //    TC Block Control Register TC_BCR    (read/write) 
 // 
 //  |------------------------------------------------------------------|------| 
  //  |                                                                   SYNC  | 
 //  |------------------------------------------------------------------|------| 
  //   31                                                               1    0 
 //   
  //  SYNC = 0 (no effect)    <===== take  default 
  //  SYNC = 1 (generate software trigger for all 3 timer channels simultaneously) 
 // 
  AT91PS_TCB pTCB = AT91C_BASE_TCB;    // create a pointer to TC Global Register structure 
  pTCB->TCB_BCR = 0;       // SYNC trigger not used 
  
 //    TC Block Mode Register  TC_BMR    (read/write) 
|-------------------------------------|-----------|-----------|-----------| 
//  |                                       TC2XC2S     TCXC1S       TC0XC0S  | 
|-------------------------------------|-----------|-----------|-----------| 
//   31                                     5       4   3       2   1         0 
 
//  TC0XC0S Select = 00 TCLK0 (PA4) 
//                 = 01 none           <===== we select this one                               
//                 = 10 TIOA1 (PA15) 
//                 = 11 TIOA2 (PA26) 
//   
//  TCXC1S  Select = 00 TCLK1 (PA28) 
//                 = 01 none           <===== we select this one                               
//                 = 10 TIOA0 (PA15) 
//                 = 11 TIOA2 (PA26) 
//   
//  TC2XC2S Select = 00 TCLK2 (PA29) 
//                 = 01 none           <===== we select this one                               
//                 = 10 TIOA0 (PA00) 
//                 = 11 TIOA1 (PA26) 
pTCB->TCB_BMR = 0x15;         // external clocks not used   
//    TC Channel Control Register  TC_CCR    (read/write) 
|----------------------------------|--------------|------------|-----------| 
//  |                                      SWTRG          CLKDIS       CLKENS  | 
|----------------------------------|--------------|------------|-----------| 
//   31                                        2             1           0      
 
//  CLKEN = 0    no effect                        
//  CLKEN = 1    enables the clock     <===== we select this one                                          
//                                              
//  CLKDIS = 0   no effect             <===== take  default                        
//  CLKDIS = 1   disables the clock                                               
//                                                  
//  SWTRG = 0    no effect              
//  SWTRG = 1    software trigger aserted counter reset and clock starts   <===== we select this one 
AT91PS_TC pTC = AT91C_BASE_TC0;    // create a pointer to channel 0 Register structure 
pTC->TC_CCR = 0x5;       // enable the clock  and start it 
 // 
 // 
 // 
 // 
 // 
 // 
 // 
 // 
 // 
 // 
 
 
  