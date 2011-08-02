
#ifdef _UNIT_TEST

#include "rtl_lightheap.h"
#include <stdio.h>

#define MaxSize (1000)

main( )
{
    TiLightPriorityQueue H;
    int i, j;

    H = lwheap_initialize( MaxSize );
    for( i=0, j=MaxSize/2; i<MaxSize; i++, j=( j+71)%MaxSize )
        lwheap_insert( j, H );

    j = 0;
    while( !lwheap_isempty( H ) )
        if( lwheap_deletemin( H ) != j++ )
            printf( "Error in lwheap_deletemin, %d\n", j );
    printf( "Done...\n" );
    return 0;
}

#endif






#include "rtl_lightheap.h"
        #include "fatal.h"
        #include <stdlib.h>

        #define MinPQSize (10)
        #define MinData (-32767)


/* START: fig6_0.txt */
        TiLightPriorityQueue
        lwheap_initialize( int MaxElements )
        {
            TiLightPriorityQueue H;

/* 1*/      if( MaxElements < MinPQSize )
/* 2*/          Error( "Priority queue size is too small" );

/* 3*/      H = malloc( sizeof( struct _TiLightHeapItem ) );
/* 4*/      if( H ==NULL )
/* 5*/          FatalError( "Out of space!!!" );

            /* Allocate the array plus one extra for sentinel */
/* 6*/      H->Elements = malloc( ( MaxElements + 1 )
                                    * sizeof( TiLightHeapValue ) );
/* 7*/      if( H->Elements == NULL )
/* 8*/          FatalError( "Out of space!!!" );

/* 9*/      H->Capacity = MaxElements;
/*10*/      H->Size = 0;
/*11*/      H->Elements[ 0 ] = MinData;

/*12*/      return H;
        }
/* END */

        void
        lwheap_makeempty( TiLightPriorityQueue H )
        {
            H->Size = 0;
        }

/* START: fig6_8.txt */
        /* H->Element[ 0 ] is a sentinel */

        void
        lwheap_insert( TiLightHeapValue X, TiLightPriorityQueue H )
        {
            int i;

            if( lwheap_isfull( H ) )
            {
                Error( "Priority queue is full" );
                return;
            }

            for( i = ++H->Size; H->Elements[ i / 2 ] > X; i /= 2 )
                H->Elements[ i ] = H->Elements[ i / 2 ];
            H->Elements[ i ] = X;
        }
/* END */

/* START: fig6_12.txt */
        TiLightHeapValue
        lwheap_deletemin( TiLightPriorityQueue H )
        {
            int i, Child;
            TiLightHeapValue MinElement, LastElement;

/* 1*/      if( lwheap_isempty( H ) )
            {
/* 2*/          Error( "Priority queue is empty" );
/* 3*/          return H->Elements[ 0 ];
            }
/* 4*/      MinElement = H->Elements[ 1 ];
/* 5*/      LastElement = H->Elements[ H->Size-- ];

/* 6*/      for( i = 1; i * 2 <= H->Size; i = Child )
            {
                /* Find smaller child */
/* 7*/          Child = i * 2;
/* 8*/          if( Child != H->Size && H->Elements[ Child + 1 ]
/* 9*/                                < H->Elements[ Child ] )
/*10*/              Child++;

                /* Percolate one level */
/*11*/          if( LastElement > H->Elements[ Child ] )
/*12*/              H->Elements[ i ] = H->Elements[ Child ];
                else
/*13*/              break;
            }
/*14*/      H->Elements[ i ] = LastElement;
/*15*/      return MinElement;
        }
/* END */

        TiLightHeapValue
        lwheap_findmin( TiLightPriorityQueue H )
        {
            if( !lwheap_isempty( H ) )
                return H->Elements[ 1 ];
            Error( "Priority Queue is Empty" );
            return H->Elements[ 0 ];
        }

        int
        lwheap_isempty( TiLightPriorityQueue H )
        {
            return H->Size == 0;
        }

        int
        lwheap_isfull( TiLightPriorityQueue H )
        {
            return H->Size == H->Capacity;
        }

        void
        lwheap_destroy( TiLightPriorityQueue H )
        {
            free( H->Elements );
            free( H );
        }

        #if 0
/* START: fig6_14.txt */
        for( i = N / 2; i > 0; i-- )
            PercolateDown( i );
/* END */
        #endif
