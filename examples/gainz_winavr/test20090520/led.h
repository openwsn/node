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

#ifndef LED_H
#define LED_H

#include "top.h"
 
	
	//------------------------led
	extern uint8_t Leds_init(void);
	uint8_t Leds_yellowOn(void);
	uint8_t Leds_yellowOff(void);
	uint8_t Leds_yellowToggle(void);
	uint8_t Leds_greenOn(void);
	uint8_t Leds_greenOff(void);
	uint8_t Leds_greenToggle(void);
	uint8_t Leds_redOn(void);
	uint8_t Leds_redOff(void);
	uint8_t Leds_redToggle(void);
 
   
#endif


