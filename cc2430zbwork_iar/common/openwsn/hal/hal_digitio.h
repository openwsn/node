#ifndef _MVC_DIGITIO_H_4328_
#define _MVC_DIGITIO_H_4328_
/**
 * mvc_digitio
 * This module controls the digit I/O in the movement controller. 
 *
 * mvc_hal => mvc_digitio
 * 
 * @state
 *  - in developing
 *  - tested by zhaoyang
 /

/* 
 * @author zhangwei(TongJi University) on 2010.09.17
 *  - first created
 * @modified by zhaoyang(TongJi University) on 2010.09.17
 */
#include "mvc_configall.h"
#include "mvc_foundation.h"
#include "mvc_digitio.h"

void digitio_open();
void digitio_close();
void digitio_setvalue( uint8 idx, uint8 value );
uint8 digitio_getvalue( uint8 idx );
void digitio_setbymask( uint8 idx, uint8 mask, uint8 value );

#define digitio_setpin(idx,pin,value) digitio_setbymask((idx),(1<<pin);value)
#define digitio_getpin(idx,pin) digitio_getpin((idx),(1<<pin))

#endif /* _MVC_DIGITIO_H_4328_ */

