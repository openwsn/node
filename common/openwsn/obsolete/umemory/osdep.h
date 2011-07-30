/*
 * =====================================================================================
 * 
 *       Filename:  osdep.h
 * 
 *    Description:  Operating System dependent services. 
 * 
 *        Version:  1.0
 *        Created:  6/19/2007 6:31:09 PM Tokyo Standard Time
 *       Revision:  none
 * 
 *         Author:  Jian Zhang , zhjwp@hotmail.com
 *                 
 *        Note(s):  This sample is porting for uCOS-II.
 *                  It can be easily portted to other OS.
 *                  For the uMemory module, only the following primitives are used:
 *                 
 *                  'DISABLE_INTERRUPT', 'ENABLE_INTERRUPT', 'CRITICAL_CREATE',
 *                  'CRITICAL_FREE', 'CRITICAL_ENTER', 'CRITICAL_LEAVE'.
 * 
 * =====================================================================================
 */

#ifndef OS_DEP_H
#define OS_DEP_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "ucos_ii.h"

#define INLINE      __inline

#define HANDLE      void *

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   int   INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */


#define DISABLE_INTERRUPT       HAL_ENTER_CRITICAL()
#define ENABLE_INTERRUPT        HAL_EXIT_CRITICAL()

extern void  *VMEM_Malloc(INT16U size);
extern void VMEM_Free(void *addr);

INLINE void * NEW(int size)
{
    return VMEM_Malloc((INT16U)size);
}

INLINE void FREE(void * buf)
{
    VMEM_Free(buf);
}

INLINE void DELAY(INT32U milli_sec)
{
    OSTimeDlyHMSM(0, 0, 0, milli_sec);
}

INLINE HANDLE CRITICAL_CREATE(INT8S *name)       
{
    return (HANDLE)OSSemCreate(1);
}

INLINE void CRITICAL_FREE(HANDLE lock)
{
    INT8U err;
    if (lock) {
        OSSemDel((OS_EVENT *)lock, OS_DEL_ALWAYS, &err);
    }
}

INLINE void CRITICAL_ENTER(HANDLE lock)
{
    INT8U err;
    if (lock) {
        OSSemPend((OS_EVENT *)lock, 0, &err);
    }
}

INLINE void CRITICAL_LEAVE(HANDLE lock)
{
    if (lock) {
        OSSemPost((OS_EVENT *)lock);
    }
}

INLINE HANDLE EVENT_CREATE(INT8S *name)
{
    return (HANDLE)OSSemCreate(NULL);
}

INLINE void EVENT_FREE(HANDLE event)
{
    INT8U err;
    if (event) {
        OSSemDel((OS_EVENT *)event, OS_DEL_ALWAYS, &err);
    }
}

INLINE void EVENT_WAIT(HANDLE event)
{
    INT8U err;
    OSSemPend ((OS_EVENT *)event, 0, &err);
}

INLINE void EVENT_SET(HANDLE event)
{
	OSSemPost((OS_EVENT *)event);
}

#endif

