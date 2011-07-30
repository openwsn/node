//----------------------------------------------------//
//-------Institute Of  Computing Technology-----------//
//------------Chinese Academic  Science---------------//
//-----中国科学院计算技术研究所先进测试技术实验室-----//
//----------------------------------------------------//

/**
 * www.wsn.net.cn
 * @copyright:nbicc_lpp
 * @data:2006.11.22
 * @version:0.0.1
 * @updata:$
 *
 */
#include "top.h"
#include "led.h"

//--------------------------------------------------------

demo_datapacket * demopacket;
//--------------------------------------------------------


//---------------------------------------------------------------
inline uint8_t hardwareInit(void)

{
HAL_SET_PIN_DIRECTIONS(S();
  return SUCCESS;
}

uint8_t demo_ieee_address[8];
uint16_t shortaddress;
//---------------------------------------------------------
int   main(void)
{

  hardwareInit();
  Leds_greenOn();
  Leds_redOn();
  Leds_yellowOn();
  IoInit();
  
  HAL_sched_init();  //任务调度初始化
  cc2420_init();
  hal_atomic_enable_interrupt();  //使能全局中断

  cc2420_setchnl(3);  //设置信道
  cc2420_setrxmode();  //打开接收器

  cc2420_enablefifop();  //打开对FIFOP得中断支持

 printf("sniffer is start........\n");
  while(1) {
	HAL_run_task();
  	}
  
}
