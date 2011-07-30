int main()
{
	nio_service_open() = init
	
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

	nios_register( addon1 )
	nios_register( addon2 )
	nios_register( addon3 )

	you can also use nio_handler to move frame into an new queue for application layer
	so that you can use read/write interface
	
	while (1) 
	{
		nios_evolve( nio );
	}
	
	or use the following 
	osx_register( nio );
	osx_execute();
	
	nio_service_close();
}


typedef struct{
}TiAddon1;

addon1_process( );

typedef struct{
}TiAddon2;

addon2_process( );


typedef struct{
}TiAddon3;

addon3_process( );









