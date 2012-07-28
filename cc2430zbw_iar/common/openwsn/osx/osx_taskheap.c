/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 * 
 * OpenWSN is a free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple 
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 * 
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi 
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 * 
 ******************************************************************************/ 

#include "osx_configall.h"
#include <string.h>
#include "osx_foundation.h"
#include "../rtl/rtl_int8queue.h"
#include "osx_taskheap.h"

#define HEAP_LEFT(idx) (((idx+1)>>1)-1)
#define HEAP_RIGHT(idx) ((idx+1)>>1)
/*
 * internal functions 
 */
static inline int8 _osx_taskheap_compare( TiOsxTaskHeapItem * item1, TiOsxTaskHeapItem * item2 );
static inline void _osx_taskheap_switch( TiOsxTaskHeap * heap, int8 idx1, int8 idx2 );
static int8 _osx_taskheap_siftup( TiOsxTaskHeap * heap, int8 idx );
static int8 _osx_taskheap_siftdown( TiOsxTaskHeap * heap, int8 idx );

/*
TiOsxTaskHeap * osx_taskheap_construct( char * mem, uint16 memsize )
{
    TiOsxTaskHeap * heap = (TiOsxTaskHeap *)mem;

    osx_assert( sizeof(TiOsxTaskHeap) <= memsize );
    memset( mem, 0x00, memsize );
    heap->count = 0;

    return heap;
}

void osx_taskheap_destroy( TiOsxTaskHeap * heap )
{
    return;
}
*/

TiOsxTaskHeap * osx_taskheap_open( TiOsxTaskHeap * heap, TiOsxTaskPool * tpl )
{
    heap->count = 0;
    heap->taskpool = tpl;
    return heap;
}

/* 
 * @warning: This function will also clear the task pool. If the taskpool is shared
 * by serveral modules, then this function will leads to error because it may clear
 * items inside taskpool used by other modules!
 */
void osx_taskheap_close( TiOsxTaskHeap * heap )
{
    heap->count = 0;
    osx_taskpool_clear( heap->taskpool );
    return;
}

uint8 osx_taskheap_capacity( TiOsxTaskHeap * heap )
{
    return CONFIG_OSX_TASKHEAP_CAPACITY;
}

uint8 osx_taskheap_count( TiOsxTaskHeap * heap )
{
    return heap->count;
}

bool osx_taskheap_empty( TiOsxTaskHeap * heap )
{
    return( heap->count == 0);
}

bool osx_taskheap_full( TiOsxTaskHeap * heap )
{
    return (heap->count == osx_taskheap_capacity(heap));
}

void osx_taskheap_clear( TiOsxTaskHeap * heap )
{
    // todo
}

TiOsxTaskHeapItem ** osx_taskheap_items( TiOsxTaskHeap * heap )
{
	return &(heap->items[0]);
}

TiOsxTaskHeapItem * osx_taskheap_apply( TiOsxTaskHeap * heap )
{
    TiOsxTaskHeapItem * item = NULL;

    int8 id;
    id = osx_taskpool_apply( heap->taskpool );
    if (id > 0)
    {
        item = osx_taskpool_gettaskdesc( heap->taskpool, id );
    }
    return item;
}

void osx_taskheap_release( TiOsxTaskHeap * heap, TiOsxTaskHeapItem * item )
{
    osx_taskpool_release( heap->taskpool, item->taskid );
}

TiOsxTaskHeapItem * osx_taskheap_root( TiOsxTaskHeap * heap )
{
    return (heap->count > 0) ? (heap->items[0]) : NULL;
}

void osx_taskheap_deleteroot( TiOsxTaskHeap * heap )
{
    osx_taskheap_deleteat( heap, 0 );
}

int8 osx_taskheap_delete( TiOsxTaskHeap * heap, TiOsxTaskHeapItem * item )
{
    int8 idx;

    idx = osx_taskheap_search( heap, item );
    if (idx >= 0)
        osx_taskheap_deleteat( heap, idx );
    else
        idx = -1;

    return idx;
}

bool osx_taskheap_deleteat( TiOsxTaskHeap * heap, uint8 idx )
{
	int8 id;

    if ((idx < 0) || (idx >= heap->count))
    {
        return false;
    }

    /* If the node to be deleted is the last node, then simply decrease heap->count.
     * If not, then overwrite the node to be deleted with the last node, and siftdown 
     * it to adjust the whole structure as a new heap.
     */
    if (idx == heap->count - 1)
    {
        id = heap->items[heap->count-1]->taskid;
        heap->count --;
        osx_taskpool_release( heap->taskpool, id );
    }
    else{
        id = heap->items[idx]->taskid;
        heap->items[idx] = heap->items[heap->count-1];
        heap->items[idx]->heapindex = idx;
        heap->count --;
        _osx_taskheap_siftdown(heap, idx);
        osx_taskpool_release( heap->taskpool, id );
    }
    return true;
}


int8 osx_taskheap_insert( TiOsxTaskHeap * heap, TiOsxTaskHeapItem * item )
{
    TiOsxTaskHeapItem * newitem;
    int8 id, idx;

    idx = -1;
    if (heap->count <= osx_taskheap_capacity(heap)-1)
    {
        id = osx_taskpool_apply( heap->taskpool );
        if (id >= 0)
        {
            newitem = osx_taskpool_gettaskdesc( heap->taskpool, id );
            newitem->taskfunction = item->taskfunction;
            newitem->taskdata = item->taskdata;
            newitem->priority = item->priority ;
            newitem->timeline = item->timeline;
            newitem->deadline = item->deadline;

            /*
             * 将item插到新的位置，然后对item作上升调整（交换元素）到合适的位置
             * 即当item比它父亲小（最小堆）/大（最大堆）时，把item和它的父亲交换
            */ 
            heap->items[heap->count] = newitem;
            newitem->heapindex = heap->count;

            heap->count ++;
            idx = _osx_taskheap_siftup( heap, heap->count - 1);
        }
    }
    
    return idx;
}

int8 osx_taskheap_search( TiOsxTaskHeap * heap, TiOsxTaskHeapItem * item ) 
{
    return (item->state > 0) ? item->heapindex : -1;
}


/*
bool osx_taskheap_search( TiOsxTaskHeap * heap, TiOsxTaskHeapItem * item ) 
{
    int i, j;
    bool found = false;
    TiOsxTaskItem * items = &(heap->items[0]);



    if(heap->count > 0)
    {
        Q.push_back( items[0] );
        Q1.push_back(0);

        while(!Q.empty() && !found)
        {
            T q = Q.front();
            i = Q1.front();
            j = (i<<1) + 1;

            if(q == item)
            {
                found = true;
            }
            else if((this->type == MaxHeap? q > item : q < item))
            {
                if(j < size)
                {
                    Q.push_back(arr[j]);
                    Q1.push_back(j);
                }
                if(j + 1 < size)
                {
                    Q.push_back(arr[j+1]);
                    Q1.push_back(j+1);
                }
            }

            Q.pop_front();
            Q1.pop_front();
        }
    }

    return found;
}
*/

// ??? 不明白
void osx_taskheap_sort( TiOsxTaskHeap * heap )
{
    int8 i, tmp;

    if(heap->count <= 0)
    {
        return;
    }

    for (i=(heap->count>>1); i>0; i--)
    {
        _osx_taskheap_siftdown( heap, i-1 );
    }

    tmp = heap->count;
    while(heap->count > 0)
    {
        _osx_taskheap_switch( heap, 0, heap->count-1 );
        heap->count --;
        _osx_taskheap_siftdown( heap, 0 );
    }
        
    heap->count = tmp;
}

//int8 _osx_taskheap_heapify( TiOsxTaskHeap * heap )
//{
//}
/* recursive */
/*
void Heap_Heapify(Heap* h,Heap_Index i,int Heap_Comp(Heap_Data,Heap_Data))

{

  Heap_Index l,r,m;

  Heap_Data t;

  l=HEAP_LEFT(i); r=HEAP_RIGHT(i);

  if(l<h->length && Heap_Comp(h->data[l],h->data[i])>0)

    m=l;

  else

    m=i;

  if(r<h->length && Heap_Comp(h->data[r],h->data[m])>0)

    m=r;

  if(m!=i)

  {

    t=h->data[i]; h->data[i]=h->data[m]; h->data[m]=t;

    Heap_Heapify(h,m,Heap_Comp);

  }

}*/
/* Heap_Heapify */


int8 _osx_taskheap_siftup( TiOsxTaskHeap * heap, int8 idx )
{
    int8 i = idx;
    int8 j = ((i+1) >> 1) - 1;
    TiOsxTaskHeapItem * itemptr = heap->items[idx];

    while((j >= 0) && (_osx_taskheap_compare(heap->items[j], itemptr) > 0))
    {
        heap->items[i] = heap->items[j];
        heap->items[i]->heapindex = i;
        i = j;
        j = ((i+1) >> 1) - 1;
    }

    heap->items[i] = itemptr;
    itemptr->heapindex = i;
    return i;
}

int8 _osx_taskheap_siftdown( TiOsxTaskHeap * heap, int8 idx )
{
    int i = idx;
    int j = (i<<1) + 1;
    bool finished = false;
    TiOsxTaskHeapItem * itemptr = heap->items[idx];

    while ((j < heap->count) && (!finished))
    {
        // decide the smaller child.
        if ((j + 1 < heap->count) && (_osx_taskheap_compare(heap->items[j], heap->items[j+1]) > 0))
        {
            j++;
        }
        if (_osx_taskheap_compare(itemptr, heap->items[j]) < 0)
        {
            finished = true;
        }
        else
        {
            heap->items[i] = heap->items[j];
            heap->items[i]->heapindex = i;
            i = j;
            j = (i<<1) + 1;
        }
    }

    heap->items[i] = itemptr;
    itemptr->heapindex = i;
    return i;
}

int8 _osx_taskheap_compare( TiOsxTaskHeapItem * item1, TiOsxTaskHeapItem * item2 )
{
    int8 ret = 0;

    if (item1->timeline < item2->timeline)
    {
        ret  = -1;
    }
    else if (item1->timeline > item2->timeline)
    {
        ret = 1;
    }
    else if (item1->priority > item2->priority)
    {
        ret = -1;
    }
    else if (item1->priority > item2->priority)
    {
        ret = 1;
    }
    else 
        ret = 0;

    return ret; 
}

void _osx_taskheap_switch( TiOsxTaskHeap * heap, int8 idx1, int8 idx2 )
{
    TiOsxTaskHeapItem * itemptr;
    int8 heapindex;

    itemptr = heap->items[idx1];
    heap->items[idx1] = heap->items[idx2];
    heap->items[idx2] = itemptr;

    heapindex = itemptr->heapindex;
    heap->items[idx2]->heapindex = heap->items[idx1]->heapindex;
    heap->items[idx1]->heapindex = heapindex;
}























/*



void heapify( TiOsxTaskHeap * heap );
void swap(TiOsxTaskHeap * heap,uint8 i,uint8 j);
uint8 min(TiOsxTaskHeap * heap,uint8 i,uint8 j);//比较两个存储单元关键值key，返回key值较小的单元号。
uint8 M=0xff；


task_heap_construct();//构造堆
{    
	 taskheap_init(que);
	 heapify();
}
void heapify( TiOsxTaskHeap * heap );//转化成最小堆，父亲节点关键值小于子节点
{      TiOsxTaskHeap * heap;
	   TiOsxTaskHeapItem m;
	   for(int i=CONFIG_OSX_TASKHEAP_CAPACITY/2,i>=0,i--)
	   {
		   //if(heap(i)->key<heap(heap(i)->parent)->key)
///
		   m->key=heap(i)->key;
		   m->taskdata=heap(i)->taskdata;
		   m->taskptr=heap(i)->taskptr;
		   heap(i)->key=heap(heap(i)->parent)->key;
		   heap(i)->taskdata=heap(heap(i)->parent)->taskdata;
		   heap(i)->taskptr=heap(heap(i)->parent)->taskptr;
		   heap(heap(i)->parent)->key=m->key;
		   heap(heap(i)->parent)->parent=m->taskdata;
///		   heap(heap(i)->parent)->taskptr=m->taskptr;

          //swap(TiOsxTaskHeap*heap,i,heap(i)->parent);

		   int k=min(TiOsxTaskHeap *heap,heap(i)->left,heap(i)->right);//选取两个子节点key值较小点
		   if(heap(i)->key>heap(k)->key)//如果子节点key之比父亲节点小则交换
		   swap(TiOsxTaskHeap *heap,i,k);
	   }
	   for(int p=i=CONFIG_OSX_TASKHEAP_CAPACITY/2,i>=0,i--)
	   {  
		   int k=min(TiOsxTaskHeap *heap,heap(i)->left,heap(i)->right);
	       if(heap(i)->key>heap(k)->key)
	   }
			   


}


uint8 task_heap_insert( TiOsxTaskHeap * heap, void * taskptr, void * taskdata, uint16 timestart );
{
                         TiOsxTaskHeap * heap;
						 if(heap(CONFIG_OSX_TASKHEAP_CAPACITY-1)->key=0xff)//判断数组是否已满
						 {heap(CONFIG_OSX_TASKHEAP_CAPACITY-1)->key=timestart;
						 heap(CONFIG_OSX_TASKHEAP_CAPACITY-1)->taskdata=taskdata;
						 heap(CONFIG_OSX_TASKHEAP_CAPACITY-1)->taskptr=taskptr;}
						 esle
						 {
						 //做出相应处理
						 }
						 heapify（）;
						 


}

bool task_heap_delete( TiOsxTaskHeap * heap, uint8 taskid );
{
                     TiOsxTaskHeap * heap;
						 if(heap(taskid)->key!=0xff;)
						 {
							 heap(taskid)->key=0xff;
						     heap(taskid)->taskdata=NULL;
						     heap(taskid)->taskptr=NULL;
						     while(heap(taskid)->right!=NULL||heap(taskid)->left!=NULL)
						     {
							 
								 uint8 x=min(TiOsxTaskHeap * heap,heap(taskid)->left,heap(taskid)->right);
							     swap(TiOsxTaskHeap*heap,taskid,x);
							     taskid=x;

					          }
							 return true;
						  }
						 else
							 return false;


						 .
}

void taskheap_init(TiOsxTaskHeap * heap);//将数组排成完全二叉树形式并赋予个单元初值为空
{
	TiOsxTaskHeap *heap;//数组中共8个存储单元
	for(int i=0,i<=7,i++)
	{   
		if(2*i+1<=7)
			heap(i)->left=2*i+1;
		else
			heap(i)->left=NULL;

		if(2*(i+1)<=7)
			heap(i)->right=2*(i+1);
		else
			heap(i)->right=NULL;
		if(i!=0)
			heap(i)->parent=int[i/2];
		else
			heap(i)->parent=NULL;
		heap(i)->key=M;//M是一个很大的数
		heap(i)->taskdata=NULL;
		heap(i)->taskptr=NULL;


	}
}

void swap(TiOsxTaskHeap * heap,uint8 i,uint8j);//交换两单元的内容
{            
	TiOsxTaskHeap * heap;
    TiOsxTaskHeapItem m;
	m->key=heap(i)->key;
	m->taskdata=heap->taskdata;
	m->taskptr=heap->taskptr;
	heap(i)->key=heap(j)->key;
	heap(i)->taskdata=heap(j)->taskdata;
	heap(i)->taskptr=heap(j)->taskptr;
	heap(j)->key=m->key;
	heap(j)->taskdata=m->taskdata;
	heap(j)->taskptr=m->taskptr;

}
uint8 min(TiOsxTaskHeap * heap,uint8 i,uint8 j);//返回关键值较小的单元号
{
	TiOsxTaskHeap * heap;
	if(heap(i)->key>heap(j)->key)
		return j;
	else
		return i;
}

void osx_taskheap_clear( TiOsxTaskHeap * heap );//清除堆中的数据
{
	taskheap_init();
}

uint8 osx_taskheap_remove( TiOsxTaskHeap * heap, uint8 idx );//取出指定的任务
{   TiOsxTaskHeap * heap;
	return heap(idx)->key;
   
}
uint8 osx_taskheap_removeminimal( TiOsxTaskHeap * heap );//取出关键值最小的任务
{
	TiOsxTaskHeap * heap;
	return heap(0)->key;
	
}

task_heap_destroy();
task_heap_first();
task_heap_next();
task_heap_current();

*******************************************************************************************************************
**********************************************************************************************************************
TiOsxTaskHeap QUE;
void on_timer_expired( void * object, TiEvent * e );
int main()
{   
	TiAppService1 * asv1;
	TiAppService2 * asv2;
	TiAppService3 * asv3;
	TiTimerAdapter * evt_timer;
	TiOsxTaskHeap *que
	target_init();
    led_open();
	led_on( LED_ALL );
	hal_delay( 1000 );
	led_off( LED_ALL );
	
	que=task_heap_construct();
	task_heap_insert( que, asv1_evolve ,asv1, 3 );
	task_heap_insert( que, asv2_evolve ,asv2, 5 );
	task_heap_insert( que, asv3_evolve ,asv3, 7 );

	evt_timer = timer_construct( (void *)&m_timer, sizeof(TiTimerAdapter) );
	timer_open( evt_timer, 1, on_timer_expired, (void*)g_osx, 0x01 );
	timer_setinterval( evt_timer, 5, 1 );
	timer_start( evt_timer );


	asv1 = asv1_open( &m_svcmem1, sizeof(TiAppService1) );
	asv2 = asv2_open( &m_svcmem2, sizeof(TiAppService2) );
	asv3 = asv3_open( &m_svcmem3, sizeof(TiAppService3) );

}
void on_timer_expired( void * object, TiEvent * e )
{
	TiEvent newe;
    g_count ++;
	uint8 Key=osx_taskheap_removeminimal( TiOsxTaskHeap * heap );
	
		if (g_count == Key)
		{
			newe.handler = que(0)->taskptr;
			task_heap_delete( que, 0);

		}
}



*/

