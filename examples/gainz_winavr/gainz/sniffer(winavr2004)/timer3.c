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
#include "timer3.h"
#include "generic.h"
#include "led.h"

//-------------------------------------------------------------------------------
inline   void Timer3_intDisable(void)
{
      { hal_atomic_t _nesc_atomic = hal_atomic_start();
       	{
	ETIMSK &= 0xef;
	}
      hal_atomic_end(_nesc_atomic); }
}

 

inline    uint8_t Timer3_fire(void){

  cc2420_writetxfifo(((demopacket->length)-1), (uint8_t *) demopacket);
  printf("send to %04x\n",demopacket->address.destination_address);
 
  while (HAL_READ_CC_SFD_PIN()==1);
    (demopacket->dsn)++;
    demopacket->address.destination_address=(++demopacket->address.destination_address);
  	
  if(demopacket->address.destination_address>MAX_RFD_DEVICE)
   	demopacket->address.destination_address=0x01;
 

  Leds_greenToggle();
  Leds_redToggle();
  Leds_yellowOff();
  
  return SUCCESS;
}

uint8_t Timer3_setIntervalAndScale(uint16_t interval, uint8_t scale)
{

  if (scale > 7) {
    return FAIL;
    }

{ hal_atomic_t _nesc_atomic = hal_atomic_start();
    {
      
	OCR3A = interval;//this is the scale
	OCR3B = 0x0000;
	OCR3C = 0x0000;
	TCNT3 = 0x0000; //this is the counter
	TCCR3A = 0x00;//Output: A: Disconnected, B: Disconnected, C: Disconnected,WGM[1..0]=00
	scale |= 0x08;//
	TCCR3B |= scale;//clk/(scale),WGM[3..2]=01
	TCCR3C = 0x00;//
	ETIMSK |= 0x10;//OCIE3 is enable 
	
    }
    hal_atomic_end(_nesc_atomic); }
  return SUCCESS;
}

void  Timer3_sethalfsymbol(uint16_t symbols)
{  	
 
      uint16_t halfsymbols;
      halfsymbols=symbols*2;
      Timer3_setIntervalAndScale(halfsymbols, 0x5);
     
}

uint16_t Timer3_setInterval(uint16_t interval)
{
      uint32_t temp;
       temp=interval;
 	if (temp>=65535) temp=temp-65535;
	return (temp);
}