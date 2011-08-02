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

typedef struct{
}TiObjectStore;

obst_construct
obst_destroy
obst_evolve
  call the evolvehandler(owner) of each objects in the list
obst_put( obst, evolvehandler, owner )
obst_get()
obst_itemcount
obst_items()

/* objectstore is one of the fundmental kernel component in OpenWSN architecture.
 * it mains a list of objects and capable evolve them. 
 * 
 * theorectically, the objectstore can manage the whole lifetime of the objects,
 * including their creation and free. however, openwsn is a very lightweight 
 * embedded system, so we want to simplify the resource management and let objectstore
 * maintains the object list and do evolve() only. 
 * 
 * since the openwsn developer will manage the create/free of objects themselves, 
 * the memory can be controlled well.
 *
 * [chinese translation]
 * objectstore是openwsn基础服务之一
 * 理论上讲，objectstore可以管理对象的整个生命周期包括create/free，但是在openwsn中，
 * 出于资源控制和简单性起见，objectstore仅仅管理对象的evolve，不管理create/free
 */

typedef bool (*TiPFunEvolve(void * owner))

typedef struct{
  TiPFunEvolve objlist[CONFIG_OBJSTORE_CAPACITY];
  void * ownerlist[CONFIG_OBJSTORE_CAPACITY];
}TObjectStoreItem;

typedef struct{
  
}TObjectStore;


objectstore.put()
objectstore.get
objectstore.items(idx)
objectstore.evolve


