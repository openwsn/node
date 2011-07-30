
#include "hal_foundation.h"
#include "hal_sensor_temp.h"


static uint8 isexist;
unsigned char i;

uint8 ROMS[8]=//{0x28,0x12,0x70,0x8f,0x00,0x00,0x00,0x05},        //1号
                      //{0x28,0x0d,0x70,0x8f,0x00,0x00,0x00,0x7a}};       //2号
                      //{0x28,0x50,0x64,0x8f,0x00,0x00,0x00,0x7d},       //3号
                      //{0x28,0xeb,0x8b,0x8f,0x00,0x00,0x00,0x37}};      //4号
                      //{0x28,0x0f,0xea,0x8e,0x00,0x00,0x00,0x91},     //5号
                      //{0x28,0x97,0x23,0x8f,0x00,0x00,0x00,0xdf}};    //6号
                      // {0x28,0xd7,0xfb,0x8f,0x00,0x00,0x00,0xca},        //7号
                       {0x28,0x41,0xf1,0x8f,0x00,0x00,0x00,0x74};       //8号

//延迟子程序      10MHZ

void delay(int time)
{
  int i;
  for(i=0;i<time;i++);
}


uint8 ResetDS(void)
{
  IO0DIR |= DQ_PIN;
  IO0CLR = DQ_PIN;
  delay(48);                  //保持DQ低480us
  IO0SET = DQ_PIN;                // 释放DQ
  delay(8);                   //等待芯片应答信号
  isexist = !!(IO0PIN & DQ_PIN);
  delay(40);                  //延时以完成整个时序
  return (isexist);           //返回应答信号，有芯片应答isexist＝0;无芯片应答isexist=1
}


//读一字节子程序

unsigned char read_byte(void)
{
  unsigned char i,t,value=0;
  for(i=0;i<8;i++) {
    IO0DIR |= DQ_PIN;
    IO0CLR  = DQ_PIN;    //将总线DQ拉低开始读时序
    t++;
    IO0SET  = DQ_PIN;    //释放总线
    IO0DIR &= ~DQ_PIN;
    t++;
    t++;
    if(IO0PIN & DQ_PIN)  value |= 0x01 << i;
    delay(6);
  }
    return value;
 }
 
 

//Write a byte

void write_byte(unsigned char val)
{
  IO0DIR |= DQ_PIN;

  for(i=8;i>0;i--)
  {
    IO0CLR  = DQ_PIN;
    IO0SET  = val & 0x01;  //每次写1位,通过val右移得到
    delay(6);              //延时60us
    IO0SET  = DQ_PIN;          //释放总线
    val >>= 1;             //计算的同时会产生一个2us的时间间隙
  }
}

void ReadTemps(unsigned char *val)      //用此函数来读取温度
{
   unsigned char i;
   ResetDS();
   write_byte(0x55);
   for(i=0;i<8;i++){
        write_byte(ROMS[i]);
      }
   write_byte(0xbe);         // Read Scratch Pad
   val[0]=read_byte();
   val[1]=read_byte();
}



