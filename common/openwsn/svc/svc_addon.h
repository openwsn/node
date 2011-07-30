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
 
/**
 * Defines the interface of an application layer addon. An node can accomodate multiple 
 * addons, which is similar to the "profile" in zigbee design. By this way, an node
 * can have different functions. The node secondary developing is mainly the addon 
 * developing because the total application architecture has already been provided
 * by the openwsn system.
 */
 
 typedef struct{
   void * txque;
   void * rxque;
   TiFunAddonInit( void * txque, void * rxque );
   TiFunAddonEventHandler evolve;
   void * parent;
   void * next;
 }TiAddonInterface;
 
 /* The addon's evolve function is called by the osx kernel or the application framework
  * periodically. And the addon itself only need to respond coming events. For example, 
  * if it got an DATA_READY event, then it can access the front item(request item) 
  * in the rxque to get the item, and push the response item into the txque.
  */
 
 理想情况下，每个addon/component 都应该有自己的txque/rxque
 但是，我们作为high restricted embedded system，只能共享全局的txque/rxque，以最大限度降低memory消耗，或者开辟少数几个que
 
 rxque and txque: stream based 
 
 
 
 process( TNioSession * session )
 session provides two queue for send/recv
 
 
 