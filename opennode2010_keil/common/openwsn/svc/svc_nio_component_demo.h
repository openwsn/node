

typedef struct{
	uint8 state;
	TiNioSession * session;
	TiTimer * timer;
	TiFrame buf;
}TiNioComponentDemo;

TiNioComponentDemo * nio_compdemo_open( TiNioComponentDemo * c, TiNioSession * session, TiTimer * timer )
{
	c->state = 0;
	c->session = session;
	return c;
}

void nio_compdemo_close( TiNioComponentDemo * c )
{
	/* @attention
	 * generally, the timer is shared by serveral components. so you cannot stop it
	 * except the timer belongs to the current component only.
	 */
	timer_stop( c->timer );
}

int8 nio_compdemo_rx_process( TiNioComponentDemo * c )
{
	TiFrame * f;
	
	f = fmque_front( session->rxque );
	if (f != NULL)
	{
		/*
		If this frame isnot for the current component, then 
			exit. so that the other component has the chance to process it.
		endif
		
		switch the frame type:
		case REQUEST from other node:
			do process/interpret the frame
			and sends the result RESPONSE frame into session.txque
			and return a special value to indicate the nio service to continue process the frame
			if there's no result RESPONSE, then simply exit.
		case RESPONSE for the REQUEST sent before:
			if timer expired then
				this frame is too late to reply back
				simply delete the frame from rxque front and exit
			endif
			do process/parse the frame
		case not for this layer only
			disclose the frame
			and still keep it inside rxque for further processing
		end
		*/			
	}
}

int8 nio_compdemo_tx_process( TiNioComponentDemo * c )
{
	TiFrame * f;
	
	f = fmque_rear( session->txque );
	if (f != NULL)
	{
		/*
		If this frame isnot for the current component, then 
			exit. so that the other component has the chance to process it.
		endif
		encapsulate/enclose the frame
		return special to indicate the nio service continue processing
		*/
	}
}

// must have
void nio_compdemo_evolve( TiNioComponentDemo * c, TiEvent * e )
{
	TiFrame * f;
	
	//nio_compdemo_rx_process( c );
	//nio_compdemo_tx_process( c );
	
	if (time arrived to a special value)
	{
		f = frame_open();
		f.set(...);
		f.set(...);
		do encapsulation if necessary;
		fmque_pushback( session->txque );
	}
}

// this isnot mandatory
int8 nio_send(  TiNioComponentDemo * c, TiFrame * f, uint8 option )
{
	if (!fmque_full(session->txque))
	{
		fmque_pushback( session->txque );
		nio_compdemo_tx_process( c );
		
		osx_postevent to indicate the nio service to process it, but this isnot 
			mandatory. because the nio service should check it.
		return;
	}
}

// this isnot mandatory
int8 nio_recv(  TiNioComponentDemo * c, TiFrame * f, uint8 option )
{
	if (!fmque_empty(session->rxque))
	{
		if the front frame is for this component then continue or else exit; 
		nio_compdemo_rx_process;
		if the frame still exists at the first of the session->rxque
			//disclose the frame f;  
			frame_copyfrom( f, fmque_front(session->rxque) );
			fmque_popfront( session->rxque );
		endif
	}
}

