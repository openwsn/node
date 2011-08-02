/**
 * @author openwsn on 2010.12.25
 * @links rtl_time
 * @state 
 * 	- interface done
 * 
 * @modified by zhangwei on 2010.12.25
 * 	- finished developing of the interface file
 */

#include "rtl_configall.h"
#include "rtl_foundation.h"
#include "rtl_time.h"
#include "rtl_datetime.h"

 
/** 
 * TiTimeBuf (Time Buffer)
 * This isn't an actual type. It denotes an buffer containing time information. 
 * This buffer is usually passed between different service accross network. The 
 * standard time buffer structure is as the following:
 * 
 * [Date 4B]{[Time 4B]{ [ms 2B] {[us 2B] {[ns 2B] [ps 2B]}}}}
 * [TiDate 4B]{[TiTime 4B]{ [uint16 ms 2B] {[uint16 us 2B] {[uint16 ns 2B] [uint16 ps 2B]}}}}
 * or 
 * [TiTime64 8B]{[uint16 ms 2B] {[uint16 us 2B] {[uint16 ns 2B] [uint16 ps 2B]}}}
 */
 typedef TiTimeBuf32 char[4];
 typedef TiTimeBuf64 char[8];
 typedef TiTimeBuf80 char[10];
 typedef TiTimeBuf96 char[12];
 typedef TiTimeBuf128 char[14];
 typedef TiTimeBuf144 char[16];
 
/** 
 * Considering the distributed measurement system may sample more than 1000 times
 * in one second (namely, >1000Hz), we use TiTimeBuf96 as our default choice. For
 * example when passing time information between an wireless sensor and the gateway.
 */
 #define TiTimeBuf TiTimeBuf96

/**
 * functions to help to operate the time buffer (TiTimeBuf). 
 * @warning: The following functions don't check whether the returned pointer 
 * is valid or not.
 */  
inline TiDate * tmbuf_dateptr(char * buf) {return (TiDate *)buf;}
inline TiTime * tmbuf_timeptr(char * buf) {return (TiTime *)(buf+4);}
inline TiDateTime * tmbuf_datetimeptr(char * buf) {return (TiDateTime *)buf;}
inline TiTime64 * tmbuf_time64ptr(char * buf) {return (TiTime64 *)buf;}
 
inline uint16 * tmbuf_msptr(char * buf) {return (TiTime *)(buf+8);}
inline uint16 * tmbuf_usptr(char * buf) {return (TiTime *)(buf+10);}
inline uint16 * tmbuf_nsptr(char * buf) {return (TiTime *)(buf+12);}
inline uint16 * tmbuf_psptr(char * buf) {return (TiTime *)(buf+14);}
 
/* 
inline void tmbuf_set_date( char * buf, TiDate * date ) { memmove(buf, date, 4); }
inline void tmbuf_get_date( char * buf, TiDate * date ) { memmove(date, buf, 4); }
inline void tmbuf_set_time( char * buf, TiTime * time ) { memmove(buf, time, 4); }
inline void tmbuf_get_time( char * buf, TiTime * time ) { memmove(time, buf, 4); }
inline void tmbuf_set_datetime( char * buf, TiDateTime * dt ) { memmove(buf, dt, 8); }
inline void tmbuf_get_datetime( char * buf, TiDateTime * dt ) { memmove(dt, buf, 8); }

inline void tmbuf_set_ms( char * tmbuf, uint16 value ) { *(uint16*)(tmbuf+8)=value; }
inline uint16 tmbuf_get_ms( char * tmbuf ) { return *(uint16*)(tmbuf+8); }
inline void tmbuf_set_us( char * tmbuf, uint16 value ) { *(uint16*)(tmbuf+10)=value; }
inline uint16 tmbuf_get_us( char * tmbuf ) { return *(uint16*)(tmbuf+10); }
inline void tmbuf_set_ns( char * tmbuf, uint16 value ) { *(uint16*)(tmbuf+12)=value; }
inline uint16 tmbuf_get_ns( char * tmbuf ) { return *(uint16*)(tmbuf+12); }
inline void tmbuf_set_ps( char * tmbuf, uint16 value ) { *(uint16*)(tmbuf+16)=value; }
inline uint16 tmbuf_get_ps( char * tmbuf ) { return *(uint16*)(tmbuf+16); }
*/

/**
 * TiTimeBufDescriptor
 * This is an helper object to help manipulate time buffer(TiTimeBuf) efficiently
 * and safely. The standard time buffer function don't check the buffer length,
 * so it's less safety than manipulate the buffer through an descriptor object.
 * Furthermore, the descriptor helps to manipuate the date inside time buffer 
 * through memory pointers, so it's more efficient.
 */
typedef struct{
  char * tmbuf;
  uint8 len;
  TiDate * dateptr;
  TiTime * timeptr;
  uint16 * msptr;
  uint16 * usptr;
  uint16 * nsptr;
  uint16 * psptr;
}TiTimeBufDescriptor;

inline TiTimeBufDescriptor * tmbdesc_format( TiTimeBufDescriptor * desc, char * tmbuf, uint8 len )
{
	uint8 count=0;
	
	memset( desc, 0x00, sizeof(TiTimeBufDescriptor) );
	
	desc->dateptr = (TiDate *)tmbuf;
	count += 4;
	if (count < len)
	{
		desc->timeptr = (TiTime *)(tmbuf + count);
		count += 4;
		if (count < len)
		{
			desc->msptr = (uint16*)(tmbuf + count);
			count += 2;
			if (count < len)
			{
				desc->usptr = (uint16*)(tmbuf + count);
				count += 2;
				if (count < len)
				{
					desc->nsptr = (uint16*)(tmbuf + count);
					count += 2;
					if (count < len)
					{
						desc->psptr = (uint16*)(tmbuf + count);
					}
				}
			}
		}
		
		return desc;
	}
	else
		return ((len < 4) ? NULL : desc);
}

inline TiTimeBufDescriptor * tmbdesc_parse( TiTimeBufDescriptor * desc, char * tmbuf, uint8 len )
{
	return tmbdesc_format(desc,tmbuf,len);
}

inline TiDate * tmbdesc_dateptr(TiTimeBufDescriptor * desc) {return desc->dateptr;};
inline TiTime * tmbdesc_timeptr(TiTimeBufDescriptor * desc) {return desc->timeptr;};
inline TiDateTime * tmbdesc_datetimeptr(TiTimeBufDescriptor * desc) {if (desc->timeptr) return (TiDateTime*)(desc->dateptr); else return NULL;};
inline TiTime64 * tmbdesc_time64ptr(TiTimeBufDescriptor * desc) {if (desc->timeptr) return (TiTime64*)(desc->dateptr); else return NULL;};
inline uint16 * tmbdesc_msptr(TiTimeBufDescriptor * desc) {return desc->msptr;};
inline uint16 * tmbdesc_usptr(TiTimeBufDescriptor * desc) {return desc->usptr;};
inline uint16 * tmbdesc_nsptr(TiTimeBufDescriptor * desc) {return desc->nsptr;};
inline uint16 * tmbdesc_psptr(TiTimeBufDescriptor * desc) {return desc->psptr;};

