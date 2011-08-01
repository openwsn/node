/**
 * Q: Introduction of the "nio" architecture?
 * R: "nio" means network I/O architecture. This architecture is designed for memory 
 * highly restricted embedded systems. Each time a new frame arrives, it will be 
 * push into the rxque inside acceptor component, or fetched from the transceiver 
 * into the rxque by the acceptor. Then the io service manages the frame dispatching.
 * All the network components must provide an process like function to be used by 
 * the dispatcher, and all of them are organized as a tree.
 */
 
/** 
 * @reference
 * - 顾锋磊, 软件工程师, IBM, 使用事件驱动模型实现高效稳定的网络服务器程序--几
 *   种网络服务器模型的介绍与比较 , 2010.10, http://www.ibm.com/developerworks/cn/linux/l-cn-edntwk/index.html?ca=drs-
 * - Linux 网络栈剖析 -- 从 socket 到设备驱动程序, http://blog.chinaunix.net/u1/43045/showart_373164.html
 * - windows网络模型学习--WINDOWS和LINUX开发模型比较, 2009, http://lin-style.javaeye.com/blog/343477
 * - LINUX网络协议栈实现分析（－）SKBUFF的实现, http://www.kernelchina.org/linuxkernel/sk_buff.pdf
 * - Linux内核协议栈源码分析, http://wenku.baidu.com/view/5ca649ec0975f46527d3e131.html (299 Pages)
 *     or http://ishare.iask.sina.com.cn/f/10220414.html
 */

/**
 * TiNioService
 * TiNioService denotes Network I/O Service, which provides the high level interface 
 * of networking functions in the architecture. 
 * 
 * - Other modules can operate the network through the TiNioService component. It's 
 *   the abstraction of the wireless network. NioService contains the network protocol 
 *   and service implementation inside.
 * - TiNioService performs network initialization and startup, so that the network
 *   based developing can be simplified.
 * - The TiNioService can be reconfigured through macros to adapte to some typical 
 *   network applications.
 * - Manages the dispatchers.
 * - Manages the topology data structure and channel data.
 */

typedef struct{
  TiNioAcceptor * nac;
  TiNioDispatcher * rawframe_dept;
  TiNioDispatcher * rawframe_dept;  raw_fmdept, mac_fmdept, net, apl_fmdept
  // topology;
  // channel or session
  // processor
  
}TiNioService;

application: addon endpoint

nioservice:  port

nioservice: channel

nio_service_open
nio_service_close
nio_service_evolve
nio_service_register( endpoint, component/addon );

init
	dispatcher->attachroot( acceptor )
	dispatcher->attachchild( mac )
	dispatcher->attachsilbing( nanosyn )
	dispatcher->attachleft( net )
	dispatcher->attachsilbing( topodisovery )
	dispatcher->attachsilbing( localization )
	dispatcher->attachsilbing( timesync )
	dispatcher->attachchild( addon1 )
	dispatcher->attachsibling( addon2 )
	dispatcher->attachsibling( addon3 ) + fixed id
	dispatcher->attachsibling( addon4 )
	curlayer
	
application
{
	nios_register( addon1 )
	nios_register( addon1 )
	nios_register( addon1 )
	nios_register( addon1 )

	you can also use nio_handler to move frame into an new queue for application layer
	so that you can use read/write interface
	
	nios_evolve( nio );
}




