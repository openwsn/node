#include "hal_configall.h"
#include "hal_foundation.h"
#include "../rtl/rtl_time.h"
/**
 * TiClockInterface
 * This interface is used by osx Time Axis Scheduling algorithm and TimeSync service.
 * Any components implementing this interface can be used by them.
 */ 
 
/**
 * Start the clock counting process.
 */
typedef void (*TiFunClockStart)( void * object );

/**
 * Stop the clock counting process.
 */
typedef void (*TiFunClockStop)( void * object );
 
/** 
 * Set the current clock time to the new value.
 * @section attention This value should always success! For efficiency reasons, this
 * function doesn't return anything to indicate failure or success.
 * @return None. 
 */
typedef void (* TiFunClockSetValue)(void * object, TiTime * value);

/**
 * Get current clock time and place it into an TiTime structure.
 */
typedef void (* TiFunClockGetValue)(void * object, TiTime * value);

/**
 * Forward the clock. The time interval is specified by an TiTime structure variable.
 * Attention it should always success. However, there maybe internal counting overflow.
 * @return None. 
 */ 
typedef void (* TiFunClockForward)(void * object, TiTime * value);

/**
 * Backward the clock. The time interval is specified by an TiTime structure variable.
 * Attention it should always success. However, there maybe internal counting underflow.
 * @return None. 
 */ 
typedef void (* TiFunClockBackward)(void * object, TiTime * value);

/**
 * If the callback listener is set, then the clock component will call this listener
 * when expired. "The lisowner" variable will be passed to the callback listener.
 */
typedef void (* TiFunSetListener)(void * object, TiFunEventHandler listener, void * lisowner );
 
/**
 * Set the timing interval. The clock will set expired flag when the timing interval
 * elapsed after the timer is started. And the listerner will be called if it's not NULL.
 *
 * @attentioin The input value of "interval" parameter is important! It's hardware
 * dependent. It should NOT be too small to enable the application have enough time 
 * to finish the execution of the ISR. 
 *
 * @param object It's an pointer to the implementation of TiClockAdapter interface.
 * @param interval Determins the timing duration when the timer expires from clock_start() call. 
 * @param repeat Decides whether the clock should work periodically.
 *      FALSE/0  trigger only once (default)
 *  	TRUE/1   period triggering
 *
 * @return None. Nothing happens even if failed.
 *
 * @warning
 * Be careful about overflow and underflow phenomenon. The component itself should
 * solve these two problems.
 */
typedef void (*TiFunClockSetInterval)( void * object, TiTime * interval, uint8 repeat );

/**
 * Returns how many time slice (decided by the TiTime definition) since last expired.
 * If the clock is always running, then the last expire time equals to start time.
 * 
 * This function can be used for query-driven programs.
 */
typedef bool (* TiFunClockElapsed)( void * object, TiTime * tm );

/**
 * Returns whether this interval setting expired. This function will only return 
 * the internal "expired" flag. Call this function will clear the internal "expired"
 * flag automatically.
 * 
 * This function can be used for query-driven programs.
 */
typedef bool (* TiFunClockExpired)( void * object, TiTime * tm );

typedef struct{
    void * object;
    TiFunClockStart start;
    TiFunClockStop stop;
    TiFunClockSetValue setvalue;
    TiFunClockGetValue getvalue;
    TiFunClockForward forward;
    TiFunClockBackward backward;
    TiFunSetListener setlistener;
    TiFunClockSetInterval setinterval;
    TiFunClockElapsed elapsed;
    TiFunClockExpired expired;
}TiClockInterface;

