#include "rtl_configall.h"
#include "rtl_foundation.h"


TiFunAssert         g_assert_report = NULL;
void *              g_dbc_io_provider = NULL;
TiFunDebugIoPutChar g_dbc_putchar = NULL;
TiFunDebugIoGetChar g_dbc_getchar = NULL;


void rtl_init( void * io_provider, TiFunDebugIoPutChar debugio_putchar, TiFunDebugIoGetChar debugio_getchar, 
    TiFunAssert assert_report )
{
    g_assert_report = assert_report;
    g_dbc_io_provider = io_provider;
    g_dbc_putchar = debugio_putchar;
    g_dbc_getchar = debugio_getchar;
}

void _rtl_assert_report( bool cond, char * file, uint16 line )
{
    if (g_assert_report)
        g_assert_report( cond, file, line );
}
