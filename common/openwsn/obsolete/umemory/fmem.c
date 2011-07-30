/*
 * =====================================================================================
 * 
 *       Filename:  fmem.c
 * 
 *    Description:  static memory allocation. 
 * 
 *        Version:  1.0
 *        Created:  6/19/2007 6:31:09 PM Tokyo Standard Time
 *       Revision:  none
 * 
 *         Author:  Jian Zhang , zhjwp@hotmail.com
 *                 
 *        Note(s):  To enable it to be used in the interrupt ISR context, instead of 
 *                  using 'critical primitive', interrupt are closed/opened.
 *                  Because this poring is based on uCOS-II, variable 'cpu_sr' is required
 *                  by primitive 'DISABLE_INTERRUPT' and 'ENABLE_INTERRUPT'. It may be 
 *                  modified when portted to other OS.
 * 
 * =====================================================================================
 */

#include "fmem.h"

INT8U FMEM_CreateParti(FMEM_PARTI *parti, void *addr, INT32U blk_num, INT32U blk_size)
{
    INT8U ret = 0;
#if OS_CRITICAL_METHOD == 3 
    OS_CPU_SR     cpu_sr = 0;
#endif

    if (blk_num) {
        INT32U num = blk_num;
        void **p_addr = (void **)addr;
        
        do {
            *p_addr = (void *)((INT8U *)p_addr + blk_size);
            p_addr = (void **)((INT8U *)p_addr + blk_size);
        } while (--num);
        
        DISABLE_INTERRUPT
        parti->base_addr = addr;
        parti->free_list = addr;
        parti->blk_size = blk_size;
        parti->blk_total_num = blk_num;
        parti->blk_free_num = blk_num;
        ENABLE_INTERRUPT;
        ret = 1;
    }
    return ret;
}

void *FMEM_Malloc(FMEM_PARTI *parti)
{
    void *addr = NULL;
#if OS_CRITICAL_METHOD == 3 
    OS_CPU_SR     cpu_sr = 0;
#endif

    DISABLE_INTERRUPT;
    if (parti->blk_free_num) {
        addr = parti->free_list;
        parti->free_list = *(void **)parti->free_list;
        parti->blk_free_num--;
    }
    ENABLE_INTERRUPT;
    return addr;
}

void  FMEM_Free(FMEM_PARTI *parti, void *addr)
{
#if OS_CRITICAL_METHOD == 3 
    OS_CPU_SR     cpu_sr = 0;
#endif

    DISABLE_INTERRUPT;
    *(void **)addr = parti->free_list;
    parti->free_list = addr;
    parti->blk_free_num++;
    ENABLE_INTERRUPT;
}

