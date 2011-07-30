#ifndef VMEM_H
#define VMEM_H

#include    "osdep.h"

typedef struct vmem_info {
    INT32U total_size;      /* total size of the vmem region */
    INT32U total_unused;    /* total unused size of the vmem region */
	INT32U largest_unused;  /* the largest unused segment */
	INT32U list_num;        /* free segment number in the free segment list. */
} VMEM_INFO;

typedef void CB_MallocFailAlert(INT16U size);

/* this macro defines the total size of RAM space can be 
 * used by the memory management module.
 */
#define CFG_VMEM_BUF_SIZE       (32 * 1024)

void   VMEM_Init(void);
void   VMEM_Done(void);
void   VMEM_CBRegister(CB_MallocFailAlert *cbfunc);
void  *VMEM_Malloc(INT16U size);
void  *VMEM_Realloc(void *addr, INT16U size);
void   VMEM_Free(void *addr);
void   VMEM_Query(VMEM_INFO *info);
void   VMEM_Test(void);

#endif
