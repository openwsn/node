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

#ifndef _SVC_TOPOLOGY_4768_
#define _SVC_TOPOLOGY_4768_

#ifdef __cplusplus
extern "C" {
#endif

/* 基于MAC层实现拓扑管理
 * 用到了MAC提供的：
 * 信道质量估计，广播，邻近点发现，功率调整
 */
typedef struct{
  uint32 id;
  uint8 age;
  uin8 hop;
  uint8 txpower;
}TiNetworkNode;

typedef struct{
  uint32 endpoint1;
  uint32 endpoint2;
  uint32 distance;
  uint8 age;
}TiNetworkEdge;

typedef struct{
  TNetworkNode[20] nodes; 
  TNetworkEdge[100] edges; 
}TiNetworkTopology;

void topo_construct
void topo_destroy
void topo_evolve

topo_getneighbors (from cache)
topo_getneighbors 

topo_getlinkquality
mac_getsignalstrength
int8 mac_probe( TiOpenMAC * mac );
int8 mac_updatestatistics( TiOpenMAC * mac );
updatelinkquanlity
topo_evolve

topo_test()
{
	probe
	updatestatistics
	printf
}

svc_topoctrl

#ifdef __cplusplus
}
#endif


#endif
