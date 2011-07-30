
/**
 * @reference
 * - 用GPIO模拟SPI协议的实现, 2008, 
 *   http://blog.csdn.net/aaronychen/archive/2008/06/20/2568162.aspx;
 * - 【转帖】用GPIO模拟SPI协议的实现, 
 *   http://hi.baidu.com/_%C5%CE%C8%FD%C4%EA_/blog/item/bc66c3862ff2713866096efe.html;
 * - Introduction to Serial Peripheral Interface, 2002, 
 *   http://www.embedded.com/story/OEG20020124S0116;
 * - Serial Peripheral Interface Bus, 
 *   http://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus;
 * - Bit-banging, http://en.wikipedia.org/wiki/Bit-banging;
 */


#define SPI_DATA    GPIO0
#define SPI_CLK     GPIO1

void spi_write(char data)
{
    int8 i = 7;
    uint8 mask[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

	for (; i >= 0; i--) 
    {
		gpio_out(SPI_CLK, GPIO_LOW_VALUE);  /*把clock线拉低，模拟一个时钟*/
		gpio_out(SPI_DATA,  ((data & mask[i]) >> i));  /*把数据送上DATA线*/
		spi_delay(10);   /*等待适当的时间，以等待数据稳定*/

		gpio_out(SPI_CLK, GPIO_HIGH_VALUE);  /*拉高clock线，让设备端接收数据*/
		spi_delay(10);
	}
}

实际上模拟SPI是很简单的事情， 只要对照SPI传输模式的时序图来模拟就行了。需要注意的是一定要有个等待时间，以使数据在数据线上稳定下来， 并使设备端有时间取数据。刚开始调试的时候可以适当把等待时间延长一点，当调通了SPI后在降下等待时间。

   我写的等待时间如下：

   #define spi_delay(delay)   \

   { \

       register uint32 i = 0;  \

       while(i < delay)  { \

            __asm{ \

                 NOP; \

                 NOP; \

                 NOP; \

                 NOP; \

              }; \

              i -= 4; \

         } \

     }

 

 呵呵，整个过程就是这样简单。

 
     Example of bit-banging the SPI Master protocol
An example of Bit-banging the SPI protocol, as a SPI master not as a SPI slave, is included below in the C programming language, with CPOL=0, CPHA=0, and eight bits per transfer. Because this is CPOL=0, the clock must be pulled low before the chip select is activated. The chip select line must be activated, which normally means being toggled low, for the peripheral before the start of the transfer, and then deactivated afterwards. Most peripherals allow or require several transfers while the select line is low; this routine might be called several times before deselecting the chip.
unsigned char SPIBitBang8BitsMode0(unsigned char byte)
{       
    unsigned bit;
 
    for (bit = 0; bit < 8; bit++) {
        /* write MOSI on trailing edge of previous clock */
        if (byte & 0x80)
            SETMOSI();
        else
            CLRMOSI();
        byte <<= 1;
 
        /* half a clock cycle before leading/rising edge */
        SPIDELAY(SPISPEED/2);
        SETCLK();
 
        /* half a clock cycle before trailing/falling edge */
        SPIDELAY(SPISPEED/2);
 
        /* read MISO on trailing edge */
        byte |= READMISO();
        CLRCLK();
    }
 
    return byte;
}

 C code example

// transmit byte serially, MSB first
void send_8bit_serial_data(unsigned char data)
{
   unsigned char i;
 
   // select device
   output_high(SD_CS);
 
   // send bits 7..0
   for(i = 0; i < 8; i++)
   {
       // consider leftmost bit
       // set line high if bit is 1, low if bit is 0
       if (data & 0x80)
           output_high(SD_DI);
       else
           output_low(SD_DI);
 
       // pulse clock to indicate that bit value should be read
       output_low(SD_CLK);
       output_high(SD_CLK);
 
       // shift byte left so next bit will be leftmost
       data <<= 1;
   }
 
   // deselect device
   output_low(SD_CS);
}