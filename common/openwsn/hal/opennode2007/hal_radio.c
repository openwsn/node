/*****************************************************************************
 * @author openwsn on 200801
 * implementation for wireless radio chip such as TI CC2420/CC2430, etc
 * the current implmentation only support CC2420 now.
 * 
 * @state
 * still in developing
 ****************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_radio.h"

#define CONFIG_RADIO_CC2420

#ifdef CONFIG_RADIO_CC2420

/* registers of cc2420 */
#define CC2420_SNOP            0x00
#define CC2420_SXOSCON        0x01
#define CC2420_STXCAL          0x02
#define CC2420_SRXON          0x03
#define CC2420_STXON          0x04
#define CC2420_STXONCCA        0x05
#define CC2420_SRFOFF          0x06
#define CC2420_SXOSCOFF        0x07
#define CC2420_SFLUSHRX        0x08
#define CC2420_SFLUSHTX        0x09
#define CC2420_SACK            0x0A
#define CC2420_SACKPEND        0x0B
#define CC2420_SRXDEC          0x0C
#define CC2420_STXENC          0x0D
#define CC2420_SAES            0x0E
#define CC2420_MAIN            0x10
#define CC2420_MDMCTRL0        0x11
#define CC2420_MDMCTRL1        0x12
#define CC2420_RSSI            0x13
#define CC2420_SYNCWORD        0x14
#define CC2420_TXCTRL          0x15
#define CC2420_RXCTRL0        0x16
#define CC2420_RXCTRL1        0x17
#define CC2420_FSCTRL          0x18
#define CC2420_SECCTRL0        0x19
#define CC2420_SECCTRL1        0x1A
#define CC2420_BATTMON        0x1B
#define CC2420_IOCFG0          0x1C
#define CC2420_IOCFG1          0x1D
#define CC2420_MANFIDL        0x1E
#define CC2420_MANFIDH        0x1F
#define CC2420_FSMTC          0x20
#define CC2420_MANAND          0x21
#define CC2420_MANOR          0x22
#define CC2420_AGCCTRL        0x23
#define CC2420_AGCTST0        0x24
#define CC2420_AGCTST1        0x25
#define CC2420_AGCTST2        0x26
#define CC2420_FSTST0          0x27
#define CC2420_FSTST1          0x28
#define CC2420_FSTST2          0x29
#define CC2420_FSTST3          0x2A
#define CC2420_RXBPFTST        0x2B
#define CC2420_FSMSTATE        0x2C
#define CC2420_ADCTST          0x2D
#define CC2420_DACTST          0x2E
#define CC2420_TOPTST          0x2F
#define CC2420_RESERVED        0x30
#define CC2420_TXFIFO          0x3E
#define CC2420_RXFIFO          0x3F
/* address for RAM of cc2420 */
#define CC2420_RAM_SHORTADR    0x16A
#define CC2420_RAM_PANID      0x168
#define CC2420_RAM_IEEEADR    0x160
#define CC2420_RAM_CBCSTATE    0x150
#define CC2420_RAM_TXNONCE    0x140
#define CC2420_RAM_KEY1        0x130
#define CC2420_RAM_SABUF      0x120
#define CC2420_RAM_RXNONCE    0x110
#define CC2420_RAM_KEY0        0x100
#define CC2420_RAM_RXFIFO      0x080
#define CC2420_RAM_TXFIFO      0x000
// MDMCTRL0 Register Bit Positions
#define CC2420_MDMCTRL0_FRAME        13  // 0 : reject reserved frame types, 1 = accept
#define CC2420_MDMCTRL0_PANCRD      12  // 0 : not a PAN coordinator
#define CC2420_MDMCTRL0_ADRDECODE    11  // 1 : enable address decode
#define CC2420_MDMCTRL0_CCAHIST      8  // 3 bits (8,9,10) : CCA hysteris in db
#define CC2420_MDMCTRL0_CCAMODE      6  // 2 bits (6,7)    : CCA trigger modes
#define CC2420_MDMCTRL0_AUTOCRC      5  // 1 : generate/chk CRC
#define CC2420_MDMCTRL0_AUTOACK      4  // 1 : Ack valid packets
#define CC2420_MDMCTRL0_PREAMBL      0  // 4 bits (0..3): Preamble length
// MDMCTRL1 Register Bit Positions
#define CC2420_MDMCTRL1_CORRTHRESH  6  // 5 bits (6..10) : correlator threshold
#define CC2420_MDMCTRL1_DEMOD_MODE  5  // 0: lock freq after preamble match, 1: continous udpate
#define CC2420_MDMCTRL1_MODU_MODE    4  // 0: IEEE 802.15.4
#define CC2420_MDMCTRL1_TX_MODE      2  // 2 bits (2,3) : 0: use buffered TXFIFO
#define CC2420_MDMCTRL1_RX_MODE      0  // 2 bits (0,1) : 0: use buffered RXFIFO
// RSSI Register Bit Positions
#define CC2420_RSSI_CCA_THRESH      8  // 8 bits (8..15) : 2's compl CCA threshold
// TXCTRL Register Bit Positions
#define CC2420_TXCTRL_BUFCUR        14  // 2 bits (14,15) : Tx mixer buffer bias current
#define CC2420_TXCTRL_TURNARND      13  // wait time after STXON before xmit
#define CC2420_TXCTRL_VAR            11  // 2 bits (11,12) : Varactor array settings
#define CC2420_TXCTRL_XMITCUR        9  // 2 bits (9,10)  : Xmit mixer currents
#define CC2420_TXCTRL_PACUR          6  // 3 bits (6..8)  : PA current
#define CC2420_TXCTRL_PADIFF        5  // 1: Diff PA, 0: Single ended PA
#define CC2420_TXCTRL_PAPWR          0  // 5 bits (0..4): Output PA level
// Mask for the CC2420_TXCTRL_PAPWR register for RF power
#define CC2420_TXCTRL_PAPWR_MASK (0x1F << CC2420_TXCTRL_PAPWR)
// RXCTRL0 Register Bit Positions
#define CC2420_RXCTRL0_BUFCUR        12  // 2 bits (12,13) : Rx mixer buffer bias current
#define CC2420_RXCTRL0_HILNAG        10  // 2 bits (10,11) : High gain, LNA current
#define CC2420_RXCTRL0_MLNAG          8  // 2 bits (8,9)  : Med gain, LNA current
#define CC2420_RXCTRL0_LOLNAG        6  // 2 bits (6,7)  : Lo gain, LNA current
#define CC2420_RXCTRL0_HICUR          4  // 2 bits (4,5)  : Main high LNA current
#define CC2420_RXCTRL0_MCUR          2  // 2 bits (2,3)  : Main med  LNA current
#define CC2420_RXCTRL0_LOCUR          0  // 2 bits (0,1)  : Main low LNA current
// RXCTRL1 Register Bit Positions
#define CC2420_RXCTRL1_LOCUR        13  // Ref bias current to Rx bandpass filter
#define CC2420_RXCTRL1_MIDCUR        12  // Ref bias current to Rx bandpass filter
#define CC2420_RXCTRL1_LOLOGAIN      11  // LAN low gain mode
#define CC2420_RXCTRL1_MEDLOGAIN    10  // LAN low gain mode
#define CC2420_RXCTRL1_HIHGM          9  // Rx mixers, hi gain mode
#define CC2420_RXCTRL1_MEDHGM        8  // Rx mixers, hi gain mode
#define CC2420_RXCTRL1_LNACAP        6  // 2 bits (6,7) Selects LAN varactor array setting
#define CC2420_RXCTRL1_RMIXT          4  // 2 bits (4,5) Receiver mixer output current
#define CC2420_RXCTRL1_RMIXV          2  // 2 bits (2,3) VCM level, mixer feedback
#define CC2420_RXCTRL1_RMIXCUR        0  // 2 bits (0,1) Receiver mixer current
// FSCTRL Register Bit Positions
#define CC2420_FSCTRL_LOCK            14 // 2 bits (14,15) # of clocks for synch
#define CC2420_FSCTRL_CALDONE        13 // Read only, =1 if cal done since freq synth turned on
#define CC2420_FSCTRL_CALRUNING      12 // Read only, =1 if cal in progress
#define CC2420_FSCTRL_LOCKLEN        11 // Synch window pulse width
#define CC2420_FSCTRL_LOCKSTAT        10 // Read only, = 1 if freq synthesizer is loced
#define CC2420_FSCTRL_FREQ            0 // 10 bits, set operating frequency
// SECCTRL0 Register Bit Positions
#define CC2420_SECCTRL0_PROTECT        9 // Protect enable Rx fifo
#define CC2420_SECCTRL0_CBCHEAD        8 // Define 1st byte of CBC-MAC
#define CC2420_SECCTRL0_SAKEYSEL      7 // Stand alone key select
#define CC2420_SECCTRL0_TXKEYSEL      6 // Tx key select
#define CC2420_SECCTRL0_RXKEYSEL      5 // Rx key select
#define CC2420_SECCTRL0_SECM          2 // 2 bits (2..4) # of bytes in CBC-MAX auth field
#define CC2420_SECCTRL0_SECMODE        0 // Security mode
// SECCTRL1 Register Bit Positions
#define CC2420_SECCTRL1_TXL            8 // 7 bits (8..14) Tx in-line security
#define CC2420_SECCTRL1_RXL            0 // 7 bits (0..7)  Rx in-line security
// BATTMON  Register Bit Positions
#define CC2420_BATTMON_OK              6 // Read only, batter voltage OK
#define CC2420_BATTMON_EN              5 // Enable battery monitor
#define CC2420_BATTMON_VOLT            0 // 5 bits (0..4) Battery toggle voltage
// IOCFG0 Register Bit Positions
#define CC2420_IOCFG0_FIFOPOL        10 // Fifo signal polarity
#define CC2420_IOCFG0_FIFOPPOL        9 // FifoP signal polarity
#define CC2420_IOCFG0_SFD              8 // SFD signal polarity
#define CC2420_IOCFG0_CCAPOL          7 // CCA signal polarity
#define CC2420_IOCFG0_FIFOTHR          0 // 7 bits, (0..6) # of Rx bytes in fifo to trg fifop
// IOCFG1 Register Bit Positions
#define CC2420_IOCFG1_HSSD            10 // 2 bits (10,11) HSSD module config
#define CC2420_IOCFG1_SFDMUX          5 // 5 bits (5..9)  SFD multiplexer pin settings
#define CC2420_IOCFG1_CCAMUX          0 // 5 bits (0..4)  CCA multiplexe pin settings
// Current Parameter Arrray Positions
enum{
CP_MAIN = 0,
CP_MDMCTRL0,
CP_MDMCTRL1,
CP_RSSI,
CP_SYNCWORD,
CP_TXCTRL,
CP_RXCTRL0,
CP_RXCTRL1,
CP_FSCTRL,
CP_SECCTRL0,
CP_SECCTRL1,
CP_BATTMON,
CP_IOCFG0,
CP_IOCFG1
} ;
// STATUS Bit Posititions
#define CC2420_XOSC16M_STABLE 6
#define CC2420_TX_UNDERFLOW 5
#define CC2420_ENC_BUSY  4
#define CC2420_TX_ACTIVE 3
#define CC2420_LOCK    2
#define CC2420_RSSI_VALID 1

#endif /* CONFIG_RADIO_CC2420 */


/*
*================================================================================================================
*
*================================================================================================================
*/
//#include "cc2420.h"
//#include "cc2420const.h"

uint8 CC2420_StrobCmd(uint8 addr)
{
uint8 status;
/*
USIGN16 uFmsState;
GetFmsState:
uFmsState = CC2420_ReadRegister(0x2c);
    switch(addr)
{
  case CC2420_SXOSCOFF:
  break;
  case CC2420_SRFOFF:
  break;
  case CC2420_SXOSCON:
  if(uFmsState == 0)
      break;
            if(uFmsState == 1)
      return 0x40;// OSC stabled
            else
  {
      CC2420_StrobCmd(CC2420_SXOSCOFF);
      goto GetFmsState;
  }
  case CC2420_SRXON:
  if(uFmsState == 1)
      break;
            else
  {
      CC2420_StrobCmd(CC2420_SRFOFF);
      goto GetFmsState;
  } 
  case CC2420_SACK:
  case CC2420_SACKPEND:
  if((uFmsState == 3) || (uFmsState == 4) ||
      (uFmsState == 5) || (uFmsState == 6))
      break;
            else
          goto GetFmsState;
  case CC2420_STXON:
  if((uFmsState == 1) ||
      (uFmsState == 2) || (uFmsState == 40) ||
      (uFmsState == 3) || (uFmsState == 4) || (uFmsState == 5)|| (uFmsState == 6) ||
      (uFmsState == 14) ||
      (uFmsState == 16))
      break;
            else
          goto GetFmsState;
  break;
  case CC2420_STXONCCA:
  if((uFmsState == 2) || (uFmsState == 40) ||
      (uFmsState == 3) || (uFmsState == 4) || (uFmsState == 5)|| (uFmsState == 6) ||
      (uFmsState == 14) ||
      (uFmsState == 16))
      break;
            else
          goto GetFmsState;
  break;
  default:
  break;
}
*/
	MAKE_CC_CS_LOW();
	status =  SPIPut(addr);
	MAKE_CC_CS_HIGH();
	return status;
}

uint8  CC2420_WriteRegister(uint8 addr, uint16 data)
{
	uint8 status;
	MAKE_CC_CS_LOW();
	status = SPIPut(addr);
	status = SPIPut(data>>8);
	status = SPIPut(data);
	MAKE_CC_CS_HIGH();
	return status;
}
uint16  CC2420_ReadRegister(uint8 addr)
{
	uint16 data;
	uint8 status;
	MAKE_CC_CS_LOW();
	status = SPIPut(addr | 0x40);
	data = SPIGet();
	data = (data<<8) | SPIGet();
	MAKE_CC_CS_HIGH();
	return data;
}
uint8  CC2420_WriteRam(uint16 addr, uint8 length, uint8* buf)
{
	uint8 ramaddr1;
	uint8 ramaddr2;
	uint8 i;
	uint8 status;
	ramaddr1 = (uint8)((addr & 0x7F) | 0x80);
	ramaddr2 = (uint8)((addr>>1) & 0xC0);
	MAKE_CC_CS_LOW();
	status = SPIPut(ramaddr1);
	status = SPIPut(ramaddr2);
	for (i=0; i<length; i++)
	{
		SPIPut(buf);
	}
	MAKE_CC_CS_HIGH();
	return status;
}

uint8  CC2420_ReadRam(uint16 addr, uint8 length, uint8* buf)
{
	uint8 ramaddr1;
	uint8 ramaddr2;
	uint8 i;
	uint8 status;
	ramaddr1 = (uint8)((addr & 0x7F) | 0x80);
	ramaddr2 = (uint8)(((addr>>1) | 0x20) & 0xE0);
	MAKE_CC_CS_LOW();
	status = SPIPut(ramaddr1);
	status = SPIPut(ramaddr2);
	for (i=0; i<length; i++)
	{
		buf = SPIGet();
	}
	MAKE_CC_CS_HIGH();
	return status;
}

uint8  CC2420_ReadRxFIFO(uint8 *length, uint8* data)
{
	uint8 status;
	uint8 i;

	status = CC2420_StrobCmd(CC2420_SNOP);
	// if the OSC is not stable yet
	if(status < 0x40) 
	{
		CC2420_Restart();
		return status;
	}
	MAKE_CC_CS_LOW();
	status = SPIPut(CC2420_RXFIFO | 0x40);
	*length = SPIGet();
	if(*length > 127)
	{
		MAKE_CC_CS_HIGH();
		MAKE_CC_CS_LOW();
		SPIPut(CC2420_SFLUSHRX);
        SPIPut(CC2420_SFLUSHRX);
        MAKE_CC_CS_HIGH();
        *length = 128;
        return status;
	}
	if (*length > 0)
	{
		for(i=0; i<*length; i++)
		{
			data = SPIGet();
			//UART_Put(data);
		}
	}
	MAKE_CC_CS_HIGH();
	return status;
}

void CC2420_FlushRxFIFO()
{
	MAKE_CC_CS_LOW();
	SPIPut(CC2420_RXFIFO | 0x40);
	SPIGet();
	MAKE_CC_CS_HIGH();
	MAKE_CC_CS_LOW();
	SPIPut(CC2420_SFLUSHRX);
    SPIPut(CC2420_SFLUSHRX);
    MAKE_CC_CS_HIGH();
}

uint8  CC2420_WriteTxFIFO(uint8 length, uint8* data)
{
	uint8 status;
	uint8 i;

	MAKE_CC_CS_LOW();
	status = SPIPut(CC2420_TXFIFO);
	
	// add another 2 bytes on the data length
	status = SPIPut(length + 2);
	for(i=0; i<length; i++)
	{
		SPIPut(data);
	}
	MAKE_CC_CS_HIGH();
	return status;
}

uint8 CC2420_Init()
{
	uint8 status;
	// initialize necessary PINs 
	SPIInit();
	MAKE_CC_RSTN_OUTPUT();
	MAKE_CC_VREN_OUTPUT();
	MAKE_CC_CS_OUTPUT();
	MAKE_FIFOP_INPUT();
	MAKE_CCA_INPUT();
	MAKE_SFD_INPUT();
	MAKE_FIFO_INPUT();
	MAKE_CC_CS_HIGH();
	// power on cc2420 and initialize
	MAKE_CC_VREN_HIGH();
	_delay_us(600);
	MAKE_CC_RSTN_LOW();
	_delay_us(1);
	MAKE_CC_RSTN_HIGH();
	_delay_us(1);
	// wait for OSC stable
	//while (!((CC2420_StrobCmd(CC2420_SXOSCON)) & 0x40));
	status = CC2420_StrobCmd(CC2420_SXOSCON);

	while (!(status & 0x40))
	{
		//CC2420_StrobCmd(CC2420_SXOSCOFF);
		status = CC2420_StrobCmd(CC2420_SXOSCON);
	}

	return status;
}
/************************************************
* set the physical communication channel.
* the value wrotten to FSCTRL.FREQ[9:0] is:
*          FSCTRL.FREQ = 357 + 5 (k-11)
*
* input: channel number in [11,26]
* return: described in PHY layer. depends
************************************************/
void CC2420_SetChannel(uint8 channel)
{
	uint16 temp;
	temp = CC2420_ReadRegister(CC2420_FSCTRL);
	temp = ((temp & 0xfc00) | (357+5*(channel - 11)));
	CC2420_WriteRegister(CC2420_FSCTRL, temp);
}

uint8 CC2420_GetChannel()
{
	uint8 temp;
	temp = ((CC2420_ReadRegister(CC2420_FSCTRL) & 0x03ff) - 357)/5 + 11;
	return temp;
}
void CC2420_SetTxPower(uint8 power)
{
	uint16 temp;
	temp = CC2420_ReadRegister(CC2420_TXCTRL);
	CC2420_WriteRegister(CC2420_TXCTRL, ((temp & 0xffe0) | power));
}
uint8 CC2420_GetTxPower()
{
	return (CC2420_ReadRegister(CC2420_TXCTRL) & 0x1f);
}
void CC2420_SetPANId(uint8 *panid)
{
	uint16 temp;
	if ((panid[0] == 0xff) && (panid[1] == 0xff))
	{
		temp = CC2420_ReadRegister(CC2420_IOCFG0);
		CC2420_WriteRegister(CC2420_IOCFG0, (temp | 0x0800));
	}
	else{
		temp = CC2420_ReadRegister(CC2420_IOCFG0);
		CC2420_WriteRegister(CC2420_IOCFG0, (temp & 0xf7ff));
	}
	temp = CC2420_ReadRegister(CC2420_IOCFG0);
	CC2420_WriteRam(CC2420_RAM_PANID, 2, panid);
}

void CC2420_GetPANId(uint8 *panid)
{
	CC2420_ReadRam(CC2420_RAM_PANID, 2, panid);
}

void CC2420_SetIEEEAddress(uint8 *ieeeaddr)
{
	CC2420_WriteRam(CC2420_RAM_IEEEADR, 8, ieeeaddr);
}

void CC2420_GetIEEEAddress(uint8 *ieeeaddr)
{
	CC2420_ReadRam(CC2420_RAM_PANID, 8, ieeeaddr);
}

void CC2420_SetShortAddress(uint8 *shortaddr)
{
	CC2420_WriteRam(CC2420_RAM_SHORTADR, 2, shortaddr);
}

void CC2420_GetShortAddress(uint8 *shortaddr)
{
	CC2420_ReadRam(CC2420_RAM_SHORTADR, 2, shortaddr);
}
void CC2420_SetCCAMode(uint8 mode)
{
	uint16 temp;
	temp = CC2420_ReadRegister(CC2420_MDMCTRL0);
	CC2420_WriteRegister(CC2420_MDMCTRL0, ((temp & 0xff3f) | (mode<<6)));
}

uint8 CC2420_GetCCAMode()
{
	return (CC2420_ReadRegister(CC2420_MDMCTRL0) & 0x00c0)>>6;
}

void CC2420_SetCoordinator(BOOL flag)
{
	uint16 temp;
	temp = CC2420_ReadRegister(CC2420_MDMCTRL0);
	if (flag)//Coordinator and Auto Ack
		CC2420_WriteRegister(CC2420_MDMCTRL0, (temp | 0x1010));
	else    //other device type and Auto Ack
		CC2420_WriteRegister(CC2420_MDMCTRL0, (temp | 0x0010));
}

void CC2420_SetAddressRecognition(BOOL flag)
{
	uint16 temp;
	temp = CC2420_ReadRegister(CC2420_MDMCTRL0);
	if (flag)
		CC2420_WriteRegister(CC2420_MDMCTRL0, (temp | 0x0800));
	else
		CC2420_WriteRegister(CC2420_MDMCTRL0, (temp & 0xf7ff));
}

void CC2420_Restart()
{
	uint8 status;
	status = CC2420_StrobCmd(CC2420_SXOSCON);
	while (!(status & 0x40))
	{
		//CC2420_StrobCmd(CC2420_SXOSCOFF);
		status = CC2420_StrobCmd(CC2420_SXOSCON);
	}
	status = CC2420_StrobCmd(CC2420_SRXON);
	MAKE_CC_CS_LOW();
	SPIPut(CC2420_SFLUSHRX);
	SPIPut(CC2420_SFLUSHRX);
	MAKE_CC_CS_HIGH();
}
