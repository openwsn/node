/**************************************************************************//**
 * @file     hal_cc2520vx.h
 * @brief    Virtual Execution Layer for cc2520 Transceiver.
 * @version  Ver. 2011.06
 * @date     2011.06.05
 *
 * @note
 * Copyright (C) 2011 OpenWSN Group. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M 
 * processor based microcontrollers.  This file can be freely distributed 
 * within development tools that are supporting such ARM based processors. 
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/

#ifndef _HAL_CC2520VX_H_4839_
#define _HAL_CC2520VX_H_4839_

#include "../hal_configall.h"
#include "../hal_foundation.h"
#include "../hal_assert.h"
#include "../hal_targetboard.h"
#include "../hal_digitio.h"

#include "hal_cc2520base.h"

/**
 * The whole cc2520 adapter/driver adopts layer design:
 * 	- hal_cc2520 provides the interface to upper layers;
 *	- hal_cc2520base contains the fundmental utility functions to operate the cc2520
 *		hardware (formerly known as hal_cc2520midware.h);
 *	- hal_cc2520vx provides the hardware independent functions/macros. The suffix 
 *		"vx" means "virtual execution" here. If you want port cc2520 to other platforms, 
 *      you only need to port the hal_cc2520vx. (formerly known as hal_cc2520depend.h
 *      hal_cc2520base.h, and hal_cc2520hdl.h)
 * 
 * This module is actually an simple encapsulator of the hardware interface between
 * the cc2520 transceiver and the MCU. The interface includes: 
 * 	- an SPI bus transmitting commands and data;
 * 	- six GPIOs functions as SFD, FIFO, FIFOP, etc. 
 * 
 * @attention Currently, we only use FIFOP in the GPIOs in this version.
 */
 
/** 
 * This module is the most low level layer in cc2520 adapter component. Attention 
 * the module depends on MCU's GPIO and SPI interface. It doesn't really depend 
 * on any cc2520 register definitions. So DON'T put any cc2520-depend source code 
 * in this module.
 */
 
void CC2520_ACTIVATE(void);
void CC2520_ENABLE_FIFOP( void);
void CC2520_DISABLE_FIFOP( void);


void CC2520_SPI_OPEN( void);
void CC2520_SPI_CLOSE(void);
void CC2520_SPI_BEGIN( void);
void CC2520_SPI_TX( uint16 ch);
uint16 CC2520_SPI_RX( void);
void CC2520_SPI_WAIT_RXRDY( void);
void CC2520_SPI_END( void); 

inline uint8 CC2520_SPI_TXRX(uint8 x) 
{
    CC2520_SPI_TX(x);
    //CC2520_SPI_WAIT_RXRDY();  // @todo
    return CC2520_SPI_RX();
}






//------------------------------------------------------------------------------
// the following should be re-organized

// todo should be modified
#define MCU_IO_GET(n1,n2) 0
#define MCU_IO_SET(n1,n2,n3) 0


/* MCU Pin Access */
#ifndef MCU_PIN_DIR_OUT
#define MCU_PIN_DIR_OUT(port,bit)   st( P##port##DIR |= BV(bit); )
#endif
#ifndef MCU_PIN_DIR_IN
#define MCU_PIN_DIR_IN(port,bit)    st( P##port##DIR &= ~BV(bit); )
#endif

// todo
#define MCU_PIN_DIR_OUT(port,bit) (0)
#define MCU_PIN_DIR_IN(port,bit) (0)

/* CC2520 I/O Definitions */
/* Basic I/O pin setup */
#define CC2520_BASIC_IO_DIR_INIT()      st( MCU_PIN_DIR_OUT(5,7); MCU_PIN_DIR_OUT(1,0); )

// todo
#define CC2520_BASIC_IO_DIR_INIT() (0)

/* MCU port control for SPI interface */
#define CC2520_DISABLE_SPI_FUNC()       st( P5SEL &= ~(BV(1) | BV(2) | BV(3)); )
#define CC2520_ENABLE_SPI_FUNC()        st( P5SEL |= BV(1) | BV(2) | BV(3); )

// todo
#define CC2520_DISABLE_SPI_FUNC() (0)
#define CC2520_ENABLE_SPI_FUNC()  (0)

/* GPIO pin direction control */
#define CC2520_GPIO_DIR_OUT(pin) \
    st( \
        if (pin == 0) CC2520_GPIO0_DIR_OUT(); \
            if (pin == 1) CC2520_GPIO1_DIR_OUT(); \
                if (pin == 2) CC2520_GPIO2_DIR_OUT(); \
                    if (pin == 3) CC2520_GPIO3_DIR_OUT(); \
                        if (pin == 4) CC2520_GPIO4_DIR_OUT(); \
                            if (pin == 5) CC2520_GPIO5_DIR_OUT(); \
                                )
#define CC2520_GPIO0_DIR_OUT()          MCU_PIN_DIR_IN(1,3)
#define CC2520_GPIO1_DIR_OUT()          MCU_PIN_DIR_IN(1,5)
#define CC2520_GPIO2_DIR_OUT()          MCU_PIN_DIR_IN(1,6)
#define CC2520_GPIO3_DIR_OUT()          MCU_PIN_DIR_IN(1,1)
#define CC2520_GPIO4_DIR_OUT()          MCU_PIN_DIR_IN(1,2)
#define CC2520_GPIO5_DIR_OUT()          MCU_PIN_DIR_IN(1,7)

#define CC2520_GPIO_DIR_IN(pin) \
    st( \
        if (pin == 0) CC2520_GPIO0_DIR_IN(); \
            if (pin == 1) CC2520_GPIO1_DIR_IN(); \
                if (pin == 2) CC2520_GPIO2_DIR_IN(); \
                    if (pin == 3) CC2520_GPIO3_DIR_IN(); \
                        if (pin == 4) CC2520_GPIO4_DIR_IN(); \
                            if (pin == 5) CC2520_GPIO5_DIR_IN(); \
                                )
#define CC2520_GPIO0_DIR_IN()           MCU_PIN_DIR_OUT(1,3)
#define CC2520_GPIO1_DIR_IN()           MCU_PIN_DIR_OUT(1,5)
#define CC2520_GPIO2_DIR_IN()           MCU_PIN_DIR_OUT(1,6)
#define CC2520_GPIO3_DIR_IN()           MCU_PIN_DIR_OUT(1,1)
#define CC2520_GPIO4_DIR_IN()           MCU_PIN_DIR_OUT(1,2)
#define CC2520_GPIO5_DIR_IN()           MCU_PIN_DIR_OUT(1,7)

/* Outputs: Power and reset control */
#define CC2520_RESET_OPIN(v)            MCU_IO_SET(5,7,v)
#define CC2520_VREG_EN_OPIN(v)          MCU_IO_SET(1,0,v)

// Outputs: GPIO
#define CC2520_GPIO0_OPIN(v)			MCU_IO_SET(1,3,v)
#define CC2520_GPIO1_OPIN(v)			MCU_IO_SET(1,5,v)
#define CC2520_GPIO2_OPIN(v)			MCU_IO_SET(1,6,v)
#define CC2520_GPIO3_OPIN(v)			MCU_IO_SET(1,1,v)
#define CC2520_GPIO4_OPIN(v)			MCU_IO_SET(1,2,v)
#define CC2520_GPIO5_OPIN(v)			MCU_IO_SET(1,7,v)

// Outputs: SPI interface
#define CC2520_CSN_OPIN(v)              MCU_IO_SET(5,0,v)
#define CC2520_SCLK_OPIN(v)             MCU_IO_SET(5,3,v)
#define CC2520_MOSI_OPIN(v)             MCU_IO_SET(5,1,v)

// Inputs: GPIO
#define CC2520_GPIO0_IPIN				MCU_IO_GET(1,3)
#define CC2520_GPIO1_IPIN				MCU_IO_GET(1,5)
#define CC2520_GPIO2_IPIN				MCU_IO_GET(1,6)
#define CC2520_GPIO3_IPIN				MCU_IO_GET(1,1)
#define CC2520_GPIO4_IPIN				MCU_IO_GET(1,2)
#define CC2520_GPIO5_IPIN				MCU_IO_GET(1,7)

// Inputs: SPI interface
#define CC2520_MISO_IPIN                MCU_IO_GET(5,2)

// SPI register definitions
#define CC2520_SPI_TX_REG               (UCB1TXBUF)
#define CC2520_SPI_RX_REG               (UCB1RXBUF)
#define CC2520_SPI_RX_IS_READY()        (UC1IFG & UCB1RXIFG)
#define CC2520_SPI_RX_NOT_READY()       (UC1IFG &= ~UCB1RXIFG)

/*todo
// todo
#define CC2520_SPI_TX_REG               (0)
#define CC2520_SPI_RX_REG               (0)
#define CC2520_SPI_RX_IS_READY()        (0)
#define CC2520_SPI_RX_NOT_READY()       (0)

// SPI access macros
#define CC2520_SPI_BEGIN()                 st( CC2520_CSN_OPIN(0); )
#define CC2520_SPI_TX(x)                   st( CC2520_SPI_RX_NOT_READY(); CC2520_SPI_TX_REG = x; )
#define CC2520_SPI_RX()                    (CC2520_SPI_RX_REG)
#define CC2520_SPI_WAIT_RXRDY()            st( while (!CC2520_SPI_RX_IS_READY()); )
#define CC2520_SPI_END()                   st( CC2520_CSN_OPIN(1); )
*/




// Platform specific definitions
// IRQ on GPIO0
#define CC2520_GPIO0_IRQ_INIT()         st( P1IES &= ~0x08; CC2520_GPIO0_IRQ_CLEAR(); )
#define CC2520_GPIO0_IRQ_ENABLE()       st( P1IE  |=  0x08; )
#define CC2520_GPIO0_IRQ_DISABLE()      st( P1IE  &= ~0x08; )
#define CC2520_GPIO0_IRQ_CLEAR()        st( P1IFG &= ~0x08; )

// todo
#define CC2520_GPIO0_IRQ_INIT()         (0)
#define CC2520_GPIO0_IRQ_ENABLE()       (0)
#define CC2520_GPIO0_IRQ_DISABLE()      (0)
#define CC2520_GPIO0_IRQ_CLEAR()        (0)

// todo should be eliminated and adapt to STM32 rather than msp430
//extern const digioConfig pinRadio_GPIO0;
extern digioConfig pinRadio_GPIO0;

/**
 * Initialize the cc2520's virtual execution interface layer.
 * @attention This function should be used by the TiCc2520Adapter component only. 
 * The final user should call cc2520_open() instead of calling this function directly.
 * @param None
 * @return None (But it should always be successful).
 */
void cc2520_dependent_init(void);

#endif /* _HAL_CC2520VX_H_4839_ */
