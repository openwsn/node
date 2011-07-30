//  ***************************************************************************** 
//          main.c 
//  
//     Demonstration program for Atmel AT91SAM7S256-EK Evaluation Board 
// 
//     blinks LED0 (pin PA0) with an endless loop 
//     blinks LED1 (pin PA1) using timer0 interrupt (200 msec rate) 
//     switch SW1 (PA19) triggers FIQ interrupt, turns on LED2 (Pin PA2) 
//     plenty of variables for debugger practice 
// 
//  Author:  James P Lynch  May 12, 2007 
//  ***************************************************************************** 
 
  
//  ******************************************************* 
//                Header  Files 
//  ******************************************************* 
#include "AT91SAM7S256.h" 
#include "board.h" 
#include "string.h" 
#include "math.h" 
#include "stdlib.h" 
 
 
//  ******************************************************* 
//                Function Prototypes 
//  ******************************************************* 
void Timer0IrqHandler(void); 
void FiqHandler(void); 
 
 
//  ******************************************************* 
//                External References 
//  ******************************************************* 
extern void LowLevelInit(void); 
extern void TimerSetup(void); 
extern unsigned enableIRQ(void); 
extern unsigned enableFIQ(void); 
 
 
//  ******************************************************* 
//               Global Variables 
//  ******************************************************* 
unsigned int  FiqCount = 0;         // global uninitialized variable   
int     q;            // global uninitialized variable 
int     r;            // global uninitialized variable 
int     s;            // global uninitialized variable 
int     m = 2;           // global initialized variable 
int     n = 3;           // global initialized variable 
int     o = 6;           // global initialized variable 
 
struct comms { 
 int   nBytes; 
 char  *pBuf; 
 char  Buffer[32]; 
}  Channel = {5, &Channel.Buffer[0], {"Faster than a speeding bullet"}}; 
 
 
 
//  ***************************************************************************** 
//          main.c 
//  
//     Demonstration program for Atmel AT91SAM7S256-EK Evaluation Board 
// 
//     blinks LED0 (pin PA0) with an endless loop 
//     blinks LED1 (pin PA1) using timer0 interrupt (200 msec rate) 
//     switch SW1 (PA19) triggers FIQ interrupt, turns on LED2 (Pin PA2) 
//     plenty of variables for debugger practice 
// 
//    Author:  James P Lynch  May 12, 2007 
// 
//  ***************************************************************************** 
int  main (void) { 
  
  // lots of variables for debugging practice 
 int     a, b, c;        // uninitialized variables 
 char    d;          // uninitialized variable 
 int     w = 1;         // initialized variable 
 int     k = 2;         // initialized variable 
 static long   x = 5;         // static initialized variable 
 static char   y = 0x04;        // static initialized variable 
  const char   *pText = "The rain in Spain";    // initialized string pointer variable 
  struct EntryLock {          // initialized structure variable 
  long   Key; 
  int    nAccesses; 
  char   Name[17]; 
  }  Access = {14705, 0, "Sophie Marceau"};     
 unsigned long  j;          // loop counter (stack variable) 
  unsigned long  IdleCount = 0;            // idle loop blink counter (2x) 
 int     *p;          // pointer to 32-bit word 
  typedef void   (*FnPtr)(void);          // create a "pointer to function" type 
 FnPtr     pFnPtr;         // pointer to a function 
 double    x5;          // variable to test library function 
 double    y5 = -172.451;       // variable to test library function 
  const char     DigitBuffer[] = "16383";      // variable to test library function 
 long    n;          // variable to test library function 
  
  // Initialize the Atmel AT91SAM7S256 (watchdog, PLL clock, default interrupts, etc.) 
 LowLevelInit(); 

// enable the Timer0 peripheral clock 
volatile AT91PS_PMC  pPMC = AT91C_BASE_PMC;    // pointer to PMC data structure 
pPMC->PMC_PCER = (1<<AT91C_ID_TC0);        // enable Timer0 peripheral clock 
// Set up the LEDs (PA0 - PA3) 
volatile AT91PS_PIO  pPIO = AT91C_BASE_PIOA;  // pointer to PIO data structure 
pPIO->PIO_PER = LED_MASK | SW1_MASK;       // PIO Enable Register - allow PIO to control pins P0 - P3 and pin 19 
pPIO->PIO_OER = LED_MASK;             // PIO Output Enable Register - sets pins P0 - P3 to outputs 
pPIO->PIO_SODR = LED_MASK;            // PIO Set Output Data Register - turns off the four LEDs 
  
// Select PA19 (pushbutton) to be FIQ function (Peripheral B) 
pPIO->PIO_BSR = SW1_MASK; 
  
// Set up the AIC  registers for Timer 0   
volatile AT91PS_AIC  pAIC = AT91C_BASE_AIC;    // pointer to AIC data structure 
pAIC->AIC_IDCR = (1<<AT91C_ID_TC0);        // Disable timer 0 interrupt in AIC Interrupt Disable Command Register   
  
 pAIC->AIC_SVR[AT91C_ID_TC0] =            // Set the TC0 IRQ handler address in AIC Source  
     (unsigned int)Timer0IrqHandler;            // Vector Register[12] 
 pAIC->AIC_SMR[AT91C_ID_TC0] =            // Set the interrupt source type and priority  
      (AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL | 0x4 );  // in AIC Source Mode Register[12] 
pAIC->AIC_ICCR = (1<<AT91C_ID_TC0);        // Clear the TC0 interrupt in AIC Interrupt Clear Command Register 
pAIC->AIC_IDCR = (0<<AT91C_ID_TC0);        // Remove disable timer 0 interrupt in AIC Interrupt Disable Command Reg   
  
pAIC->AIC_IECR = (1<<AT91C_ID_TC0);        // Enable the TC0 interrupt in AIC Interrupt Enable Command Register 
  
// Set up the AIC registers for FIQ (pushbutton SW1) 
pAIC->AIC_IDCR = (1<<AT91C_ID_FIQ);        // Disable FIQ interrupt in AIC Interrupt Disable Command Register     
 pAIC->AIC_SMR[AT91C_ID_FIQ] =            // Set the interrupt source type in AIC Source  
     (AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE);      // Mode Register[0] 
pAIC->AIC_ICCR = (1<<AT91C_ID_FIQ);        // Clear the FIQ interrupt in AIC Interrupt Clear Command Register 
pAIC->AIC_IDCR = (0<<AT91C_ID_FIQ);        // Remove disable FIQ interrupt in AIC Interrupt Disable Command Register   
  
pAIC->AIC_IECR = (1<<AT91C_ID_FIQ);        // Enable the FIQ interrupt in AIC Interrupt Enable Command Register 
  
// Three functions from the libraries 
a = strlen(pText);          // strlen( ) returns length of a string 
 x5 = fabs(y5);           // fabs( ) returns absolute value of a double 
n = atol(DigitBuffer);         // atol( ) converts string to a long 

// Setup timer0 to generate a 10 msec periodic interrupt 
 TimerSetup(); 
// enable interrupts 
 enableIRQ(); 
 enableFIQ(); 
// endless blink loop 
 while (1) { 
  if  ((pPIO->PIO_ODSR & LED1) == LED1)    // read previous state of LED1 
   pPIO->PIO_CODR = LED1;       // turn LED1 (DS1) on  
  else 
   pPIO->PIO_SODR = LED1;       // turn LED1 (DS1) off 
   
  for (j = 1000000; j != 0; j-- );       // wait 1 second 1000000 
  
  IdleCount++;          // count # of times through the idle loop 
  pPIO->PIO_SODR = LED3;        // turn LED3 (DS3) off 
            
  // uncomment following four lines to cause a data abort(3 blink code) 
  //if  (IdleCount >= 10) {           // let it blink 5 times then crash 
  //  p = (int *)0x800000;       // this address doesn't exist 
  //  *p = 1234;         // attempt to write data to invalid address 
  //} 
   
  // uncomment following four lines to cause a prefetch abort (two blinks) 
  //if  (IdleCount >= 10) {           // let it blink 5 times then crash 
  //  pFnPtr = (FnPtr)0x800000;     // this address doesn't exist 
  //  pFnPtr();          // attempt to call a function at a illegal address 
  //} 
 } 
}

  