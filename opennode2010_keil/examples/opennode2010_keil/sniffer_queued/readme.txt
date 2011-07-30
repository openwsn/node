sniffer_queued

This is an improved sniffer. Different to the basic sniffer_ascii and sniffer_binary project, this project further add a frame queue in the application (provided by the acceptor component). This queue can greatly decrease the probablity of frame lossing at the sniffer node. 

attention: If the sniffer is busy at output the previous frame to the computer through the slow UART, the continuous frame may be lost during this period.


@todo 

You'd better change the uart output to interrupt driven mode, which can further improve the outputing performance. However, currently seems problems.... :)
