

typedef struct{
	uint8 state;
	char txbuf[20];
	char rxbuf[20];	
}TiNioApp1;

void app1_evolve( TiNioApp1 * app, TiEvent * e );
