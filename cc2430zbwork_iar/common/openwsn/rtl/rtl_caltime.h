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
