
#include "svc_nodebase.h"

static TiNodeDescriptor * _nbase_getnodeptr( TiNodeBase * nbase, uint8 idx );
static TiNodeDescriptor * _nbase_findemptynodeptr( TiNodeBase * nbase, uint8 pidx );

static TiRelationDescriptor * _nbase_getrelationptr( TiNodeBase * nbase, uint8 idx );
static TiRelationDescriptor * _nbase_findemptyrelationptr( TiNodeBase * nbase, uint8 pidx );


TiNodeBase * nbase_construct( void * mem, uint16 memsize )
{
    memset( mem, 0x00, memsize );
    return (TiNodeBase*)mem;
}

TiNodeBase * nbase_open( TiNodeBase * nbase,uint8 state,uint16 pan, uint16 shortaddress,uint8 nio_channel,uint8 capability )
{
    int i;

    for ( i=0;i<CONFIG_NBASE_NODE_CAPACITY;i++)
    {
        nbase->nodes[i].state =0;
    }

    for ( i=0;i<CONFIG_NBASE_RELATION_CAPACITY;i++)
    {
        nbase->relations[i].state=0;
    }

    nbase->state = state;
    nbase->pan = pan;
    nbase->shortaddress = shortaddress;
    nbase->nio_channel = nio_channel;
    nbase->capability = capability;

 }

void nbase_close( TiNodeBase * nbase )
{

}

void nbase_clear( TiNodeBase * nbase )
{
    int i;

    for ( i=0;i<CONFIG_NBASE_NODE_CAPACITY;i++)
    {
        nbase->nodes[i].state =0;
    }

    for ( i=0;i<CONFIG_NBASE_RELATION_CAPACITY;i++)
    {
        nbase->relations[i].state=0;
    }
}

bool nbase_save( TiNodeBase * nbase )
{

}

bool nbase_load( TiNodeBase * nbase )
{

}


TiNodeDescriptor * nbase_getnodetable( TiNodeBase * nbase )
{
    return &nbase->nodes[0];
}

TiNodeDescriptor * nbase_getnodetptr( TiNodeBase * nbase, uint16 address )
{
    int i;
    for ( i=0;i<CONFIG_NBASE_NODE_CAPACITY;i++)
    {
        if ( nbase->nodes[i].address == address)
        {
            break;
        }
    }

    if ( i<CONFIG_NBASE_NODE_CAPACITY)
    {
        return &nbase->nodes[i];
    }
    else
    {
        return NULL;
    }
}

TiRelationDescriptor * nbase_getrelationtable( TiNodeBase * nbase )
{
    return &nbase->relations[0];
}

TiRelationDescriptor * nbase_getrelationptr( TiNodeBase * nbase, uint16 addrfrom, uint16 addrto )
{
    int i;
    for ( i=0;i<CONFIG_NBASE_RELATION_CAPACITY;i++)
    {
        if ( (nbase->relations[i].addrfrom ==addrfrom)&&(nbase->relations[i].addrto==addrto))
        {
            break;
        }
    }

    if ( i<CONFIG_NBASE_RELATION_CAPACITY)
    {
        return &nbase->relations[i];
    }
    else
    {
        return 0;
    }
}

TiNodeDescriptor * nbase_getnodebyaddress( TiNodeBase * nbase, uint16 address )
{
    int i;
    for ( i=0;i<CONFIG_NBASE_NODE_CAPACITY;i++)
    {
        if ( nbase->nodes[i].address==address)
        {
            break;
        }
    }
    if ( i<CONFIG_NBASE_NODE_CAPACITY)
    {
        return &nbase->nodes[i];
    }
    else
    {
        return NULL;
    }
}

TiRelationDescriptor * nbase_getrelationbyaddress( TiNodeBase * nbase, uint16 addrfrom,uint16 addrto )
{
    int i;
    for ( i=0;i<CONFIG_NBASE_RELATION_CAPACITY;i++)
    {
        if ( (nbase->relations[i].addrfrom==addrfrom)&&(nbase->relations[i].addrto==addrto))
        {
            break;
        }
    }

    if ( i<CONFIG_NBASE_RELATION_CAPACITY)
    {
        return &nbase->relations[i];
    }
    else
    {
        return NULL;
    }
}

/** Put one node descriptor into database */
TiNodeDescriptor * nbase_setnode( TiNodeBase * nbase, uint16 address, TiNodeDescriptor * node )
{
    int i;
    for ( i=0;i<CONFIG_NBASE_NODE_CAPACITY;i++)
    {
        if ( nbase->nodes[i].state==0)
        {
            nbase->nodes[i].address = address;
            nbase->nodes[i].lifetime = node->lifetime;
            nbase->nodes[i].pan = node->pan;
            nbase->nodes[i].tag = node->tag;
            nbase->nodes[i].weight = node->weight;
            nbase->nodes[i].rssi = node->rssi;
            nbase->nodes[i].state = 1;
            break;
        }
    }

    if ( i<CONFIG_NBASE_NODE_CAPACITY)
    {
        return &nbase->nodes[i];
    }
    else
    {
        return NULL;
    }
}

/** Put one relation descriptor into database */
TiRelationDescriptor * nbase_setrelation( TiNodeBase * nbase, uint16 addrfrom, uint16 addrto, TiRelationDescriptor * relation )
{
    int i;
    for ( i=0;i<CONFIG_NBASE_RELATION_CAPACITY;i++)
    {
        if ( nbase->relations[i].state==0)
        {
            nbase->relations[i].addrfrom = addrfrom;
            nbase->relations[i].addrto = addrto;
            nbase->relations[i].rssi = relation->rssi;
            nbase->relations[i].weight = relation->weight;
            nbase->relations[i].state = 1;
            break;
        }
    }

    if ( i<CONFIG_NBASE_RELATION_CAPACITY)
    {
        return &nbase->relations[i];
    }
    else
    {
        return NULL;
    }
}

/**
 * Return all index of TiNodeDescriptors related to specified node. You can use 
 * nbase_getnodeptr(nbase,idx) to get the memory pointer to the TiNodeDescriptor
 * item in the database for fast access.
 */
uint8 nbase_getnodeneighbors( TiNodeBase * nbase, uint16 address, TiNodeDescriptor ** nodeptr, uint8 capacity )
{
    uint8 count;
    int i;
    count =0;
    for ( i=0;i<CONFIG_NBASE_RELATION_CAPACITY;i++)
    {
        if ( nbase->relations[i].addrfrom == address)
        {
            nodeptr[count] = _nbase_getnodeptr( nbase,i);
            count ++;
            if ( count >=capacity)
            {
                break;
            }
        }
    }

    return count;
}

/** 
 * Returns all index of TiRelationDescriptors related to specified node. You can
 * use nbase_getrelationptr(nbase,idx) to get the memory pointer to that TiRelationDescriptor 
 * for fast access.
 */
uint8 nbase_getnoderelations( TiNodeBase * nbase, uint16 address, TiRelationDescriptor ** relptr, uint8 capacity )
{
    uint8 count;
    int i;
    count = 0;
    for ( i=0;i<CONFIG_NBASE_RELATION_CAPACITY;i++)
    {
        if ( nbase->relations[i].addrfrom==address)
        {
            relptr[count] = _nbase_getrelationptr(nbase,i);
            count ++;
            if ( count >= capacity)
            {
                break;
            }
        }
        
    }
    return count;
}


bool nbase_deletenode( TiNodeBase * nbase, uint16 address )
{
    int i;
    for ( i=0;i<CONFIG_NBASE_NODE_CAPACITY;i++)
    {
        if ( nbase->nodes[i].address == address)
        {
            break;
        }
    }

    if ( i<CONFIG_NBASE_NODE_CAPACITY)
    {
        nbase->nodes[i].state=0;
        return true;
    } 
    else
    {
        return false;
    }
}


bool nbase_deleterelation( TiNodeBase * nbase, uint16 addrfrom, uint16 addrto )
{
    int i;
    for ( i=0;i<CONFIG_NBASE_RELATION_CAPACITY;i++)
    {
        if ( ( nbase->relations[i].addrfrom==addrfrom)&&( nbase->relations[i].addrto==addrto))
        {
            break;
        }
    }

    if ( i<CONFIG_NBASE_RELATION_CAPACITY)
    {
        nbase->relations[i].state=0;
        return true;
    }
    else
    {
        return false;
    }
}

static TiNodeDescriptor * _nbase_getnodeptr( TiNodeBase * nbase, uint8 idx )
{
    return &nbase->nodes[idx];
}

static TiNodeDescriptor * _nbase_findemptynodeptr( TiNodeBase * nbase, uint8  pidx )
{
    int i;
    for ( i=0;i<CONFIG_NBASE_NODE_CAPACITY;i++)
    {
        if ( nbase->nodes[i].state == 0)
        {
            break;
        }
    }

    if ( i<CONFIG_NBASE_NODE_CAPACITY)
    {
        pidx = i;
        return &nbase->nodes[i];
    }
    else
    {
        return NULL;
    }
}

static TiRelationDescriptor * _nbase_getrelationptr( TiNodeBase * nbase, uint8 idx )
{
    return &nbase->relations[idx];
}

static TiRelationDescriptor * _nbase_findemptyrelationptr( TiNodeBase * nbase, uint8  pidx )
{
    int i;
    for ( i=0;i<CONFIG_NBASE_RELATION_CAPACITY;i++)
    {
        if ( nbase->relations[i].state == 0)
        {
            break;
        }
    }

    if ( i<CONFIG_NBASE_RELATION_CAPACITY)
    {
        pidx = i;
        return &nbase->relations[i];
    }
    else
    {
        return NULL;
    }
}

