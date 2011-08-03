#ifndef _RTL_LIGHTHEAP_H_8678_
#define _RTL_LIGHTHEAP_H_8678_

/* Reference
 * - Mark Allen Weiss, Data Structures and Algorithm Analysis in C, http://users.cis.fiu.edu/~weiss/
 *   source code downloaded from http://users.cis.fiu.edu/~weiss/dsaa_c2e/files.html
 */

typedef int TiLightHeapItem;

        struct HeapStruct;
        typedef struct HeapStruct *PriorityQueue;

        PriorityQueue Initialize( int MaxElements );
        void Destroy( PriorityQueue H );
        void MakeEmpty( PriorityQueue H );
        void Insert( TiLightHeapItem X, PriorityQueue H );
        TiLightHeapItem DeleteMin( PriorityQueue H );
        TiLightHeapItem FindMin( PriorityQueue H );
        int IsEmpty( PriorityQueue H );
        int IsFull( PriorityQueue H );

#endif _RTL_LIGHTHEAP_H_8678_
