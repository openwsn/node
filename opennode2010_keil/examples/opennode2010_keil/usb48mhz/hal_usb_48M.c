
#include "apl_foundation.h"
#include "../../common/openwsn/hal/opennode2010/hal_led.h"

void RCC_Configuration(void);

int  main (void)
{ 
  
  int i;
  RCC_Configuration();//现在配置的是48M，如果不调用该函数系统应该在8M下运行。

  led_open();
 
  while( 1) 
  {
	  led_toggle( LED_RED);
	  hal_delay(1000);//如果不配置时钟采用默认时钟，led闪的频率应该是1S，由于时钟被配置到48M，所以led闪得频率要快于1S。
  
   
  }
  return 0;
}




void RCC_Configuration(void)
{
	/* system clocks configuration -----------------系统时钟配置-------------------*/
	/* RCC system reset(for debug purpose) */
	RCC_DeInit();                                    //将外设RCC寄存器重设为缺省值
	/* Enable HSE */
	//RCC_HSEConfig(RCC_HSE_ON);                    //开启外部高速晶振（HSE）
	/* Wait till HSE is ready */ 
	//HSEStartUpStatus = RCC_WaitForHSEStartUp();    //等待HSE起振
	//if(HSEStartUpStatus == SUCCESS)               //若成功起振，（下面为系统总线时钟设置）
	//{
		/* Enable Prefetch Buffer */
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //使能FLASH预取指缓存
		/* Flash 2 wait state */
		FLASH_SetLatency(FLASH_Latency_2);   //设置FLASH存储器延时时钟周期数(根据不同的系统时钟选取不同的值)

		/* HCLK = SYSCLK */
		RCC_HCLKConfig(RCC_SYSCLK_Div1);   //设置AHB时钟=72 MHz
		/* PCLK2 = HCLK/2 */
		RCC_PCLK2Config(RCC_HCLK_Div2);   //设置APB1时钟=36 MHz(APB1时钟最大值)
		/* PCLK1 = HCLK/2 */
		RCC_PCLK1Config(RCC_HCLK_Div8);   //设置APB2时钟=72 MHz
		/* Configure ADCCLK such as ADCCLK = PCLK2/2 */
		RCC_ADCCLKConfig(RCC_PCLK2_Div2); //RCC_PCLK2_Div2,4,6,8
		/* PLLCLK = 8MHz * 9 = 72 MHz */
		//RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12); //PLL必须在其激活前完成配置（设置PLL时钟源及倍频系数）
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
		/* Enable PLL */
		RCC_PLLCmd(ENABLE);
		/* Wait till PLL is ready */
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}
		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		/* Wait till PLL is used as system clock source */
		while(RCC_GetSYSCLKSource() != 0x08)
		{
		}   
	//}
}

 
 