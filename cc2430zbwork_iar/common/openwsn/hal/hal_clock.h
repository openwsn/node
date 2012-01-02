/**
 * Q: What's TiClockAdapter?
 * R: clock adapter is an hardware component adapter helps to implement a clock
 * (usually the system clock). An system clock keeps the time. You can read/write
 * this component to get/set the current time. The time is usually represented as
 * a 64b integer value. 
 * 
 * Q: What's the difference between TiTimerAdapter and TiClockAdapter?
 * R: "timer" is pretty powerful in time measurement, but the duration is usually
 * very short. For example, an 8bit timer can only records from 0-255, and a 16b
 * time is only from 0 to 65535. Furthermore, the timer hardware usually have more
 * functionalities such as "capture", multi-channel snapshot, etc.
 *    The clock is for time measurement only. But it usually uses multi-bytes to
 * represent the current time. The overflow is rarely occured for a clock component
 * because the time variable is much bigger than a timer component.
 * 
 * Q: How to improve the occuracy of a clock?
 * R: Different to the clock software in desktop PC, this clock is more accurate 
 * because it can read out the offset value in one tick. A tick is defined as the
 * interval between two clock counting interrupts. So the current tick count plus
 * the offset is a precise value.
 *
 * Q: How to deal with the time lagging phenomena if the interrupt is disabled?
 * R: Using interrupt request queue, so that the clock component can keep up pace
 * with the real time. Every time the clock interrupt occurs, the ISR pushes an 
 * time increase request into the queue. Through the queue technique, the interrupt
 * request won't lost. 
 * 
 * Attention, the interrupt request queue isn't maintained by the clock component 
 * itself. Since the TiClockAdapter is in "hal" layer, we want it to be simpler
 * and efficient. You can use the system event queue provided in the osx kernel 
 * as such a clock interrupt queue. 
 * 
 * You can register an interupt mode event handler(listerner) to perform the above 
 * "push into queue" operations. By default, the clock itself will increase the 
 * internal tick counter. If you provides the handler(listerner), then the clock
 * component will depend on the listener to increase the tick counter.
 * 
 * Q: What's the draw back of TiClockAdapter?
 * R: Clock is simple and efficient because it uses TiTime64 to represent the time.
 * For Real time clock(RTC), it's usually less efficient because RTC uses TiCalendarTime
 * which is much more complicated. However, a lot of systems uses hardware to implement
 * the RTC, which will be more efficient and energy saving.
 * 
 * Another big difference is that: the TiClockAdapter is affected by the MCU power
 * control. If the MCU goes sleep mode, then the clock will also ceased. and If 
 * the interrupt is disabled, then the clock will also stopped. However, the RTC
 * is usually designed to be executed independently even the MCU is in sleep mode.
 * 
 * Q: What's the difference between TiClockAdapter and TiSysTicker?
 * R: TiSysTicker is an simple component to generate event periodically to drive
 * the osx kernel to run. The TiClockAdapter also implements the osx kernel required 
 * interface and can replace the ticker.
 *    We suggest you replace TiSysTicker if possible because the TiClockAdapter
 * is designed for more accurate applications. A high accuracy clock is much better
 * for timing-restricted applications.
 */ 
 
bool (* TiClockAdapterGetOffset)( void * object, TiSystemTime * tm );

/**
 * TiClockAdapter
 * An efficient, occurate time counting component.
 *
 * interval: time length of one tick
 * curtime: essentially the tick counter.
 *
 * getoffset: This is an function pointer. This function pointer is used to return 
 *      the offset value inside a single piece of slice. Since the lowest resolution
 *      is the length of one slice, it's usually pretty large for high precision
 *      measurement applicatioins. Since the slice counter is increased by the call
 *      of wtm_inputevent() function, the time resolution is decided by the interval
 *      between two calls. This is usually done by the timer hardware's interrupt,
 *      and it's usually not too fast. For example, every 10 ms a interrupt is generated.
 *      So if you want to get the precise time value, you should add the offset inside
 *      one slice. The offset is usually read from the timer hardware. For example, 
 *      the timer_elapsed() function call return how many counts since last expiration.
 * 
 *      If you don't provide this function and the value is null, then the time resolution
 *      is decided by the call of wtm_inputevent() function.
 * 
 * listener: callback function every tick. If you don't provide this listener function,
 *      then the clock will use an internal one, but this one is affected by the 
 *      interrupt flag. If the interrupt is disabled, then this internal ISR will
 *      not run, which lower the accuracy.
 * lisowner: listener's owner object.
 */
typedef struct{
  uint8 state;
  uint32 interval;
  TiSystemTime curtime;
  TiClockAdapterGetOffset getoffset;
  TiFunEventHandler listener;
  void * lisowner;
  uint8 option;
  // TiSystemTime epoch;
}TiClockAdapter;


/** 
 * Q: an example of getoffset function
 * R:
 * bool time0_get_offset( void * object, TiDateTime * value )
 * {
 *   memset( value, 0x00, sizeof(TiDateTime) );
 *   value->offset = time_elapsed( object );
 *   return true;
 * }
 */

TiClockAdapter * clock_construct( char * buf, uint16 size );
void clock_destroy( TiClockAdapter * clk );

/**
 * Initialize an TiClockAdapter component.  
 *
 * @param interval Length of one tick duration. 
 * @param listener If you attach an event handler here, then the clock will be 
 *    push to run by the handler. By default, it's pushed forward by an internal
 *    interrupt handler.
 * @param lisowner An component pointer to the listener's owner.
 */
TiClockAdapter * clock_open( TiClockAdapter * clk, uint16 interval, TiFunEventHandler listener, void * lisowner );
void clock_close( TiClockAdapter * clk );

void clock_curtime( TiClockAdapter * clk, TiSystemTime * curtime );
void clock_setcurtime( TiClockAdapter * clk, TiSystemTime * curtime );
//TiSystemTime * clock_curtimeptr( TiClockAdapter * clk );
//void clock_snapshot( TiClockAdapter * clk, TiSystemTime * curtime );
void clock_forward( TiClockAdapter * clk, uint16 milliseconds );
void clock_backward( TiClockAdapter * clk, uint16 milliseconds );

void clock_offset( TiClockAdapter * clk, TiSystemTime * offset );
void clock_setoffset( TiClockAdapter * clk, TiSystemTime * offset );
void clock_tune( TiClockAdapter * clk, TiSystemTime * delta );






the above is ok

listener
evolve 

clock_read( sec )
clock_write( sec )






void clock_step( void * wtmptr, TiEvent * e );

/**
 * Everytime this function called, the inside timer slice counter will increase. 
 * periodically call this function to push the world timer component to run continuously.
 * 
 * @attention
 * Generally, this function is often executed in interrupt mode. So don't do too
 * much work inside this function in order to keep timing precision.
 */
void clock_inputevent( void * wtmptr, TiEvent * e );

/**
 * If you want the TiClockAdapter component to call the listener at some time, then
 * you should call clock_evolve() to check for that event, because the listener is
 * actually called by the evolve mechanism.
 */
void clock_evolve( TiClockAdapter * clk, TiEvent * e );


在listener中做什么事情：
- clock_forward()
- do user things such as push a message into system event queue, which will activate the main loop


void clock_setlistener( TiClockAdapter * clk, TiFunEventHandler listener, void * lisowner );

/**
 * Set future time. If the current time equals the future time, then clock_expired() will
 * return true. If the listener callback hander is set, then this handler will be 
 * invoked by the evolve() function.
 */

void clock_setfuture( TiClockAdapter * clk, TiSystemTime * future );
void clock_setinterval( TiClockAdapter * clk, TiSystemTime * interval, uint8 option );

/** 
 * Return how many ms elapsed since this interval begins.
 */
void clock_elapsed( TiClockAdapter * clk, TiSystemTime * offset );

bool clock_expired( TiClockAdapter * clk );
void clock_setexpired( TiClockAdapter * clk, bool value );














clk





TiClockAdapter

TiDeviceClock  

clock

sclk 

hardware level: TiTimerAdapter, TiOsxTimerAdapter (formerly known as TiSysTimer)
rtl: rtl_time, rtl_systime, rtl_caltime, rtl_datetime, calendar time, world time
svc: svc_systimer, svc_timer

本来也是可以按照c语言方式用uint64来存储time即可，不用区分date和time
我们命名为datetime其实不好，应该改
对sensor而言，是不care具体的date time的，它无所谓day night，只要精度高即可
如果区分，都是在sink上实现的。

在MCU上，不妨就用一个整数，但是为了扩展考虑，可以后扩展，故又利用了b1位

Ti

TiTime
time的二进制存储
2B flag, extend flag, value 2^14 year
不好不好
要尽可能简单
只分2 segments or extended segments

based on ms
from application started --- as origin 0
the device doesn't  care year/month/day or more

the system timer also implement the TiOsxTimer interface, so it can be used 
to drive the osx kernel

typedef char[10] TiSystemTime;

first 6 byte is major 
last 4 byte is minor
and last is micro


typedef uint32 version_t;
#define TiVersion version_t

#define MAKE_VERSION(major,minor,micro) (((major&0x0FF)<<16) | ((minor&0xFF)<<8) | (micro&0xFF))
#define MAJOR_VERSION(ver) ((ver>>16) & 0xFF)
#define MINOR_VERSION(ver) ((ver>>8) & 0xFF)
#define MICRO_VERSION(ver) (ver&0xFF)

time64_majorptr( based on seconds )   6B
time64_minorptr( based on millseconds )  2B

time80_add
time80_sub
time80_majorptr( based on seconds )   6B
time80_minorptr( based on millseconds )  2B
time80_microptr( based on microseconds ) 2B
time80_

typedef uint32 TiTime32

typedef struct{
    unsigned char[6] major;
    uint16 minor;
}TiTime64;


typedef struct{
    unsigned char[6] major;
    uint16 minor;
    uint16 micro;
}TiTime80;

time80_pack
time80_unpack
snapshot

    

systime_clear
systime_set()   
systime_get
systime_sub
systime_add
systime_major
systime_minor
systime_plus
systime_major
systime_minor

systimer_cease/sleepfor(ms)

#define TiSystemTime TiTime80

TiTime80 可以支持用micro支持到us
TiSystemTimer类用于计时

还是把这个类放到hal吧，以充分利用硬件能力提高性能

TiCountTimer 
TiClock
TiClockAdapter


systimer

TiTimer


High Resolution Time

/**
 * TiWorldTimer
 * This component is used to record the current date time. It's based on an physical
 * timer or physical real time clock(RTC). It can:
 * - Can records the precision time, even including the nano-seconds or more.
 * - Can records very long time duration. Currently, the time origin is 2000.01.01 00:00:00:0000
 * - The world timer is based on hardware timer (through TiTimerAdapter) or hardware 
 *   RTC (through TiRtcAdapter) depends on specified target hardware. So the implementation
 *   may various depends on the hardware. Remember to access this object through it's
 *   interface functions intead of accessing the structure variable directly.
 * 
 * The TiWorldTimer itself has an internal time counter. This counter will increase
 * when it receives an event. So generally, you should configure an TiTimerAdapter
 * or TiRtcAdapter to generate these events to drive the TiWorldTimer to run. Since
 * it's based on them, it's more easily to port to other platforms.
 * 
 * For efficiency and resolution reasons, this component uses TiDateTime to record 
 * the time instead of the inefficient TiCalendarTime. 
 * 
 * Attention currently, the TiDateTime occupies 10 bytes so that it's capable to 
 * records even the micro seconds (us). It's useful in high precision measurement 
 * applications. The TiDateTime can also expand to record nano seconds, but the 
 * current implementation doesn't support ns and ps by default.
 */

bool (* TiWorldTimerGetOffset)( void * object, char * TiDateTime );

/**
 * getoffset: This is an function pointer. This function pointer is used to return 
 *      the offset value inside a single piece of slice. Since the lowest resolution
 *      is the length of one slice, it's usually pretty large for high precision
 *      measurement applicatioins. Since the slice counter is increased by the call
 *      of wtm_inputevent() function, the time resolution is decided by the interval
 *      between two calls. This is usually done by the timer hardware's interrupt,
 *      and it's usually not too fast. For example, every 10 ms a interrupt is generated.
 *      So if you want to get the precise time value, you should add the offset inside
 *      one slice. The offset is usually read from the timer hardware. For example, 
 *      the timer_elapsed() function call return how many counts since last expiration.
 * 
 *      If you don't provide this function and the value is null, then the time resolution
 *      is decided by the call of wtm_inputevent() function.
 */



typedef struct{
  uint8 state;
  TiDateTime curtime;

  TiWorldTimerGetOffset getoffset;
  uint16 interval;
  uint16 counter0;
  uint16 counter1;
  int16 counter0delta;

  TiDateTime futuretime;
  uint8 expired;
  TiFunEventHandler listener;
  void * lisowner;
  uint8 option;
}TiWorldTimer;

/** 
 * Q: an example of getoffset function
 * R:
 * bool time0_get_offset( void * object, TiDateTime * value )
 * {
 *   memset( value, 0x00, sizeof(TiDateTime) );
 *   value->offset = time_elapsed( object );
 *   return true;
 * }
 */

TiWorldTimer * wtm_construct( char * buf, uint16 size );
void wtm_destroy( TiWorldTimer * wtm );

on_clk_trigger, provided by the application
This function will read the offset value from the 

/**
 * wtm_open()
 * Initialize an TiWorldTimer component. Attention, you should attach the wtm_inputevent()
 * and an hardware timer to really push the world timer component to run.
 *
 * @param slice_length Length of one time slice. Generally based on milliseconds.
 * 
 */
TiWorldTimer * wtm_open( TiWorldTimer * wtm, uint16 slice_length, TiFunEventHandler listener, void * lisowner );
void wtm_destroy( TiWorldTimer * wtm );

/**
 * Everytime this function called, the inside timer slice counter will increase. 
 * periodically call this function to push the world timer component to run continuously.
 * 
 * @attention
 * Generally, this function is often executed in interrupt mode. So don't do too
 * much work inside this function in order to keep timing precision.
 */
void wtm_inputevent( void * wtmptr, TiEvent * e );

/**
 * If you want the TiWorldTimer component to call the listener at some time, then
 * you should call wtm_evolve() to check for that event, because the listener is
 * actually called by the evolve mechanism.
 */
void wtm_evolve( TiWorldTimer * wtm, TiEvent * e );


void wtm_forward( TiWorldTimer * wtm, uint16 milliseconds );
void wtm_backward( TiWorldTimer * wtm, uint16 milliseconds );

void wtm_curtime( TiWorldTimer * wtm, , TiDateTime * curtime );
void wtm_setcurtime( TiWorldTimer * wtm, , TiDateTime * curtime );
TiDateTime * wtm_curtimeptr( TiWorldTimer * wtm );
void wtm_snapshot( TiWorldTimer * wtm, TiDateTime * curtime );

void wtm_setoffset( TiWorldTimer * wtm, TiDateTime * offset );
void wtm_tune( TiWorldTimer * wtm, TiDateTime * delta );

void wtm_setlistener( TiWorldTimer * wtm, TiFunEventHandler listener, void * lisowner );

/**
 * Set future time. If the current time equals the future time, then wtm_expired() will
 * return true. If the listener callback hander is set, then this handler will be 
 * invoked by the evolve() function.
 */

void wtm_setfuture( TiWorldTimer * wtm, TiDateTime * future );
void wtm_setinterval( TiWorldTimer * wtm, TiDateTime * interval, uint8 option );

/** 
 * Return how many ms elapsed since this interval begins.
 */
void wtm_elapsed( TiWorldTimer * wtm, TiDateTime * offset );

bool wtm_expired( TiWorldTimer * wtm );
void wtm_setexpired( TiWorldTimer * wtm, bool value );


