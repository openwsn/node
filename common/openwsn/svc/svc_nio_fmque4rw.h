
/**
 * TiNioFrameQueue4Rw provides an send/recv(read/write) interface to the network
 * service. If you don't want to develope an standard "nio" component and prefer
 * using the traditional send/recv interface, you can add this component into 
 * "nio" network component tree.
 * 
 * This component is designed to work with MAC, LLC, NET layer component. This means
 * you can use it no matter you want to build a one-to-many data gathering application
 * or an adhoc/mesh network based application.
 * 
 * @attention
 * 	- This component uses TiFrameQueue to buffer the frames received or to be sent.
 *    You should call recv() to retrieve the frame out as soon as possible to avoid
 *    lossing it.
 *  - The txque inside this component is actually an pointer. This component needn't
 *    to buffer the frame to be sent. These frames can often be passed directly 
 *    to the MAC layer or even the transceiver directly for transmission.
 */
 
typedef struct{
	TiFrameQueue * txque;
	TiFrameQueue * rxque;
}TiNioFrameQueue4Rw;

nio_fmque4rw_open
nio_fmque4rw_close

nio_fmque4rw_rxque
nio_fmque4rw_txque

nio_fmque4rw_send
nio_fmque4rw_recv

/** Called by the nio frame dispatcher/aggregator in the nio service */
nio_fmque4rw_rx_process

/** Called by the nio frame dispatcher/aggregator in the nio service */
nio_fmque4rw_tx_process

/** evolve() is to process the frames in rxque and txque. */
nio_fmque4rw_evolve


