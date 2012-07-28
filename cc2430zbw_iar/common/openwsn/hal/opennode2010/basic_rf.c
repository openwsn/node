
/***********************************************************************************
  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, 
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com. 
***********************************************************************************/

/***********************************************************************************
  Filename:     basic_rf.c

  Description:  Basic RF library

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include "../hal_configall.h"
#include "../hal_foundation.h"
#include "hal_cc2520base.h"
#include "../hal_cpu.h"
#include "../hal_mcu.h"
#include "../hal_targetboard.h"
#include "../hal_digitio.h"
#include "basic_rf.h"
#include "basic_rf_security.h"
#include "util.h"
#include <string.h>

#define halIntOff() hal_disable_interrupts()
#define halIntOn() hal_enable_interrupts()

#define FAILED 1
//digioConfig pinRadio_GPIO0;

/***********************************************************************************
* CONSTANTS AND DEFINES
*/

// Packet and packet part lengths
#define PKT_LEN_MIC                         8
#define PKT_LEN_SEC                         PKT_LEN_UNSEC + PKT_LEN_MIC
#define PKT_LEN_AUTH                        8
#define PKT_LEN_ENCR                        24

// Packet overhead ((frame control field, sequence number, PAN ID,
// destination and source) + (footer))
// Note that the length byte itself is not included included in the packet length
#define BASIC_RF_PACKET_OVERHEAD_SIZE       ((2 + 1 + 2 + 2 + 2) + (2))
#define BASIC_RF_MAX_PAYLOAD_SIZE	        (127 - BASIC_RF_PACKET_OVERHEAD_SIZE - BASIC_RF_AUX_HDR_LENGTH - BASIC_RF_LEN_MIC)
#define BASIC_RF_ACK_PACKET_SIZE	        5
#define BASIC_RF_FOOTER_SIZE                2

// The time it takes for the acknowledgment packet to be received after the data packet has been
// transmitted
#define BASIC_RF_ACK_DURATION		        (0.5 * 32 * 2 * ((4 + 1) + (1) + (2 + 1) + (2)))
#define BASIC_RF_SYMBOL_DURATION	        (32 * 0.5)

// The length byte
#define BASIC_RF_PLD_LEN_MASK               0x7F

// Frame control field
#define BASIC_RF_FCF_NOACK                  0x8841
#define BASIC_RF_FCF_ACK                    0x8861
#define BASIC_RF_FCF_ACK_BM                 0x0020
#define BASIC_RF_FCF_BM                     (~BASIC_RF_FCF_ACK_BM)
#define BASIC_RF_ACK_FCF		            0x0002
#define BASIC_RF_SEC_ENABLED_FCF_BM         0x0008

// Auxiliary Security header
#define BASIC_RF_AUX_HDR_LENGTH             5
#define BASIC_RF_LEN_AUTH                   BASIC_RF_PACKET_OVERHEAD_SIZE + BASIC_RF_AUX_HDR_LENGTH - BASIC_RF_FOOTER_SIZE
#define BASIC_RF_SECURITY_M                 2
#define BASIC_RF_LEN_MIC                    8

// Footer
#define BASIC_RF_CRC_OK_BM                  0x80

/***********************************************************************************
* TYPEDEFS
*/
// The receive struct
typedef struct {
    uint8 seqNumber;
    uint16 srcAddr;
    uint16 srcPanId;
    int8 length;
    uint8* auxSecHdr;
    uint8 *pPayload;
    uint8 ackRequest;
    int8 rssi;
    volatile uint8 isReady;
    uint8 status;
} basicRfRxInfo_t;

// Tx state
typedef struct {
    uint8 txSeqNumber;
    volatile uint8 ackReceived;
    uint8 receiveOn;
    uint32 frameCounter;
} basicRfTxState_t;


// Basic RF packet header (IEEE 802.15.4)
#pragma pack(1)
typedef struct {
    uint8   packetLength;
    uint16  frameControlField;
    uint8   seqNumber;
    uint16  panId;
    uint16  destAddr;
    uint16  srcAddr;
    #ifdef SECURITY_CCM
    uint8   securityControl;
    uint32  frameCounter;
    #endif
} basicRfPktHdr_t;
#pragma pack()


/***********************************************************************************
* LOCAL VARIABLES
*/
static basicRfRxInfo_t  rxi=      { 0xFF }; // Make sure sequence numbers are
static basicRfTxState_t txState=  { 0x00 }; // initialised and distinct.

static basicRfCfg_t* pConfig;
static uint8 txMpdu[BASIC_RF_MAX_PAYLOAD_SIZE+BASIC_RF_PACKET_OVERHEAD_SIZE];
static uint8 rxMpdu[BASIC_RF_MAX_PAYLOAD_SIZE+BASIC_RF_PACKET_OVERHEAD_SIZE];
static uint8 auxSecurityHeader[BASIC_RF_AUX_HDR_LENGTH];

/***********************************************************************************
* GLOBAL VARIABLES
*/

/***********************************************************************************
* LOCAL FUNCTIONS
*/
static uint8 basicRfBuildHeader(uint8* buffer, uint16 destAddr, uint8 payloadLength);
static uint8 basicRfBuildMpdu(uint16 destAddr, uint8* pPayload, uint8 length);
static void basicRfRxFrmDoneIsr(void);

/***********************************************************************************
* @fn          basicRfBuildHeader
*
* @brief       Builds packet header according to IEEE 802.15.4 frame format
*
* @param       buffer - Pointer to buffer to write the header
*              destAddr - destination short address
*              payloadLength - length of higher layer payload
*
* @return      uint8 - length of header
*/
static uint8 basicRfBuildHeader(uint8* buffer, uint16 destAddr, uint8 payloadLength)
{
    basicRfPktHdr_t *pHdr;

    pHdr= (basicRfPktHdr_t*)buffer;

    // Populate packet header
    pHdr->packetLength = payloadLength + BASIC_RF_PACKET_OVERHEAD_SIZE;
    pHdr->frameControlField = pConfig->ackRequest ? BASIC_RF_FCF_ACK : BASIC_RF_FCF_NOACK;
    pHdr->seqNumber= txState.txSeqNumber;
    pHdr->panId= pConfig->panId;
    pHdr->destAddr= destAddr;
    pHdr->srcAddr= pConfig->myAddr;

    #ifdef SECURITY_CCM

    // Add security to FCF, length and security header
    pHdr->frameControlField |= BASIC_RF_SEC_ENABLED_FCF_BM;
    pHdr->packetLength += PKT_LEN_MIC;
    pHdr->packetLength += BASIC_RF_AUX_HDR_LENGTH;

    pHdr->securityControl= SECURITY_CONTROL;
    pHdr->frameCounter=    txState.frameCounter;

    #endif

    return sizeof(basicRfPktHdr_t);
}


/***********************************************************************************
* @fn          basicRfBuildMpdu
*
* @brief       Builds mpdu (MAC header + payload) according to IEEE 802.15.4
*              frame format
*
* @param       destAddr - Destination short address
*              pPayload - pointer to buffer with payload
*              length - length of payload buffer
*
* @return      uint8 - length of mpdu
*/
static uint8 basicRfBuildMpdu(uint16 destAddr, uint8* pPayload, uint8 length)
{
    uint8 hdrLength = 0;
    int n=0;

    hdrLength = basicRfBuildHeader(txMpdu, destAddr, length);

    for(n=0;n<length;n++)
    {
        txMpdu[hdrLength+n] = *(pPayload+n);
    }
    return hdrLength+n; // total mpdu length
}


/***********************************************************************************
* @fn          basicRfRxFrmDoneIsr
*
* @brief       Interrupt service routine for received frame from radio
*              (either data or acknowlegdement)
*
* @param       rxi - file scope variable info extracted from the last incoming
*                    frame
*              txState - file scope variable that keeps tx state info
*
* @return      none
*/
static void basicRfRxFrmDoneIsr(void)
{
    uint16 frameControlField;
    uint8 length;
    uint8 pStatusWord[2];
    uint8 index = 0;
    uint8 authStatus=0;
    uint8 prevSeqNumber; // Sequence number of previous packet
/* @todo zhangwei had to comment the following line. you should enable it
    // Clear interrupt and disable new RX frame done interrupt
    halDigioIntClear(&pinRadio_GPIO0);
    halDigioIntDisable(&pinRadio_GPIO0);
*/

    // Enable all other interrupt sources (enables interrupt nesting)
    halIntOn();

    // Clear the exception and the IRQ
    CLEAR_EXC_RX_FRM_DONE();

    // Read payload length.
    halRfReadRxBuf((uint8*)&length,1);

    length &= BASIC_RF_PLD_LEN_MASK; // Ignore MSB

    // Is this an acknowledgment packet?
    // Only ack packets may be 5 bytes in total, so this check is sufficient with CC2520
    // to determine if a frame is an ack frame.
    if (length == BASIC_RF_ACK_PACKET_SIZE) {
        uint8 ackSeq;

        // Read the frame control field and the data sequence number
        halRfReadRxBuf((uint8*)&frameControlField,2);

        rxi.ackRequest = !!(frameControlField & BASIC_RF_FCF_ACK_BM);
        halRfReadRxBuf(&ackSeq,1);

        // Read the status word and check for CRC OK
        halRfReadRxBuf(pStatusWord,2);

        // Indicate the successful ACK reception if CRC and sequence number OK
        if ((pStatusWord[1] & BASIC_RF_CRC_OK_BM) && (ackSeq == txState.txSeqNumber)) {
            txState.ackReceived = TRUE;
        }

        // No, it is data
    } else {
        // It is assumed that the CC2520 rejects packets with invalid length.
        // Subtract the number of bytes in the frame overhead to get actual payload.

        rxi.length = length - BASIC_RF_PACKET_OVERHEAD_SIZE;

        #ifdef SECURITY_CCM
        rxi.length -= BASIC_RF_AUX_HDR_LENGTH;
        rxi.length -= BASIC_RF_LEN_MIC;
        authStatus = halRfRecvFrameUCCM(rxMpdu, length, rxi.length, BASIC_RF_LEN_AUTH, BASIC_RF_SECURITY_M);
        #else
        halRfRecvFrame(rxMpdu, length);
        #endif

        // Read the frame control field and the data sequence number
        memcpy(&frameControlField, rxMpdu+index, sizeof(frameControlField));
        index += sizeof(frameControlField);

        rxi.ackRequest = !!(frameControlField & BASIC_RF_FCF_ACK_BM);
        prevSeqNumber = rxi.seqNumber;
        rxi.seqNumber = *(rxMpdu+index);
        index += sizeof(rxi.seqNumber);

        // Skip the destination PAN and address (that's taken care of by hardware address recognition!)
        index += 2; // PAN ID
        index += 2; // Dest. Addr

        // Read the source address
        memcpy(&rxi.srcAddr, rxMpdu+index, sizeof(rxi.srcAddr));
        index += sizeof(rxi.srcAddr);

        #ifdef SECURITY_CCM
        index += BASIC_RF_AUX_HDR_LENGTH; // skip auxiliary security header
        #endif

        // Read the packet payload
        rxi.pPayload = rxMpdu+index;

        index += rxi.length;

        #ifdef SECURITY_CCM
        index += BASIC_RF_LEN_MIC; // skip MIC
        #endif

        // Read the FCS to get the RSSI and CRC
        memcpy(pStatusWord, rxMpdu+index, sizeof(pStatusWord));
        rxi.rssi = pStatusWord[0];

        // Notify the application about the received data packet if the CRC is OK
        // Throw packet if the previous packet had the same sequence number
        if( (pStatusWord[1] & BASIC_RF_CRC_OK_BM) && (prevSeqNumber != rxi.seqNumber) ) {
            // If security is used check also that authentication passed
            #ifdef SECURITY_CCM
            if( authStatus==SUCCESS ) {
                if ( (frameControlField & (BASIC_RF_FCF_BM)) == (BASIC_RF_FCF_NOACK | BASIC_RF_SEC_ENABLED_FCF_BM)) {
                    rxi.isReady = TRUE;
                }
            }
            #else
            if ( ((frameControlField & (BASIC_RF_FCF_BM)) == BASIC_RF_FCF_NOACK) ) {
                rxi.isReady = TRUE;
            }
            #endif
        }

    }

    // Enable RX frame done interrupt again
    halIntOff();
    halDigioIntEnable(&pinRadio_GPIO0);
}


/***********************************************************************************
* GLOBAL FUNCTIONS
*/

/***********************************************************************************
* @fn          basicRfInit
*
* @brief       Initialise basic RF datastructures. Sets channel, short address and
*              PAN id in the chip and configures interrupt on packet reception
*
* @param       pRfConfig - pointer to BASIC_RF_CONFIG struct.
*                          This struct must be allocated by higher layer
*              txState - file scope variable that keeps tx state info
*              rxi - file scope variable info extracted from the last incoming
*                    frame
*
* @return      none
*/
basicRFStatus_t basicRfInit(basicRfCfg_t* pRfConfig)
{
    if (halRfInit()==FAILED)
        return FAILED;

    halIntOff();

    // Set the protocol configuration
    pConfig = pRfConfig;
    rxi.pPayload   = NULL;
    rxi.auxSecHdr = auxSecurityHeader;

    txState.receiveOn = TRUE;
    txState.frameCounter = 0;

    // Set channel
    halRfSetChannel(pConfig->channel);

    // Write the short address and the PAN ID to the CC2520 RAM
    halRfSetShortAddr(pConfig->myAddr);
    halRfSetPanId(pConfig->panId);

    // if security is enabled, write key and nonce
    #ifdef SECURITY_CCM
    basicRfSecurityInit(pConfig);
    #endif

    // Set up interrupt on RX_FRM_DONE @ GPIO0 (received data or acknowlegment)
    halDigioIntSetEdge(&pinRadio_GPIO0, HAL_DIGIO_INT_RISING_EDGE);
    halDigioIntConnect(&pinRadio_GPIO0, &basicRfRxFrmDoneIsr);
    halDigioIntEnable(&pinRadio_GPIO0);

    // And clear the exception
    CLEAR_EXC_RX_FRM_DONE();

    halIntOn();

    return SUCCESS;
}


/***********************************************************************************
* @fn          basicRfSendPacket
*
* @brief       Send packet
*
* @param       destAddr - destination short address
*              pPayload - pointer to payload buffer. This buffer must be
*                         allocated by higher layer.
*              length - length of payload
*              txState - file scope variable that keeps tx state info
*              mpdu - file scope variable. Buffer for the frame to send
*
* @return      basicRFStatus_t - SUCCESS or FAILED
*/
basicRFStatus_t basicRfSendPacket(uint16 destAddr, uint8* pPayload, uint8 length)
{
    uint8 mpduLength;
    uint8 status;

    // Turn on receiver if its not on
    if(!txState.receiveOn) {
        halRfReceiveOn();
    }

    // Check packet length
    length = min(length, BASIC_RF_MAX_PAYLOAD_SIZE);

    // Wait until the transceiver is idle
    WAIT_TRANSCEIVER_READY();

    // Turn off RX frame done interrupt to avoid interference on the SPI interface
    halDigioIntDisable(&pinRadio_GPIO0);

    mpduLength = basicRfBuildMpdu(destAddr, pPayload, length);

    #ifdef SECURITY_CCM
    halRfWriteTxBufCCM(txMpdu, mpduLength, length, BASIC_RF_LEN_AUTH, BASIC_RF_SECURITY_M);
    txState.frameCounter++; // Increment frame counter field
    halRfIncNonceTx(); // Increment nonce value
    #else
    halRfWriteTxBuf((uint8*)txMpdu, mpduLength);
    #endif

    // Turn on RX frame done interrupt for ACK reception
    halDigioIntEnable(&pinRadio_GPIO0);

    // Send frame with CCA. return FAILED if not successful
    if(halRfTransmitCCA() != SUCCESS) {
        status = FAILED;
    }

    // Wait for the acknowledge to be received, if any
    if (pConfig->ackRequest) {
        txState.ackReceived = FALSE;

        // We'll enter RX automatically, so just wait until we can be sure that the ack reception should have finished
        // The timeout consists of a 12-symbol turnaround time, the ack packet duration, and a small margin
        hal_delayus((12 * BASIC_RF_SYMBOL_DURATION) + (BASIC_RF_ACK_DURATION) + (2 * BASIC_RF_SYMBOL_DURATION) + 10);

        // If an acknowledgment has been received (by RxFrmDoneIsr), the ackReceived flag should be set
        status = txState.ackReceived ? SUCCESS : FAILED;

    } else {
        status = SUCCESS;
    }

    // Turn off the receiver if it should not continue to be enabled
    if (!txState.receiveOn) {
        halRfReceiveOff();
    }

    if(status == SUCCESS) {
        txState.txSeqNumber++;
    }
    return status;

}


/***********************************************************************************
* @fn          basicRfPacketIsReady
*
* @brief       Check if a new packet is ready to be read by next higher layer
*
* @param       none
*
* @return      uint8 - TRUE if a packet is ready to be read by higher layer
*/
uint8 basicRfPacketIsReady(void)
{
    return rxi.isReady;
}


/***********************************************************************************
* @fn          basicRfReceive
*
* @brief       Copies the payload of the last incoming packet into a buffer
*
* @param       pRxData - pointer to data buffer to fill. This buffer must be
*                        allocated by higher layer.
*              len - Number of bytes to read in to buffer
*              rxi - file scope variable holding the information of the last
*                    incoming packet
*
* @return      uint8 - number of bytes actually copied into buffer
*/
uint8 basicRfReceive(uint8* pRxData, uint8 len, int16* pRssi)
{
    // Accessing shared variables -> this is a critical region
    // Critical region start
    halIntOff();
    memcpy(pRxData, rxi.pPayload, min(rxi.length, len));
    if(pRssi != NULL) {
        if(rxi.rssi < 128){
            *pRssi = rxi.rssi - halRfGetRssiOffset();
        }
        else{
            *pRssi = (rxi.rssi - 256) - halRfGetRssiOffset();
        }
    }
    rxi.isReady = FALSE;
    halIntOn();

    // Critical region end

    return min(rxi.length, len);
}


/***********************************************************************************
* @fn          basicRfReceiveOn
*
* @brief       Turns on receiver on radio
*
* @param       txState - file scope variable
*
* @return      none
*/
void basicRfReceiveOn(void)
{
    txState.receiveOn = TRUE;
    halRfReceiveOn();
}


/***********************************************************************************
* @fn          basicRfReceiveOff
*
* @brief       Turns off receiver on radio
*
* @param       txState - file scope variable
*
* @return      none
*/
void basicRfReceiveOff(void)
{
    txState.receiveOn = FALSE;
    halRfReceiveOff();
}
