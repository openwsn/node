rxbuf, txbuf, rxoutput, txinput


iof_aloha_open( rxque, txque /rxbuf, txbuf)

onmsgrecv( mac, session, msg ) / session.write=send
cont = iof_aloha_rxhandle( mac, rxbuf, txbuf )
cont = iof_aloha_txhandle( mac, rxbuf, txbuf )
cont = iof_aloha_write/send( mac, txbuf )


iof_aloha_evolve( mac, e )
{
	switch (e)
	{
		FRAME_ARRIVED
			if data 
				generate a ack and put it into txbuf
				return not contiue;
			else if ack
				remove the data frame temporarily in aloha service
			else
				simply ignore them
		TIMEOUT
			if wait for ack too long then discard the frame inside buffer. timer should be stopped
			if in the ack waiting time, then simply wait
		SENDING
			if output buffer is empty, then put the txbuf into txbuf and modify default frame settings
				if this is the dataframe, then you should also start the timer
			if output buffer is full, then simply ignore
}





iof_flood_evolve( net, e )
{
	switch (e)
		FRAME_ARRIVAL
			if the frame is for itself, then 
				put it to the output buffer
				return continue to passed on
			else
				try put it into the tx queue
			end


		SEND
			if output buffer is empty, then put the txbuf into txbuf and modify default frame settings
				if this is the dataframe, then you should also start the timer
			if output buffer is full, then simply ignore
}


