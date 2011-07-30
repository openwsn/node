#include "hal_shtxx.h"

/******************************************* 
        m128  uart 头文件 
   
********************************************/ 

#define UART0_RXDIEON  UCSR0B=UCSR0B|0X80 
#define UART0_RXDIEOFF UCSR0B=UCSR0B&0X7F 
#define UART1_RXDIEON  UCSR1B=UCSR1B|0X80 
#define UART1_RXDIEOFF UCSR1B=UCSR1B&0X7F 

//#if mcu_fosc=  
#define  fosc 8000000   //8M晶振11059200 
#define  baud 9600       //波特率 

/*==================================================== 
【函数原形】: void init_UART(void)         
【参数说明】:              
【功能说明】: 波特率为19200, 
【修改时间】:  2006.06.01                  
====================================================*/ 

void init_UART(void) 
{ 
//--UART0-- 
UCSR0B=(1<<RXEN0)|(1<<TXEN0);                        //允许发送接收,无中断--|(1<<RXCIE0) 
UBRR0L=(fosc/16/(baud-1))%256;                          //baud high bit 
UBRR0H=(fosc/16/(baud-1))/256;                          //baud low  bit 
UCSR0C=(1<<UCSZ01)|(1<<UCSZ00);                        //8位数据位,1位停止位(1<<USBS0)| 
//--UART1-- 
UCSR1B=(1<<RXEN1)|(1<<TXEN1);        //                //允许发送接收,无中断---|(1<<RXCIE1) 
UBRR1L=(fosc/16/(baud-1))%256;                          //baud high bit 
UBRR1H=(fosc/16/(baud-1))/256;                          //baud low  bit 
UCSR1C=(1<<UCSZ11)|(1<<UCSZ10);//                  //8位数据位,1位停止位(1<<USBS1)| 
} 
/*==================================================== 
【函数原形】: void UART_TXD(uint8_t UARTdata) 
【参数说明】:              
【功能说明】:?M48?UART 发送数据      
【修改时间】:            2006.06.01                  
====================================================*/ 
void UART_TXD(uint8_t UARTdata) 
{ 
while(!(UCSR0A&(1<<UDRE0)));  // 
UDR0=UARTdata; 
} 
void UART_TXD1(uint8_t UARTdata1) 
{ 
while(!(UCSR1A&(1<<UDRE1)));  // 
UDR1=UARTdata1; 
} 
/*==================================================== 
【函数原形】: void UART_RXD(uint8_t UARTdata) 
【参数说明】:              
【功能说明】:?M48?UART 接收数据      
【修改时间】:            2006.06.01                  
====================================================*/ 
uint8_t UART_RXD(void) 
{ 
while(!(UCSR0A&(1<<RXC0))); 
return (UDR0); 
}/**/ 


uint8_t UART_RXD1(void) 
{ 
while(!(UCSR1A&(1<<RXC1))); 
return (UDR1); 
} 

/******************************************* 
                   main 
mcu:m128 
fosc: 8M 
*******************************************/ 
#include "avr/io.h" 
#include "stdio.h" 
#include "UART.h" 
#define  uchar unsigned char 
#define  uint unsigned  int 
//---------------------- 
#define IO_PF 
#define IO_DATA  0 
#define IO_SCK  1 
#include"temp_humi.h"
//#include"adc_sencor.h" 
int main(void) 
{ 
double tp=0,hi=0,voi=0; 
//int tp=0,hi=0; 
init_UART(); 
fdevopen(UART_TXD,0,0);                         //printf 指向UART_TXD(); 
while(1) 
{  
 temp_humi_test(&tp,&hi); 
 //read_data_task(&voi,);	
 printf("Temp=%5.1f'C   Humi=%5.1f%%",tp,hi); //用串口在PC机上监视 
 printf(" "); 
 //delay(3000); 
} 
}
