#ifndef _RTL_DISPATCHER_H_8496_
#define _RTL_DISPATCHER_H_8496_

/* rtl_dispatcher
 * this is an C language version of dispatcher object. dispatcher is used to connect
 * multiple objects and transmit events between sender and receiver.
 *
 * dispatcher object is similar to notifier. but it simply route the events to 
 * their corresponding receiver. it won't duplicate the events and sent it to all
 * as notifier does. 
 *
 * @state
 *	finished. not fully tested	
 *
 * @author zhangwei on 200812
 * @modified by zhangwei on 200903
 *	- revision 
 */ 

#include "rtl_configall.h"
#include "rtl_foundation.h"
#include "rtl_vectorx.h"

/* control whether the disp_post() function can be used in the project 
 * you should include "os_scheduler.h" in your project because this module
 * will use some functions in that module, but you cannot include it directly
 * in this one. 
 */
#define CONFIG_DISPA_POST_ENABLE

#define DISPA_HOPESIZE(capacity) (sizeof(TiDispatcher) - sizeof(TiVectorX) + VECTORX_HOPESIZE(capacity))

typedef struct{
	uintx       memsize;
	void *      owner;
	TiVectorX   handlers;
}TiDispatcher;

TiDispatcher *  dispa_create( uintx capacity );
void            dispa_free( TiDispatcher * dpa );
TiDispatcher *  dispa_construct( char * buf, uintx size );
void            dispa_destroy( TiDispatcher * dpa );
bool            dispa_attach( TiDispatcher * dpa, TiEvent * e, TiFunEventHandler handler, void * object );
bool            dispa_detach( TiDispatcher * dpa, TiEvent * e, TiFunEventHandler handler );
void            dispa_send( TiDispatcher * dispa, TiEvent * e );

/* void dispa_post( TiDispatcher * notif, TiEvent e );
 * dispa_post() is similar to dispa_send(), but it's an asynchrous call. it will
 * place the event into the scheduler, and let the scheduler to make the function
 * calls.
 */
#ifdef CONFIG_DISPA_POST_ENABLE
	#define dispa_post(dispa,e,sche) sche_put(sche,dispa_send,dispa,e)
#endif

#endif 


