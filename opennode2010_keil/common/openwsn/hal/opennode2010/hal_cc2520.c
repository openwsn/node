// @todo
// @todo
// @todo
// @todo
// @todo
// @todo
// @todo
// @todo
// @todo

/* @attention
 * If you want to disable all the assertions in this macro, you should undef CONFIG_DEBUG.
 * You should do this in release version */

#undef  CONFIG_DEBUG
#define CONFIG_DEBUG

#include "hal_configall.h"
#include <string.h>
#include <stdio.h>
#include "hal_foundation.h"
#include "hal_cpu.h"
#include "hal_interrupt.h"
#include "hal_assert.h"
#include "hal_targetboard.h"
#include "hal_led.h"
#include "hal_uart.h"
#include "hal_debugio.h"

/* In "hal_cc2520base.h", we implement the most fundamental cc2420 operating functions.
 * If you want to port hal_cc2420 to other platforms, you can simply revise the 
 * hal_cc2420vx.h. The other part inside hal_cc2420.h can keep unchanged.
 */
#include "hal_cc2520vx.h"
#include "hal_cc2520base.h"
#include "hal_cc2520.h"

TiCc2520Adapter * cc2520_construct( void * mem, uint16 size )
{
	hal_assert( sizeof(TiCc2520Adapter) <= size );
	memset( mem, 0x00, size );
	return (TiCc2520Adapter *)mem;
}

void cc2520_destroy( TiCc2520Adapter * cc )
{
	cc2520_close( cc );
}

TiCc2520Adapter * cc2520_open( TiCc2520Adapter * cc, uint8 id, TiFunEventHandler listener, 
	void * lisowner, uint8 option )
{
    cc->id = 0;
    cc->state = 0; //CC2420_STATE_RECVING;
    cc->listener = listener;
    cc->lisowner = lisowner;
    cc->option = option;
	cc->rssi = 0;
	cc->lqi = 0;
	cc->spistatus = 0;
	cc->rxlen = 0;
	
	// todo


    return cc;
} 

void cc2520_close( TiCc2520Adapter * cc )
{
	//cc2520_disable_fifop( cc );
    //hal_detachhandler( INTNUM_CC2520_FIFOP );
}

uint8 cc2520_state( TiCc2520Adapter * cc )
{
	return cc->state;
}


/***********************************************************************************
  Filename:     hal_rf.c

  Description:  hal radio interface

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include "hal_mcu.h"
#include "hal_cpu.h"
#include "hal_digio.h"
#include "hal_cc2520.h"
#include "hal_cc2520base.h"
#include "hal_targetboard.h"
#include "hal_assert.h"
#include "hal_cc2520vx.h"

#define SUCCESS 0
#define FAILED 1
#define HAL_INT_ON() hal_enable_interrupts()
#define HAL_INT_OFF() hal_disable_interrupts()


/***********************************************************************************
* CONSTANTS AND DEFINES
*/
// Security
#define ADDR_RX                   0x200
#define ADDR_TX                   ADDR_RX + PKT_LEN_MAX
#define ADDR_NONCE_RX             0x310                 // Nonce for incoming packets
#define ADDR_NONCE_TX             0x320                 // Nonce for outgoing packets
#define ADDR_K                    0x340                 // Key address
#define TXBUF_START               0x100
#define RXBUF_START               0x180
#define AUTHSTAT_H_BM             0x08                  // AUTHSTAT_H bit of DPUSTAT
// register
#define TX_FRM_DONE_BM            0x02                  // TX_FRM_DONE bit of EXCFLAG0
// register

#define HIGH_PRIORITY             1
#define LOW_PRIORITY              0
#define PKT_LEN_MAX               127

// CC2520 RSSI Offset
#define RSSI_OFFSET               76

/***********************************************************************************
* LOCAL FUNCTIONS
*/

static void halRfSecurityInitRAM(uint8* key, uint8* nonceRx, uint8* nonceTx);
static void halRfWriteReg8(uint8 addr, uint8 value);
static HAL_RF_STATUS halRfWriteMem8(uint16 addr, uint8 value);
static HAL_RF_STATUS halRfWriteMem16(uint16 addr, uint16 value);
static HAL_RF_STATUS halRfStrobe(uint8 cmd);


/***********************************************************************************
* @fn      halRfSecurityInitRAM
*
* @brief   Write key and nonce to Radio RAM
*
* @param   none
*
* @return  uint8 - SUCCESS or FAILED
*/
static void halRfSecurityInitRAM(uint8* key, uint8* nonceRx, uint8* nonceTx)
{
    // Write key
    CC2520_MEMWR(ADDR_K,KEY_LENGTH,key);

    // Write nonce RX
    CC2520_MEMWR(ADDR_NONCE_RX,NONCE_LENGTH,nonceRx);

    // Write nonce TX
    CC2520_MEMWR(ADDR_NONCE_TX,NONCE_LENGTH,nonceTx);

    // Reverse key
    CC2520_MEMCPR(HIGH_PRIORITY,KEY_LENGTH,ADDR_K,ADDR_K);

    // Reverse nonces
    CC2520_MEMCPR(HIGH_PRIORITY,NONCE_LENGTH,ADDR_NONCE_RX,ADDR_NONCE_RX);
    CC2520_MEMCPR(HIGH_PRIORITY,NONCE_LENGTH,ADDR_NONCE_TX,ADDR_NONCE_TX);
}


/***********************************************************************************
* @fn      halRfWriteReg8
*
* @brief   Write byte to register
*
* @param   uint8 addr - address
*          uint8 value
*
* @return  none
*/
static void halRfWriteReg8(uint8 addr, uint8 value)
{
    CC2520_REGWR8(addr, value);
}


/***********************************************************************************
* @fn      halRfWriteMem8
*
* @brief   Write byte to memory
*
* @param   uint16 addr - address
*          uint8 value
*
* @return  HAL_RF_STATUS - chip status byte
*/
static HAL_RF_STATUS halRfWriteMem8(uint16 addr, uint8 value)
{
    return CC2520_MEMWR8(addr, value);
}


/***********************************************************************************
* @fn      halRfWriteMem16
*
* @brief   Write two bytes to memory
*
* @param   uint16 addr - address
*          uint16 value
*
* @return  HAL_RF_STATUS - chip status byte
*/
static HAL_RF_STATUS halRfWriteMem16(uint16 addr, uint16 value)
{
    return CC2520_MEMWR16(addr, value);
}


/***********************************************************************************
* @fn      halRfStrobe
*
* @brief   Send strobe command
*
* @param   uint8 cmd - command
*
* @return  HAL_RF_STATUS - chip status byte
*/
static HAL_RF_STATUS halRfStrobe(uint8 cmd)
{
    return(CC2520_INS_STROBE(cmd));
}


/***********************************************************************************
* GLOBAL FUNCTIONS
*/

/***********************************************************************************
* @fn      halRfPowerUp
*
* @brief   Power up
*
* @param   none
*
* @return  none
*/
void halRfPowerUp(void)
{
    // Power up CC2520
    CC2520_VREG_EN_OPIN(1);
    halMcuWaitUs(CC2520_VREG_MAX_STARTUP_TIME);
    CC2520_RESET_OPIN(1);
    halMcuWaitUs(CC2520_XOSC_MAX_STARTUP_TIME);
}


/***********************************************************************************
* @fn      halRfResetChip
*
* @brief   Reset chip
*
* @param   none
*
* @return  none
*/
void halRfResetChip(void)
{
    // Controlled reset of CC2520
    // Toggle RESETn
    CC2520_VREG_EN_OPIN(0);
    CC2520_RESET_OPIN(0);
    CC2520_VREG_EN_OPIN(1);

    halMcuWaitUs(CC2520_VREG_MAX_STARTUP_TIME);  // us
    CC2520_RESET_OPIN(1);
    halRfWaitXoscStable();
}


/***********************************************************************************
* @fn      halRfInit
*
* @brief   Power up, sets default tuning settings, enables autoack and configures
*          chip IO
*
* @param   none
*
* @return  SUCCESS if the radio has started, FAILURE otherwise
*/
HAL_RF_STATUS halRfInit(void)
{
	// todo
    // Avoid GPIO0 interrupts during reset
    // halDigioIntDisable(&pinRadio_GPIO0);

    // Make sure to pull the CC2520 RESETn and VREG_EN pins low
    CC2520_RESET_OPIN(0);
    CC2520_SPI_END();
    CC2520_VREG_EN_OPIN(0);
    halMcuWaitUs(1100);

    // Enable the voltage regulator and wait for it (CC2520 power-up)
    CC2520_VREG_EN_OPIN(1);
    halMcuWaitUs(CC2520_VREG_MAX_STARTUP_TIME);

    // Release reset
    CC2520_RESET_OPIN(1);

    // Wait for XOSC stable to be announced on the MISO pin
    if (halRfWaitXoscStable()==FAILED)
        return FAILED;

    // Write tuning settings
    halRfWriteReg8(CC2520_TXPOWER, 0xF7);  // Max TX output power
    halRfWriteReg8(CC2520_CCACTRL0, 0xF8); // CCA treshold -80dBm

    // Recommended RX settings
    halRfWriteMem8(CC2520_MDMCTRL0, 0x85);
    halRfWriteMem8(CC2520_MDMCTRL1, 0x14);
    halRfWriteMem8(CC2520_RXCTRL, 0x3F);
    halRfWriteMem8(CC2520_FSCTRL, 0x5A);
    halRfWriteMem8(CC2520_FSCAL1, 0x2B);
    halRfWriteMem8(CC2520_AGCCTRL1, 0x11);
    halRfWriteMem8(CC2520_ADCTEST0, 0x10);
    halRfWriteMem8(CC2520_ADCTEST1, 0x0E);
    halRfWriteMem8(CC2520_ADCTEST2, 0x03);

    // CC2520 setup
    CC2520_BSET(CC2520_MAKE_BIT_ADDR(CC2520_FRMCTRL0, 5));  // Enable AUTOACK

    // CC2520 GPIO setup
    CC2520_CFG_GPIO_OUT(0, 1 + CC2520_EXC_RX_FRM_DONE);
    CC2520_CFG_GPIO_OUT(1,     CC2520_GPIO_SAMPLED_CCA);
    CC2520_CFG_GPIO_OUT(2,     CC2520_GPIO_RSSI_VALID);
    CC2520_CFG_GPIO_OUT(3,     CC2520_GPIO_SFD);
    CC2520_CFG_GPIO_OUT(4,     CC2520_GPIO_SNIFFER_DATA);
    CC2520_CFG_GPIO_OUT(5,     CC2520_GPIO_SNIFFER_CLK);

    return SUCCESS;
}


/***********************************************************************************
* @fn      halRfWaitXoscStable
*
* @brief   Wait for the crystal oscillator to stabilise.
*
* @param   none
*
* @return  SUCCESS if oscillator starts, FAILED otherwise
*/
HAL_RF_STATUS halRfWaitXoscStable(void)
{
    uint8 i;

    // Wait for XOSC stable to be announced on the MISO pin
    i= 100;
    CC2520_CSN_OPIN(0);
    while (i>0 && !CC2520_MISO_IPIN) {
        halMcuWaitUs(10);
        --i;
    }
    CC2520_CSN_OPIN(1);

    return i>0 ? SUCCESS : FAILED;
}


/***********************************************************************************
* @fn      halRfGetChipId
*
* @brief   Get chip id
*
* @param   none
*
* @return  uint8 - result
*/
uint8 halRfGetChipId(void)
{
    return(CC2520_MEMRD8(CC2520_CHIPID));
}


/***********************************************************************************
* @fn      halRfGetChipVer
*
* @brief   Get chip version
*
* @param   none
*
* @return  uint8 - result
*/
uint8 halRfGetChipVer(void)
{
    return(CC2520_MEMRD8(CC2520_VERSION));
}


/***********************************************************************************
* @fn      halRfGetStatusByte
*
* @brief   Get chip status byte
*
* @param   none
*
* @return  HAL_RF_STATUS - chip status byte
*/
HAL_RF_STATUS halRfGetStatusByte(void)
{
    return halRfStrobe(CC2520_INS_SNOP);
}


/***********************************************************************************
* @fn      halRfGetRandomByte
*
* @brief   Return random byte
*
* @param   none
*
* @return  uint8 - random byte
*/
uint8 halRfGetRandomByte(void)
{
    return CC2520_RANDOM8();
}


/***********************************************************************************
* @fn      halRfGetRssiOffset
*
* @brief   Return RSSI Offset
*
* @param   none
*
* @return  uint8 - RSSI offset
*/
uint8 halRfGetRssiOffset(void)
{
    return RSSI_OFFSET;
}


/***********************************************************************************
* @fn      halRfSetChannel
*
* @brief   Set RF channel. Channel must be in the range 11-26
*
* @param   none
*
* @return  none
*/
void halRfSetChannel(uint8 channel) {
    halRfWriteReg8(CC2520_FREQCTRL, 0x0B + ((channel - 11) * 5));
}


/***********************************************************************************
* @fn      halRfSetShortAddr
*
* @brief   Write short address to chip
*
* @param   none
*
* @return  HAL_RF_STATUS - chip status byte
*/
HAL_RF_STATUS halRfSetShortAddr(uint16 shortAddr) {
    return halRfWriteMem16(CC2520_RAM_SHORTADDR, shortAddr);
}


/***********************************************************************************
* @fn      halRfSetPanId
*
* @brief   Write PAN Id to chip
*
* @param   none
*
* @return  HAL_RF_STATUS - chip status byte
*/
HAL_RF_STATUS halRfSetPanId(uint16 panId) {
    return halRfWriteMem16(CC2520_RAM_PANID, panId);
}


/***********************************************************************************
* @fn      halRfSetPower
*
* @brief   Set TX output power
*
* @param   uint8 power - power level: TXPOWER_MIN_4_DBM, TXPOWER_0_DBM or
*                        TXPOWER_4_DBM
*
* @return  uint8 - SUCCESS or FAILED
*/
uint8 halRfSetPower(uint8 power)
{
    uint8 n;

    switch(power)
    {
    case TXPOWER_MIN_4_DBM: n = CC2520_TXPOWER_MIN_4_DBM; break;
    case TXPOWER_0_DBM: n = CC2520_TXPOWER_0_DBM; break;
    case TXPOWER_4_DBM: n = CC2520_TXPOWER_4_DBM; break;

    default:
        return FAILED;
    }
    CC2520_REGWR8(CC2520_TXPOWER, n);
    return SUCCESS;
}


/***********************************************************************************
* @fn      halRfRecvFrame
*
* @brief   Read RX buffer
*
* @param   uint8* data - data buffer. This must be allocated by caller.
*          uint8 length - number of bytes
*
* @return  HAL_RF_STATUS - chip status byte
*/
HAL_RF_STATUS halRfRecvFrame(uint8* data, uint8 length)
{
    return CC2520_RXBUF(length, data);
}


/***********************************************************************************
* @fn      halRfWriteTxBuf
*
* @brief   Write to TX buffer
*
* @param   uint8* data - buffer to write
*          uint8 length - number of bytes
*
* @return  HAL_RF_STATUS - chip status byte
*/
HAL_RF_STATUS halRfWriteTxBuf(uint8* data, uint8 length)
{
    // Copy packet to TX FIFO
    return CC2520_TXBUF(length,data);
}


/***********************************************************************************
* @fn      halRfReadRxBuf
*
* @brief   Read RX buffer
*
* @param   uint8* data - data buffer. This must be allocated by caller.
*          uint8 length - number of bytes
*
* @return  HAL_RF_STATUS - chip status byte
*/
HAL_RF_STATUS halRfReadRxBuf(uint8* data, uint8 length)
{
    return CC2520_RXBUF(length, data);
}


/***********************************************************************************
* @fn      halRfTransmitCCA
*
* @brief   Transmit frame with Clear Channel Assessment.
*
* @param   none
*
* @return  uint8 - SUCCESS or FAILED
*/
uint8 halRfTransmitCCA(void)
{
    uint16 timeout = 2500; // 2500 x 20us = 50ms
    uint8 status=0;

    // Wait for RSSI to become valid
    while(!CC2520_RSSI_VALID_PIN);

    // Reuse GPIO2 for TX_FRM_DONE exception
    //HAL_INT_OFF();
	hal_disable_interrupts();
    CC2520_CFG_GPIO_OUT(2, 1 + CC2520_EXC_TX_FRM_DONE);
	//HAL_INT_ON();
	hal_enable_interrupts();

    // Wait for the transmission to begin before exiting (makes sure that this function cannot be called
    // a second time, and thereby cancelling the first transmission.
    while(--timeout > 0) {
        HAL_INT_OFF();
        halRfStrobe(CC2520_INS_STXONCCA);
        HAL_INT_ON();
        if (CC2520_SAMPLED_CCA_PIN) break;
        halMcuWaitUs(20);
    }
    if (timeout == 0) {
        status = FAILED;
        halRfStrobe(CC2520_INS_SFLUSHTX);
    }
    else {
        status = SUCCESS;
        // Wait for TX_FRM_DONE exception
        while(!CC2520_TX_FRM_DONE_PIN);
        HAL_INT_OFF();
        CC2520_CLEAR_EXC(CC2520_EXC_TX_FRM_DONE);
        HAL_INT_ON();
    }

    // Reconfigure GPIO2
    HAL_INT_OFF();
    CC2520_CFG_GPIO_OUT(2,     CC2520_GPIO_RSSI_VALID);
    HAL_INT_ON();
    return status;
}


/***********************************************************************************
* @fn      halRfSecurityInit
*
* @brief   Security init. Write nonces and key to chip.
*
* @param   none
*
* @return  none
*/
void halRfSecurityInit(uint8* key, uint8* nonceRx, uint8* nonceTx)
{
    halRfSecurityInitRAM(key, nonceRx, nonceTx);
}


/***********************************************************************************
* @fn      halRfRecvFrameUCCM
*
* @brief   Decrypts and reverse authenticates with CCM then reads out received
*          frame
*
* @param   uint8* data - data buffer. This must be allocated by caller.
*          uint8 length - number of bytes
*          uint8 encrLength - number of bytes to decrypt
*          uint8 authLength - number of bytes to reverse authenticate
*          uuint8 m - ets length of integrity code (m=1,2,3 gives lenght of integrity
*                   field 4,8,16)
*
* @return  uint8 - SUCCESS or FAILED
*/
HAL_RF_STATUS halRfRecvFrameUCCM(uint8* data, uint8 length, uint8 encrLength, uint8 authLength, uint8 m)
{
    uint8 dpuStat;

    CC2520_RXBUFMOV(HIGH_PRIORITY, ADDR_RX, length, NULL);
    WAIT_DPU_DONE_H();

    // Find Framecounter value in received packet starting from 10th byte and update nonce
    // Copy in to nonce bytes (3-6) frame counter bytes
    // Incoming frame uses nonce Rx
    CC2520_MEMCP(HIGH_PRIORITY, 4, ADDR_RX+10, ADDR_NONCE_RX+3);
    WAIT_DPU_DONE_H();

    // Copy in short address to nonce bytes (7-8)
    CC2520_MEMCP(HIGH_PRIORITY, 2, ADDR_RX+7, ADDR_NONCE_RX+7);
    WAIT_DPU_DONE_H();

    CC2520_UCCM(HIGH_PRIORITY,ADDR_K/16, encrLength, ADDR_NONCE_RX/16, ADDR_RX, ADDR_RX+authLength, authLength, m);
    WAIT_DPU_DONE_H();

    // Check authentication status
    dpuStat = CC2520_REGRD8(CC2520_DPUSTAT);

    // Read from RX work buffer into data buffer
    CC2520_MEMRD(ADDR_RX, length, data);

    if( (dpuStat & AUTHSTAT_H_BM) != AUTHSTAT_H_BM ) {
        // Authentication failed
        return FAILED;
    }
    else return SUCCESS;
}


/***********************************************************************************
* @fn      halRfWriteTxBufCCM
*
* @brief   Encrypt and authenticate plaintext then fill TX buffer
*
* @param   uint8* data - data buffer. This must be allocated by caller.
*          uint8 length - number of bytes
*          uint8 encrLength - number of bytes to decrypt
*          uint8 authLength - number of bytes to reverse authenticate
*          uuint8 m - length of integrity code (m=1,2,3 gives lenght of integrity
*                   field 4,8,16)
*
* @return  HAL_RF_STATUS - chip status byte
*/
HAL_RF_STATUS halRfWriteTxBufCCM(uint8* data, uint8 length, uint8 encrLength, uint8 authLength, uint8 m)
{
    uint8 micLength;

    // Check range of m
    hal_assert(m<=4);

    if(m>0) {
        micLength = 0x2<<m;
    }
    else if(m==0) {
        micLength=0;
    }

    // Write packet to work buffer
    CC2520_MEMWR(ADDR_TX, length, data);

    // skip the length byte and start from the next byte in TXBUF
    // Outgoing frame uses nonce_tx
    CC2520_CCM(HIGH_PRIORITY,ADDR_K/16, encrLength, ADDR_NONCE_TX/16, ADDR_TX+1, 0, authLength, m);
    WAIT_DPU_DONE_H();

    // copy from work buffer to TX FIFO
    CC2520_TXBUFCP(HIGH_PRIORITY, ADDR_TX, length+micLength, NULL);
    WAIT_DPU_DONE_H();

    return halRfGetStatusByte();
}


/***********************************************************************************
* @fn      halRfIncNonceTx
*
* @brief   Increments frame counter field of stored nonce TX
*
* @param   none
*
* @return  none
*/
void halRfIncNonceTx(void)
{
    // Increment frame counter field of 16 byte nonce TX
    // Frame counter field is 4 bytes long

    // Increment framecounter bytes (3-6) of nonce TX
    CC2520_INC(HIGH_PRIORITY, 2, ADDR_NONCE_TX+3);
    WAIT_DPU_DONE_H();
}


/***********************************************************************************
* @fn      halRfReceiveOn
*
* @brief   Turn receiver on
*
* @param   none
*
* @return  HAL_RF_STATUS - chip status byte
*/
HAL_RF_STATUS halRfReceiveOn(void)
{
    return halRfStrobe(CC2520_INS_SRXON);
}


/***********************************************************************************
* @fn      halRfReceiveOff
*
* @brief   Turn receiver off
*
* @param   none
*
* @return  HAL_RF_STATUS - chip status byte
*/
HAL_RF_STATUS halRfReceiveOff(void)
{
    return halRfStrobe(CC2520_INS_SRFOFF);
}



