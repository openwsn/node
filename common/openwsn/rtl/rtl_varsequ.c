
#include "rtl_configall.h"
#include "rtl_foundation.h"
#include "rtl_assert.h"
#include "rtl_variant.h"
#include "rtl_varlist.h"
#include "rtl_varsequ.h"
/*******************************************************************************
 * rtl_xtpvar
 *
 * @modified by zhangwei on 2010.05.01
 *	- first created
 * @modified by yanshixing on 2010.05.13
 * 	- modified the xtp_varsequ_unpack(), I don't think var should be created before this function
 * 		is called. The var should be created according to the data in buffer( var sequence)
 *
 ******************************************************************************/

/*******************************************************************************
 * TiXtpVariant 
 ******************************************************************************/


/*******************************************************************************
 * TiXtpVarSequence
 * 该类协助将变量的值写入内存缓冲区,通常该内存缓冲区就是TiXtpPacket的Payload部分.
 ******************************************************************************/

static void _pack16( char * buf, uint16 n16 )
{
    buf[0] = (uint8)(n16 >> 8);
    buf[1] = (uint8)(n16 & 0xFF);
}

static uint16 _unpack16( char * buf )
{
    return (((uint16)buf[0]) << 8) | (buf[1]);
}


uint16 xtp_varsequ_pack( TiXtpVarSequence * varsequ, char * buf, uint16 size )
{
    uint16 count=0, i;
    char * pc = buf;
    TiVariant * var;
    char * value;

    _pack16( buf, varlist_count(varsequ) );
    pc += 2;
    count += 2;

    var = varlist_first( varsequ );
    while (var != NULL)
    {
        rtl_assert( count+3 < size );
        
        *pc++ = var_type( var );
        _pack16( pc, var_length(var) );
        pc += 2;
        count += 3;

        rtl_assert( count + var_length(var) <= size );
        value = (char*)var_dataptr(var);
        for (i=0; i<var_length(var); i++)
            *pc++ = *value++;

        var = varlist_next( varsequ );
    }
    return count;
}

/* xtp_varsequ_unpack
 * parse the input buffer which contains an variable list. and put the values
 * into corresponding variable object.
 *
 * before you calling this function, you should create related variable objects
 * and put them into the var sequence.
 * 
 * @attention
 *  - this function assumes you have already created the variables and append them
 *    into var sequence. these variables already have enough memory spaces to accept
 *    corresponding variable values parsed from the buffer.
 */
uint16 xtp_varsequ_unpack( TiXtpVarSequence * varsequ, char * buf, uint16 len )
{
    uint16 count;
    uint8 i;
    char * pc = buf;
    TiVariant * var;
    uint16 ret = 0;

    count = _unpack16(pc);
    pc =  pc + 2;
    ret = ret + 2;

    for(i=0;i<count;i++)
    {
    	var = var_create( sizeof(TiVariant) );
    	var_set_type( var, pc[0] );
    	pc++;
    	ret++;
    	var_set_length( var, _unpack16(pc) );
    	pc = pc + 2;
    	ret = ret + 2;
    	var_set_value( var, var_type(var), pc, var_length(var)*var_size(var) );
    	pc = pc + var_length(var)*var_size(var);
    	ret = ret + var_length(var)*var_size(var);
    	varlist_append(varsequ, var);
    }

    return ret;

    /*if (count == xtp_varsequ_count(xtp))
    {
        pc = buf;
        var = xtp_varsequ_first( varsequ );
        while (var != NULL)
        {
            assert(pc < endptr);
            type = (*pc++);

            datalen = _unpack( pc );
            pc += 2;

            assert(pc < endptr);
            
            var_setvalue( var, type, pc, datalen );
            pc += datalen;
            assert(pc < endptr);

            var = xtp_varsequ_next( varsequ );
        }
    }
    return 0;*/
}

