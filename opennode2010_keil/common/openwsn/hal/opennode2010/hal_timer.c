
#include "../hal_configall.h"
#include <string.h>
#include "../hal_foundation.h"
#include "../hal_cpu.h"
#include "../hal_mcu.h"
#include "../hal_assert.h"
#include "../hal_timer.h"
#include "../hal_led.h"
#include "../hal_debugio.h"

#ifdef __cplusplus
extern "C" {
#endif

//目前使用较为简单的通用定时器TIMx( x=2、3、4、5)

TiTimerAdapter * timer_construct( char * buf, uint8 size )
{
	hal_assert( sizeof(TiTimerAdapter) <= size );
	memset( buf, 0x00, size );
	return (TiTimerAdapter *)buf;
}


void timer_destroy( TiTimerAdapter * timer )
{
	
}

//todo id 可取 2、3、4（通用定时器编号）//固件库没有提供对TIM5的中断操作函数
//timer_open前应线set_interval;option用来选择是否采用中断，如果采用中断则必须要写中断函数及在函数中清除中断标志位
//否则程序无法运行，若不采用中断则可以通过查询中断标志位TIM_GetITStatus(TIM3, TIM_IT_Update)来判断时间是否到并及时清除中断标志位。
TiTimerAdapter * timer_open( TiTimerAdapter * timer, uint8 id, TiFunEventHandler listener, void * listenowner, uint8 option )
{
    timer->id = id;
	timer->listener = listener;
	timer->listenowner = listenowner;
	timer->option = option;
	return timer;
}

void timer_close( TiTimerAdapter * timer )
{
	 timer_stop( timer);
}


 
void timer_setinterval( TiTimerAdapter * timer, uint16 interval,uint16 prescaler )
{
	timer->interval = interval;
    timer->prescale_factor = prescaler;
}


void timer_start( TiTimerAdapter * timer )//中断临时先采用0级
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    switch( timer->id)
    {
        case 2:
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

            if (timer->option)
            {
                NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
                NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = timer->priority;
                NVIC_InitStructure.NVIC_IRQChannelSubPriority = timer->subpriority; 
                NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
                NVIC_Init(&NVIC_InitStructure);
            }

            TIM_TimeBaseStructure.TIM_Period = timer->interval; 
            TIM_TimeBaseStructure.TIM_Prescaler = timer->prescale_factor;
            TIM_TimeBaseStructure.TIM_ClockDivision = 0;
            TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
            TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
            TIM_ClearFlag(TIM2, TIM_FLAG_Update);/*清除更新标志位*/
            TIM_ARRPreloadConfig(TIM2, DISABLE);/*预装载寄存器的内容被立即传送到影子寄存器 */
            TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);   
            TIM_Cmd(TIM2, ENABLE);
            break;
        case 3:
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

            if ( timer->option)
            {
                NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
                NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = timer->priority;
                NVIC_InitStructure.NVIC_IRQChannelSubPriority = timer->subpriority; 
                NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
                NVIC_Init(&NVIC_InitStructure);
            }

            TIM_TimeBaseStructure.TIM_Period = timer->interval; 
            TIM_TimeBaseStructure.TIM_Prescaler = timer->prescale_factor;
            TIM_TimeBaseStructure.TIM_ClockDivision = 0;
            TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
            TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
            TIM_ClearFlag(TIM3, TIM_FLAG_Update);/*清除更新标志位*/
            TIM_ARRPreloadConfig(TIM3, DISABLE);/*预装载寄存器的内容被立即传送到影子寄存器 */
            TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);   
            TIM_Cmd(TIM3, ENABLE);
            break;
        case 4:
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

            if (timer->option)
            {
                NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
                NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = timer->priority;
                NVIC_InitStructure.NVIC_IRQChannelSubPriority = timer->subpriority; 
                NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
                NVIC_Init(&NVIC_InitStructure);
            }

            TIM_TimeBaseStructure.TIM_Period = timer->interval; 
            TIM_TimeBaseStructure.TIM_Prescaler = timer->prescale_factor;
            TIM_TimeBaseStructure.TIM_ClockDivision = 0;
            TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
            TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
            TIM_ClearFlag(TIM4, TIM_FLAG_Update);/*清除更新标志位*/
            TIM_ARRPreloadConfig(TIM4, DISABLE);/*预装载寄存器的内容被立即传送到影子寄存器 */
            TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);   
            TIM_Cmd(TIM4, ENABLE);
            break;
       /* case 5:
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

            if ( timer->option)
            {
                NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
                NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = timer->priority;
                NVIC_InitStructure.NVIC_IRQChannelSubPriority = timer->subpriority; 
                NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
                NVIC_Init(&NVIC_InitStructure);
            }

            TIM_TimeBaseStructure.TIM_Period = timer->interval; 
            TIM_TimeBaseStructure.TIM_Prescaler = timer->prescale_factor;
            TIM_TimeBaseStructure.TIM_ClockDivision = 0;
            TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
            TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
            TIM_ClearFlag(TIM5, TIM_FLAG_Update);*//*清除更新标志位 
            TIM_ARRPreloadConfig(TIM5, DISABLE);*//*预装载寄存器的内容被立即传送到影子寄存器 
            TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);   
            TIM_Cmd(TIM5, ENABLE);
            break;*/
        default:
            break;
    }
}


void timer_stop( TiTimerAdapter * timer )//这里面还应该加上关终端的函数
{
    NVIC_InitTypeDef NVIC_InitStructure;

	switch( timer->id)
    {
        case 2:
            TIM_Cmd( TIM2,DISABLE);

            NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = timer->priority;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = timer->subpriority; 
            NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
            NVIC_Init(&NVIC_InitStructure);

            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);
            break;
        case 3:
            TIM_Cmd( TIM3,DISABLE);

            NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = timer->priority;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = timer->subpriority; 
            NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
            NVIC_Init(&NVIC_InitStructure);

            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);
            break;
        case 4:
            TIM_Cmd( TIM4,DISABLE);

            NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = timer->priority;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = timer->subpriority; 
            NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
            NVIC_Init(&NVIC_InitStructure);

            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE);
            break;
      /*  case 5:
            TIM_Cmd( TIM5,DISABLE);

            NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = timer->priority;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority =timer->subpriority; 
            NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
            NVIC_Init(&NVIC_InitStructure);

            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, DISABLE);
            break;*/
        default:
            break;
    }
}


void timer_restart( TiTimerAdapter * timer, uint16 interval, uint16 prescale )
{
	timer_setinterval( timer, interval, prescale );
	timer_start( timer );
}



void timer_setlistener( TiTimerAdapter * timer, TiFunEventHandler listener, void * listenowner )
{
	timer->listener = listener;
	timer->listenowner = listenowner;
}

uint8 timer_expired( TiTimerAdapter *timer)//1 time interrupt occur,0 not occur.
{
    uint8 ret;

    switch ( timer->id)
    {
        case 2:
            if ( TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
            {
                ret = 0x01;
            }
            else
            {
                ret = 0x00;
            }
            break;
        case 3:
            if ( TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
            {
                ret = 0x01;
            }
            else
            {
                ret = 0x00;
            }
            break;
        case 4:
            if ( TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
            {
                ret = 0x01;
            }
            else
            {
                ret = 0x00;
            }
            break;
        default:
            if ( TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
            {
                ret = 0x01;
            }
            else
            {
                ret = 0x00;
            }
            break;
    }

    return ret;
}

void timer_CLR_IT( TiTimerAdapter *timer)//clear the interrupt bit
{
    switch( timer->id)
    {
        case 2:
            TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
            break;
        case 3:
            TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
            break;
        case 4:
            TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
            break;
        default:
            TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
            break;
    }
}


void timer_setprior( TiTimerAdapter * timer, uint8 preprior,uint8 subprior )
{
     timer->priority = preprior;
     timer->subpriority = subprior;
}



