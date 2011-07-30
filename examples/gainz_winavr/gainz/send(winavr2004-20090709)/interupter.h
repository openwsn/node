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
#ifndef interupter_h
#define interupter_h

#include "timer3.h"
#include "uart0.h"
 
	void __attribute((signal))   __vector_7(void);
 
       void __attribute((interrupt))   __vector_26(void);
 
#endif 