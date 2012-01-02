#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_time.h"
/* This module isn't a fully independent module. It's actually part of the "svc_timesync"
 * module to help implement high precision time synchronization protocol. 
 * 
 * This module isn't mandantory to implement the TimeSync service, but it can be 
 * used to improve the rapidity and accurary based on hardware capabilities.
 */
 

/** 
 * An function pointer. Since the TimeSync service uses the system timer, and the 
 * system timer is implemented as a service layer component on top of hal layer, 
 * so I had to leave a function pointer in this module to help get the current 
 * system time.
 */
typedef (void)(TiFunGetCurrentSystemTime)(void * object, TiSystemTime * time );

/**
 * Initialize hardware used by the TimeSync service.
 *
 * @param object Is the system timer object pointer used by the getcurtime function.
 * @param getcurtime An function pointer to the system timer in order to get the 
 *  	current system time.
 */ 
void hal_timesync_init( void * object, TiFunGetCurrentSystemTime getcurtime );

/**
 * Modify the current TimeSync request frame and load the current system time
 * into the frame payload. The application should start transmitting this frame
 * after calling this function as fast as possible.
 */
void hal_timesync_modify_request( TiFrame * frame );

/**
 * This function is called by the transceiver's SFD interrupt request service routine.
 * It's often used to capture the current time to measure the time delay in a single 
 * send/echo round between two nodes.
 * 
 * Attention this function is usually executed inside ISR.
 */
void hal_timesync_on_respones_arrival( TiFrame * frame );

/**
 * Retrieve the request sending time and response arrival time. These two values 
 * are used by the TimeSync service to change local system time.
 *
 * @return false if the response frame hasn't received yet.
 */
bool hal_timesync_retrieve( TiSystemTime * sendtime, TiSystemTime * backtime );

