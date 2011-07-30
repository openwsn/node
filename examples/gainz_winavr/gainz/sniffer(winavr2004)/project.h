//----------------------------------------------------//
//-------Institute Of  Computing Technology-----------//
//------------Chinese Academic  Science---------------//
//-----中国科学院计算技术研究所先进测试技术实验室-----//
//----------------------------------------------------//

/**
 * www.wsn.net.cn
 * @copyright:nbicc_lpp
 * @data:2005.11.22
 * @version:0.0.1
 * @updata:$
 *
 */

#ifndef rfj_H
#define rfj_H

 
typedef signed char int8_t;
typedef unsigned char uint8_t;
//typedef int int16_t;
typedef unsigned int uint16_t;
typedef long int32_t;
//typedef unsigned long uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;
/*typedef int16_t intptr_t;*/
typedef uint16_t uintptr_t;
typedef unsigned int size_t;
typedef int wchar_t;

//--------------------------------------------------------------------
typedef int (*_compar_fn_t)(const void *, const void *);
typedef unsigned char bool;


enum  {
  FALSE = 0, 
  TRUE = 1
};

enum  {
  FAIL = 0, 
  SUCCESS = 1
};

enum  {
  NULL = 0x0
};

enum  {
  TIMER_REPEAT = 0, 
  TIMER_ONE_SHOT = 1, 
  NUM_TIMERS = 2
};


enum  {
  LedsC_RED_BIT = 1, 
  LedsC_GREEN_BIT = 2, 
  LedsC_YELLOW_BIT = 4
};

enum  {
  Timer0_maxTimerInterval = 230
};
 
typedef struct _demo_addressfield
{
  uint16_t destination_pan;
  uint16_t destination_address;
  uint16_t source_pan;
  uint16_t source_address;
} demo_addressfield;

typedef struct _demo_datapacket
{
  uint8_t length;
  uint16_t fcf;
  uint8_t dsn;
  demo_addressfield address;
  uint16_t  payload;
 } demo_datapacket;

 
 uint8_t des_address;



#endif





