#include <stdio.h>
#include "debugdevice.h"
#include "../../common/openwsn/rtl/rtl_debugio.h"

void debug_putchar( TiDebugIoDevice * owner, char c )
{
    //putchar(c);
    printf("%c",c);
}

char debug_getchar( TiDebugIoDevice * owner )
{
    char c;
    sscanf("%c", &c);
    return c;
    //return getchar();
}

void debug_assert_report( bool cond, char * file, uint16 line )
{
	char * msg = "assert: ";
	while (!cond)
	{
		dbc_string( msg );
		dbc_string( file );
		dbc_putchar( '(' );
		dbc_putchar( '0' );
		dbc_putchar( 'x' );
		dbc_n16toa( line );
		dbc_putchar( ')' );
	}
}
