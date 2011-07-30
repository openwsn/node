http://www.avrtool.com/avr/gccavr/200710/751.html

基于WinAVR的DS18B20源程序
基于WinAVR的DS18B20源程序
作者：未知    AVR单片机来源：网络    点击数：1142    更新时间：2007-10-1    
* 
******************************************************** 
*   文件:   ds18b20.c 
*   功能:   AVR微控制器 VS 温度芯片DS18B20(TO-92) 
*   工具:   WinAVR20040404(AVR-GCC) 
*   作者:   孤欲化境(qjy_dali) 
*   E-mail: qjy_dali@sohu.com 
*   日期:   6/02/2004 
*   版本:   1.41 
*   声明:   你可随意地拷贝，复制或修改这个程序，但请你注明你的修改。本作者不对这个程序的后果负责，无论是明 确的，还是隐含的(^_^)。自由软件不是万能的，但它 的确是令人振奋的。支持一下GCC!!! 
******************************************************** 
*/
#ifndef  _DS18B20_C_ 
#define  _DS18B20_C_


/*      我的一线温度芯片DS18B20被连接到AVR微控制器ATmega8 16PI(PDIP28)的PD3(INT1)引脚，但是我只是用了PD3功能而 没有使用中断INT1功能。DS18B20(TO-92)的连接如下： 
*  +-------------+-------------------------------------+ 
*  | Pin-1(GND)  | GND(ground)                         | 
*  +-------------+-------------------------------------| 
*  | Pin-2(DQ)   | 通过240欧姆的电阻连接到ATmega8的    | 
*  |             | PD3引脚，同时用一个3K的电阻上拉到   | 
*  |             | VCC(5V)。                           | 
*  +-------------+-------------------------------------+ 
*  | Pin-3(VD)   | GND(ground)                         | 
*  +-------------+-------------------------------------+ 
*      说明：很显然，我采用的是"总线窃电"模式，这是DS18B20数据手册认可的工作模式之一。串联240欧姆电阻的用意 是为了防止有缺陷的用户程序损坏DS18B20的可能性。例如， 如果用户没有正确地用OC(集电极开路)或OD(漏极开路)结构去驱动DS18B20，而是错误地选择了推挽式结构，则DS18B20会立 即或在"被虐待"一段时间后"死翘翘"(^_^)。当然，240欧姆的 取值未必是最恰当的，设计者可自己去优化。这需要设计者仔细 阅读数据手册。 
* 
*      此外，我采用了DS18B20的默认精度(12位)，并未修改，我也没有使用它的其它功能。简而言之，这只是一个简单的读取温度 的实例。用户可以自己去改进，自己去研究数据手册(我并未完全 看，只看了想看的一点点 ^_^ )。但是我可以肯定一点，这个程 序我是实践过的!并且成功地读取了温度值! 
* 
*      请注意我是如何驱动DS18B20的：我是用AVR的方向寄存器而不是输出端口寄存器! 同时预先在输出端口寄存器中写入0。 这实际上相当于一个三态门：只不过输入被接地，使它成了一 个OC门，使能端成了这个OC门的实际输入! 
* 
*      1-wire总线的电气特性与I2C总线相似，具有线与功能，所以，总线上的任一设备都可在合适的时间强行拉低总线，但是总线要呈现高电平，则必须是每一个设备都释放了总线。就像我下面的 宏DQ_TO_1()，它只是释放了总线，但不是说总线一定被强行驱动至高电平，总线的高电平是由上拉电阻实现的。 
*/ 
#define  DQ_18B20       (1<<3)                // PD3 
#define  DQ_TO_0()      (DDRD |=  DQ_18B20)   // PD3='0' 
#define  DQ_TO_1()      (DDRD &= ~DQ_18B20)   // PD3='float' 
#define  DQ_status()    (PIND & DQ_18B20)     // read PD3 pin

/*     请认真检查你的AVR微控制器的时钟频率! 特别注意：频率定义的单位是MHz! 并且请使用浮点数! 假如你的晶振是12MHz，  你应该写成12.0000或12.0之类。 
*     我的实验电路的晶振是：11.0592MHz 
*/ 
#ifndef  CPU_CRYSTAL 
#define  CPU_CRYSTAL    (11.0592) 
#endif

/*     请包含WinAVR系统提供的延时头文件"delay.h"，其中给出两个延时模块，我用16位的那个(16-bit count, 4 cycles/l- oop.)，细节请看这个头文件。 
*/ 
#define  wait_us(us)\ 
    _delay_loop_2((INT16U)((us)*CPU_CRYSTAL/4))

/*---------------- 函数原型声明 ------------------*/ 
// 1个初始化模块 
void   ds18b20_config(void);       // 配置端口

// 3个基本模块 
BOOL   ds18b20_reset(void);        // 复位DS18B20 
void   ds18b20_write(INT8U dat);   // 写字节到DS18B20 
INT8U  ds18b20_read(void);         // 读字节从DS18B20

// 2个应用模块 
void   convert_T(void);            // 启动温度转换 
INT16U read_T(void);               // 读取转换值


/*------------------------------------------------------- 
*  配置(使能)AVR与DS18B20的接口 
*/ 
void ds18b20_config(void) 
{ 
    DDRD  &= ~DQ_18B20;   // 输入模式(上电时为高电平) 
    PORTD &= ~DQ_18B20;   // 输出锁存器写0，以后不再更改 
}

/*------------------------------------------------------- 
*    复位1-wire总线，并探测是否有温度芯片DS18B20(TO-92 
*  封装)挂在总线上，有返回SUCC，没有返回FAIL 
*/ 
BOOL ds18b20_reset(void) 
{ 
    BOOL bus_flag;

    DQ_TO_0();      // 设置1-wire总线为低电平(占领总线)...

    /* 现在延迟480us~960us, 与硬件密切相关，但应尽可能选小值(480us)， 把抖动留给系统(比如在延迟期间发生中断导致延迟变长)。 
     */ 
    wait_us(490);   // 490us

    cli();          // 下面这段时间要求比较严格，为保险起见，关中断 
    DQ_TO_1();      // 设置1-wire总线为高电平(释放总线) 
     
    /* 这个浮点数是由编译器计算好的，而不是由你的MCU在运行时临时计算的，  所以不会占用用户MCU的时间，不必担心(看看前面的宏你就可以确定了) 
     */ 
    wait_us(67.5);  // 最佳时间: 60us+7.5us!(忙延时，只是一种策略) 
     
    // 探测总线上是否有器件     
    if(DQ_status()) bus_flag=FAIL;   // 复位单总线但没有发现有器件在线 
    else bus_flag=SUCC;              // 复位单总线并发现有器件在线 
     
    sei();          // 退出临界代码区(开中断)

    /* 保证Master释放总线的时间(不是说总线处于高电平的时间)不小于 480us即可，这一时间从读总线状态之前就开始了，所以这里把这个 时间计算在内。在Master释放总线的前半段，也是被动器件声明它们在线之时。 
     */ 
    wait_us(490-67.5);   // 490-67.5us

    return(bus_flag); 
}

/*-------------------------------------------------------- 
*  写命令或数据到温度芯片DS18B20(发送一个字节) 
*/ 
void ds18b20_write(INT8U dat) 
{ 
    INT8U count;

    // 每个字节共8位，一次发一位 
    for(count=0; count<8; count++) { 
        cli();                   // 保证绝对不会发生中断! 
        DQ_TO_0();               // 设置1-wire总线为低电平 
        wait_us(2);              // about 2us 
         
        if(dat&0x01) DQ_TO_1();  // 并串转换，先低位后高位 
        else DQ_TO_0(); 
        dat >>= 1;               // 下一位做好准备 
         
        // 60us~120us(实际不能到120us, 因为其它语句也用时间了!) 
        wait_us(62);             // 62us 
         
        DQ_TO_1(); 
        sei();                   // 恢复系统中断 
        wait_us(2);              // 2us 
    } 
}

/*--------------------------------------------------------- 
*  从温度芯片DS18B20读配置或数据(接收一个字节) 
*/ 
INT8U ds18b20_read(void) 
{ 
    INT8U count,dat;

    dat = 0x00;       // 数据接收准备 
     
    // 每个字节共8位，一次收一位 
    for(count=0; count<8; count++) { 
        cli();        // 保证绝对不会发生中断! 
         
        // 从总线拉低到读总线状态，不能大于15us! 
        DQ_TO_0();    // 设置1-wire总线为低电平(拉低总线以同步) 
        wait_us(2);   // 2us 
        DQ_TO_1();    // 设置1-wire总线为高电平(释放总线) 
        wait_us(4);   // 4us         
        dat >>= 1; 
        if(DQ_status()) dat|=0x80;   // 读取总线电平，先收低位再收高位 
         
        sei();        // 恢复系统中断 
        wait_us(62);  // 必须大于60us 
    } 
    return(dat); 
}

/*------------------------------------------------------- 
*     我的电路中只有一个器件DS18B20，所以不需要多个器件的ID 识别，跳过之后，启动温度转换，但在启动后，用户应等待几百个 毫秒，才能读到这次的转换值，这是DS18B20的数据手册规定的。因为温度转换是需要时间的嘛!(^_^) 
*/ 
void convert_T(void) 
{ 
    if(ds18b20_reset()==SUCC) {  // 如果复位成功 
        ds18b20_write(0xcc);     // 跳过多器件识别 
        ds18b20_write(0x44);     // 启动温度转换 
    } 
}

/*------------------------------------------------------- 
*  读取转换后的温度值 
*  我假定DS18B20一定是正确的，所以没有返回有关状态。当你故意把DS18B20从电路中拔下而能让程序告诉你出错时，你可以自己修 改这段代码! 
*/ 
INT16U read_T(void) 
{ 
    INT16U value=0; 
     
    if(ds18b20_reset()==SUCC) {  // 如果复位成功 
        ds18b20_write(0xcc);     // 跳过多器件识别 
        ds18b20_write(0xbe);     // 读暂存器 
        value  = (INT16U)ds18b20_read();       // 低字节 
        value += (INT16U)(ds18b20_read())<<8;  // 高字节 
    } 
    return(value); 
}


#endif 
/* 文件ds18b20.c结束 */

 

VRFREAKS 上有个更全面的了 

PROJECT_59

UPDATED 9/Mar/2003 version 4.67 
The I2C driver is improved a bit.

1) HD44780 LCD with MULTIPLE LCD units 
2) UART, SINGLE OR DUAL 
3) SOFTWARE UART TTL & RS232 CAPABLE 
4) ANALOG TO DIGITAL CONVERTER 
5) HEXADECIMAL MATRIX KEYBOARD WITH EDITOR 
6) I2C MASTER and LM75 i2c temperature sensor driver 
7) 1 WIRE and DS18X20 with search rom function 
8) SRF08 & SRF04 ULTRASONIC RANGING MODULE 
9) DELAY FUNCTIONS IN ASM

 

 

 

 Asnake 发表于 2004-6-20 10:31 AVR 单片机 ←返回版面    

我把他贴过来!是C文件部分，其它的可以去下载 


/********************************************************************************************************* 
Title  :   C include file for the DS1820 library (ds1820.c) 
Author:    Chris efstathiou   
E-mail:    hendrix@otenet.gr 
Homepage:  ........................ 
Date:      2/Dec/2002 
Compiler:  AVR-GCC with AVR-AS 
MCU type:  any AVR MCU device 
Comments:  This software is FREE but without any warranty of any kind. 
*********************************************************************************************************/

/*  
    Although i could had written it in the new style, i prefer the old one for compatibility sake. 
*/

/********************************************************************************************************/ 
/*                                   PREPROCESSOR DIRECTIVES                                            */ 
/********************************************************************************************************/

#ifndef _IO_REG_MACRO_MODE_ 
#define _IO_REG_MACRO_MODE_  1       /* In case you have the new assignment mode io headers */ 
#endif

#ifndef  _SFR_ASM_COMPAT 
#define  _SFR_ASM_COMPAT     1       /* This is for GCC 3.2 */ 
#endif

 

#if AVRGCC_VERSION == 330

#include <avr/io.h> 
#include <avr/eeprom.h> 
#include <avr/pgmspace.h>

#else

#include <io.h> 
#include <eeprom.h> 
#include <pgmspace.h>

#endif

#include "one_wire.h" 
#include "ds18x20.h"  
#include "lcd_io.h"

 

/********************************************************************************************************/ 
/*                                   TYPE DEFINITIONS                                                   */ 
/********************************************************************************************************/

 

 

/********************************************************************************************************/ 
/*                                   LOCAL FUNCTION PROTOTYPES                                          */ 
/********************************************************************************************************/ 
static void send_selected_rom(void);

 

 

/********************************************************************************************************/ 
/*                                   GLOBAL VARIABLES                                                   */ 
/********************************************************************************************************/ 
unsigned char conversion_in_progress=0; 
unsigned char power_status=0; 
unsigned char ds18x20_rom_code[8]; 
signed int    temperature=0;


/********************************************************************************************************/ 
/*                                   LOCAL FUNCTIONS                                                    */ 
/********************************************************************************************************/ 
static void send_selected_rom(void) 
{ 
#if ONLY_1_DEVICE_ON_THE_BUS == 1 
  ow_read_rom(ds18x20_rom_code); 
  ow_command(OW_SKIP_ROM); 
#elif ONLY_1_DEVICE_ON_THE_BUS == 0 
unsigned char x=0;

  ow_command(OW_MATCH_ROM);   
  for(x=0; x<8; x++) { ow_put_byte(ds18x20_rom_code[x]); } 
#endif

return; 
}

/********************************************************************************************************/ 
/*                                   PUBLIC FUNCTIONS                                                   */ 
/********************************************************************************************************/


/*######################################################################################################*/ 
/*                                 DS1820 - DS18S20 FUNCTIONS                                           */ 
/*######################################################################################################*/

#if ONLY_1_DEVICE_ON_THE_BUS == 0 
void ds18x20_select_device(unsigned char *mem_array, unsigned char mem_type) 
{

unsigned char x=0;

if(mem_type == LOCATION_IS_RAM) 
{  
    for(x=0; x<8; x++) { *(ds18x20_rom_code+x)=(*(mem_array+x)); }  
} 
else if(mem_type == LOCATION_IS_FLASH) 
      {  
         for(x=0; x<8; x++) { *(ds18x20_rom_code+x)=PRG_RDB((const unsigned char*)(mem_array+x)); } 
      } 
else if(mem_type == LOCATION_IS_EEPROM) 
      { 
         for(x=0; x<8; x++) { ds18x20_rom_code[x]=eeprom_rb((unsigned int)(mem_array+x)); } 
      }

return; 
} 
#endif 
/*######################################################################################################*/

unsigned char ds18x20_get_power_status(void) 
{ 
    ow_reset(); 
    send_selected_rom(); 
    ow_put_byte(READ_POWER_SUPPLY); 
    power_status=ow_get_bit(); 
    ow_reset();


return(power_status); 
} 
/*######################################################################################################*/

signed int ds18x20_get_temp(unsigned char wait) 
{ 
signed char       bit=0; 
unsigned char     t_resolution=0, skip_convert_command=0;

 

/* First we must see if the DS18X20 is busy converting and if yes return the previus value. */ 
ow_reset(); 
send_selected_rom(); 
if(ow_get_bit()==0) { return(temperature); }

/* Now we need to know the DS18B20 resolution if there is one of course */ 
if(ds18x20_rom_code[0]==DS18B20) 
{ 
    t_resolution=ds18x20_get_reg(DS18X20_CONFIG); 
    t_resolution &= DS18X20_12_BIT; 
    if(t_resolution==DS18X20_12_BIT) { t_resolution=12; } 
    else if(t_resolution==DS18X20_11_BIT) { t_resolution=11; } 
    else if(t_resolution==DS18X20_10_BIT) { t_resolution=10; } 
    else{ t_resolution=9; } 
}

if(ds18x20_get_power_status() == 0 || wait)  
{ 
    ow_reset(); 
    send_selected_rom(); 
    ow_put_byte(CONVERT_T); 
    ow_pull_hard_dq(1); 
    if(t_resolution) { DELAY_MS( (DS18X20_CONVERSION_TIME_MS/(1<<(12-t_resolution))) ); } 
    else{DELAY_MS(DS18X20_CONVERSION_TIME_MS); } 
    ow_pull_hard_dq(0); 
    skip_convert_command=1; 
}

/* If we reached here conversion is complete so lets update the temperature value. */ 
temperature=0; 
ow_reset(); 
send_selected_rom(); 
ow_put_byte(READ_SCRATCHPAD);

/* read the Temperature registers, LSBit of LSByte first */ 
for(bit=0; bit<16; bit++) 
  { 
     if(ow_get_bit()) { temperature |= (1<<bit); } 
  }

if(ds18x20_rom_code[0]==DS18B20)  
{ 
     bit=(temperature&0x0F); 
     if(temperature < 0) { bit|=0xF0; } 
     bit=(bit*10)/16; 
     temperature=(((temperature/16)*10)+bit); 
} 
else{   
        bit=(temperature&0x01); 
        if(temperature < 0) { bit|=0xFE; } 
        bit=(bit*10)/2; 
        temperature=(((temperature/2)*10)+bit); 
    }

/* Send a new CONVERT_T command */ 
if(skip_convert_command==0) 
{ 
    ow_reset(); 
    send_selected_rom(); 
    ow_put_byte(CONVERT_T); 
} 
ow_reset();

 

return(temperature); 
} 
/*######################################################################################################*/

void ds18x20_get_scratchpad(unsigned char *mem_array) 
{ 
unsigned char x=0;

    ow_reset(); 
    send_selected_rom(); 
    ow_put_byte(READ_SCRATCHPAD); 
    for(x=0; x<9; x++) {*(mem_array+x)=ow_get_byte(); } 
    ow_reset();

return; 
} 
/*######################################################################################################*/

void ds18x20_set_reg(unsigned char reg, signed int value) 
{ 
unsigned char scratchpad[9]; 
unsigned char x=0, write_end_pos=3;

if(ds18x20_rom_code[0]==DS18B20) { write_end_pos=4; } 

   ds18x20_get_scratchpad(scratchpad); 
   scratchpad[reg]=value; 
   ow_reset(); 
   send_selected_rom(); 
   ow_put_byte(WRITE_SCRATCHPAD);

   /* write the byte, lsb first */ 
   for(x=2; x<=write_end_pos; x++) 
     { 
        ow_put_byte(scratchpad[x]); 
     }

   ow_reset();


return; 
} 
/*######################################################################################################*/ 
signed int ds18x20_get_reg(unsigned char reg) 
{ 
unsigned char scratchpad[9];

    ds18x20_get_scratchpad(scratchpad);


return(scratchpad[reg]); 
}

/*######################################################################################################*/ 
/*                                         T H E   E N D                                                */ 
/*######################################################################################################*/

 
 
 http://www.dzsc.com/dzbbs/20051117/200765193711703960.html
 
 http://www.dzsc.com/dzbbs/20051117/200765193711703960.html
 
基于WinAVR的DS18B20源程序，供参考
作者：qjy_dali　栏目：单片机

 基于WinAVR的DS18B20源程序，供参考
/*
********************************************************
*   文件:   ds18b20.c
*   功能:   AVR微控制器 VS 温度芯片DS18B20(TO-92)
*   工具:   WinAVR20040404(AVR-GCC)
*   作者:   孤欲化境(qjy_dali)
*   E-mail: qjy_dali@sohu.com
*   日期:   6/02/2004
*   版本:   1.41
*   声明:   你可随意地拷贝，复制或修改这个程序，但请你注明
*          你的修改。本作者不对这个程序的后果负责，无论是明
*          确的，还是隐含的(^_^)。自由软件不是万能的，但它
*          的确是令人振奋的。支持一下GCC!!!
********************************************************
*/

#ifndef  _DS18B20_C_
#define  _DS18B20_C_


/*      我的一线温度芯片DS18B20被连接到AVR微控制器ATMEGA8
*  -16PI(PDIP28)的PD3(INT1)引脚，但是我只是用了PD3功能而
*  没有使用中断INT1功能。DS18B20(TO-92)的连接如下：
*  +-------------+-------------------------------------+
*  | Pin-1(GND)  | GND(ground)                         |
*  +-------------+-------------------------------------|
*  | Pin-2(DQ)   | 通过240欧姆的电阻连接到ATMEGA8的    |
*  |             | PD3引脚，同时用一个3K的电阻上拉到   |
*  |             | VCC(5V)。                           |
*  +-------------+-------------------------------------+
*  | Pin-3(VD)   | GND(ground)                         |
*  +-------------+-------------------------------------+
*      说明：很显然，我采用的是"总线窃电"模式，这是DS-
*  18B20数据手册认可的工作模式之一。串联240欧姆电阻的用意
*  是为了防止有缺陷的用户程序损坏DS18B20的可能性。例如，
*  如果用户没有正确地用OC(集电极开路)或OD(漏极开路)结构去
*  驱动DS18B20，而是错误地选择了推挽式结构，则DS18B20会立
*  即或在"被虐待"一段时间后"死翘翘"(^_^)。当然，240欧姆的
*  取值未必是最恰当的，设计者可自己去优化。这需要设计者仔细
*  阅读数据手册。
*
*      此外，我采用了DS18B20的默认精度(12位)，并未修改，我
*  也没有使用它的其它功能。简而言之，这只是一个简单的读取温度
*  的实例。用户可以自己去改进，自己去研究数据手册(我并未完全
*  看，只看了想看的一点点 ^_^ )。但是我可以肯定一点，这个程
*  序我是实践过的!并且成功地读取了温度值!
*
*      请注意我是如何驱动DS18B20的：我是用AVR的方向寄存器
*  而不是输出端口寄存器! 同时预先在输出端口寄存器中写入0。
*  这实际上相当于一个三态门：只不过输入被接地，使它成了一
*  个OC门，使能端成了这个OC门的实际输入!
*
*      1-wire总线的电气特性与I2C总线相似，具有线与功能，所
*  以，总线上的任一设备都可在合适的时间强行拉低总线，但是总线
*  要呈现高电平，则必须是每一个设备都释放了总线。就像我下面的
*  宏DQ_TO_1()，它只是释放了总线，但不是说总线一定被强行驱动
*  至高电平，总线的高电平是由上拉电阻实现的。
*/
#define  DQ_18B20       (1<<3)                // PD3
#define  DQ_TO_0()      (DDRD |=  DQ_18B20)   // PD3='0'
#define  DQ_TO_1()      (DDRD &= ~DQ_18B20)   // PD3='float'
#define  DQ_status()    (PIND & DQ_18B20)     // read PD3 pin

/*     请认真检查你的AVR微控制器的时钟频率! 特别注意：频率
* 定义的单位是MHz! 并且请使用浮点数! 假如你的晶振是12MHZ，
* 你应该写成12.0000或12.0之类。
*     我的实验电路的晶振是：11.0592MHZ.html">11.0592MHZ
*/
#ifndef  CPU_CRYSTAL
#define  CPU_CRYSTAL    (11.0592)
#endif

/*     请包含WinAVR系统提供的延时头文件"delay.h"，其中给出
*  两个延时模块，我用16位的那个(16-bit count, 4 cycles/l-
*  oop.)，细节请看这个头文件。
*/
#define  wait_us(us)\
    _delay_loop_2((INT16U)((us)*CPU_CRYSTAL/4))

/*---------------- 函数原型声明 ------------------*/
// 1个初始化模块
void   ds18b20_config(void);       // 配置端口

// 3个基本模块
BOOL   ds18b20_reset(void);        // 复位DS18B20
void   ds18b20_write(INT8U dat);   // 写字节到DS18B20
INT8U  ds18b20_read(void);         // 读字节从DS18B20

// 2个应用模块
void   convert_T(void);            // 启动温度转换
INT16U read_T(void);               // 读取转换值


/*-------------------------------------------------------
*  配置(使能)AVR与DS18B20的接口
*/
void ds18b20_config(void)
{
    DDRD  &= ~DQ_18B20;   // 输入模式(上电时为高电平)
    PORTD &= ~DQ_18B20;   // 输出锁存器写0，以后不再更改
}

/*-------------------------------------------------------
*    复位1-wire总线，并探测是否有温度芯片DS18B20(TO-92
*  封装)挂在总线上，有返回SUCC，没有返回FAIL
*/
BOOL ds18b20_reset(void)
{
    BOOL bus_flag;

    DQ_TO_0();      // 设置1-wire总线为低电平(占领总线)...

    /* 现在延迟480us~960us, 与硬件密切相关，但应尽可能选小值(480us)，
       把抖动留给系统(比如在延迟期间发生中断导致延迟变长)。
     */
    wait_us(490);   // 490us

    cli();          // 下面这段时间要求比较严格，为保险起见，关中断
    DQ_TO_1();      // 设置1-wire总线为高电平(释放总线)
    
    /* 这个浮点数是由编译器计算好的，而不是由你的MCU在运行时临时计算的，
       所以不会占用用户MCU的时间，不必担心(看看前面的宏你就可以确定了)
     */
    wait_us(67.5);  // 最佳时间: 60us+7.5us!(忙延时，只是一种策略)
    
   