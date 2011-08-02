#include "rtl_configall.h"
#include <string.h>
#include "rtl_foundation.h"
#include "rtl_dataexchange.h"
#include "rtl_variant.h"
#include "rtl_varsequ.h"

// todo
uint16 dxc_pack( uint8 ctrl, uint8 flowno, char * timepoint, TiXtpVarSequence * varsequ, char * buf, uint16 size )
{
/*    char * pc = buf;
    uint16 count=0;
    uint8 i;

    *pc++ = ctrl;
    *pc++ = flowno;
    count = 2;

    for (i=0; i<10; i++)
        *pc++ = (*timepoint++);
    count += 10;

    count += xtp_varsequ_pack( varsequ, pc, size-count );
    return count;*/
}

// todo
uint16 dxc_unpack( uint8 * ctrl, uint8 * flownumber, char * timepoint, TiXtpVarSequence * varsequ, char * buf, uint16 len )
{
	    uint16 count=0;
	    uint16 i;


	    *ctrl = buf[0];
	    *flownumber = buf[1];
	    count = 2;

	    for (i=0; i<10; i++)
	        timepoint[i] = buf[i+2];
	    count += 10;

	    //varsequence begin in the [variable count] bytes
	    count += xtp_varsequ_unpack( varsequ, &buf[count], len-count );
	    return count;
}

