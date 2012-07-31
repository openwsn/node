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
#include "svc_configall.h"
#include <string.h>
#include "svc_foundation.h"
#include "svc_route.h"

/* svc_route
 * 路由组件
 * 路由组件在收到frame arrival事件后，会从MAC层读取该frame，然后对其进行处理：
 * - 如果该frame是给自己的(根据frame中的目标地址判断)，则进一步作如下处理
 *   o 如果携带拓扑控制信息，则交给topology control组件处理
 *   o 如果携带位置信息，则交给localization组件处理
 *   o 如果携带同步数据，则交给time synchronization组件处理
 *   o 如果携带其它可识别的命令，则交给对应模块处理
 * - 如果该frame不是给自己的，则从route table中找到下一跳结点转发出去或者广播
 *   出去。
 */


