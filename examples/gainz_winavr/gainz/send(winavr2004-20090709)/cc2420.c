//----------------------------------------------------//
//-------Institute Of  Computing Technology-----------//
//------------Chinese Academic  Science---------------//
//-----中国科学院计算技术研究所先进测试技术实验室-----//
//----------------------------------------------------//

/**
 * www.wsn.net.cn
 * @copyright:nbicc_lpp
 * @data:2005.11.22
 * @version:0.0.1
 * @updata:$
 *
 */
#include "cc2420.h"
//send

//------------------------------HPL-------------------------------------------------------------------
inline  uint8_t cc2420_pin_init(void)
{

  cc2420_bramspi = TRUE;
  HAL_MAKE_MISO_INPUT();
  HAL_MAKE_MOSI_OUTPUT();
  HAL_MAKE_SPI_SCK_OUTPUT();
  HAL_MAKE_CC_RSTN_OUTPUT();
  HAL_MAKE_CC_VREN_OUTPUT();
  HAL_MAKE_CC_CS_OUTPUT();
  HAL_MAKE_CC_FIFOP1_INPUT();
  HAL_MAKE_CC_CCA_INPUT();
  HAL_MAKE_CC_SFD_INPUT();
  HAL_MAKE_CC_FIFO_INPUT();
  
    return SUCCESS;
}

inline static  uint8_t SPI_init(void)
{
   { hal_atomic_t _nesc_atomic = hal_atomic_start();
    {
      HAL_MAKE_SPI_SCK_OUTPUT();
      HAL_MAKE_MISO_INPUT();
      HAL_MAKE_MOSI_OUTPUT();
      SPSR |= 1 << 0;//SPI STATE 
      SPCR |= 1 << 4;
      SPCR &= ~(1 << 3);
      SPCR &= ~(1 << 2);
      SPCR &= ~(1 << 1);
      SPCR &= ~(1 << 0);
      SPCR |= 1 << 6;
    }
    hal_atomic_end(_nesc_atomic); }
	return SUCCESS;
}

inline  uint8_t cc2420_reginit(void)
{
  
  // Set default parameters
  cc2420_currentparameters[CP_MAIN] = 0xf800;
  cc2420_currentparameters[CP_MDMCTRL0] = ((0 << cc2420_MDMCTRL0_PANCRD) |(0 << cc2420_MDMCTRL0_ADRDECODE) | //is not coordinator disable auto adr_decode
       (2 << cc2420_MDMCTRL0_CCAHIST) | (3 << cc2420_MDMCTRL0_CCAMODE)  | //cca mode is 3
       (1 << cc2420_MDMCTRL0_AUTOCRC) | (2 << cc2420_MDMCTRL0_PREAMBL)); //auto crc is set 

  cc2420_currentparameters[CP_MDMCTRL1] = 20 << cc2420_MDMCTRL1_CORRTHRESH;//default
  cc2420_currentparameters[CP_RSSI] =     0xE080;//default 
  cc2420_currentparameters[CP_SYNCWORD] = 0xA70F;
  cc2420_currentparameters[CP_TXCTRL] = ((1 << cc2420_TXCTRL_BUFCUR) | //default is 2-1.16ma,now is 980ua
       (1 << cc2420_TXCTRL_TURNARND) | (3 << cc2420_TXCTRL_PACUR) | //default ,turnaround time is 12 symbols
       (1 << cc2420_TXCTRL_PADIFF) | (0x1f << cc2420_TXCTRL_PAPWR));//default
 cc2420_currentparameters[CP_RXCTRL0] = ((1 << cc2420_RXCTRL0_BUFCUR) | //default is 980ua
       (2 << cc2420_RXCTRL0_MLNAG) | (3 << cc2420_RXCTRL0_LOLNAG) | 
       (2 << cc2420_RXCTRL0_HICUR) | (1 << cc2420_RXCTRL0_MCUR) | 
       (1 << cc2420_RXCTRL0_LOCUR));//default
 cc2420_currentparameters[CP_RXCTRL1]  = ((1 << cc2420_RXCTRL1_LOLOGAIN)   | (1 << cc2420_RXCTRL1_HIHGM    ) |//default
	                                   (1 << cc2420_RXCTRL1_LNACAP)     | (1 << cc2420_RXCTRL1_RMIXT    ) |
									   (1 << cc2420_RXCTRL1_RMIXV)      | (2 << cc2420_RXCTRL1_RMIXCUR  ) );//default
							    
 cc2420_currentparameters[CP_FSCTRL]   = ((1 << cc2420_FSCTRL_LOCK)        | ((357+5*(cc2420_DEF_CHANNEL-11)) << cc2420_FSCTRL_FREQ));//2405mhz
 cc2420_currentparameters[CP_SECCTRL0] = ((0 << cc2420_SECCTRL0_PROTECT) |(1 << cc2420_SECCTRL0_CBCHEAD)  |
	                                   (1 << cc2420_SECCTRL0_SAKEYSEL)  | (1 << cc2420_SECCTRL0_TXKEYSEL) |
									   (1 << cc2420_SECCTRL0_SECM ) );//disable the security
 cc2420_currentparameters[CP_SECCTRL1] = 0;
 cc2420_currentparameters[CP_BATTMON]  = 0;//battery monitor is disable 
    //cc2420_currentparameters[CP_IOCFG0]   = (((TOSH_DATA_LENGTH + 2) << cc2420_IOCFG0_FIFOTHR) | (1 <<cc2420_IOCFG0_FIFOPPOL)) ;
	//set fifop threshold to greater than size of  msg, fifop goes active at end of msg
 cc2420_currentparameters[CP_IOCFG0]   = (((127) << cc2420_IOCFG0_FIFOTHR) | (1 <<cc2420_IOCFG0_FIFOPPOL)) ;//polarity is inverted as compareed to the specification
 cc2420_currentparameters[CP_IOCFG1]   =  0;
  return SUCCESS;

}

//---------------------------------------------------------设置cc2420寄存器的值
bool cc2420_setreg(void)

{
  uint16_t data;
  
  cc2420_writereg(cc2420_MAIN, cc2420_currentparameters[CP_MAIN]);
  cc2420_writereg(cc2420_MDMCTRL0, cc2420_currentparameters[CP_MDMCTRL0]);
  data = cc2420_readreg(cc2420_MDMCTRL0);
  if (data != cc2420_currentparameters[CP_MDMCTRL0]) {
    printf("reginit is unsuccess\n");
    return FALSE;
    }
  cc2420_writereg(cc2420_MDMCTRL1, cc2420_currentparameters[CP_MDMCTRL1]);
  cc2420_writereg(cc2420_RSSI, cc2420_currentparameters[CP_RSSI]);
  cc2420_writereg(cc2420_SYNCWORD, cc2420_currentparameters[CP_SYNCWORD]);
  cc2420_writereg(cc2420_TXCTRL, cc2420_currentparameters[CP_TXCTRL]);
  cc2420_writereg(cc2420_RXCTRL0, cc2420_currentparameters[CP_RXCTRL0]);
  cc2420_writereg(cc2420_RXCTRL1, cc2420_currentparameters[CP_RXCTRL1]);
  cc2420_writereg(cc2420_FSCTRL, cc2420_currentparameters[CP_FSCTRL]);

  cc2420_writereg(cc2420_SECCTRL0, cc2420_currentparameters[CP_SECCTRL0]);
  cc2420_writereg(cc2420_SECCTRL1, cc2420_currentparameters[CP_SECCTRL1]);
  cc2420_writereg(cc2420_IOCFG0, cc2420_currentparameters[CP_IOCFG0]);
  cc2420_writereg(cc2420_IOCFG1, cc2420_currentparameters[CP_IOCFG1]);

  cc2420_sendcmd(cc2420_SFLUSHTX);
  cc2420_sendcmd(cc2420_SFLUSHRX);
   printf("reginit is success\n");
  return TRUE;
}

//----------------------------------------------------------------FIFOP中断处理中转
inline uint8_t cc2420_fifopintr(void){
    
 
   if (!HAL_READ_CC_FIFO_PIN()) {
      //printf("收到一个包,但芯片缓存 溢出\n");
      cc2420_readreg(0x3F);
      rfpackernum=0;
      cc2420_sendcmd(0x08);
      cc2420_sendcmd(0x08);
      return SUCCESS;
    }
    
    rfpackernum++;
    
   // printf("收到一个包,启动接收任务\n");
    if (cc2420_rxbufuse>1) 
		{
		      //printf("没有可用的ram空间，等待中!");
		      return SUCCESS;
		
    	       }
    else 
		{
              cc2420_rxbufuse++;
		HAL_post(&cc2420_readrxfifo);
    	      }
    return SUCCESS;
 }
//---------------------------------------------------------打开对fifop的中断支持,下降延触发
inline  uint8_t cc2420_enablefifop(void)
{
  EICRB &= ~(1 << 4);
  EICRB |= 1 << 5;
  EIMSK |= 1 << 6;
  return SUCCESS;
}

//---------------------------------------------------------向cc2420发送一个命令
uint8_t cc2420_sendcmd(uint8_t addr)
{
   uint8_t status;

  { hal_atomic_t _nesc_atomic = hal_atomic_start();

    {
      HAL_CLR_CC_CS_PIN();
      SPDR = addr;//这是单片机内部spi数据寄存器的地址
      while (!(SPSR & 0x80)) {//这是单片机内部spi状态寄存器的地址
        }
      ;
      status = SPDR;
    }
    hal_atomic_end(_nesc_atomic); }
    HAL_SET_CC_CS_PIN();
  return status;
}
//---------------------------------------------------------向cc2420指定的地址写数据
uint8_t cc2420_writereg(uint8_t addr, uint16_t data)
{
  uint8_t status;
  { hal_atomic_t _nesc_atomic = hal_atomic_start();
    {
      cc2420_bramspi = FALSE;
      HAL_CLR_CC_CS_PIN();
      SPDR = addr;
      while (!(SPSR & 0x80)) {
        }
      ;
      status = SPDR;
      if (addr > 0x0E) {
          SPDR = data >> 8;
          while (!(SPSR & 0x80)) {
            }
          ;
          SPDR = data & 0xff;
          while (!(SPSR & 0x80)) {
            }
          ;
        }
      cc2420_bramspi = TRUE;
    }
    hal_atomic_end(_nesc_atomic); }
  HAL_SET_CC_CS_PIN();
  return status;
}

//---------------------------------------------------------------从cc2420指定的地址读数据
uint16_t cc2420_readreg(uint8_t addr)
{
  uint16_t data = 0;
  uint8_t status;
  { hal_atomic_t _nesc_atomic = hal_atomic_start();
    {
      cc2420_bramspi = FALSE;
      HAL_CLR_CC_CS_PIN();
      SPDR = addr | 0x40;
      while (!(SPSR & 0x80)) {
        }
      ;
      status = SPDR;
      SPDR = 0;
      while (!(SPSR & 0x80)) {
        }
      ;
      data = SPDR;
      SPDR = 0;
      while (!(SPSR & 0x80)) {
        }
      ;
      data = (data << 8) | SPDR;
      HAL_SET_CC_CS_PIN();
      cc2420_bramspi = TRUE;
    }
    hal_atomic_end(_nesc_atomic); }
  return data;
}

uint8_t cc2420_readlargeram(uint16_t addr, uint8_t length, uint8_t *buffer)
{
   uint8_t i = 0;
   uint8_t status;
  if (!cc2420_bramspi) {
    return FALSE;
    }

  { hal_atomic_t _nesc_atomic = hal_atomic_start();

    {
      cc2420_bramspi = FALSE;
      cc2420_ramaddr = addr;
      cc2420_ramlen = length;
      
      HAL_CLR_CC_CS_PIN();
		  
      SPDR = (cc2420_ramaddr & 0x7F) | 0x80;
      while (!(SPSR & 0x80)) {
        }
      ;
      status = SPDR;
      //printf("status1 is %x\n",status);
       
       SPDR = (cc2420_ramaddr >> 1) & 0xe0;
       while (!(SPSR & 0x80)) ;
	status=SPDR;
      	SPDR=0;
	for (i = 0; i < cc2420_ramlen; i++) {
		 	 
                      while (!(SPSR & 0x80)) {
					  	
                       }
		        buffer[i]=SPDR;
			 SPDR=0;
		 	//printf("%x\n",buffer[i]);
        }
    }
  HAL_SET_CC_CS_PIN();
    hal_atomic_end(_nesc_atomic); }
  cc2420_bramspi = TRUE;
  return SUCCESS;
}

//---------------------------------------------------------连续写cc2420ram
uint8_t cc2420_writelargeram(uint16_t addr, uint8_t length, uint8_t *buffer)
{
  uint8_t i = 0;
  uint8_t status;
  if (!cc2420_bramspi) {
    return FALSE;
    }
  { hal_atomic_t _nesc_atomic = hal_atomic_start();

    {
      cc2420_bramspi = FALSE;
      cc2420_ramaddr = addr;
      cc2420_ramlen = length;
      cc2420_rambuf = buffer;
      HAL_CLR_CC_CS_PIN();
      SPDR = (cc2420_ramaddr & 0x7F) | 0x80;
      while (!(SPSR & 0x80)) {
        }
      ;
      status = SPDR;
      
      SPDR = (cc2420_ramaddr >> 1) & 0xC0;
      while (!(SPSR & 0x80)) {
        }
      ;
      status = SPDR;

      for (i = 0; i < cc2420_ramlen; i++) {
          SPDR = cc2420_rambuf[i];
          while (!(SPSR & 0x80)) {
            }
          ;
        }
    }
    HAL_SET_CC_CS_PIN();
    hal_atomic_end(_nesc_atomic); }
  cc2420_bramspi = TRUE;
  return SUCCESS;
}

//-------------------------------------------------------------------------
uint8_t cc2420_writetxfifo(uint8_t len, uint8_t *msg)
{
  uint8_t i = 0;
  uint8_t status;
  { hal_atomic_t _nesc_atomic = hal_atomic_start();
    {
      cc2420fifo_bspi = FALSE;
      cc2420fifo_txlength = len;
      cc2420fifo_txbuf = msg;
      HAL_CLR_CC_CS_PIN();
      SPDR = 0x3E;
      while (!(SPSR & 0x80)) {
        }
      ;
      status = SPDR;
      for (i = 0; i < cc2420fifo_txlength; i++) {
          SPDR = *cc2420fifo_txbuf;
          cc2420fifo_txbuf++;
          while (!(SPSR & 0x80)) {
            }
          ;
        }
      cc2420fifo_bspi = TRUE;
    }
    hal_atomic_end(_nesc_atomic); }
  HAL_SET_CC_CS_PIN();
 // printf("a packet will be send!\n");
  cc2420_sendcmd(cc2420_STXON); //send 
  //printf("a packet is success send!\n");
  return status;
}
//-----------------------------------------------------------------------
void  cc2420_readrxfifo()
{
  uint8_t status;
  uint8_t i;
  //printf("开始一个读fifo任务\n");
  { hal_atomic_t _nesc_atomic = hal_atomic_start();
    {
      cc2420fifo_bspi = FALSE;
      HAL_CLR_CC_CS_PIN();
	  
      SPDR = 0x3F | 0x40;//read rxfifo
      while (!(SPSR & 0x80))     ;
      status = SPDR;
      SPDR = 0;
      while (!(SPSR & 0x80))      ;
	  
      cc2420_rxlength = SPDR;
	  
      //printf("开始分配空间给rxbuf,rxflag is %x\n",cc2420_rxflag);
	  
    
      cc2420_rxbuf[cc2420_rxflag] =malloc(cc2420_rxlength +1);
     // printf("point is %04x\n",cc2420_rxbuf[cc2420_rxflag] );
   
	  
      if (cc2420_rxlength > 0) {
          cc2420_rxbuf[cc2420_rxflag] [0] = cc2420_rxlength ;

          cc2420_rxlength ++;

          
          for (i = 1; i < cc2420_rxlength ; i++) {
              SPDR = 0;
              while (!(SPSR & 0x80)) {
                }
              ;
              cc2420_rxbuf[cc2420_rxflag] [i] = SPDR;
            }
        }
     
      cc2420fifo_bspi = TRUE;
     
      rfpackernum--;
      cc2420_rxflag++;  
      cc2420_rxflag&=0x01;
      
    }
    hal_atomic_end(_nesc_atomic); }
  HAL_SET_CC_CS_PIN();
  HAL_post(&demo_handlepacket);
}

//-------------------------------------------------------------------------
inline  uint8_t cc2420_oscon(void)

{
  uint8_t i;
  uint8_t status;
  bool bXoscOn = FALSE;
  i = 0;
  cc2420_sendcmd(cc2420_SXOSCON);
  while (i < 200 && bXoscOn == FALSE) {
      HAL_uwait(100);
      status = cc2420_sendcmd(cc2420_SNOP);
      status = status & (1 << 6);
      if (status) {

        bXoscOn = TRUE;
        }

      i++;
    }
  if (!bXoscOn) {
      printf("osc is unsuccess now\n");
    return FAIL;
    }
  printf("osc is success now\n");
  return SUCCESS;
}

inline uint8_t cc2420_oscoff(void) {
    cc2420_sendcmd(cc2420_SXOSCOFF);   //turn-off crystal
    return SUCCESS;
  }

//--------------------------------------------------------
inline  uint8_t cc2420_VREFOn(void){
    HAL_SET_CC_VREN_PIN();                    //turn-on  
    HAL_uwait(1800);    
    printf("vref is on now\n");
	return SUCCESS;
  }
inline  uint8_t cc2420_VREFOff(void){
    HAL_CLR_CC_VREN_PIN();                    //turn-off  
    HAL_uwait(1800);  
	return SUCCESS;
  }
//------------------------------------------------------------------
inline  uint8_t cc2420_enableAutoAck() {
    cc2420_currentparameters[CP_MDMCTRL0] |= (1 << cc2420_MDMCTRL0_AUTOACK);
    return cc2420_writereg(cc2420_MDMCTRL0,cc2420_currentparameters[CP_MDMCTRL0]);
  }

inline  uint8_t cc2420_disableAutoAck() {
    cc2420_currentparameters[CP_MDMCTRL0] &= ~(1 << cc2420_MDMCTRL0_AUTOACK);
    return cc2420_writereg(cc2420_MDMCTRL0,cc2420_currentparameters[CP_MDMCTRL0]);
  }

inline  uint8_t cc2420_enableAddrDecode() {
    cc2420_currentparameters[CP_MDMCTRL0] |= (1 << cc2420_MDMCTRL0_ADRDECODE);
    return cc2420_writereg(cc2420_MDMCTRL0,cc2420_currentparameters[CP_MDMCTRL0]);
  }

inline  uint8_t cc2420_disableAddrDecode() {
    cc2420_currentparameters[CP_MDMCTRL0] &= ~(1 << cc2420_MDMCTRL0_ADRDECODE);
    return cc2420_writereg(cc2420_MDMCTRL0,cc2420_currentparameters[CP_MDMCTRL0]);
  }

//---------------------------------------------------------命令cc2420进入rx模式

inline   uint8_t cc2420_setrxmode(void)

{
  cc2420_sendcmd(cc2420_SRXON);
  return SUCCESS;
}

inline   uint8_t cc2420_settxmode(void)

{
  cc2420_sendcmd(cc2420_STXON);
  return SUCCESS;
}

inline   uint8_t cc2420_settxccamode(void)

{
  cc2420_sendcmd(cc2420_STXONCCA);
  return SUCCESS;
}

inline   uint8_t cc2420_setrfoff(void)

{
  cc2420_sendcmd(cc2420_SRFOFF);
  return SUCCESS;
}


//---------------------------------------------------------设置cc2420 的频道
 inline uint8_t cc2420_setchnl(uint8_t chnl)

{
  int fsctrl;

  fsctrl = 357 + 5 * (chnl - 11);
  cc2420_currentparameters[CP_FSCTRL] = (cc2420_currentparameters[CP_FSCTRL] & 0xfc00) | (fsctrl << 0);
  cc2420_writereg(cc2420_FSCTRL, cc2420_currentparameters[CP_FSCTRL]);
  return SUCCESS;
}

//---------------------------------------------------------设置cc2420短地址，同时写入cc2420
inline uint8_t cc2420_setshortaddress(uint16_t addr)

{
   addr = toLSB16(addr);
   cc2420_writelargeram(cc2420_RAM_SHORTADR, 2, (uint8_t *)&addr);
   printf("shortaddress is set\n");
   return SUCCESS;
}

inline uint8_t cc2420_getshortaddress(uint16_t * addr)

{
     cc2420_readlargeram(cc2420_RAM_SHORTADR, 2, (uint8_t *)addr);
     printf("shortaddress is get\n");
	 return SUCCESS;
}

 inline uint8_t cc2420_setpanid(uint16_t id)
 {
 	id = toLSB16(id);
       cc2420_writelargeram(cc2420_RAM_PANID, 2, (uint8_t *)&id);
       printf("PANID is set\n");
       return SUCCESS;
 }

inline uint8_t cc2420_getpanid(uint16_t * id)

{
     cc2420_readlargeram(cc2420_RAM_PANID, 2, (uint8_t *)&id);
     printf("PANID is get\n");
     return SUCCESS;
}
//------------------------------------------------------------------------------
 

 



inline void cc2420_init(void)
{
	cc2420_pin_init();
	SPI_init();
	cc2420_VREFOn();
	HAL_CLR_CC_RSTN_PIN();
	HAL_uwait(10);  
	HAL_SET_CC_RSTN_PIN();
	HAL_uwait(10); 
	cc2420_oscon();
	cc2420_reginit();
	cc2420_setreg();
       cc2420_rxbuf_now=0;
       cc2420_rxlength_now=0;
	cc2420_rxflag=0;
	cc2420_rxbufuse=0;
	rfpackernum=0;
       printf("cc2420 init is success\n");
}


void demo_handlepacket(void)
{
       uint8_t num,i,flag;
	flag=(cc2420_rxflag+cc2420_rxbufuse)&0x01;
	cc2420_rxbuf_now=cc2420_rxbuf[flag];
	//intf("\nreceive a packet in channel %04x!\n",cc2420_readreg(cc2420_FSCTRL));
	
	num=cc2420_rxbuf_now[0]+1;
	for (i=0;i<num;i++)
		printf("%02x",cc2420_rxbuf_now[i]);
	   
	usart_putbyte(0x7e);
	usart_putbyte(0x42);
	
	usart_putbyte(0x21);
	usart_putbyte(0x02);
	usart_putbyte(0x03);
	usart_putbyte(cc2420_rxbuf_now[10]);//source address
	usart_putbyte(0x01);
	usart_putbyte(cc2420_rxbuf_now[3]);//adc sequec
	
	usart_putbyte(cc2420_rxbuf_now[12]);//adc data0
	usart_putbyte(cc2420_rxbuf_now[13]);//adc data0
	
	usart_putbyte(cc2420_rxbuf_now[14]);//adc data1
	usart_putbyte(cc2420_rxbuf_now[15]);//adc data1
	
	usart_putbyte(cc2420_rxbuf_now[16]);//adc data2
	usart_putbyte(cc2420_rxbuf_now[17]);//adc data2
	
	usart_putbyte(cc2420_rxbuf_now[18]);//adc data3
	usart_putbyte(cc2420_rxbuf_now[19]);//adc data3
	
	usart_putbyte(cc2420_rxbuf_now[20]);//adc data4
	usart_putbyte(cc2420_rxbuf_now[21]);//adc data4
	
	usart_putbyte(cc2420_rxbuf_now[22]);//adc data5
	usart_putbyte(cc2420_rxbuf_now[23]);//adc data5
	
	usart_putbyte(cc2420_rxbuf_now[24]);//adc data6
	usart_putbyte(cc2420_rxbuf_now[25]);//adc data6
	
	usart_putbyte(cc2420_rxbuf_now[26]);//adc data7
	usart_putbyte(cc2420_rxbuf_now[27]);//adc data7
	
	usart_putbyte(cc2420_rxbuf_now[28]);//adc data8
	usart_putbyte(cc2420_rxbuf_now[29]);//adc data8
	
	usart_putbyte(cc2420_rxbuf_now[30]);//adc data9
	usart_putbyte(cc2420_rxbuf_now[31]);//adc data9
	
	usart_putbyte(0x01);//crc1
	usart_putbyte(0x01);//crc0
	
	usart_putbyte(0x7e);
	
	 
	free(cc2420_rxbuf[flag]);
	cc2420_rxbufuse--;
	if  ((cc2420_rxbufuse==0) && (rfpackernum!=0))
		{
		   cc2420_rxbufuse++;
		   HAL_post(&cc2420_readrxfifo);
		}
	cc2420_rxbuf[flag]=NULL;
	//printf("释放rxbuf,now point is %04x\n",cc2420_rxbuf[flag]);
}
	



