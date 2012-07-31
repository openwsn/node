/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 *
 * OpenWSN is a free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 *
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 *
 ******************************************************************************/
/******************************************************************************
 * rtl_random
 * This module implements a random generator.
 * 
 * @author Sun Qiang(TongJi University) on 20091028
 *	- first created.
 * 
 * @history
 * @modified by zhangwei on 20091102
 *	- revision
 * @modified by Sun Qiang(TongJi University) on 20091103
 *	- Text & function formation
 * @modified by Sun Qiang(Tongji University) on 20091110
 *  - revision
 * @modified by Sun Qiang(Tongji University) on 20091112
 *  - revision
 * @modified by Sun Qiang(Tongji University) on 20091123
 *  - remove higher level statistic functions to the new module rtl_stochastic
 *
 *****************************************************************************/ 

/* Reference
 * - 纯C代码的随机数发生器, http://www.skywind.name/blog/?p=94
 * - 密码学里的随机数发生器, http://www.xfocus.net/articles/200209/451.html
 * - 伪随机数, http://zh.wikipedia.org/w/index.php?title=伪随机数&variant=zh-cn
 * - Intel 随机数发生器白皮书, http://developer.intel.com/design/security/rng/CRIwp.htm
 * - /dev/random 源代码, http://www.openpgp.net/random/
 * - Yarrow 源代码, http://www.counterpane.com/Yarrow0.8.71.zip
 * - Yarrow-160:关于Yarrow密码伪随机数发生器的设计和分析笔记, http://www.counterpane.com/yarrow-notes.html
 */

#include "rtl_random.h"

const uint16 RAND_MAX16 = 0x7FFF;
static uint16 m_rand_next = RANDOM_INITIAL_SEED;

/******************************************************************************
 * basic random generator functions
 *****************************************************************************/ 

/* Initialize the seed of the random generator. By default, the seed is 1. 
 * Generally, the user should change the seed.
 */ 
void rand_open( uint16 n )
{
	m_rand_next = n;
}

void rand_close()
{
	return;
}

/* Return a random number betweem 0 and RAND_MAX [0,RAND_MAX)
 * x_next=a*x_0+b; a & b are integers. 
 * The implement of the function of srand
 */
uint16 rand_read()
{

	m_rand_next = m_rand_next * 0x6781 + 0x1532;

    // return (unsigned int)(m_rand_next/(2 * (RAND_MAX16 +1L)) % (RAND_MAX16+1L));

    //(m_rand_next/((RAND_MAX16+1L)<<1) % (RAND_MAX16+1L)); 
    return (unsigned int) m_rand_next % RAND_MAX16;
}

/* return a random number in [0, n). The value range includes 0 but excludes n.*/

uint8 rand_uint8( uint8 n )
{
	return (uint8)rand_read() % n;
}

uint16 rand_uint16( uint16 n )
{
	return rand_read();
}

uint32 rand_uint32( uint32 n )
{
	uint32 val;
	val = (((uint32)rand_read())<<16) | ((uint32)rand_read());
	return (val % n);
}

/* return a float type random value between 0 and 1 [0,1) */
float rand_float()
{


	return ((float)rand_read() / (float) RAND_MAX16);
}

/* return a double type random value between 0 and 1 [0,1) */
double rand_double()
{

	return ((double)rand_read() / (double) RAND_MAX16);

}
