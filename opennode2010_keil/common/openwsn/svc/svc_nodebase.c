
#include "svc_nodebase.h"

static TiNodeDescriptor * _nbase_getnodeptr( TiNodeBase * nbase, uint8 idx );
static TiNodeDescriptor * _nbase_findemptynodeptr( TiNodeBase * nbase, uint8 pidx );

static TiRelationDescriptor * _nbase_getrelationptr( TiNodeBase * nbase, uint8 idx );
static TiRelationDescriptor * _nbase_findemptyrelationptr( TiNodeBase * nbase, uint8 pidx );


TiNodeBase * nbase_construct( void * mem, uint16 memsize )
{
    return (TiNodeBase*)mem;
}

void nbase_construct( TiNodeBase * nbase )
{
    return;
}

TiNodeBase * nbase_open( TiNodeBase * nbase );
{
    int i;

    memset( em, 0x00, memsize);

    nbase->state = NBASE_STATE_FREE; 
    nbase->pan = 0x00; // todo
    nbase->shortaddress = 0; // todo
    nbase->nio_channel = 0; // todo;
    nbase->capability = 0; //

    for (i=0; i<CONFIG_NBASE_NODE_CAPACITY; i++)
    {
        nbase->nodes[i].state =0;
    }

    for (i=0; i<CONFIG_NBASE_RELATION_CAPACITY; i++)
    {
        nbase->relations[i].state=0;
    }
}

void nbase_close( TiNodeBase * nbase )
{

}

void nbase_clear( TiNodeBase * nbase )
{
    int i;

    for ( i=0;i<CONFIG_NBASE_NODE_CAPACITY;i++)
    {
        nbase->nodes[i].state = NBASE_STATE_FREE;
    }

    for ( i=0;i<CONFIG_NBASE_RELATION_CAPACITY;i++)
    {
        nbase->relations[i].state = NBASE_STATE_FREE;
    }
}

bool nbase_save( TiNodeBase * nbase )
{
    return false;
}

bool nbase_load( TiNodeBase * nbase )
{
    return false;
}

TiNodeDescriptor * nbase_getnodetable( TiNodeBase * nbase )
{
    return &(nbase->nodes[0]);
}

TiNodeDescriptor * nbase_getnodetptr(TiNodeBase * nbase, uint16 idx)
{
    svc_assert(idx < CONFIG_NBASE_NODE_CAPACITY);
    return &(nbase->nodes[idx]);
}

TiRelationDescriptor * nbase_getrelationtable( TiNodeBase * nbase )
{
    return &(nbase->relations[0]);
}

TiRelationDescriptor * nbase_getrelationptr(TiNodeBase * nbase, uint8 idx)
{
    svc_assert(idx < CONFIG_NBASE_RELATION_CAPACITY);
    return &(nbase->relations[idx]);
}

TiNodeDescriptor * nbase_getnodebyaddress(TiNodeBase * nbase, uint16 address, TiNodeDescriptor * node)
{
    int i;
    TiNodeDescriptor * cur = NULL;
    bool found = false;
    
    for (i=0; i<CONFIG_NBASE_NODE_CAPACITY; i++)
    {
        cur = &(nbase->node[i]);
        if (cur->state != NBASE_STATE_FREE)
        {
            if (cur->address == address)
            {
                found = true;
                *node = *cur;
                break;
            }
        }
    }

    return (found ? node : NULL);
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

/** 
 * Put one node descriptor into database.
 * 
 * @attention
 * - If the database has already an record about the input node, then this function
 *   will update that record, or else insert a new one. If there're no available 
 *   space, then it will return NULL to indicate failure.
 * 
 * @param address Node address
 * @param node The node descriptor to be put into the database.
 * @return 
 *      Not Null(equal to node) indicate success.
 *      NULL means failed.
 */
TiNodeDescriptor * nbase_setnode( TiNodeBase * nbase, uint16 address, TiNodeDescriptor * node )
{
    int i;
    TiNodeDescriptor * cur = NULL;
    bool found = false;
    

    for (i=0; i<CONFIG_NBASE_NODE_CAPACITY; i++)
    {
        cur = &(nbase->nodes[i]);
        if (cur->state != NBASE_STATE_FREE) && (cur->address == address)
        {
            found = truel
            break;
        }
    }
    
    if (!found)
    {
        for (i=0; i<CONFIG_NBASE_NODE_CAPACITY; i++)
        {
            cur = &(nbase->nodes[i]);
            if (cur->state == NBASE_STATE_FREE)
            {
                found = truel
                break;
            }
        }
    }
    
    if (found)
    {
        // @attention: 
        // - This function only copys part of the variable member values in node 
        // structure into database. This can avoid overriding some important internal 
        // managed status variable.
            
        cur->address = address;
        cur->pan = node->pan;
        cur->tag = node->tag;
        cur->weight = node->weight;
        cur->rssi = node->rssi;
        cur->state = NBASE_STATE_ACTIVE;
        return node;
    }
    else
        return NULL;
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

