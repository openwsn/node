#ifndef _HAL_DEVICE_INTERFACE_H_4892_
#define _HAL_DEVICE_INTERFACE_H_4892_
/*******************************************************************************
 * hal_device.h
 * this file defines the interface of pheriphal device, including bit I/O device, 
 * byte I/O device and block I/O device.
 * 
 * @state
 *  developing
 *
 * @history
 * @author zhangwei(tongji university) on 2010.05.10
 * @modified by zhangwei on 2010.05.10
 *	- revision. support sleep/wakeup/shutdown/startup operation by a single switchtomode() 
 *    function.
 *
 ******************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"

#define DEVICE_MODE_POWERDOWN     0
#define DEVICE_MODE_SLEEP         1
#define DEVICE_MODE_ACTIVE        2

/* TiBitDeviceInterface
 * bit based read/write device interface
 */
struct _TiBitDeviceInterface;

typedef int16  (* TiFunBitDeviceWrite)( struct _TiBitDeviceInterface * dev, char * buf, uint16 len, uint8 option );
typedef int16  (* TiFunBitDeviceRead)( struct _TiBitDeviceInterface * dev, char * buf, uint16 size, uint8 option );
typedef void   (* TiFunBitDeviceEvolve)( struct _TiBitDeviceInterface * dev, TiEvent * e );
typedef void   (* TiFunBitDeviceSwitchToMode)( struct _TiBitDeviceInterface * dev, uint8 mode );

struct _TiBitDeviceInterface{
    // void * provider;
    TiFunBitDeviceWrite write;
    TiFunBitDeviceRead read;
    TiFunBitDeviceEvolve evolve;
    TiFunBitDeviceSwitchToMode switchtomode;
};

#define TiBitDeviceInterface struct _TiBitDeviceInterface


/* TiByteDeviceInterface */

struct _TiByteDeviceInterface;

typedef int16  (* TiFunByteDeviceWrite)( void * dev, char * buf, uint16 len, uint8 option );
typedef int16  (* TiFunByteDeviceRead)( void * dev, char * buf, uint16 size, uint8 option );
typedef void   (* TiFunByteDeviceEvolve)( void * dev, TiEvent * e );
typedef void   (* TiFunByteDeviceSwitchToMode)( void * dev, uint8 mode );

struct _TiByteDeviceInterface{
	void * owner;
    void * provider;
    TiFunByteDeviceWrite write;
    TiFunByteDeviceRead read;
    TiFunByteDeviceEvolve evolve;
    TiFunByteDeviceSwitchToMode switchtomode;
};

#define TiByteDeviceInterface struct _TiByteDeviceInterface

/* TiBlockDeviceInterface
 * block based read/write device interface
 */

struct _TiBlockDeviceInterface;

typedef int16  (* TiFunBlockDeviceWrite)( struct _TiBlockDeviceInterface * dev, char * buf, uint16 len, uint8 option );
typedef int16  (* TiFunBlockDeviceRead)( struct _TiBlockDeviceInterface * dev, char * buf, uint16 size, uint8 option );
typedef void   (* TiFunBlockDeviceEvolve)( struct _TiBlockDeviceInterface * dev, TiEvent * e );
typedef void   (* TiFunBlockDeviceSwitchToMode)( struct _TiBlockDeviceInterface * dev, uint8 mode );

struct _TiBlockDeviceInterface{
	void * owner;
    void * provider;
    TiFunBlockDeviceWrite write;
    TiFunBlockDeviceRead read;
    TiFunBlockDeviceEvolve evolve;
    TiFunBlockDeviceSwitchToMode switchtomode;
};

#define TiBlockDeviceInterface struct _TiBlockDeviceInterface


#endif /* _HAL_DEVICE_INTERFACE_H_4892_ */

