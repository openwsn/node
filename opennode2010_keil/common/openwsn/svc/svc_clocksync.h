/**
 * svc_clocksync
 * This module implements the clock synchronization service. If this module is accepted
 * in the system, then every nodes in the network will perform clock synchronization
 * according to a pre-defined period. 
 * 
 * This module is based on the real time clock hardware(RTC). It will continue even 
 * the MCU is in sleeping mode. It's mainly used for long term sleeping control.
 * 
 * Clocksync is doing with non-precise long duration walltime synchronizations. If you
 * simply want short duration high resolution syncrhonization, such as synchronization
 * before the measurement, you should use timesync service.
 */
#include "svc_clock.h"




/**
 * ClockSync Protocol:
 * runs on top of MAC protocol.
 * If a node wants to synchronize with a neighbor node, then it broadcast a sync-request 
 * frame to its neighbor nodes, and adjust its own clock by checking the sync-response 
 * frames. Attention on sync-request frame may result multiple sync-response. Attention
 * the communication conflictions in this case. (=>todo)
 * 
 * If a node want to force its neighbor nodes do synchronization with him, then it
 * can broadcast a sync-force frame.
 * 
 * //For developer friendly consideratioins, this module uses TiCalTime structure to
 * //represents the time.
 *
 * 
 * Packet Format
 *
 * clocksync request: [Protocol Id 1B][Control Byte 1B][Sender Clock Time 10B]
 * clocksync response: [Protocol Id 1B][Control Byte 1B][Receive Time 10B][Sending Time 10B]
 *
 * Control Field := [b7...b0]
 * b7...b2: reserved. they must be 0 now
 * b1b0: 00 means sync-request, and 01 means sync-response, 10 means sync-force 
 */


clksync_construct
clksync_destroy
clksync_open( perod, protocol_id )
clksync_close
clksync_evolve
clksync_stop
clksync_pause
clksync_resume
clksync_current
clksync_set/get
clksync_forward
clksync_backward
clksync_adjust

