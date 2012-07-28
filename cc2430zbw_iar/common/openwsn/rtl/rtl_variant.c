#include "rtl_configall.h"
#include <string.h>
#include <stdlib.h>
#include "rtl_foundation.h"
#include "rtl_assert.h"
#include "rtl_variant.h"

/*******************************************************************************
 * rtl_xtpvar
 *
 * @modified by zhangwei on 2010.05.01
 *	- first created
 * @modified by yanshixing on 2010.05.09
 *  - finish the implement, not test.
 ******************************************************************************/

TiVariant * var_construct(void * mem, uint16 memsize )
{
	TiVariant * var = (TiVariant *)mem;
	rtl_assert( sizeof(TiVariant) <= memsize );
	var->memsize = memsize;
	var->length = 0;
	var->type = VAR_TYPE_NULL;
	return var;
}
void var_destroy(TiVariant * var )
{
	rtl_assert( var != NULL );
	var->length = 0;
}

TiVariant * var_create( uint16 size )
{
	TiVariant * var = (TiVariant *)malloc( size );
	var = var_construct( (void *)var, size );
	return var;
}

void var_free( TiVariant * var )
{
	if (var != NULL)
	{
		var_destroy( var );
		free( var );
	}
}

void var_clear( TiVariant * var )
{
	var_destroy( var );
}

bool var_isnull( TiVariant * var )
{
	if( ((var->type&VAR_NULL_BIT)==1) || (var->length==0) )
	{
		var->type = VAR_NULL_BIT;
		var->length = 0;
		return true;
	}
	else
		return false;
}

TiVarType var_type( TiVariant * var )
{
	return var->type;
}
void var_set_type( TiVariant * var, TiVarType type )
{
	var->type = type;
}

uint16 var_size( TiVariant * var )
{
	switch( var->type )
	{
		case VAR_TYPE_BIT:
		case VAR_TYPE_BOOL:
		case VAR_TYPE_CHAR:
		case VAR_TYPE_INT8:
		case VAR_TYPE_UINT8:
			return 1;
		case VAR_TYPE_INT16:
		case VAR_TYPE_UINT16:
			return 2;
		case VAR_TYPE_INT32:
		case VAR_TYPE_UINT32:
			return 4;
		case VAR_TYPE_STRING:
		case VAR_TYPE_BINARY:
			return 1;
		default:
			return 0;
	}
}
uint16 var_length( TiVariant * var )
{
	return var->length;
}

void var_set_length( TiVariant * var, uint16 len )
{
	var->length = len;
}

char * var_dataptr( TiVariant * var )
{
	rtl_assert( var != NULL );
	return (char*)(&(var->mem));
}

/*bool var_equal( TiVariant * var1, TiVariant * var2 )
{
	if( (var1->length == var2->length)
			&& (var1->type==var2->type)
			&& (var1->space==var2->space) )
		return true;
	else
		return false;
}*/
TiVariant * var_duplicate( TiVariant * var );//?: to do what?

TiVariant * var_assignfrom( TiVariant * var, TiVariant * varfrom )
{
	var->type = varfrom->type;
	var->length = varfrom->length;
	memmove( &(var->mem.bytes[0]), &(varfrom->mem.bytes[0]), sizeof(var->mem.bytes) );
	return var;
}
TiVariant * var_assignto( TiVariant * var, TiVariant * varto )
{
	varto->type = var->type;
	varto->length = var->length;
	memmove( &(varto->mem.bytes[0]), &(var->mem.bytes[0]), sizeof(var->mem.bytes) );
	return var;
}

uint16 var_set_value( TiVariant * var, TiVarType type, char * buf, uint16 len )
{
	rtl_assert( var != NULL );
	//rtl_assert( var->length <= vardatasize );todo:make sure the vardatasize
	var->type = type;
	uint8 itemsize  = var_size(var);
	char * pc = var_dataptr(var);
	memmove( pc, buf, len );
	len = len/itemsize;
	var->length = len;
	return len;
}
uint16 var_get_value( TiVariant * var, uint8 * type, char * buf, uint16 size )
{
	rtl_assert( var != NULL );
	//rtl_assert( var->length <= vardatasize );todo:define the vardatasize
	*type = var->type;
	uint8 itemsize  = var_size(var);
	uint16 len = var->length*itemsize;
	char * pc = var_dataptr(var);
	memmove( buf, pc, len );
	return len;
}

uint8 var_bitvalue( TiVariant * var )
{
	if(var->type == VAR_TYPE_BIT)
	{
		return var->mem.bitvalue;
	}
	else
		return 0;
}
void var_set_bitvalue( TiVariant * var, uint8 bit )
{
	var->type = VAR_TYPE_BIT;
	var->length = 1;
	var->mem.bitvalue = bit;
}
bool var_boolvalue( TiVariant * var )
{
	if(var->type == VAR_TYPE_BOOL)
	{
		return var->mem.boolvalue;
	}
	else
		return false;
}
void var_set_boolvalue( TiVariant * var, bool value )
{
	var->type = VAR_TYPE_BOOL;
	var->length = 1;
	var->mem.boolvalue = value;
}
char var_get_char( TiVariant * var )
{
	if(var->type == VAR_TYPE_CHAR)
	{
		return var->mem.cvalue;
	}
	else
		return 0;
}
void var_set_charvalue( TiVariant * var, char value )
{
	var->type = VAR_TYPE_CHAR;
	var->length = 1;
	var->mem.cvalue = value;
}
int8 var_int8value( TiVariant * var )
{
	if(var->type == VAR_TYPE_INT8)
	{
		return var->mem.int8value;
	}
	else
		return 0;
}
void var_set_int8( TiVariant * var, int8 value )
{
	var->type = VAR_TYPE_INT8;
	var->length = 1;
	var->mem.int8value = value;
}
uint8 var_uint8value( TiVariant * var )
{
	if(var->type == VAR_TYPE_UINT8)
	{
		return var->mem.uint8value;
	}
	else
		return 0;
}
void var_set_uint8( TiVariant * var, uint8 value )
{
	var->type = VAR_TYPE_UINT8;
	var->length = 1;
	var->mem.uint8value = value;
}
int16 var_int16value( TiVariant * var )
{
	if(var->type == VAR_TYPE_INT16)
	{
		return var->mem.int16value;
	}
	else
		return 0;
}
void var_set_int16( TiVariant * var, int16 value )
{
	var->type = VAR_TYPE_INT16;
	var->length = 1;
	var->mem.int16value = value;
}
uint16 var_uint16value( TiVariant * var )
{
	if(var->type == VAR_TYPE_UINT16)
	{
		return var->mem.uint16value;
	}
	else
		return 0;
}
void var_set_uint16( TiVariant * var, char value )
{
	var->type = VAR_TYPE_UINT16;
	var->length = 1;
	var->mem.uint16value = value;
}
uint16 var_string( TiVariant * var, char * buf, uint16 size )
{
	if(var->type == VAR_TYPE_STRING)
	{
		uint16 count = 0;
		char * pc = (char *)var->mem.ptrvalue;
		while(pc[count] != '\0')
		{
			buf[count] = pc[count];
			count++;
		}
		return count;
	}
	else
		return 0;
}
uint16 var_set_string( TiVariant * var, char * str )
{
	var->type = VAR_TYPE_STRING;
	var->length = 1;
	//!the data in the str buffer must have a '/0' to indicate the end.
	uint16 count = 0;
	char * pc = (char *)var->mem.ptrvalue;
	while(str[count] != '\0')
	{
		pc[count] = str[count];
		count++;
	}
	pc[count] = '\0';
	count++;
	return count;
}
unsigned char * var_binary( TiVariant * var, char * buf, uint16 size )
{
	if(var->type == VAR_TYPE_BINARY)
	{
		unsigned char * pc = &(var->mem.bytes[0]);
		memmove(buf, pc, var->length);
		return buf;
	}
	else
		return NULL;
}
uint16 var_set_binary( TiVariant * var, char * buf, uint16 len )
{
	var->type = VAR_TYPE_BINARY;
	var->length = len;
	unsigned char * pc = &(var->mem.bytes[0]);
	memmove(pc, buf, len);
	var->length = len;
	return len;
}

// int32 var_attach( TiVariant * var, TiFunOnValueChanged handler );
// int32 var_detach( TiVariant * var );

uint32 var_dump( TiVariant * var, char * buf, uint32 size )
{
	rtl_assert( var != NULL );
	char * pc = var_dataptr(var);
	memmove( buf, pc, var->memsize-5 );
	return (var->memsize-5);
}
