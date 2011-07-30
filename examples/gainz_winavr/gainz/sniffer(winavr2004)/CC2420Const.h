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
#ifndef _cc2420CONST_H
#define _cc2420CONST_H

#ifndef cc2420_DEF_CHANNEL
#define cc2420_DEF_CHANNEL	3  //channel select
#endif
 
#define cc2420_DEF_PRESET 2405  //freq select

#define cc2420_DEF_FCF_LO          0x08
#define cc2420_DEF_FCF_HI          0x01  // without ACK
#define cc2420_DEF_FCF_HI_ACK      0x21  // with ACK
#define cc2420_DEF_FCF_TYPE_BEACON 0x00
#define cc2420_DEF_FCF_TYPE_DATA   0x01
#define cc2420_DEF_FCF_TYPE_ACK    0x02
#define cc2420_DEF_FCF_BIT_ACK     5
#define cc2420_DEF_BACKOFF         500
#define cc2420_SYMBOL_TIME         16 // 2^4
// 20 symbols make up a backoff period
// 10 jiffy's make up a backoff period
// due to timer overhead, 30.5us is close enough to 32us per 2 symbols
#define cc2420_SYMBOL_UNIT         10
// delay 20 jiffies when waiting for the ack
#define cc2420_ACK_DELAY           20

#define cc2420_SNOP            0x00
#define cc2420_SXOSCON         0x01
#define cc2420_STXCAL          0x02
#define cc2420_SRXON           0x03
#define cc2420_STXON           0x04
#define cc2420_STXONCCA        0x05
#define cc2420_SRFOFF          0x06
#define cc2420_SXOSCOFF        0x07
#define cc2420_SFLUSHRX        0x08
#define cc2420_SFLUSHTX        0x09
#define cc2420_SACK            0x0A
#define cc2420_SACKPEND        0x0B
#define cc2420_SRXDEC          0Rx0C
#define cc2420_STXENC          0x0D
#define cc2420_SAES            0x0E
#define cc2420_MAIN            0x10
#define cc2420_MDMCTRL0        0x11
#define cc2420_MDMCTRL1        0x12
#define cc2420_RSSI            0x13
#define cc2420_SYNCWORD        0x14
#define cc2420_TXCTRL          0x15
#define cc2420_RXCTRL0         0x16
#define cc2420_RXCTRL1         0x17
#define cc2420_FSCTRL          0x18
#define cc2420_SECCTRL0        0x19
#define cc2420_SECCTRL1        0x1A
#define cc2420_BATTMON         0x1B
#define cc2420_IOCFG0          0x1C
#define cc2420_IOCFG1          0x1D
#define cc2420_MANFIDL         0x1E
#define cc2420_MANFIDH         0x1F
#define cc2420_FSMTC           0x20
#define cc2420_MANAND          0x21
#define cc2420_MANOR           0x22
#define cc2420_AGCCTRL         0x23
#define cc2420_AGCTST0         0x24
#define cc2420_AGCTST1         0x25
#define cc2420_AGCTST2         0x26
#define cc2420_FSTST0          0x27
#define cc2420_FSTST1          0x28
#define cc2420_FSTST2          0x29
#define cc2420_FSTST3          0x2A
#define cc2420_RXBPFTST        0x2B
#define cc2420_FSMSTATE        0x2C
#define cc2420_ADCTST          0x2D
#define cc2420_DACTST          0x2E
#define cc2420_TOPTST          0x2F
#define cc2420_RESERVED        0x30
#define cc2420_TXFIFO          0x3E
#define cc2420_RXFIFO          0x3F

#define cc2420_RAM_SHORTADR    0x16A
#define cc2420_RAM_PANID       0x168
#define cc2420_RAM_IEEEADR     0x160
#define cc2420_RAM_CBCSTATE    0x150
#define cc2420_RAM_TXNONCE     0x140
#define cc2420_RAM_KEY1        0x130
#define cc2420_RAM_SABUF       0x120
#define cc2420_RAM_RXNONCE     0x110
#define cc2420_RAM_KEY0        0x100
#define cc2420_RAM_RXFIFO      0x080
#define cc2420_RAM_TXFIFO      0x000

// MDMCTRL0 Register Bit Positions
#define cc2420_MDMCTRL0_FRAME        13  // 0 : reject reserved frame types, 1 = accept
#define cc2420_MDMCTRL0_PANCRD       12  // 0 : not a PAN coordinator
#define cc2420_MDMCTRL0_ADRDECODE    11  // 1 : enable address decode
#define cc2420_MDMCTRL0_CCAHIST      8   // 3 bits (8,9,10) : CCA hysteris in db
#define cc2420_MDMCTRL0_CCAMODE      6   // 2 bits (6,7)    : CCA trigger modes
#define cc2420_MDMCTRL0_AUTOCRC      5   // 1 : generate/chk CRC
#define cc2420_MDMCTRL0_AUTOACK      4   // 1 : Ack valid packets
#define cc2420_MDMCTRL0_PREAMBL      0   // 4 bits (0..3): Preamble length

// MDMCTRL1 Register Bit Positions
#define cc2420_MDMCTRL1_CORRTHRESH   6   // 5 bits (6..10) : correlator threshold
#define cc2420_MDMCTRL1_DEMOD_MODE   5   // 0: lock freq after preamble match, 1: continous udpate
#define cc2420_MDMCTRL1_MODU_MODE    4   // 0: IEEE 802.15.4
#define cc2420_MDMCTRL1_TX_MODE      2   // 2 bits (2,3) : 0: use buffered TXFIFO
#define cc2420_MDMCTRL1_RX_MODE      0   // 2 bits (0,1) : 0: use buffered RXFIFO

// RSSI Register Bit Positions
#define cc2420_RSSI_CCA_THRESH       8   // 8 bits (8..15) : 2's compl CCA threshold

// TXCTRL Register Bit Positions
#define cc2420_TXCTRL_BUFCUR         14  // 2 bits (14,15) : Tx mixer buffer bias current
#define cc2420_TXCTRL_TURNARND       13  // wait time after STXON before xmit
#define cc2420_TXCTRL_VAR            11  // 2 bits (11,12) : Varactor array settings
#define cc2420_TXCTRL_XMITCUR        9   // 2 bits (9,10)  : Xmit mixer currents
#define cc2420_TXCTRL_PACUR          6   // 3 bits (6..8)  : PA current
#define cc2420_TXCTRL_PADIFF         5   // 1: Diff PA, 0: Single ended PA
#define cc2420_TXCTRL_PAPWR          0   // 5 bits (0..4): Output PA level

// RXCTRL0 Register Bit Positions
#define cc2420_RXCTRL0_BUFCUR        12  // 2 bits (12,13) : Rx mixer buffer bias current
#define cc2420_RXCTRL0_HILNAG        10  // 2 bits (10,11) : High gain, LNA current
#define cc2420_RXCTRL0_MLNAG          8  // 2 bits (8,9)   : Med gain, LNA current
#define cc2420_RXCTRL0_LOLNAG         6  // 2 bits (6,7)   : Lo gain, LNA current
#define cc2420_RXCTRL0_HICUR          4  // 2 bits (4,5)   : Main high LNA current
#define cc2420_RXCTRL0_MCUR           2  // 2 bits (2,3)   : Main med  LNA current
#define cc2420_RXCTRL0_LOCUR          0  // 2 bits (0,1)   : Main low LNA current

// RXCTRL1 Register Bit Positions
#define cc2420_RXCTRL1_LOCUR         13  // Ref bias current to Rx bandpass filter
#define cc2420_RXCTRL1_MIDCUR        12  // Ref bias current to Rx bandpass filter
#define cc2420_RXCTRL1_LOLOGAIN      11  // LAN low gain mode
#define cc2420_RXCTRL1_MEDLOGAIN     10  // LAN low gain mode
#define cc2420_RXCTRL1_HIHGM          9  // Rx mixers, hi gain mode
#define cc2420_RXCTRL1_MEDHGM         8  // Rx mixers, hi gain mode
#define cc2420_RXCTRL1_LNACAP         6  // 2 bits (6,7) Selects LAN varactor array setting
#define cc2420_RXCTRL1_RMIXT          4  // 2 bits (4,5) Receiver mixer output current
#define cc2420_RXCTRL1_RMIXV          2  // 2 bits (2,3) VCM level, mixer feedback
#define cc2420_RXCTRL1_RMIXCUR        0  // 2 bits (0,1) Receiver mixer current

// FSCTRL Register Bit Positions
#define cc2420_FSCTRL_LOCK            14 // 2 bits (14,15) # of clocks for synch
#define cc2420_FSCTRL_CALDONE         13 // Read only, =1 if cal done since freq synth turned on
#define cc2420_FSCTRL_CALRUNING       12 // Read only, =1 if cal in progress
#define cc2420_FSCTRL_LOCKLEN         11 // Synch window pulse width
#define cc2420_FSCTRL_LOCKSTAT        10 // Read only, = 1 if freq synthesizer is loced
#define cc2420_FSCTRL_FREQ             0 // 10 bits, set operating frequency 

// SECCTRL0 Register Bit Positions
#define cc2420_SECCTRL0_PROTECT        9 // Protect enable Rx fifo
#define cc2420_SECCTRL0_CBCHEAD        8 // Define 1st byte of CBC-MAC
#define cc2420_SECCTRL0_SAKEYSEL       7 // Stand alone key select
#define cc2420_SECCTRL0_TXKEYSEL       6 // Tx key select
#define cc2420_SECCTRL0_RXKEYSEL       5 // Rx key select
#define cc2420_SECCTRL0_SECM           2 // 2 bits (2..4) # of bytes in CBC-MAX auth field
#define cc2420_SECCTRL0_SECMODE        0 // Security mode

// SECCTRL1 Register Bit Positions
#define cc2420_SECCTRL1_TXL            8 // 7 bits (8..14) Tx in-line security
#define cc2420_SECCTRL1_RXL            0 // 7 bits (0..7)  Rx in-line security

// BATTMON  Register Bit Positions
#define cc2420_BATTMON_OK              6 // Read only, batter voltage OK
#define cc2420_BATTMON_EN              5 // Enable battery monitor
#define cc2420_BATTMON_VOLT            0 // 5 bits (0..4) Battery toggle voltage

// IOCFG0 Register Bit Positions
#define cc2420_IOCFG0_FIFOPOL         10 // Fifo signal polarity
#define cc2420_IOCFG0_FIFOPPOL         9 // FifoP signal polarity
#define cc2420_IOCFG0_SFD              8 // SFD signal polarity
#define cc2420_IOCFG0_CCAPOL           7 // CCA signal polarity
#define cc2420_IOCFG0_FIFOTHR          0 // 7 bits, (0..6) # of Rx bytes in fifo to trg fifop

// IOCFG1 Register Bit Positions
#define cc2420_IOCFG1_HSSD            10 // 2 bits (10,11) HSSD module config
#define cc2420_IOCFG1_SFDMUX           5 // 5 bits (5..9)  SFD multiplexer pin settings
#define cc2420_IOCFG1_CCAMUX           0 // 5 bits (0..4)  CCA multiplexe pin settings

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
#define cc2420_XOSC16M_STABLE	6
#define cc2420_TX_UNDERFLOW	5
#define cc2420_ENC_BUSY		4
#define cc2420_TX_ACTIVE	3
#define cc2420_LOCK   		2
#define cc2420_RSSI_VALID	1
 
#endif /* _cc2420CONST_H */
