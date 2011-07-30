#ifndef FMEM_H
#define FMEM_H

#include    <osdep.h>

typedef struct fmem_parti {
    void *base_addr;
    void *free_list;
    INT32U blk_size;
    INT32U blk_total_num;
    INT32U blk_free_num;
} FMEM_PARTI;

INT8U FMEM_CreateParti(FMEM_PARTI *parti, void *addr, INT32U blk_num, INT32U blk_size);
void *FMEM_Malloc(FMEM_PARTI *parti);
void  FMEM_Free(FMEM_PARTI *parti, void *addr);

#endif

