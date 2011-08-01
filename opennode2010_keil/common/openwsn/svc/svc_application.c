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
 
 apc_init
 apc_attach( addon )
 apc_detach( addon )
 apc_evolve()
 
 /**
  * TiApplication
  * The application component maintains an component tree and data frame dispatching 
  * rules. 
  */
  
apc_init()
{
	tree root is always the network acceptor
	and the application component itself will do frame dispatching between mac component
		and time sync component
	mac child is frame dispatching 
	and this dispatching dispatch frames to other components
	and ontop of net layer, also an dispatching
	
	so totally 3 dispatching frame possible. but the above two may be one or less.
}
 
tree形的结构似乎还是不能很好的解决network protocol stack这个特例
tree应该属于osx的维护范畴较好, tree与event routing path可以很好结合

TiFrameDispatcher对象
所有的mac,net,localization,high level time sync等在framedispatcher前都是平等的，没有层次之分
层次体现在数据的动态处理过程中，而不是静态结构上

这样也可以保持最高的灵活性
系统可以提供各种各样的FrameDispatcher,因为这里面包含一些特化的功能。固然可以再通用化，比如
TiMacBasedFrameDispatcher
TiNetBasedFrameDispatcher
TiNetAcceptorBasedFrameDispatcher
但是感觉有点复杂，这些东西应该是具体变量，尽量不要做为一个独立的类型出现，以维护TiFrameDispatcher的通用型

TiFrameDispatcher

open( choice_offset )
attach
detach
evolve

因为效率不是最关键的，所以我们可以规定：
每次都必须处理完rxque中的front后，才可以有下一步的动作，不允许session交叉执行。

application_evolve()
{
	dispatcher d[3] = {TiNetAcceptorBasedFrameDispatcher, TiMacBasedFrameDispatcher, TiNetBasedFrameDispatcher }
	f = acceptor.front of rxque;
	do {
		d = dispatcher[i];
		d.dispatch( f );
		if f is not the front frame then 
			break;
		endif
		i++ until reach the last dispatcher
	}
}

但是这种设计固定下来只能是三级，实际中也够用，但是略显不灵活

apc_attach( layer=0,1,2, component )
dispatcher_attach / dpt_attach

 
 
rxque的问题很好的解决了
但是txque的问题呢？需要逐层封装frame， 怎么办？每个component并不明确知道自己下一层是哪个对象在负责传输
况且是高度组件化的。由哪个对象完成底层传输上层应该不要去管

=> 完全借助txque.tail

apc检查txque.tail item, 如果发现frame的当前layer不是最底层---应该是acceptor或frame有能力判断，
那么就逐层调用通信组件，直至被acceptor完全接受。但这样子做，每次只能发送1个frame
这在实际中是不行的，因为可能一次发送要求发多个，例如某些接近类似broadcast的行为

是不是需要在应用层单独开辟临时的txbuf? 

还有，就是这种设计无法处理一个frame包含两种信息需多个组件连续处理的情况
dispatcher除了dispatch之外，还要看看处理结果的返回值，是否需要在component list中继续向后move
而不是dispatch一个之后就结束 
 

/* TiSystem is the highest level object in an application. it has the 
 * following functions:
 * - receive messages from other objects and environment and call corresponding 
 *	 message handlers. the message source include other objects, interrupt service
 *	 handler, RTC, Timer, bytes arrived from UART, packets arrived from wireless,
 *   scheduler object.
 * - call the evolve() function of other objects. this will drive the state 
 *   transfer of other objects.
 * - call the extern function/class, which is the entrance of user defined functions.
 *	 (like the main() function which is the entry function of the whole application)
 *
 * the openwsn developer must provide it's own app class. this class is named as 
 * TiApplication, and it should provide three interface functions:
 * - construct();
 * - destroy();
 * - evolve();
 */

typedef struct{
}TiSystem;

TiApplication

TiPFunConstruct
TiPFunDestroy
TiPFunEvolve
TiPFunFilterExecute

/* g_system: pre-defined global object */
g_tsys.attach( &startobject )


svc_construct()
svc_evolve()
svc_destroy()

typedef struct
}TiObjectInterface;

run()
{
	g_system.construct
	g_system.put(udo);
}

g_system.registerservice( udo_create, udo_construct, udo_evolve, udo_destroy, udo_free );

udo_create
udo_construct
udo_evolve
udo_destroy
udo_free




/* @attention
 * in OOP design and implementation, every object shuold be able to contain their 
 * message queue and has message handlers. however, in openwsn project, only the 
 * TiSystem object has message queue. this is intended to simplify the system 
 * design and data route.
 */ 

/* @attention
 * msgsend() is elaborately designed and it can be called inside ISR functions 
 * and any other objects. 
 */ 
tsys_create()
tsys_construct()
tsys_destroy()
tsys_free()
tsys_run()
tsys_trigger(msg)
tsys_msghandler()
tsys_msgsend
tsys_msgrecv

int main()
{
	g_tsystem.construct();
	g_tapp.construct();
	g_tsystem.run();
	g_tapp.destroy();
	g_tsystem.destroy();
}
