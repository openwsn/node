http://www.eefocus.com/billbot501/blog/11-03/206432_30946.html
source code can be downloaded

LM75A是一个高速I2C接口的温度传感器，可以在-55～+125的温度范围内将温度直接转换为数字信号，并实现0.125的精度。PIC32可以通过I2C总线直接读取其内部寄存器中的数据，并可通过I2C对4个数据寄存器进行操作，设置不同的工作模式。LM75A有3个可选的逻辑地址管脚，使得同一总线上可同时连接8个器件而不发生地址冲突。
LM75A与PIC32的连接是以I2C总线的形式，如图3-11所示，总线上需要加上拉电阻，因为通信开始之前，I2C总线必须空闲或者不忙，这意味着总线上的所有器件都必须释放SCL和SDA线，SCL和SDA线被总线的上拉电阻拉高，上拉电阻取10K左右.


http://www.eefocus.com/x10031987/blog/11-04/207868_05130.html

LM75温度传感器驱动　 2011-04-01 22:23
 加入收藏
 转发分享
 人人网
 开心网
 新浪微博
 搜狐博客
 百度收藏
 谷歌收藏
 qq书签
 豆瓣
 淘江湖
 Facebook
 Twitter
 Digg
 Yahoo!Bookmarks
/***********************************************************/
LM75温度传感器，使用IIC接口，读写及转换时间短，温度检测灵
敏度要比传统的DS18B20高，适合做温控管理，热操作等热力系统
本设计使用AVR单片机，驱动温度传感器DS75及LM75系列源代码，
时钟7M，32倍频，温度传感器在F端口，9位温度转换精度。
BY Johnny,2011.3.23
************************************************************/
#define LM75_SDA 3
#define LM75_SDL 2
/***********************************************************
函数名：delayus
调  用：无
参  数：无
返回值：无
结  果：延时
备  注：
/***********************************************************/
void delayus(unsigned char del)
{
         while(del--);
}
/***********************************************************
函数名：init_lm75
调  用：无
参  数：无
返回值：无
结  果：初始化
备  注：
/***********************************************************/
void init_lm75(void)
{
         SETBIT(DDRF,LM75_SDA);              //only output
         SETBIE(DDRF,LM75_SDL);
 
         SETBIT(PORTF,LM75_SDA);
         delayus(10);
         SETBIT(PORTF,LM75_SDL);
         delayus(10);
         CLEARBIT(PORTF,LM75_SDA);
         delayus(20);
         CLEARBIT(PORTF,LM75_SDL);
         delayus(20);
}
 
/***********************************************************
函数名：sendbyte_lm75
调  用：无
参  数：buf
返回值：字节长度
结  果：发送一个字节，成功返回0x91，否则返回0
备  注：
/***********************************************************/
unsigned char sendbyte_lm75 (unsigned char buf)
{
         unsigned char i=0;
         SETBIT(DDRF,LM75_SDA); //only output
         SETBIE(DDRF,LM75_SDL);
         for(i=8;i>0;i--)
         {
                   CLEARBIT(PORTF,LM75_SDL);               //sdl low
                   delayus(20);
                   if(buf&0x80)                                                  //bit7==1
                             SETBIT(PORTF,LM75_SDA);
                   else  CLEARBIT(PORTF,LM75_SDA);    //bit7==0
                   buf<<=1;
                   SETBIT(PORTF,LM75_SDL);
                   delayus(20);
         }
         CLEARBIT(PORTF,LM75_SDL);                //put sdl low
         delayus(20);                                                    //wait for ack
         SETBIT(PORTF,LM75_SDA);
         delayus(5);
         CLEARBIT(DDRF,LM75_SDA);     //sdl input
         SETBIT(PORTF,LM75_SDL);
                   delayus(20);
         if(GETBIT(PINF,LM75_SDA))         //fail
         {
                   SETBIT(DDRF,LM75_SDL);
                   CLEARBIT(PORTF,LM75_SDL);
                   return 0;
         }
         else
         {
                   SETBIT(DDRF,LM75_SDL);
                   CLEARBIT(PORTF,LM75_SDL);
                   return 0x91;
         }
}
/***********************************************************
函数名：getbyte_lm75
调  用：无
参  数：无
返回值：接收字节
结  果：接收一个字节
备  注：
/***********************************************************/
unsigned char getbyte_lm75(void)
{
         unsigned char i=0,buf=0;
         CLEARBIT(DDRF,LM75_SDA);
         SETBIE(DDRF,LM75_SDL);
         for(i=0;i<8;i++)
         {
                   CLEARBIT(PORTF,LM75_SDL);
                   delayus(20);
                   SETBIT(PORTF,LM75_SDL);
                   delayus(10);
                   buf<<=1;
                   if(GETBIT(PINF,LM75_SDA))
                   {
                            buf+=1;
                            delayus(10);
                   }
                   CLEARBIT(PORTF,LM75_SDL);
         }
         return buf;
}
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
/***********************************************************
函数名：stop_lm75
调  用：无
参  数：无
返回值：无
结  果：停止
备  注：
/***********************************************************/
void stop_lm75(void)
{
         SETBIT(DDRF,LM75_SDA);
         SETBIE(DDRF,LM75_SDL);
         CLEARBIT(PORTF,LM75_SDA);
         delayus(20);
         SETBIT(PORTF,LM75_SDL);
         delayus(20);
         SETBIT(PORTF,LM75_SDA);
         delayus(20);
}
/***********************************************************
函数名：ack_lm75
调  用：无
参  数：ack
返回值：无
结  果：应答
备  注：
/***********************************************************/
void ack_lm75(unsigned char ack)
{
         SETBIT(DDRF,LM75_SDA);
         SETBIE(DDRF,LM75_SDL);
 
         if(ack)      CLEARBIT(PORTF,LM75_SDA);
         else  SETBIT(PORTF,LM75_SDA);
 
         delayus(8);
         SETBIT(PORTF,LM75_SDL);
         delayus(20);
         CLEARBIT(PORTF,LM75_SDL);
         delayus(10);
}
 
 
 
/***********************************************************
函数名：temperature_lm75
调  用：无
参  数：无
返回值：字节长度
结  果：温度值
备  注：
/***********************************************************/
unsigned char temperature_lm75(void)
{
         unsigned char temp[2];
         unsigned char i;
 
         init_lm75();
         sendbyte_lm75(0x91);
         temp[1]=getbyte_lm75();
         ack_lm75(1);
         temp[0]=getbyte_lm75();
         ack_lm75(0);
         stop_lm75();
 
         if(temp[1]&0x80)                        //temperature was minus
                   temp[1]=~temp[1]+0x80;
         return temp[1];
}
 