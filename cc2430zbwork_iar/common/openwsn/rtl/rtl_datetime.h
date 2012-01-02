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

/**
 * TiDate
 * Date的设计以2000年1月1日 00:00:00为基准，表示自那之后或之前的天数。天数从1开始。
 * TiDate uses 4B to store the date based on 2000.01.01 00:00:00:0000. It's the day 
 * count from that point. Attention we only need 14b(0-16384) to represent 0-9999, 
 * so there're actually 16b left for future use.
 * 
 * 注意到表示0000-9999只需要14b（0-16384），所以4个B事实上还剩余很多，最高的32-14=18
 * 个位保留待用，其中最高位规定为符号位。如果用30b来表示天数，则最多可以表示：2^30天
 * ，足够用了。
 */
typedef int32 TiDate;

/** 
 * 4B，存储午夜（00:00:00）之后的毫秒数。由于1个地球日每天的毫秒数86400 *1000毫秒，
 * 所以只需要27b，4个字节还剩余5个bit未用，我们规定最高位表示符号位。
 */
typedef uint32 TiTime;

/**
 * 2B，存储base date（2000年1月1日 00:00:00）之前或之后的天数，注意到表示0000-9999
 * 只需要14b（0-16384），所以4个B事实上还剩余2b，我们规定最高位表示符号位。
 */
typedef uint16 TiShortDate;

/**
 * 2B，存储午夜（00:00:00）之后的秒数。由于一个地球日每天只有24*60*60=86400秒，
 * 需要占用17位，超过了2B范围，所以很讨厌。怎么办？所以SQL Server中shorttime只存储
 * 午夜之后的分钟数，这样最多3600分钟，占用12b，还剩余4b保留待用。
 */
typedef uint16 TiShortTime;

/** 8B，由TiDate和TiTime拼接而来 */
typedef struct{
  TiDate date;
  TiTime time;
}TiDateTime;

/** 4B，由TiShortDate和TiShortTime复合而来 */
typedef struct{
  TiShortDate date;
  TiShortTime time;
}TiShortDateTime;

TiDateTime * datetime_plus( TiDateTime * dt1, TiDateTime * dt2 );
TiDateTime * datetime_minus( TiDateTime * dt1, TiDateTime * dt2 );
TiDateTime * datetime_forward( TiDateTime * dt, uint32 interval );
TiDateTime * datetime_backward( TiDateTime * dt, uint32 interval );


/*
TiShortDateTime

TiTimeStamp = TiDateTime + ms/us/ps
这一点与ISO8601对timestamp的理解是不一致的，也许我们需要用一个更好的名词

以上称之为Standard体系中的DateTime
char[4] TiDate
define TiDate char[4]

typedef struct{
    TiDate date;
    TiTime time;
    uint16 offset;
}TiDateTime;






Timebuf / datetime
uint8 datetime_size( TiDateTime * datetime );
void datetime_getdate( TiDateTime * datetime, TiDate * date )
TiDate * datetime_getdateptr( TiDateTime * datetime )
void datetime_gettime( TiDateTime * datetime, TiTime * time )
TiTime * datetime_gettimeptr( TiDateTime * datetime )
void datetime_getoffset( TiDateTime * datetime, char * offset, uint8 size );
char * datetime_getoffsetptr( TiDateTime * datetime )

void datetime_add( TiDateTime * datetime1, TiDateTime datetime2 )
void datetime_sub( TiDateTime * datetime1, TiDateTime datetime2 )

datetime_pack( TiDateTime * datetime, char * buf, uint8 size );
datetime_unpack( TiDateTime * datetime, char * buf, uint8 len );

uint32 datetime_day( TiDateTime * datetime );
uint32 datetime_seconds( TiDateTime * datetime );
uint32 datetime_milliseconds( TiDateTime * datetime );
uint16 datetime_microseconds( TiDateTime * datetime );
uint16 datetime_nanoseconds( TiDateTime * datetime );

uint32 datetime_to_caltime( TiDateTime * datetime, TiCalendarTime * caltime );
uint32 datetime_from_caltime( TiDateTime * datetime, TiCalendarTime * caltime );




第二套表示方法就是与Calendar有关系，这里需要注意time zone的影响。
TiCalendarTime 包含年月日时分秒的表示
TiCalendarTimeDuration 两个时间变量之间的差
TiCalendarTimeInterval　两个时间变量组成的起止时间对

UTC Time 世界时/世界标准时间Coordinated Universal Time  
coordinated universal time: Greenwich Mean Time updated with leap seconds

注意，这是一个结构体，结构体的摆放尽量统一
一些值可以通过访问结构体的成员获得，一些值应该通过计算获得，以提高某些情况下的效率，但是结构体的定义应该是统一的，并遵循ISO8601。遗憾的是，作为结构体之后，就难以很方便的扩展ms/us/ns/ps。

第三套存储用于在网络上传输，解决大小端问题，我们规定每个变量遵从网络字节序，并且变量与变量之间无空白字节。
TiDateBuf 
TiTimeBuf
TiDateTimeBuf
TiShortDateTimeBuf

typedef char[4] TiDateBuf;
typedef char[4] TiTimeBuf;
typedef char[8] TiDateTimeBuf;
typedef char[4] TiShortDateTimeBuf;


我们所做的扩展：
基于TiDateTimeBuf进行
最高字节的次高位表示是否扩展。如果扩展，则后面再加2个字节表示us，以后依次进行。都是判断次高位来判断是否有扩展字节。


第4套表示专用于资源高度受限的嵌入式系统，例如wireless sensor
因为在此类系统中，我们并不完全需要全功能的RTC，我们不需要年月日时分秒的划分。只要用1个数来表示时间即可，这样可最大程度提高效率，因为经常没有全功能的RTC支持。此外，还需要支持ms/ns以满足测控系统的精度要求。

规定：用一个nB的长整数表示时刻，通过软件长整数运算实现加减。注意到软件长整数运算通常可以用汇编指令优化，因此可以求得几乎是最高的效率。当然，要把这样一个长整数转化为calendar time会很麻烦，但是后者是由更加强大的gateway或backend service实现的，不必一定放在资源很紧张的MCU中实现。

我们可以直接用TiDateTime及其扩展形式，共10个字节
也可以用2000.01.01 00:00:00之后的ms数来表示。１年的所有毫秒也能在35b共5B内表示完毕，但是通常timer中断没有这么快，一般数ms才中断一次，这个time counter才会增加。但是这样还没有考虑us/ns/ps问题。

决定：在嵌入式系统中使用标准的TiDateTime方式，如果需要，就在最后拼上2B us扩展，如果需要，还可以继续拼ns,ps,...。在我们的default设置中，缺省就加上了2B的us扩展，因此我们遇到的TiDateTime多占用10个字节。

修正

加扩展的方式是在当前level上判断是否有下一级扩展
最基础的就是4B date方式
然后依靠第二个位判断是否有ms表示，
ms占用4个字节
如果ms第二个位为1的话，则表示有us表示
us占用2个字节
然后依此逐级判断。
所以我们的嵌入式系统中用的时间表示是：
[Date 4B]{[Time 4B]{ [ms 2B] {[us 2B] {[ns 2B] [ps 2B]}}}}
至于尾巴上这些数据是否存在，就要看扩展位的值。
在网络上传输的时候，就是这样子传输的。
函数 timebuf_size() / datetime_size() 返回字节数

Timebuf / datetime
Uint8 datetime_size( datetime )
int32 datetime_getdate( datetime )
char * datetime_getdatebuf( datetime )
Int32 datetime_gettime( datetime )
Char * datetime_gettimebuf( datetime )
Datetime_getseconds
Get_milliseconds
Get_nanoseconds 
Datetime_pack / encode / write 对象持续化
Datetime_unpack / decode / read

程序中的TiDateTime等都应该作为对象看待，其中可以有为高效计算设置的中间变量。
而在网络上传输时用的都是TiDateTimeComm结构，后者其实就是个字节数组，没有什么特别。

注意，在这种设计中，我们无所谓世界协调时和时区的概念。我们只有日和毫秒这个概念。本来我还想统一成为一个8字节的毫秒表示，但考虑到后面换算，暂时保留date的区分，毕竟，date不区分，后面ns还是要区分的，总归扩展机制是需要的。

如果我们直接用8字节的ms表示有问题吗？事实上，6个字节如果全部用来表示毫秒，也已有很大的。这样，可以用2个字节再表示us。

这种概念可称之为　序列时 SequenceTime / Local Time / Fast Time / 

Ticker 对象/SliceCounter
对基本的Timer包装得来，提供了SystemTimer接口，可以作为SystemTimer使用以驱动osx内核。当然，RTC也提供了SystemTimer接口，也可驱动osx内核运行。

如果以一个tick或者time slice / time slot作为时间同步最小单位，还是过于粗糙，因为通信中要求time slot要尽可能对准。

ticker对象：是硬件Timer对象的一个软件封装，用于提供长时间的连续运行的实际时间
度量，与PC机计时ticker不同的是，还要提供在一个tick/time slice内微调的能力。
一个tick最好跟ms有一个简单准确的换算关系，但这在实际中由于硬件限制经常不能做到。没办法，只好过一段时间修正一下。

TiTicker : public TiTimer
_correct_offset
.open( timeslice )
.forward( tick_count )  参数为tick_count 以求高效
.backward( tick_count )
.adjust
.slicecount = value
.setvalue

.fineadjust / .finetune / .slicetune / slice

=> milliseconds
=> microseconds

TiTicker有点类似于TiSvcTimer因为实现了长时间计时，但与SvcTimer用途定位不同。

用于osx的ticker和用于communication的timer ticker要求是不同的，不建议混在一起实现。


*/
