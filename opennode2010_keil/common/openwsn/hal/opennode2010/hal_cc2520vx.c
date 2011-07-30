#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_cc2520vx.h"

// todo
#define BIT3 3
// todo end

// todo
// in the previous version, its const
//const digioConfig pinRadio_GPIO0 = {1, 3, BIT3, HAL_DIGIO_INPUT,  0};
digioConfig pinRadio_GPIO0 = {1, 3, BIT3, HAL_DIGIO_INPUT,  0};

static void halRadioSpiInit(uint32 divider);
static void halMcuRfInterfaceInit(void);

/***********************************************************************************
* @fn          halRadioSpiInit
*
* @brief       Initalise Radio SPI interface
*
* @param       none
*
* @return      none
*/
static void halRadioSpiInit(uint32 divider)
{
/* todo
    UCB1CTL1 |= UCSWRST;                          // Put state machine in reset
    UCB1BR0 = LOWORD(divider);
    UCB1BR1 = HIWORD(divider);
    P5DIR |= 0x01;
    P5SEL |= 0x0E;                               // P7.3,2,1 peripheral select (mux to ACSI_A0)
    UCB1CTL1 = UCSSEL0 | UCSSEL1;                // Select SMCLK
    UCB1CTL0 |= UCCKPH | UCSYNC | UCMSB | UCMST; // 3-pin, 8-bit SPI master, rising edge capture
    UCB1CTL1 &= ~UCSWRST;                        // Initialize USCI state machine
*/	
}

/***********************************************************************************
* @fn      halMcuRfInterfaceInit
*
* @brief   Initialises SPI interface to CC2520 and configures reset and vreg
*          signals as MCU outputs.
*
* @param   none
*
* @return  none
*/
static void halMcuRfInterfaceInit(void)
{
    // Initialize the CC2520 interface
    CC2520_SPI_END();
    CC2520_RESET_OPIN(0);
    CC2520_VREG_EN_OPIN(0);
    CC2520_BASIC_IO_DIR_INIT();
}

/**
 * @brief Initialize the cc2520's virtual execution interface layer.
 * @attention This function should be used by the TiCc2520Adapter component only. 
 * The final user should call cc2520_open() instead of calling this function directly.
 * @param None
 * @return None (But it should always be successful).
 */
void cc2520_dependent_init(void)
{
    halRadioSpiInit(0);
    halMcuRfInterfaceInit();
    
    halDigioConfig(&pinRadio_GPIO0);
}
