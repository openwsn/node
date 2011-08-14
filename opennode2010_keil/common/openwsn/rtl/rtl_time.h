
#include "rtl_configall.h"
#include "rtl_foundation.h"

/**
 * C language support 32 bit time_t and 64 bit time_t. 
 */

typedef struct{
    uint8   prefix;
    uint32  value;
    uint8   suffix;
}TiTime; 

//#define TiSystemTime TiTime


/*
typedef TiTime :  64 bit structure
multiple = prefix = prefix = scale 1B 
uint32 value time_t
fraction = suffix = postfix = 1B = 尾数mantissa

或者采取类似浮点数可浮动的表示方法


convert to/from 32bit
convert to/from 64bit
convert to YMDHMS calendar format
set epoch
read
write

epoch起点从何？

map_epoch( TiTime tm, TiCalendar * cal )
*/


time_from32
time_from64
time_to32
time_to64
time_forward
time_backward
time_setepoch
// time_read( char * buf, uint8 size )
// time_write( char * buf, uint8 size )

time_read16( char * buf )
time_write16( char * buf )
time_read32( char * buf )
time_write32( char * buf )
time_read64( char * buf )
time_write64( char * buf )
time_read80( char * buf )
time_write80( char * buf )



//#define   TiSystemTime uint64
#define   TiTime8      uint8 
#define   TiTime16     uint16    
#define   TiTime32     uint32 
#define   TiTime64     uint64

<<<<<<< .minetypedef uint64 TiSystemTime;
=======void tm8_reset( uint8 * var, uint8 value );
/*
inline void tm8_clear( TiTime8 * var );
>>>>>>> .theirs
<<<<<<< .mineinline bool systime_byte_plus(int8 op1, int8 op2)
{
	if (op1 + op2)
}
=======inline TiTime8 * tm8_plus( TiTime8 * var1, TiTime8 * var2 );
>>>>>>> .theirs
<<<<<<< .mine/**
 * Plus two time value.
 * @return true if success and false is overflow.
 */
inline bool systime_plus( TiSystemTime * op1, TiSystemTime * op2 )
{
	
}
=======inline TiTime8 * tm8_minus( TiTime8 * var1, TiTime8 * var2 );
 
inline bool tm8_forward( TiTime8 * var, uint8 interval );
>>>>>>> .theirs
<<<<<<< .mine/**
 * Minus two time value.
 * @return true if success and false is overflow.
 */
inline bool systime_minus( TiSystemTime * op1, TiSystemTime * op2 )
{
}
=======inline bool tm8_backward( TiTime8 * var, uint8 interval );
>>>>>>> .theirs
<<<<<<< .mine#define systime_forward(tm,step) systime_plus(tm,step)
#define systime_backward(tm,step) systime_minus(tm,step)
=======inline void tm64_reset( TiTime64 * var, uint64 value );
>>>>>>> .theirs
inline void tm64_clear( TiTime64 * var );

<<<<<<< .mine
COMPOENT_ATTACH( COMP_A, COMP_B );

USE_COMPONENT( TiUartAdapter, uart_ );
USE_COMPONENT( T, prefix );

#define mac_send(T) prefix##_send
#define mac_recv prefix##_recv

/* interface requirement */
#define TiPhyAdapter 
TiPhyAdapter * phy_send
#define MAKE_USE_OF(type,prefix) \\
	type * ##prefix##_send   \
	prefix_recv
	
/* implementation  */

/* how to use it */
include "interface.h"

IMPL_INTERFACE_BY_COMPONENT

IMPL_PHYADAPTER_BY_COMPONENT(interface, implementation, identifer)
IMPL_PHYADAPTER_BY_COMPONENT(TiPhyAdapter, TiCc2420Adapter, cc2420)
IMPL_PHYADAPTER_BY_COMPONENT(TiPhyAdapter, TiCc2520Adapter, cc2520)
IMPL_PHYADAPTER_BY_COMPONENT;

nio_phy_send
nio_mac_send
nio_mac_recv
nio_net_send
nio_llc_send
nio_ses_send
nio_app_send



xxx_use

compa_xxx
compa_xxxx






/*********************** the following are undetermined ***********************/
=======inline TiTime64 * tm64_plus( TiTime64 * var1, TiTime64 * var2 );
>>>>>>> .theirs

inline TiTime64 * tm64_minus( TiTime64 * var1, TiTime64 * var2 ); 


inline bool tm64_forward( TiTime64 * var, uint64 interval );


inline bool tm64_backward( TiTime64 * var, uint64 interval );

*/


/*
typedef uint8[8] TiSystemTime;






typedef uint8  TiTime8;  //TiSystemTime8  epoch tick  tm8  rtl_systime  rtl_caltime  rtl  sysclock
typedef uint16 TiTime16;  //TiSystemTime32 
typedef uint32 TiTime32;
typedef uint64 TiTime64;

#define TiSystemTime TiTime64


void tm8_reset( uint8 * var, uint8 value ) {*var=value;}
inline void tm8_clear( TiTime8 * var ) {*var=0;}

inline TiTime8 * tm8_plus( TiTime8 * var1, TiTime8 * var2 )
{
	(*var1) += (*var2);
	return var1;
}

inline TiTime8 * tm8_minus( TiTime8 * var1, TiTime8 * var2 ) 
{
	(*var1) -= (*var2);
	return var1;
}

inline bool tm8_forward( TiTime8 * var, uint8 interval )
{
	// return false to indicate overflow during forward
	if (*var + interval < *var)
		return false;
	else
		return true;
}

inline bool tm8_backward( TiTime8 * var, uint8 interval )
{
	// return false to indicate overflow during forward
	if (*var - interval > *var)
		return false;
	else
		return true;
}



inline void tm64_reset( TiTime64 * var, uint64 value ) {*var=value;}
inline void tm64_clear( TiTime64 * var ) {*var=0;}

inline TiTime64 * tm64_plus( TiTime64 * var1, TiTime64 * var2 )
{
	(*var1) += (*var2);
	return var1;
}

inline TiTime64 * tm64_minus( TiTime64 * var1, TiTime64 * var2 ) 
{
	(*var1) -= (*var2);
	return var1;
}

inline bool tm64_forward( TiTime64 * var, uint64 interval )
{
	// return false to indicate overflow during forward
	if (*var + interval < *var)
		return false;
	else
		return true;
}

inline bool tm64_backward( TiTime64 * var, uint64 interval )
{
	// return false to indicate overflow during forward
	if (*var - interval > *var)
		return false;
	else
		return true;
}

*/