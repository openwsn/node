
hal_clock和hal_rtc应该只保留1个
建议保留hal_clock
他们对外的接口采用TiCalTime，对开发人员友好

还是TiSytemTime呢？利用额外的库函数完成system time(uint64)和caltime的转换呢？
有的硬件只支持systemtime, 如atmega128，有的却支持直接读出caltime

hal_rtc => caltime
hal_clock => system time
hal_systicker => osx kernel 专用 support 32 bit system time, simple and efficient
  implement the osx timer interface required by the osx

TiClockAdapter * clock_construct( char * buf, uint16 size )
{
}

void clock_destroy( TiClockAdapter * clk )
{
}

on_clk_trigger, provided by the application
This function will read the offset value from the 



/* The only TiSysTimer instance. It's used by the OSX kernel only */
TiSysTimer * m_systimer = NULL;


static void _systimer_interrupt_handler( void * object, TiEvent * e );
static inline void _systm_enable( TiSysTimer * timer );
static inline void _systm_disable( TiSysTimer * timer );

