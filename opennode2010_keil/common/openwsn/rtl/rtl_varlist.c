#include "rtl_configall.h"
#include "rtl_foundation.h"
#include "rtl_assert.h"
#include "rtl_varlist.h"
#include <stdlib.h>

/*******************************************************************************
 * rtl_varlist
 *
 * @modified by zhangwei on 2010.05.01
 *	- first created
 * @modified by yanshixing on 2010.05.09
 *  - finish the implement, not test.
 ******************************************************************************/


TiVarList * varlist_construct( void * mem, uint16 memsize )
{
    TiVarList * varlist = (TiVarList *)mem;

    rtl_assert( sizeof(TiVarList) <= memsize );

    varlist->memsize = memsize;
    varlist->count = 0;
    varlist->capacity = (memsize - sizeof(TiVarList));
    varlist->capacity /= sizeof(void *);
    varlist->current = 0;
    return varlist;
}

void varlist_destroy( TiVarList * varlist )
{
    return;
}

TiVarList * varlist_create( uint8 capacity )
{
    uint16 memsize ;
    char * buf;
    TiVarList * varlist;

    memsize = sizeof(TiVarList) + capacity * sizeof(void *);
    buf = malloc( memsize );
    if (buf != NULL)
    	varlist = varlist_construct( buf, memsize );
    else
        varlist = NULL;

    return varlist;
}

void varlist_free( TiVarList * varlist )
{
    free( varlist );
}

uint16 varlist_count( TiVarList * varlist )
{
    return varlist->count;
}

TiVariant * varlist_items( TiVarList * varlist, uint16 idx )
{
	TiVariant ** table;
	table = varlist_table( varlist );
	return table[idx];
}

TiVariant ** varlist_table( TiVarList * varlist )
{
    return (TiVariant **)((char *)varlist + sizeof(TiVarList));
}

bool varlist_append( TiVarList * varlist, TiVariant * var )
{
    TiVariant ** table;

    if (varlist->count < varlist->capacity)
    {
        table = varlist_table( varlist );
        table[varlist->count] = var;
        varlist->count ++;
        return true;
    }
    else
        return false;
}

TiVariant * varlist_first( TiVarList * varlist )
{
    varlist->current = 0;
    if (varlist->count > 0)
        return varlist_items( varlist, 0 );
    else
        return NULL;
}

TiVariant * varlist_next( TiVarList * varlist )
{
    TiVariant * var = NULL;

    varlist->current ++;
    if (varlist->current < varlist->count)
    {
        var = varlist_items(varlist, varlist->current);
    }
    else
    	var = NULL;

    return var;
}
