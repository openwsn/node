#ifndef _RTL_SERIALIZE_H_7899_
#define _RTL_SERIALIZE_H_7899_
/*******************************************************************************
 * rtl_serialize.c
 * an simple serialization module
 * support some ordinary data types, but not including structures and objects. 
 *  
 * @author zhangwei on 2009.02.22
 *  - first created
 * @modified by zhangwei on 2009.05.01
 *  - revision. compile passed
 ******************************************************************************/

/* include <stdint.h> t enable the uintN_t types */

#include "rtl_configall.h"
#include <stdint.h> 
#include "rtl_foundation.h"

#define pack754_32(f) (pack754((f), 32, 8))
#define pack754_64(f) (pack754((f), 64, 11))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))

long long pack754(long double f, unsigned bits, unsigned expbits);
long double unpack754(long long i, unsigned bits, unsigned expbits);

void packi16(unsigned char *buf, unsigned int i);
void packi32(unsigned char *buf, unsigned long i);

unsigned int unpacki16(unsigned char *buf);
unsigned long unpacki32(unsigned char *buf);

size_t pack(unsigned char *buf, char *format, ...);
void unpack(unsigned char *buf, char *format, ...);


#ifdef TEST01
int test01(void);
#endif

#ifdef TEST02
int test02(void);
#endif

#ifdef TEST03
int test03(void);
#endif

#endif /* _RTL_SERIALIZE_H_7899_ */

