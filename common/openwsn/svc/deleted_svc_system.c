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

TiPFunFilterExecute( state, pointer, input, output, option )

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

系统会自动寻找一个叫TUserDataObject/UserService的对象
No, 这样接口太复杂

还是老样子，系统自动寻找一个叫svc_execute()的函数，在这个函数内部，由用户自己完成create, construct, evolve, destroy, free

svc/user_execute( state, input, output, option ) 
类似于dllmain
与Filter是一样的，因此，user部分可以自然而然的也作为filter链进系统，当然也可以不纳入filter体系
与其他例外的不同是调用此函数时对象尚未创建，所以第一个参数本质上是state，这样，该函数可以根据state来分操作

如何与RTOS配合？
我们的东西可以作为RTOS的一个thread运行


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
tsys_msgsend( id, handler )
tsys_msgrecv( id )
tsys_filters
tsys_objectstore

int main()
{
	g_tsystem.construct();
	g_tapp.construct();
	g_tsystem.run();
	g_tapp.destroy();
	g_tsystem.destroy();
}


注意，system是最高层，一般可以任意的包含其他层次，但是下层不应该包含system
但是，底层要发message上来，怎么办呢？
这是通过改造svc_foundation实现的。
system启动的时候，会对svc_foundation.msgsend /msgrecv 两个指针赋值，这样就可以了。然后其他模块就可以调用msgsend/msgrecv了

_register_msgsend
_register_msgrecv

msgsend( objfrom, objto, msgid )
msgsend( msgid, handler )

注
msg处理应该在系统级实现，是每一个object都可以享受的东西，只要它提供了msghandler函数并且注册过

msg.id = ISR, HARDWARE, TIMER, OBJECT....   // system message + user defined message , depending on the value of ID
msg.objectfrom = pointer
msg.objectto = pointer

msg的处理事实上也是filter模式
最default的msghandler由最高层TiSystem提供，如果某些object希望拦截该消息，也可以提供自己的msghandler并注册给foundation，
然后在其中再调用g_system.handler执行缺省处理

TiSystem.msgdispatcher()
{
	check if there're messages;
	if have, peek one, else exit
	check: msg.id, objectfrom, objectto
   if objectto != NULL
	   call msg.handler( objectto, objectfrom, id )
   else
		call this.handler( this, objectfrom, id )
		   
}

this.handler( TiSystem, objectfrom, id )
{
	swtich(id)
	case
	case
}

作为系统基础服务，提供一个msgqueue_t
所有的对象都可以使用之，只不过TiSystem会提供缺省的msg接收处理

在ISR中可以这样写
msgsend( id, NULL, NULL )
objectto == NULL就意味着这个msg最后会给TiSystem处理

msgsend( queue, id, NULL, NULL )
在msgsend中由于带了objectto，就不必指明handler，但是由于这是C语言，可能就要采用handler(owner,...)方式，即objectto和handler都要给出

