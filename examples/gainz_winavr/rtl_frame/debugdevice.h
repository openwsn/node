#ifndef _RTL_IODEVICE_H_6789_
#define _RTL_IODEVICE_H_6789_

#include "../../common/openwsn/rtl/rtl_configall.h"
#include "../../common/openwsn/rtl/rtl_foundation.h"

typedef struct{
    void * data;
}TiDebugIoDevice;

void debug_putchar( TiDebugIoDevice * owner, char c );
char debug_getchar( TiDebugIoDevice * owner );
void debug_assert_report( bool cond, char * file, uint16 line );

#endif
