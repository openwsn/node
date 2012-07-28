#ifndef _DEVX_FOUNDATION_H_3721_
#define _DEVX_FOUNDATION_H_3721_
/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2020 zhangwei(TongJi University)
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

/*******************************************************************************
 * foundation.h
 * this is the base foundation file of all the files in the application.
 *
 * @author zhangwei on 20060906
 * @modified by zhangwei on 20060906
 * - add "signed" in the typedef of int8
 * this is because some compile will assume char as unsigned type, while here
 * we need a signed char.
 *
 * @modified by zhangwei on 2011.06.03 
 * - First created this file. It's based on the stm32f10x_conf.h in example 
 *   STM32F10x_StdPeriph_Examples/SPI/Simplex_Interrupt in STM32F10x_StdPeriph_Library 3.5.0
 *   Some contents may be modified to adapt to the current project.
 *
 * @modified by zhangwei on 2011.06.03 
 * - Revised
 ******************************************************************************/

#include "configall.h"
#include "./rtl/rtl_foundation.h"
#include "./hal/hal_foundation.h"
#include "./svc/svc_foundation.h"


#define USE_FULL_ASSERT    1

/* Exported macro ------------------------------------------------------------*/

#ifdef  USE_FULL_ASSERT
/**
 * @brief  The assert_param macro is used for function's parameters check.
 * @param  expr: If expr is false, it calls assert_failed function which reports 
 *         the name of the source file and the source line number of the call 
 *         that failed. If expr is true, it returns no value.
 * @retval None
 */
#define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
void assert_failed(uint8_t* file, uint32_t line);
#else
#define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */


#endif /* _DEVX_FOUNDATION_H_3721_ */
