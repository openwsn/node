
#include "apl_foundation.h"
#include "../../common/openwsn/hal/opennode2010/hal_led.h"

void RCC_Configuration(void);

int  main (void)
{ 
  
  int i;
  //RCC_Configuration();//现在配置的是48M，如果不调用该函数系统应该在8M下运行。
// SystemInit();

 // led_open();
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8|GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

   GPIO_SetBits( GPIOC, GPIO_Pin_13);


  GPIO_SetBits( GPIOA, GPIO_Pin_0);
  GPIO_SetBits( GPIOA, GPIO_Pin_1);
  GPIO_SetBits( GPIOA, GPIO_Pin_2);
  GPIO_SetBits( GPIOA, GPIO_Pin_3);
  GPIO_SetBits( GPIOA, GPIO_Pin_4);
  GPIO_SetBits( GPIOA, GPIO_Pin_5);
  GPIO_SetBits( GPIOA, GPIO_Pin_6);
  GPIO_SetBits( GPIOA, GPIO_Pin_7);
  GPIO_SetBits( GPIOA, GPIO_Pin_8);
  GPIO_SetBits( GPIOA, GPIO_Pin_9);
  GPIO_SetBits( GPIOA, GPIO_Pin_10);
  GPIO_SetBits( GPIOA, GPIO_Pin_11);
    GPIO_SetBits( GPIOA, GPIO_Pin_12);
    GPIO_SetBits( GPIOA, GPIO_Pin_13);
    GPIO_SetBits( GPIOA, GPIO_Pin_14);

//  GPIO_ResetBits( GPIOA, GPIO_Pin_0);
//  GPIO_ResetBits( GPIOA, GPIO_Pin_1);
//  GPIO_ResetBits( GPIOA, GPIO_Pin_2);
//  GPIO_ResetBits( GPIOA, GPIO_Pin_3);
//  GPIO_ResetBits( GPIOA, GPIO_Pin_4);
//  GPIO_ResetBits( GPIOA, GPIO_Pin_5);
//  GPIO_ResetBits( GPIOA, GPIO_Pin_6);
//  GPIO_ResetBits( GPIOA, GPIO_Pin_7);
//  GPIO_ResetBits( GPIOA, GPIO_Pin_8);
//  GPIO_ResetBits( GPIOA, GPIO_Pin_9);
//  GPIO_ResetBits( GPIOA, GPIO_Pin_10);
//  GPIO_ResetBits( GPIOA, GPIO_Pin_11);
//  GPIO_ResetBits( GPIOA, GPIO_Pin_12);
//  GPIO_ResetBits( GPIOA, GPIO_Pin_13);
//  GPIO_ResetBits( GPIOA, GPIO_Pin_14);


  while( 1) 
  {
	  led_toggle( LED_RED);
	  hal_delay(1000);
  /*
  led_on(LED_RED);//GPIO_SetBits( GPIOA, GPIO_Pin_8);
  hal_delayms( 1000);
 // for(i=0;i<1350000;i++);
  led_off(LED_RED);//GPIO_ResetBits( GPIOA,GPIO_Pin_8);
  hal_delayms(1000);
  */
   
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
		//RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); //PLL必须在其激活前完成配置（设置PLL时钟源及倍频系数）
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

 
 /*

#include "apl_foundation.h"
#include "../../common/openwsn/hal/opennode2010/hal_assert.h"
 
int main()
{
    int i;
	led_open();
    hal_assert(0);
	while(1)
	{
	}
    return 0;
}

  */

