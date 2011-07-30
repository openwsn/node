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
#include "interupter.h"


void __attribute((signal))   __vector_7(void)
{
  cc2420_fifopintr();
}

 

void __attribute((interrupt))   __vector_26(void)
{
  { hal_atomic_t _nesc_atomic = hal_atomic_start();
    {
      if (Timer3_set_flag!=0) {

	     OCR3A  = Timer3_setInterval(2000);
	     
	    //Timer3_set_flag--;
         
      }
     
      }
    hal_atomic_end(_nesc_atomic); }
    Timer3_fire();
}
 