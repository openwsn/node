//----------------------------------------------------//
//-------Institute Of  Computing Technology-----------//
//------------Chinese Academic  Science---------------//
//-----中国科学院计算技术研究所先进测试技术实验室-----//
//----------------------------------------------------//

/**
 * www.wsn.net.cn
 * @copyright:nbicc_lpp
 * @data:2005.11.22
 * @version:0.0.1
 * @updata:$
 *
 */
#ifndef _AVRHARDWARE_H
#define _AVRHARDWARE_H

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
//#include "cc2420Const.h"

// check for version 3.3 of GNU gcc or later
#if ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 3))
#define __outw(val, port) outw(port, val);
#endif

#ifndef __inw
#ifndef __SFR_OFFSET
#define __SFR_OFFSET 0
#endif /* !__SFR_OFFSET */
#define __inw(_port) inw(_port)

#define __inw_atomic(__sfrport) ({				\
	uint16_t __t;					\
	bool bStatus;					\
	bStatus = bit_is_set(SREG,7);			\
	cli();						\
	__t = inw(__sfrport);				\
	if (bStatus) sei();				\
	__t;						\
 })

#endif /* __inw */
// cc2420 Interrupt definition
#define cc2420_FIFOP_INT_ENABLE()  sbi(EIMSK , INT6)
#define cc2420_FIFOP_INT_DISABLE() cbi(EIMSK , INT6)
//---------------------
#define HAL_ASSIGN_PIN_list(name, port, bit) \
    void HAL_SET_##name##_PIN(void);  \
    void HAL_CLR_##name##_PIN(void);  \
    int    HAL_READ_##name##_PIN(void);\
    void HAL_MAKE_##name##_OUTPUT(void);  \
    void HAL_MAKE_##name##_INPUT(void) ;
//--------------------------------------------------
#define HAL_ASSIGN_PIN(name, port, bit) \
    void HAL_SET_##name##_PIN(void) {PORT##port |= (1<<(bit));} \
    void HAL_CLR_##name##_PIN(void) {PORT##port &= (~(1<<(bit)));} \
    int HAL_READ_##name##_PIN()							\
  {return (PIN##port & (1 << bit)) != 0;}								\
    void HAL_MAKE_##name##_OUTPUT(void) {DDR##port |= (1<<(bit));} \
    void HAL_MAKE_##name##_INPUT(void) {DDR##port &= (~(1<<(bit)));} 


/* Watchdog Prescaler
 */
enum {
  HAL_period16 = 0x00, // 47ms
  HAL_period32 = 0x01, // 94ms
  HAL_period64 = 0x02, // 0.19s
  HAL_period128 = 0x03, // 0.38s
  HAL_period256 = 0x04, // 0.75s
  HAL_period512 = 0x05, // 1.5s
  HAL_period1024 = 0x06, // 3.0s
  HAL_period2048 = 0x07 // 6.0s
};

//--------------------------------------------------------------------------
// LED assignments
HAL_ASSIGN_PIN_list(RED_LED, A, 2);
HAL_ASSIGN_PIN_list(GREEN_LED, A, 1);
HAL_ASSIGN_PIN_list(YELLOW_LED, A, 0);

HAL_ASSIGN_PIN_list(SERIAL_ID, A, 4);
HAL_ASSIGN_PIN_list(BAT_MON, A, 5);
HAL_ASSIGN_PIN_list(THERM_PWR, A, 7);

// ChipCon control assignments

#define HAL_CC_FIFOP_INT SIG_INTERRUPT6
HAL_ASSIGN_PIN_list(CC_RSTN, A, 6); // chipcon reset
HAL_ASSIGN_PIN_list(CC_VREN, A, 5); // chipcon power enable
//HAL_ASSIGN_PIN_list(CC_FIFOP1, D, 7);  // fifo interrupt
HAL_ASSIGN_PIN_list(CC_FIFOP1, E, 6);  // fifo interrupt

HAL_ASSIGN_PIN_list(CC_CCA, D, 6);	  // 
HAL_ASSIGN_PIN_list(CC_SFD, D, 4);	  // chipcon packet arrival
HAL_ASSIGN_PIN_list(CC_CS, B, 0);	  // chipcon enable
HAL_ASSIGN_PIN_list(CC_FIFO, B, 7);	  // chipcon fifo

HAL_ASSIGN_PIN_list(RADIO_CCA, D, 6);	  // 

// Flash assignments
HAL_ASSIGN_PIN_list(FLASH_SELECT, A, 3);
HAL_ASSIGN_PIN_list(FLASH_CLK,  D, 5);
HAL_ASSIGN_PIN_list(FLASH_OUT,  D, 3);
HAL_ASSIGN_PIN_list(FLASH_IN,  D, 2);

// interrupt assignments
HAL_ASSIGN_PIN_list(INT0, E, 4);
HAL_ASSIGN_PIN_list(INT1, E, 5);
HAL_ASSIGN_PIN_list(INT2, E, 6);
HAL_ASSIGN_PIN_list(INT3, E, 7);

// spibus assignments 
HAL_ASSIGN_PIN_list(MOSI,  B, 2);
HAL_ASSIGN_PIN_list(MISO,  B, 3);
//HAL_ASSIGN_PIN_list(SPI_OC1C, B, 7);
HAL_ASSIGN_PIN_list(SPI_SCK,  B, 1);

// power control assignments
HAL_ASSIGN_PIN_list(PW0, C, 0);
HAL_ASSIGN_PIN_list(PW1, C, 1);
HAL_ASSIGN_PIN_list(PW2, C, 2);
HAL_ASSIGN_PIN_list(PW3, C, 3);
HAL_ASSIGN_PIN_list(PW4, C, 4);
HAL_ASSIGN_PIN_list(PW5, C, 5);
HAL_ASSIGN_PIN_list(PW6, C, 6);
HAL_ASSIGN_PIN_list(PW7, C, 7);

// i2c bus assignments
HAL_ASSIGN_PIN_list(I2C_BUS1_SCL, D, 0);
HAL_ASSIGN_PIN_list(I2C_BUS1_SDA, D, 1);

// uart assignments
HAL_ASSIGN_PIN_list(UART_RXD0, E, 0);
HAL_ASSIGN_PIN_list(UART_TXD0, E, 1);
HAL_ASSIGN_PIN_list(UART_XCK0, E, 2);
	
HAL_ASSIGN_PIN_list(UART_RXD1, D, 2);
HAL_ASSIGN_PIN_list(UART_TXD1, D, 3);
HAL_ASSIGN_PIN_list(UART_XCK1, D, 5);

inline void  HAL_wait_250ns(void);
inline void  HAL_uwait(int u_sec);
 inline 	int is_host_lsb(void);
inline uint16_t toLSB16(uint16_t a);
inline uint16_t fromLSB16(uint16_t a);
inline uint8_t  rcombine(uint8_t  r1, uint8_t  r2);
inline  uint8_t rcombine4(uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4);
 
void HAL_SET_PIN_DIRECTIONS(void);
//---------------------------------------------------------------------------


#define HAL_CYCLE_TIME_NS 136

// each nop is 1 clock cycle
// 1 clock cycle on mica2 == 136ns



enum {
  HAL_ADC_PORTMAPSIZE = 12
};

enum 
{
//  HAL_ACTUAL_CC_RSSI_PORT = 0,
//  HAL_ACTUAL_VOLTAGE_PORT = 7,
  HAL_ACTUAL_BANDGAP_PORT = 30,  // 1.23 Fixed bandgap reference
  HAL_ACTUAL_GND_PORT     = 31   // GND 
};

enum 
{
 // TOS_ADC_CC_RSSI_PORT = 0,
 // TOS_ADC_VOLTAGE_PORT = 7,
  TOS_ADC_BANDGAP_PORT = 10,
  TOS_ADC_GND_PORT     = 11
};

#endif 
