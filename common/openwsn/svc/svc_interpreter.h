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

#ifndef _SVC_INTERPRETER_H_6489_
#define _SVC_INTERPRETER_H_6489_

#include "svc_configall.h"
#include <string.h>
#include "svc_foundation.h"

#ifdef __cplusplus
extern "C" {
#endif

负责帧内格式解析

/* svc_interpreter
 * 数据帧解释执行组件
 * 解释的依据就是frame中的供service层次使用的command字节。
 * interpreter组件在收到frame arrival事件后，会从MAC层读取该frame，然后对其进行处理：
 * - 如果该frame是给自己的(根据frame中的目标地址判断)，则进一步作如下处理
 *   o 如果携带拓扑控制信息，则交给topology control组件处理
 *   o 如果携带位置信息，则交给localization组件处理
 *   o 如果携带同步数据，则交给time synchronization组件处理
 *   o 如果携带其它可识别的命令，则交给对应模块处理
 * - 如果该frame不是给自己的，则从route table中找到下一跳结点转发出去或者广播
 *   出去。
 */

/* Q: interpreter组件如何拿到MAC层发出的frame arrival事件？
 * R: 两种方式
 * - interpreter调用MAC的setnotifier注册一个侦听器，MAC在收到数据包后会调用该
 *   listener。listener就是一个常见的TiFunEventHandler类型的函数指针。interpreter
 *   的状态演化函数接口evolve()即可承担此功能，而不必特别处理。
 *   => 但是，在这种方式下，evolve()很可能是在中断态下运行的，这可能运行很长时间
 *   很不好
 * - 利用MAC层自带的缺省的listener回调函数，该listener会将frame arrival事件push
 *   到osx kernel的系统事件队列中，并由osx kernel最终将该事件event发给interpreter
 *   解释执行。这种方式通过kernel中转了1次，可将费时的interpreter evolve()执行
 *   拿到中断态外执行，在实际中优先采用，不容易出问题。
 *
 *　 attention: 注册listener的过程由主程序承担，这样interpreter可以不去管底层究竟
 *   采用哪一个MAC，即对象组件之间的粘合由主程序承担。
 *
 * Q: interpreter组件内部是否提供frame缓冲?
 * R: wireless sensor node资源有限，不在interpreter中提供数据包缓冲，内部只包含
 *   一个待处理的frame，而且处理是同步的（意味着传送指针即可，避免了frame copy）。
 *   整个openwsn内部，只在两个地方提供frame缓冲，一个是MAC内部，为了避免到达的
 *   数据包丢失，一个是net组件，协助提供QoS。这样做是为了简化设计与实现。
 */

/*
interpreter
interpreter_construct
interpreter

intpter_
*/

#ifdef __cplusplus
}
#endif

#endif
