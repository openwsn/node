/*
 * =====================================================================================
 * 
 *       Filename:  vmem.c
 * 
 *    Description:  dynamic size memory allocation. 
 * 
 *        Version:  1.0
 *        Created:  6/19/2007 6:31:09 PM Tokyo Standard Time
 *       Revision:  none
 * 
 *         Author:  Jian Zhang , zhjwp@hotmail.com
 *        Note(s):  none
 * 
 * =====================================================================================
 */

#include "vmem.h"

typedef struct vmem_seg {
    INT16U prev;
    INT16U next;
} VMEM_SEG;

typedef struct vmem_free_seg {
    INT16U prev;
    INT16U next;
    struct vmem_free_seg *next_free;
} VMEM_FREE_SEG;

#define SIZEOF_VMEMSEG      (sizeof(VMEM_SEG))
#define VMEM_PAGE_SIZE      SIZEOF_VMEMSEG
#define ROUND_BUF_SIZE      (((CFG_VMEM_BUF_SIZE + VMEM_PAGE_SIZE - 1) / VMEM_PAGE_SIZE) * VMEM_PAGE_SIZE)
#define SIZEOF_TAIL(seg)    ((INT32U)((INT8U *)vmem_buf + ROUND_BUF_SIZE - (INT8U *)(seg)))

#define GET_PREV(seg)       (((seg)->prev >> 1) << 1)
#define SET_PREV(seg, p)    ((seg)->prev = p | ((seg)->prev & VMEM_USED))
#define GET_NEXT(seg)       ((seg)->next)
#define SET_NEXT(seg, n)    (((seg)->next) = n)

/* 'used' flag is the last bit of 'prev' member of each segment. */
#define VMEM_USED           ((INT16U)(0x0001))
#define ISNOT_USED(seg)     (!((seg)->prev & VMEM_USED))
#define SET_USED(seg)       ((seg)->prev = (seg)->prev | VMEM_USED)
#define CLEAR_USED(seg)     ((seg)->prev = (seg)->prev & ~VMEM_USED)

#define IS_TAIL(seg)        (GET_NEXT(seg) == 0)
#define IS_HEAD(seg)        (GET_PREV(seg) == 0)

#define NSEG(seg)           ((VMEM_FREE_SEG *)((INT8U *)(seg) + GET_NEXT(seg)))
#define PSEG(seg)           ((VMEM_FREE_SEG *)((INT8U *)(seg) - GET_PREV(seg)))


/* vmem_buf should be aligned and the size of vmem_buf should to be devided into VMEM_PAGE_SIZE */
static VMEM_SEG vmem_buf[ROUND_BUF_SIZE / SIZEOF_VMEMSEG];
static HANDLE g_vmem_lock = NULL;
static CB_MallocFailAlert *g_vmem_cbfunc = NULL;

static VMEM_FREE_SEG *vfree_list;

/**********************************************************************************

1. Structure of the segment links

 vmem_buf
    |       |-> head                                             |-> tail    
    |       |                                                    |          
    \---->  --------------------------------------------------------------------
            ||.......||....||            ||....||         ||.....||            |
            --------------------------------------------------------------------
                           |             
                           |-> vfree_list

2. 'head' and 'tail'
  'head' segment: the first segment in vmem_buf. The 'prev' of this segment is 0;
  'tail' segment: the last segment in vmem_buf.  The 'next' of this segment is 0;

3. 'vfree_list'
  'vfree_list' is the head node of the free segment list.

**************************************************************************************/

void VMEM_CBRegister(CB_MallocFailAlert *cbfunc)
{
    g_vmem_cbfunc = cbfunc;
}

void VMEM_Init(void)
{
    memset(vmem_buf, 0, ROUND_BUF_SIZE);
    vfree_list = (VMEM_FREE_SEG *)vmem_buf;
    SET_NEXT(vfree_list, 0);                    /* 0 means it is the tail segment */
    SET_PREV(vfree_list, 0);                    /* 0 means it is the head segment */
    CLEAR_USED(vfree_list);
    vfree_list->next_free = (VMEM_FREE_SEG *)0; /* 0 means it is the tail of the free segment list */
    g_vmem_lock = CRITICAL_CREATE(NULL);
}

void VMEM_Done(void)
{
    CRITICAL_FREE(g_vmem_lock);
}

void *VMEM_Malloc(INT16U size)
{
    VMEM_FREE_SEG *rseg = NULL; 
    VMEM_FREE_SEG *nseg = NULL;
    VMEM_FREE_SEG *pseg = NULL;

    /* Align the size of the allocated memory to the page size */
    size = SIZEOF_VMEMSEG + ((size + VMEM_PAGE_SIZE - 1) / VMEM_PAGE_SIZE) * VMEM_PAGE_SIZE;

    CRITICAL_ENTER(g_vmem_lock);

    /* Allocate memory from vfree_list pointer. */ 
    if (vfree_list == NULL) {
        goto MALLOC_RET;
    }
    nseg = vfree_list;
    /* find a big enough segment in the free segment list pointed by 'vfree_list'. */
    do {
        INT32U seg_len;
        if (!IS_TAIL(nseg)) {
            seg_len = GET_NEXT(nseg);
            if (seg_len >=size) {
                if (seg_len > size + SIZEOF_VMEMSEG) {
                    /* There are enough space to split it into two segments */
                    SET_USED(nseg);
                    SET_PREV(NSEG(nseg), seg_len - size);
                    SET_NEXT(PSEG(NSEG(nseg)), seg_len - size);
                    SET_NEXT(nseg, size);
                    SET_PREV(NSEG(nseg), size);
                    CLEAR_USED(NSEG(nseg));
                    rseg = nseg;
                    /* adjust the free segment list. */
                    if (nseg == vfree_list) {
                        vfree_list = NSEG(nseg);
                        vfree_list->next_free = nseg->next_free;
                    }
                    else {
                        NSEG(nseg)->next_free = nseg->next_free;
                        pseg->next_free = NSEG(nseg);
                    }
                    break;
                }
                else {  
                    /* nseg->next is size or (size + SIZEOF_VMEMSEG).
                     * It can't be split into two segments, this whole segment
                     * will be allocated.
                     */
                    SET_USED(nseg);
                    rseg = nseg;
                    /* adjust the free segment list. */                
                    if (nseg == vfree_list) {
                        vfree_list = nseg->next_free;
                    }
                    else {
                        pseg->next_free = nseg->next_free;
                    }
                    break;
                }
            }
        }
        else {
            /* nseg is the tail segment */
            seg_len = SIZEOF_TAIL(nseg);
            if (seg_len >= size) {
                if (seg_len > size + SIZEOF_VMEMSEG) {
                    SET_USED(nseg);
                    SET_NEXT(nseg, size);
                    SET_PREV(NSEG(nseg), size);
                    CLEAR_USED(NSEG(nseg));
                    SET_NEXT(NSEG(nseg), 0);
                    rseg = nseg;
                    /* adjust the free segment list. */                
                    if (nseg == vfree_list) {
                        vfree_list = NSEG(nseg);
                        vfree_list->next_free = nseg->next_free;
                    }
                    else {
                        NSEG(nseg)->next_free = nseg->next_free;
                        pseg->next_free = NSEG(nseg);
                    }
                    break;
                } 
                else {
                    SET_USED(nseg);
                    rseg = nseg;
                    /* adjust the free segment list. */                
                    if (nseg == vfree_list) {
                        vfree_list = nseg->next_free;
                    }
                    else {
                        pseg->next_free = nseg->next_free;
                    }
                    break;
                }
            }
        }
        
        pseg = nseg;
        nseg = nseg->next_free;
    } while (nseg);

MALLOC_RET:
    CRITICAL_LEAVE(g_vmem_lock);
    
    if (rseg == NULL) {
        if (g_vmem_cbfunc) {
            (*g_vmem_cbfunc)(size);
        }
        return NULL;
    }
    else {
        return (void *)((INT8U *)rseg + SIZEOF_VMEMSEG);
    }
}

void VMEM_Free(void *addr)
{
    VMEM_FREE_SEG *fseg;
    INT8U          seg_flag = 0;
    
    if (addr == NULL) {
        return;
    }
    fseg = (VMEM_FREE_SEG *)((INT8U *)addr - SIZEOF_VMEMSEG);
    CRITICAL_ENTER(g_vmem_lock);
    CLEAR_USED(fseg);

    /* Converge with the next free segment */
    if (!IS_TAIL(fseg)) {
        if (ISNOT_USED(NSEG(fseg))) {
            if (IS_TAIL(NSEG(fseg))) {
                /* fseg is the previous segment of the tail segment. */
                SET_NEXT(fseg, 0);
            }
            else {
                SET_PREV(NSEG(NSEG(fseg)), GET_NEXT(fseg) + GET_NEXT(NSEG(fseg)));
                SET_NEXT(fseg, GET_NEXT(fseg) + GET_NEXT(NSEG(fseg)));
            }
            seg_flag += 1;
        }
    }

    /* Converge with the previous free segment */
    if (!IS_HEAD(fseg)) {
        if (ISNOT_USED(PSEG(fseg))) {
            if (!IS_TAIL(fseg)) {
                SET_PREV(NSEG(fseg), GET_NEXT(fseg) + GET_NEXT(PSEG(fseg)));
                SET_NEXT(PSEG(fseg), GET_NEXT(fseg) + GET_NEXT(PSEG(fseg)));
            }
            else {
                /* fseg is the tail segment. */
                SET_NEXT(PSEG(fseg), 0);
            }
            seg_flag += 2;
        }
    }

    /*  value of free_flag: 
     *      0: no segment converging.
     *      1: fseg is only converged with its next segment.
     *      2: fseg is only converged with its previous segment.
     *      3: fseg is converged with both its previous and its next segments.
     */
    if (seg_flag == 3) {
        fseg = PSEG(fseg);
        fseg->next_free = (fseg->next_free)->next_free;        
    }
    else if (seg_flag != 2) {
        VMEM_FREE_SEG *nseg = NULL;
        VMEM_FREE_SEG *pseg = NULL;

        if (vfree_list == NULL) {
            vfree_list = fseg;
            vfree_list->next_free = 0;
        }
        else if (vfree_list > fseg) {
            if (IS_TAIL(fseg)) {
                /* in the case that fseg is converged with its next segment, 
                 * tail segment, and the vfree_list is pointed to that tail segment.
                 */
                vfree_list = fseg;
                vfree_list->next_free = 0;
            }
            else {
                /* check if the segment pointed by vfree_list is converged with fseg. */
                fseg->next_free = (vfree_list < NSEG(fseg)) ? vfree_list->next_free : vfree_list;
                vfree_list = fseg;
            }
        }
        else {
            /* insert 'fseg' into the free segment list. */
            nseg = vfree_list;

            if (seg_flag == 0) {
                do {
                    if (nseg > fseg) {
                        fseg->next_free = nseg;
                        pseg->next_free = fseg;            
                        break;
                    }
                    pseg = nseg;
                    nseg = nseg->next_free;
                } while (nseg);
                if (!nseg) {
                    /* 'fseg' is the tail node of the free segment list. */
                    pseg->next_free = fseg;
                    fseg->next_free = 0;
                }
            }
            else {
                do {
                    if (nseg > fseg) {
                        fseg->next_free = nseg->next_free;
                        pseg->next_free = fseg;            
                        break;
                    }
                    pseg = nseg;
                    nseg = nseg->next_free;
                } while (nseg);
            }
        }
    }    
    CRITICAL_LEAVE(g_vmem_lock);
}

void *VMEM_Realloc(void *addr, INT16U size) 
{
    VMEM_SEG *aseg;
	void *addr_new;
	INT16U size_old;
	
    if (addr == NULL) {
        return NULL;
    }

    aseg = (VMEM_SEG *)((INT8U *)addr - SIZEOF_VMEMSEG);
    size_old = (INT16U)((IS_TAIL(aseg) ? SIZEOF_TAIL(aseg) : GET_NEXT(aseg)) - SIZEOF_VMEMSEG);    

    addr_new = VMEM_Malloc(size);
    if (addr_new) {
        memcpy(addr_new, addr, size_old);
        VMEM_Free(addr);		
    }
    return addr_new;
}

void VMEM_Query(VMEM_INFO *info)
{
    VMEM_FREE_SEG *nseg = vfree_list;
    INT32U list_num = 0;
    INT32U largest_unused = 0;
    INT32U total_unused = 0;
    
    info->total_size = ROUND_BUF_SIZE;
    CRITICAL_ENTER(g_vmem_lock);
    while (nseg) {
        INT32U seg_size = IS_TAIL(nseg) ? SIZEOF_TAIL(nseg) : GET_NEXT(nseg);
        
        total_unused += seg_size;
        list_num++;
        if (seg_size > largest_unused) {
            largest_unused = seg_size;
        }    
        nseg = nseg->next_free;
    };
    info->total_unused = total_unused;
    info->largest_unused = largest_unused;
    info->list_num = list_num;
    
    CRITICAL_LEAVE(g_vmem_lock);
}

