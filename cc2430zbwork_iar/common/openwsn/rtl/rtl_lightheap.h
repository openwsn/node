#ifndef _RTL_LIGHTHEAP_H_8678_
#define _RTL_LIGHTHEAP_H_8678_

/* Reference
 * - Mark Allen Weiss, Data Structures and Algorithm Analysis in C, http://users.cis.fiu.edu/~weiss/
 *   source code downloaded from http://users.cis.fiu.edu/~weiss/dsaa_c2e/files.html
 */

#define TiLightPriorityQueue TiLightHeap

struct _TiLightHeapItem;

struct _TiLightHeapItem{
    uintx state;
    void * data;
}

typedef struct{
    uintx capacity;
    uintx count;
    uintx itemsize;
}TiLightHeap;

lwheap_insert( item, key )
lwheap_deleteat( idx, * item )
lwheap_deletemax( *item )
lwheap_deletemin( *item )
lwheap_find( item )
lwheap_sort()
lwheap_heapify()


以下是我参考过的一篇帖子

数据结构——堆的操作和实现

当应用优先级队列或者进行堆排序时，一般利用堆来实现。堆是一个完全（除最底层

外都是满的）二叉树，并满足如下条件：


1、根结点若有子树，则子树一定也是堆。


2、根结点一定大于（或小于）子结点。


因为要求堆必须是完全二叉树，所以可以用线性的数据结构，比如数组，来实现堆。

利用数组实现，则对于长为N的堆中的元素从0到N-1排列，有：


i的父结点：Parent(i)=(i+1)/2-1


i的左叶子：Left(i)=(i+1)*2-1


i的右叶子：Right(i)=(i+1)*2


堆的操作主要以一个“堆化”（Heapify）操作为基础，调整堆中的结点后，应用堆化操

作将其下降至合适的高度，且保持堆的性质。


插入时结点时，先将结点放入堆的最后位置，再逐步提升至合适的位置即可。





以下是用C实现的堆的相关操作：
以下内容为程序代码:


int Heap_Init(Heap* h,Heap_Index size)

{

  if(!h)

    return 1;

  if(!(h->data=(Heap_Data*)malloc(sizeof(Heap_Data)*size)))

    return 1;

  h->size=size;

  h->length=0;

  return 0;

}/* Heap_Init */



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

}/* Heap_Heapify */



int Heap_Increase_Key(

  Heap* h,Heap_Index i,Heap_Data x,

  int Heap_Comp(Heap_Data,Heap_Data))

{

  if(Heap_Comp(x,h->data[i])<0)

    return 1;

  while(i>0 && Heap_Comp(x,h->data[HEAP_PARENT(i)])>0)

  {

    h->data[i]=h->data[HEAP_PARENT(i)];

    i=HEAP_PARENT(i);

  }

  h->data[i]=x;

  return 0;

}/* Heap_Increase_Key */



int Heap_Insert(Heap* h,Heap_Data x,int Heap_Comp(Heap_Data,Heap_Data))

{

  Heap_Index i;

  if(h->length >= h->size)

    return 1;

  i=h->length++;

  if(i>0 && Heap_Comp(x,h->data[HEAP_PARENT(i)])>0)

  {

    h->data[i]=h->data[HEAP_PARENT(i)];

    return Heap_Increase_Key(h,HEAP_PARENT(i),x,Heap_Comp);

  }

  else

    h->data[i]=x;

  return 0;

}/* Heap_Insert */



Heap_Data Heap_Top(Heap* h)

{

  return h->data[0];

}/* Heap_Top */



int Heap_Delete(Heap* h,Heap_Data* x,int Heap_Comp(Heap_Data,Heap_Data))

{

  if(h->length <= 0)

    return 1;

  *x=h->data[0];

  h->data[0] = h->data[--h->length];

  Heap_Heapify(h,0,Heap_Comp);

  return 0;

}/* Heap_Delete */



int Heap_Destory(Heap* h)

{

  if(!h || !(h->data))

    return 1;

  free(h->data);

  h->data=NULL;

  return 0;

}/* Heap_Destory */



int Heap_Clean(Heap* h)

{

  if(!h)

    return 1;

  h->length=0;

  return 0;

}/* Heap_Clean */



int Heap_Sort(

  Heap_Data* data,Heap_Index size,

  int Heap_Comp(Heap_Data,Heap_Data))

{

  Heap h;

  Heap_Index i;

  Heap_Data t;

  if(!data)

    return 1;

  h.length=h.size=size;

  h.data=data;

  for(i=h.length/2;i>=0;i--)

    Heap_Heapify(&h,i,Heap_Comp);

  for(h.length--;h.length>=0;h.length--)

  {

    t=h.data[0];h.data[0]=h.data[h.length];h.data[h.length]=t;

    Heap_Heapify(&h,0,Heap_Comp);

  }

  return 0;

}
return i;


/* following are obsolete */



typedef int TiLightHeapValue;

struct _TiLightHeapItem;

struct _TiLightHeapItem
{
    int Capacity;
    int Size;
    TiLightHeapValue *Elements;
};

typedef struct _TiLightHeapItem *TiLightPriorityQueue;

TiLightPriorityQueue lwheap_initialize( int MaxElements );
void lwheap_destroy( TiLightPriorityQueue H );
void lwheap_makeempty( TiLightPriorityQueue H );
void lwheap_insert( TiLightHeapValue X, TiLightPriorityQueue H );
TiLightHeapValue lwheap_deletemin( TiLightPriorityQueue H );
TiLightHeapValue lwheap_findmin( TiLightPriorityQueue H );
int lwheap_isempty( TiLightPriorityQueue H );
int lwheap_isfull( TiLightPriorityQueue H );

#endif _RTL_LIGHTHEAP_H_8678_
