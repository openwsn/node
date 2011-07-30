/********************************************************************
* 文 件 名：main.c
* 功 能： 按照IEEE 802.15.4标准组成帧并发送出去
* 说 明： 无
********************************************************************/
#include "top.h"

uint8_t demo_ieee_address[8];
uint16_t shortaddress;
demo_datapacket * demopacket;//具体参见project.h

/********************************************************************
* 名 称：main ()
* 功 能：初始化系统，组包发送
* 入口参数：无
* 出口参数：无	
********************************************************************/

int  main(void)
{
HAL_SET_PIN_DIRECTIONS();
  Leds_greenOn();
  Leds_redOn();
  Leds_yellowOn();
  IoInit();
  cc2420_init();
  hal_atomic_enable_interrupt();//打开全局中断
  cc2420_setchnl(11);//设置信道参数
  cc2420_setrxmode();//打开接收器
  cc2420_enableAddrDecode();//允许地址解析
  cc2420_setpanid(0x0001);//设置PANid
  cc2420_setshortaddress(0x0000);//设置节点的短地址
  cc2420_enablefifop();//打开对fifop的中断支持

  des_address=0x01;
  demopacket=malloc(sizeof(demo_datapacket));//申请内存空间
  demopacket->length=0x0f;//帧长度域
  demopacket->fcf=0x8841;//帧控制域
  demopacket->dsn=0x01;//数据帧序列号
  demopacket->address.destination_pan=0x0001;//目的节点的PANid
  demopacket->address.destination_address=des_address;//目的节点地址
  demopacket->address.source_pan=0x0001;//源节点的PANid
  demopacket->address.source_address=0x0000;//源节点地址
  demopacket->payload=0x0000;//帧负载域
  uint16_t packet_number=0;
  for (uint8_t i=0; i<5000; i++)
 	{
 	  for(uint8_t j=0; j<255; j++)//延时255毫秒
 	  	{
			_delay_loop_2(8*250);
 	  	}
	  packet_number++;
	  printf("第%d个包",packet_number);
	  cc2420_writetxfifo(((demopacket->length)-1), (uint8_t *) demopacket);//开始发送
	  Leds_redToggle();
	  Leds_greenToggle();
	  printf("发送往节点 %d\n",demopacket->address.destination_address);
	  printf("------帧序列号为%d\n\n",demopacket->dsn);
	  demopacket->dsn++;
  	}
}
