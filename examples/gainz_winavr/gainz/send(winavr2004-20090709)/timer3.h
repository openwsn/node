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
#ifndef rftimer3_H
#define  rftimer3_H
#include "top.h"
//#include "rfdev.h"

       uint8_t Timer3_set_flag;
	uint8_t Timer3_mscale;
	uint8_t Timer3_nextScale;
	uint16_t Timer3_minterval;
	void       Timer3_intDisable(void);
	uint8_t  Timer3_fire(void);
	uint8_t  Timer3_setIntervalAndScale(uint16_t interval, uint8_t scale);
	void  Timer3_sethalfsymbol(uint16_t symbols);
   	uint16_t Timer3_setInterval(uint16_t interval);
		
#endif
