 /*
 * 下面是定时器的程序
 */

#include "apl_foundation.h"
//#include "openwsn/hal/opennode2010/cm3/core/core_cm3.h"
#include "openwsn/rtl/rtl_debugio.h"
#include "openwsn/hal/hal_timer.h"
#include "openwsn/hal/hal_interrupt.h"
                                   
//#define TEST1
#define TEST2

static TiTimerAdapter g_timer3;
static TiTimerAdapter g_timer2;
static void _timer2_handler(void * object, TiEvent * e);
static void _timer3_handler(void * object, TiEvent * e);

int main( void)
{
    char * msg = "welcome to timer...\r\n";
    TiTimerAdapter * tm3;
    TiTimerAdapter * tm2;
    
    target_init();
    
	led_open();
	led_on( LED_RED );
	hal_delayms( 500 );
	led_off( LED_ALL );
	dbc_mem( msg, strlen(msg) );

    tm3 = timer_construct( (void *)&g_timer3, sizeof(g_timer3) );
    tm2 = timer_construct( (void *)&g_timer2, sizeof(g_timer2) );

    timer_setinterval( tm3, 1000, 0 );
    timer_setinterval( tm2, 1000, 0 );

    timer_setscale( tm2, 7999 );
    timer_setscale( tm3, 7999 );

    tm3 = timer_open( tm3,3,NULL,NULL,0x01);
    tm2 = timer_open( tm2,2,NULL,NULL,0x01);
    
    timer_setprior( tm3,0,0);
    timer_setprior( tm2,0,0);
    hal_attachhandler( INTNUM_TIMER2, _timer2_handler, tm2);
    hal_attachhandler( INTNUM_TIMER3, _timer3_handler, tm3 );

    timer_start( tm3);
    timer_start( tm2);
    
#ifdef TEST1    
    // __disable_irq();
    // __enable_irq();
    //
    hal_enable_interrupts();
    while (1)
    {/*
       if ( timer_expired( tm3))//若想用查询模式则timer->option必须为0x00.
       {
           timer_CLR_IT( tm3);
           led_toggle( LED_RED);
       }*/
    }
#endif

#ifdef TEST2
    while (1)
    {
        //若想用查询模式则timer->option必须为0x00.
       if (timer_expired(tm3))
       {
           timer_CLR_IT(tm3);
           led_toggle(LED_RED);
           dbc_putchar(0xf3);
       }
    }
#endif
}

/*
void TIM3_IRQHandler(void)
{

    //TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    //GPIO_WriteBit(GPIOA, GPIO_Pin_8, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_8)));

    //GPIO_ResetBits( GPIOA,GPIO_Pin_8);
   
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        led_toggle( LED_RED);
    }
    
}

void TIM2_IRQHandler( void)
{
    
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        USART_Send( 0xff);
    }
    
}
*/

static void _timer2_handler(void * object, TiEvent * e)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) 
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        dbc_putchar( 0xf1 );
    }
}
static void _timer3_handler(void * object, TiEvent * e)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        dbc_putchar( 0xf2 );
        led_toggle( LED_RED);
    }
}
