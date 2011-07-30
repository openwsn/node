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
#include "avrhardware.h"
#include "project.h"
inline void  HAL_wait_250ns() {
      asm volatile  ("nop" ::);
      asm volatile  ("nop" ::);
}

inline void  HAL_uwait(int u_sec) {
    while (u_sec > 0) {
      asm volatile  ("nop" ::);
      asm volatile  ("nop" ::);
      asm volatile  ("nop" ::);
      asm volatile  ("nop" ::);
      asm volatile  ("nop" ::);
      asm volatile  ("nop" ::);
      asm volatile  ("nop" ::);
      asm volatile  ("nop" ::);
      u_sec--;
    }
}

//HAL_ASSIGN_PIN(ts
HAL_ASSIGN_PIN(RED_LED, A, 2);
HAL_ASSIGN_PIN(GREEN_LED, A, 1);
HAL_ASSIGN_PIN(YELLOW_LED, A, 0);

HAL_ASSIGN_PIN(SERIAL_ID, A, 4);
HAL_ASSIGN_PIN(BAT_MON, A, 5);
HAL_ASSIGN_PIN(THERM_PWR, A, 7);

// ChipCon control assignments

#define HAL_CC_FIFOP_INT SIG_INTERRUPT6
HAL_ASSIGN_PIN(CC_RSTN, A, 6); // chipcon reset
HAL_ASSIGN_PIN(CC_VREN, A, 5); // chipcon power enable
//HAL_ASSIGN_PIN(CC_FIFOP1, D, 7);  // fifo interrupt
HAL_ASSIGN_PIN(CC_FIFOP1, E, 6);  // fifo interrupt

HAL_ASSIGN_PIN(CC_CCA, D, 6);	  // 
HAL_ASSIGN_PIN(CC_SFD, D, 4);	  // chipcon packet arrival
HAL_ASSIGN_PIN(CC_CS, B, 0);	  // chipcon enable
HAL_ASSIGN_PIN(CC_FIFO, B, 7);	  // chipcon fifo

HAL_ASSIGN_PIN(RADIO_CCA, D, 6);	  // 

// Flash assignments
HAL_ASSIGN_PIN(FLASH_SELECT, A, 3);
HAL_ASSIGN_PIN(FLASH_CLK,  D, 5);
HAL_ASSIGN_PIN(FLASH_OUT,  D, 3);
HAL_ASSIGN_PIN(FLASH_IN,  D, 2);

// interrupt assignments
HAL_ASSIGN_PIN(INT0, E, 4);
HAL_ASSIGN_PIN(INT1, E, 5);
HAL_ASSIGN_PIN(INT2, E, 6);
HAL_ASSIGN_PIN(INT3, E, 7);

// spibus assignments 
HAL_ASSIGN_PIN(MOSI,  B, 2);
HAL_ASSIGN_PIN(MISO,  B, 3);
//HAL_ASSIGN_PIN(SPI_OC1C, B, 7);
HAL_ASSIGN_PIN(SPI_SCK,  B, 1);

// power control assignments
HAL_ASSIGN_PIN(PW0, C, 0);
HAL_ASSIGN_PIN(PW1, C, 1);
HAL_ASSIGN_PIN(PW2, C, 2);
HAL_ASSIGN_PIN(PW3, C, 3);
HAL_ASSIGN_PIN(PW4, C, 4);
HAL_ASSIGN_PIN(PW5, C, 5);
HAL_ASSIGN_PIN(PW6, C, 6);
HAL_ASSIGN_PIN(PW7, C, 7);

// i2c bus assignments
HAL_ASSIGN_PIN(I2C_BUS1_SCL, D, 0);
HAL_ASSIGN_PIN(I2C_BUS1_SDA, D, 1);

// uart assignments
HAL_ASSIGN_PIN(UART_RXD0, E, 0);
HAL_ASSIGN_PIN(UART_TXD0, E, 1);
HAL_ASSIGN_PIN(UART_XCK0, E, 2);
	
HAL_ASSIGN_PIN(UART_RXD1, D, 2);
HAL_ASSIGN_PIN(UART_TXD1, D, 3);
HAL_ASSIGN_PIN(UART_XCK1, D, 5);

void HAL_SET_PIN_DIRECTIONS(void)
{
  /*  outp(0x00, DDRA);
  outp(0x00, DDRB);
  outp(0x00, DDRD);
  outp(0x02, DDRE);
  outp(0x02, PORTE);
  */

  HAL_MAKE_RED_LED_OUTPUT();
  HAL_MAKE_YELLOW_LED_OUTPUT();
  HAL_MAKE_GREEN_LED_OUTPUT();

      
  HAL_MAKE_PW7_OUTPUT();
  HAL_MAKE_PW6_OUTPUT();
  HAL_MAKE_PW5_OUTPUT();
  HAL_MAKE_PW4_OUTPUT();
  HAL_MAKE_PW3_OUTPUT(); 
  HAL_MAKE_PW2_OUTPUT();
  HAL_MAKE_PW1_OUTPUT();
  HAL_MAKE_PW0_OUTPUT();

//cc2420 pins  
  HAL_MAKE_MISO_INPUT();
  HAL_MAKE_MOSI_OUTPUT();
  HAL_MAKE_SPI_SCK_OUTPUT();
  HAL_MAKE_CC_RSTN_OUTPUT();    
  HAL_MAKE_CC_VREN_OUTPUT();
  HAL_MAKE_CC_CS_INPUT(); 
  HAL_MAKE_CC_FIFOP1_INPUT();    
  HAL_MAKE_CC_CCA_INPUT();
  HAL_MAKE_CC_SFD_INPUT();
  HAL_MAKE_CC_FIFO_INPUT(); 

  HAL_MAKE_RADIO_CCA_INPUT();



  HAL_MAKE_SERIAL_ID_INPUT();
  HAL_CLR_SERIAL_ID_PIN();  // Prevent sourcing current

  HAL_MAKE_FLASH_SELECT_OUTPUT();
  HAL_MAKE_FLASH_OUT_OUTPUT();
  HAL_MAKE_FLASH_CLK_OUTPUT();
  HAL_SET_FLASH_SELECT_PIN();
    
  HAL_SET_RED_LED_PIN();
  HAL_SET_YELLOW_LED_PIN();
  HAL_SET_GREEN_LED_PIN();
}

//--------------------------------------------------------------
 
inline  int is_host_lsb(void)
{
  const uint8_t n[2] = { 1, 0 };

  return * (uint16_t *)n == 1;
}

 inline uint16_t toLSB16(uint16_t a)
{
  return is_host_lsb() ? a : (a << 8) | (a >> 8);
}


 inline uint16_t fromLSB16(uint16_t a)
{
  return is_host_lsb() ? a : (a << 8) | (a >> 8);
}

//---------------------------------------------------------------------------
 inline uint8_t rcombine(uint8_t  r1, uint8_t  r2)
{
  return r1 == FAIL ? FAIL : r2;
}

 inline  uint8_t rcombine4(uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4)
{
  return rcombine(r1, rcombine(r2, rcombine(r3, r4)));
}
 