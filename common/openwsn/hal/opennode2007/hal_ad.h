#ifndef _HAL_AD_H_3467_
#define _HAL_AD_H_3467_

/******************************************************************************
 * @author MaKun on 200612
 * @note
 * 	for AD transformation
 * 
 * @history
 * @modified by zhangwei on 20070101
 * 	revise source code from MaKun
 *
 *****************************************************************************/
 
#include "hal_foundation.h"

typedef struct{
  uint8 id;
  //TiFunEventHandler callback;
  //void * callback_owner;
}TiAdConversion;

TiAdConversion * ad_construct( uint8 id, char * buf, uint8 size );
void ad_configutre(TiAdConversion * ad);
void ad_destroy( TiAdConversion *ad );
void ad_start( TiAdConversion * ad, TiFunEventHandler callback, void * owner );
uint16 ad_read(TiAdConversion * ad, char * buf, uint8 size, uint8 opt );

#endif