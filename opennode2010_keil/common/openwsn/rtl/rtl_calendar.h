


TiTimeDescriptor


typedef struct{
  
}TiCalendarTime;



与日期和时间相关的数据结构 
在标准C/C++中，我们可通过tm结构来获得日期和时间，tm结构在time.h中的定义如下： 
#ifndef _TM_DEFINED  
struct tm {  
int tm_sec; /* 秒 – 取值区间为[0,59] */  
int tm_min; /* 分 - 取值区间为[0,59] */  
int tm_hour; /* 时 - 取值区间为[0,23] */  
int tm_mday; /* 一个月中的日期 - 取值区间为[1,31] */  
int tm_mon; /* 月份（从一月开始，0代表一月） - 取值区间为[0,11] */  
int tm_year; /* 年份，其值等于实际年份减去1900 */  
int tm_wday; /* 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */  
int tm_yday; /* 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 */  
int tm_isdst; /* 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的进候，tm_isdst为0；不了解情况时，tm_isdst()为负。*/  
};  
#define _TM_DEFINED  
#endif 
ANSI C标准称使用tm结构的这种时间表示为分解时间(broken-down time)。 
而日历时间（Calendar Time）是通过time_t数据类型来表示的，用time_t表示的时间（日历时间）是从一个时间点（例如：1970年1月1日0时0分0秒）到此时的秒数。在time.h中，我们也可以看到time_t是一个长整型数： 
#ifndef _TIME_T_DEFINED  
typedef long time_t; /* 时间值 */  
#define _TIME_T_DEFINED /* 避免重复定义 time_t */  
#endif 
大家可能会产生疑问：既然time_t实际上是长整型，到未来的某一天，从一个时间点（一般是1970年1月1日0时0分0秒）到那时的秒数（即日历时间）超出了长整形所能表示的数的范围怎么办？对time_t数据类型的值来说，它所表示的时间不能晚于2038年1月18日19时14分07秒。为了能够表示更久远的时间，一些编译器厂商引入了64位甚至更长的整形数来保存日历时间。比如微软在Visual C++中采用了__time64_t数据类型来保存日历时间，并通过_time64()函数来获得日历时间（而不是通过使用32位字的time()函数），这样就可以通过该数据类型保存3001年1月1日0时0分0秒（不包括该时间点）之前的时间。 
在time.h头文件中，我们还可以看到一些函数，它们都是以time_t为参数类型或返回值类型的函数： 
double difftime(time_t time1, time_t time0);  
time_t mktime(struct tm * timeptr);  
time_t time(time_t * timer);  
char * asctime(const struct tm * timeptr);  
char * ctime(const time_t *timer); 
此外，time.h还提供了两种不同的函数将日历时间（一个用time_t表示的整数）转换为我们平时看到的把年月日时分秒分开显示的时间格式tm： 
struct tm * gmtime(const time_t *timer);  
struct tm * localtime(const time_t * timer); 
通过查阅MSDN，我们可以知道Microsoft C/C++ 7.0中时间点的值（time_t对象的值）是从1899年12月31日0时0分0秒到该时间点所经过的秒数，而其它各种版本的Microsoft C/C++和所有不同版本的Visual C++都是计算的从1970年1月1日0时0分0秒到该时间点所经过的秒数。 



并入到rtl_time模块吧


TiCalendarTime

typedef struct{
}TiCalendarTime;

采用压缩BCD码存储
[B0..B9]

B0: 符号
B1-B2 Year
B3 M
B4 D
B5 HH
B6 MM
B7 SS
B8 ms
B9 ms
但注意，这种存储方法只能支持到ms
为了在串口传输时方便，在这种方式中，要求所有byte都是可见字符，这样传输显示处理起来方便
最高byte还是要设法作为控制字节是用
