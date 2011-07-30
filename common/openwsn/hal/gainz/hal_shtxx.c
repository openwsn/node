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

#include "../hal_shtxx.h"
#include "../hal_assert.h"

TiShtSensor * sht_construct( void * mem, uint16 size )
{
	memset( mem, 0x00, size );
	hal_assert( sizeof(TiShtSensor) <= size );
	return (TiShtSensor *)mem;
}

void sht_destroy( TiShtSensor * sht )
{
	sht_close( sht );
}

TiShtSensor * sht_open( TiShtSensor * sht, uint8 id )
{
}

void sht_close( TiShtSensor * sht )
{
}


char sht_write_byte( TiShtSensor * sht, unsigned char value )
{
}

char sht_read_byte( TiShtSensor * sht, unsigned char ack )
{
}

void sht_reset( TiShtSensor * sht )
{
}

void sht_value( TiShtSensor * sht, float *tp, float *hi )
{
}





#ifdef  IO_PA   
  #define DIR_   DDRA 
  #define IO_    PORTA 
  #define DAT_   PINA 
#endif  
#ifdef  IO_PB   
  #define DIR_   DDRB 
  #define IO_    PORTB 
  #define DAT_   PINB 
#endif  

#ifdef  IO_PC   
  #define DIR_   DDRC 
  #define IO_    PORTC 
  #define DAT_   PINC 
#endif   

#ifdef  IO_PD   
  #define DIR_   DDRD 
  #define IO_    PORTD 
  #define DAT_   PIND 
#endif   

#ifdef  IO_PE   
  #define DIR_   DDRE 
  #define IO_    PORTE 
  #define DAT_   PINE 
#endif   

#ifdef  IO_PF   
  #define DIR_   DDRF 
  #define IO_    PORTF 
  #define DAT_   PINF 
#endif   

#ifdef  IO_PG   
  #define DIR_   DDRG 
  #define IO_    PORTG 
  #define DAT_   PING 
#endif   
/**/ 
#ifndef  IO_DATA   
  #define DIR_   DDRC 
  #define IO_    PORTC 
  #define DAT_   PINC 
  #define IO_DATA   0   
  #define IO_SCK    1     
#endif   

//========================================================================== 
#define  _nop_()  asm volatile( "nop \t" "nop \t" "nop \t" "nop \t" "nop \t" "nop \t" "nop \t" "nop \t""nop \t" "nop \t"        "nop \t" "nop \t" "nop \t": : ) 
#define        DATA_in     DIR_&=~(1<<IO_DATA);IO_|=(1<<IO_DATA)//set in and release DATA-line     
#define        DATA_out    DIR_|=(1<<IO_DATA)                   //set out  

#define        DATA_in_N   (DAT_&(1<<IO_DATA))// 
#define        DATA_out_1  IO_|=(1<<IO_DATA) 
#define        DATA_out_0  IO_&=~(1<<IO_DATA) 

#define        SCK_1           DIR_|=(1<<IO_SCK);IO_|=(1<<IO_SCK) 
#define        SCK_0           DIR_|=(1<<IO_SCK);IO_&=~(1<<IO_SCK) 
/* 
#define        DATA_in     DDRC&=~(1<<0)//in 
#define        DATA_out    DDRC|=(1<<0) //out  

#define        DATA_in_N   (PINC&(1<<0))// 
#define        DATA_out_1  PORTC|=(1<<0) 
#define        DATA_out_0  PORTC&=~(1<<0) 

#define        SCK_1           PORTC|=(1<<1) 
#define        SCK_0           PORTC&=~(1<<1) 

*/ 
#define noACK 0 
#define ACK   1 
                            //adr  command  r/w 
#define STATUS_REG_W 0x06   //000   0011    0 
#define STATUS_REG_R 0x07   //000   0011    1 
#define MEASURE_TEMP 0x03   //000   0001    1 
#define MEASURE_HUMI 0x05   //000   0010    1 
#define RESET        0x1e   //000   1111    0 

/* todo: please replace the delay functions with the new version in hal_cpu */

/* 新增如下两个函数提供延迟功能，请代替文中的大量nop,让时间可控 */
static inline void cpu_delay250ns();
static inline void cpu_delay(uintx msec); 


/* attention
 * _cpu_delay250ns() and _cpu_delay() depend on specific hardware. the current
 * settings only adapt to ICT GAINZ node because it uses Atmega128 MCU running 
 * with 8MHz oscillator. 
 */
void cpu_delay250ns() 
{
	asm volatile  ("nop" ::);
	asm volatile  ("nop" ::);
}

/* parameter
 *	- msec    milli seconds
 */
void cpu_delay(uintx msec) 
{
	while (msec > 0) 
	{
		asm volatile  ("nop" ::);
		asm volatile  ("nop" ::);
		asm volatile  ("nop" ::);
		asm volatile  ("nop" ::);
		asm volatile  ("nop" ::);
		asm volatile  ("nop" ::);
		asm volatile  ("nop" ::);
		asm volatile  ("nop" ::);
		msec --;
	}
}


/*==================================================== 
【函数原形】:char s_write_byte(unsigned char value) 
【参数说明】:  
【功能说明】: 
【修改时间】:  2007.01.09                  
====================================================*/ 
char s_write_byte(unsigned char value) 
//---------------------------------------------------------------------------------- 
// writes a byte on the Sensibus and checks the acknowledge  
{   
  unsigned char i,error=0;   
   DATA_out;//out data 
   DATA_out_1; 
   SCK_0; 
   _nop_(); 
   SCK_1;                   //Initial state 
   _nop_(); 
   DATA_out_0; 
   _nop_(); 
   SCK_0; 
   _nop_();         
   SCK_1; 
   _nop_(); 
   DATA_out_1; 
   _nop_(); 
   SCK_0; 
   _nop_();_nop_(); _nop_();     
          
 for (i=0x80;i>0;i/=2)               //shift bit for masking 
  {  
  if (i & value) DATA_out_1;            //masking value with i , write to SENSI-BUS 
    else {DATA_out_0;}                         
          _nop_();_nop_();  
    SCK_1;                                        //clk for SENSI-BUS 
    _nop_();_nop_();                               //pulswith approx. 5 us           
    SCK_0; 

  }   // DATA_out_1;  //release DATA-line     
  _nop_();_nop_(); 
  SCK_1;DATA_in; 
  error=DATA_in_N;                 //clk for SENSI-BUS        
  _nop_();_nop_();                               //pulswith approx. 5 us           
  SCK_0; 

  return error;                   //error=1 in case of no acknowledge 
} 

/*==================================================== 
【函数原形】:char s_read_byte(unsigned char ack) 
【参数说明】:  
【功能说明】: 
【修改时间】:  2007.01.09                  
====================================================*/ 
char s_read_byte(unsigned char ack) 
//---------------------------------------------------------------------------------- 
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1"  
{  
  unsigned char i,val=0; 
//DATA_out; 
//DATA_out_1;                     //release DATA-line     
  
 SCK_0; 
 DATA_in; 
  for (i=0x80;i>0;i/=2)             //shift bit for masking 
  { _nop_();_nop_();         
    SCK_1;                            //clk for SENSI-BUS     
    _nop_(); 
    if (DATA_in_N) val=(val | i);    //read bit              
        _nop_(); 
        SCK_0;         
  
  } 
  _nop_();_nop_(); 
  DATA_out;//out data 
  if(ack==1)DATA_out_0; //in case of "ack==1" pull down DATA-Line 
   else DATA_out_1; 
  _nop_();_nop_();                      
  SCK_1;                                  //clk #9 for ack 
  _nop_();_nop_();         //pulswith approx. 5 us  
  SCK_0;                                                     
       // DATA_out_1;                     //release DATA-line 
  return val; 
} 

/*==================================================== 
【函数原形】:void s_connectionreset(void) 
【参数说明】: 
【功能说明】: 
【修改时间】:  2007.01.09                  
====================================================*/ 
void s_connectionreset(void) 
{   
  unsigned char i;  
  DATA_out; DATA_out_1; 
 SCK_0;                    //Initial state 
  for(i=0;i<9;i++)                  //9 SCK cycles 
  {        _nop_();_nop_(); 
    SCK_1; 
    _nop_();_nop_(); 
    SCK_0; 
  } 
  _nop_();  _nop_();   
   SCK_1;                   //Initial state 
   _nop_(); 
   DATA_out_0; 
   _nop_(); 
   SCK_0; 
   _nop_();         
   SCK_1; 
   _nop_(); 
   DATA_out_1; 
   _nop_(); 
   SCK_0; 
   _nop_();_nop_();_nop_();_nop_();_nop_();  
} 

/*==================================================== 
【函数原形】:void  temp_humi_test(float *tp,float *hi) 
【参数说明】:*tp为温度存入地址,   *hi 为湿度存入地址      
【功能说明】: 读取温湿度值 
【修改时间】:  19days                 
====================================================*/ 
void  temp_humi_test(float *tp,float *hi) 
{ uchar err=0; 
  volatile float Temp=0,Humi=0;;  
  volatile uint  THVAL=0,RHVAL=0; 
  uint i; 
  //init_IO(); 
  s_connectionreset(); 
  err=s_write_byte(MEASURE_TEMP);  
  
for(i=0;i<20000;i++) 
  {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();} 
    
   DATA_in;//in data 
   if(DATA_in_N==0)  
        { 
        THVAL=s_read_byte(ACK);  
        THVAL=(THVAL<<8)+s_read_byte(ACK);  
        s_read_byte(noACK);  
        Temp=THVAL*0.01 - 40; 
    *tp=Temp;  
         //printf("Temp=%d  ",tp);          
        }else s_connectionreset(); 
         
        //========================= 
 for(i=0;i<20000;i++) 
  {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();} 
  
   RHVAL=0; 
          //====================== 
  err= s_write_byte(MEASURE_HUMI); 
  for(i=0;i<20000;i++) 
  {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();} 
   DATA_in;//in data 
   if(DATA_in_N==0)  
        { 
        RHVAL=s_read_byte(ACK);  
        RHVAL=(RHVAL<<8)+s_read_byte(ACK);  
        s_read_byte(noACK);  
        Humi=(-0.0000028)*RHVAL*RHVAL + 0.0405*RHVAL - 4.0; 
        Humi=(Temp-25)*(0.01+0.00008*RHVAL)+Humi;  
        *hi=Humi; 
        //printf("Humi=%d  ",hi); 
        } else s_connectionreset(); 
//================== 
 for(i=0;i<10000;i++) 
  {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();} 
//==================         

}  

