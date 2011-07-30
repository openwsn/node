/**************************************************************************************************
  Filename:       mac_spi.c
  Revised:        $Date: 2007-09-11 18:59:21 -0700 (Tue, 11 Sep 2007) $
  Revision:       $Revision: 15378 $

  Description:    Describe the purpose and contents of the file.


  Copyright 2006-2007 Texas Instruments Incorporated. All rights reserved.

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
**************************************************************************************************/

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */

/* hal */
#include "hal_types.h"
#include "hal_mac_cfg.h"

/* low-level specific */
#include "mac_spi.h"

/* debug */
#include "mac_assert.h"


/* ------------------------------------------------------------------------------------------------
 *                                          Test
 * ------------------------------------------------------------------------------------------------
 */

void txtest()
{
	
}

void rxtest()
{
	
}

/* ------------------------------------------------------------------------------------------------
 *                                          Constants
 * ------------------------------------------------------------------------------------------------
 */
#define REG_READ_NOT_WRITE_BIT      0x40  /* binary: 0100 0000 */

#define RAM_ADDR_FIRST_BYTE_MASK    0x7F  /* binary: 0111 1111 */
#define RAM_ADDR_SECOND_BYTE_MASK   0xC0  /* binary: 1100 0000 */
#define RAM_NOT_REGISTER_BIT        0x80  /* binary: 1000 0000 */
#define RAM_READ_NOT_WRITE_BIT      0x20  /* binary: 0010 0000 */


#define FIFO_ACCESS_TAG_BIT         0x8000  /* binary: 1000 0000  0000 0000 */
#define FIFO_WRITE                  0
#define FIFO_READ                   REG_READ_NOT_WRITE_BIT
#define RAM_WRITE                   0
#define RAM_READ                    RAM_READ_NOT_WRITE_BIT


/* ------------------------------------------------------------------------------------------------
 *                                         Global Variables
 * ------------------------------------------------------------------------------------------------
 */
MAC_ASSERT_DECLARATION( uint8 macSpiRadioPower; )


/* ------------------------------------------------------------------------------------------------
 *                                       Local Prototypes
 * ------------------------------------------------------------------------------------------------
 */
static uint16 macSpiRegAccess(uint8 regAddrByte, uint16 regWriteValue);
static void macSpiRamFifoAccess(uint16 addr, uint8 * pData, uint8 len, uint8 readBit);


/**************************************************************************************************
 * @fn          macSpiInit
 *
 * @brief       Initialize SPI.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
void macSpiInit(void)
{
  HAL_MAC_SPI_SET_CHIP_SELECT_OFF();
  HAL_MAC_SPI_INIT();
}


/**************************************************************************************************
 * @fn          macSpiCmdStrobe
 *
 * @brief       Send command strobe to the radio.  Returns status byte read during transfer
 *              of strobe command.
 *
 * @param       regAddr - address of register to strobe
 *
 * @return      status byte of radio
 **************************************************************************************************
 */
uint8 macSpiCmdStrobe(uint8 regAddr)
{
  uint8 statusByte;
  halMacSpiIntState_t s;

  MAC_ASSERT(macSpiRadioPower & MAC_SPI_RADIO_POWER_VREG_ON);  /* radio must be powered */

  /*-------------------------------------------------------------------------------
   *  Disable interrupts that use SPI.
   */
  HAL_MAC_SPI_ENTER_CRITICAL_SECTION(s);

  /*-------------------------------------------------------------------------------
   *  Turn chip select "off" and then "on" to clear any current SPI access.
   */
  HAL_MAC_SPI_SET_CHIP_SELECT_OFF();
  HAL_MAC_SPI_SET_CHIP_SELECT_ON();

  /*-------------------------------------------------------------------------------
   *  Send the command strobe.  Wait for SPI access to complete.
   */
  HAL_MAC_SPI_WRITE_BYTE(regAddr);
  HAL_MAC_SPI_WAIT_DONE();

  /*-------------------------------------------------------------------------------
   *  Read the readio status byte returned by the command strobe.
   */
  statusByte = HAL_MAC_SPI_READ_BYTE();

  /*-------------------------------------------------------------------------------
   *  Turn off chip select.  Enable interrupts that call SPI functions.
   */
  HAL_MAC_SPI_SET_CHIP_SELECT_OFF();
  HAL_MAC_SPI_EXIT_CRITICAL_SECTION(s);

  /*-------------------------------------------------------------------------------
   *  Return the status byte.
   */
  return(statusByte);
}


/**************************************************************************************************
 * @fn          macSpiReadReg
 *
 * @brief       Read value from radio regiser.
 *
 * @param       regAddr - address of register
 *
 * @return      register value
 **************************************************************************************************
 */
uint16 macSpiReadReg(uint8 regAddr)
{
  return(  macSpiRegAccess((uint8)(regAddr | REG_READ_NOT_WRITE_BIT), 0 )  );
}


/**************************************************************************************************
 * @fn          macSpiWriteReg
 *
 * @brief       Write value to radio register.
 *
 * @param       regAddr  - address of register
 * @param       regValue - register value to write
 *
 * @return      none
 **************************************************************************************************
 */
void macSpiWriteReg(uint8 regAddr, uint16 regValue)
{
  macSpiRegAccess(regAddr, regValue);
}


/*=================================================================================================
 * @fn          macSpiRegAccess
 *
 * @brief       Access 16-bit radio register.  This function performs a read or write.  The
 *              calling code must configure the read/write bit of the register's address byte.
 *              This bit is set or cleared based on the type of access.
 *
 * @param       regAddrByte - address byte of register; the read/write bit already configured
 *
 * @return      register value
 *=================================================================================================
 */
static uint16 macSpiRegAccess(uint8 regAddrByte, uint16 regWriteValue)
{
  uint16 regReadValue;
  halMacSpiIntState_t s;

  MAC_ASSERT(macSpiRadioPower & MAC_SPI_RADIO_POWER_VREG_ON);  /* radio must be powered */

  /*-------------------------------------------------------------------------------
   *  Disable interrupts that call SPI functions.
   */
  HAL_MAC_SPI_ENTER_CRITICAL_SECTION(s);

  /*-------------------------------------------------------------------------------
   *  Turn chip select "off" and then "on" to clear any current SPI access.
   */
  HAL_MAC_SPI_SET_CHIP_SELECT_OFF();
  HAL_MAC_SPI_SET_CHIP_SELECT_ON();

  /*-------------------------------------------------------------------------------
   *  Send register address byte.  The read/write bit has already been set or
   *  cleared by the calling function.  Wait for SPI access to complete.
   */
  HAL_MAC_SPI_WRITE_BYTE(regAddrByte);
  HAL_MAC_SPI_WAIT_DONE();

  /*-------------------------------------------------------------------------------
   *  Send the high byte of register value.  If this operation is a read, this
   *  value is not used and is just dummy data.  Wait for SPI access to complete.
   */
  HAL_MAC_SPI_WRITE_BYTE(regWriteValue >> 8);
  HAL_MAC_SPI_WAIT_DONE();

  /*-------------------------------------------------------------------------------
   *  If this is a read operation, SPI data register now contains the high byte
   *  of the register value.  Store it to the high byte of return value.
   *
   *  If this is a write operation, the SPI data register contains junk data.
   *  For efficiency, it is stored anyway (although it will never be used).
   *
   */
  regReadValue = (uint16) HAL_MAC_SPI_READ_BYTE() << 8;

  /*-------------------------------------------------------------------------------
   *  Send the low byte of register value.  If this operation is a read, this
   *  value is not used and is just dummy data.  Wait for SPI access to complete.
   */
  HAL_MAC_SPI_WRITE_BYTE(regWriteValue & 0xff);
  HAL_MAC_SPI_WAIT_DONE();

  /*-------------------------------------------------------------------------------
   *  If this is a read operation, SPI data register now contains the low byte
   *  of the register value.  Store it to the low byte of return value.
   *
   *  If this is a write operation, the SPI data register contains junk data.
   *  For efficiency, it is stored anyway (although it will never be used).
   */
  regReadValue |= HAL_MAC_SPI_READ_BYTE();

  /*-------------------------------------------------------------------------------
   *  Turn off chip select.  Enable interrupts that call SPI functions.
   */
  HAL_MAC_SPI_SET_CHIP_SELECT_OFF();
  HAL_MAC_SPI_EXIT_CRITICAL_SECTION(s);

  /*-------------------------------------------------------------------------------
   *  Return the register value.
   */
  return(regReadValue);
}


/**************************************************************************************************
 * @fn          macSpiWriteRam
 *
 * @brief       Write data to radio's RAM.
 *
 * @param       ramAddr    - radio RAM address
 * @param       pWriteData - pointer to data to write
 * @param       len        - length of data in bytes
 *
 * @return      none
 **************************************************************************************************
 */
void macSpiWriteRam(uint16 ramAddr, uint8 * pWriteData, uint8 len)
{
  macSpiRamFifoAccess(ramAddr , pWriteData, len, RAM_WRITE);
}


/**************************************************************************************************
 * @fn          macSpiReadRam
 *
 * @brief       Read data from radio's RAM.
 *
 * @param       ramAddr    - radio RAM address
 * @param       pReadData  - pointer for storing read data
 * @param       pWriteData - pointer to data to write
 * @param       len        - length of data in bytes
 *
 * @return      none
 **************************************************************************************************
 */
void macSpiReadRam(uint16 ramAddr, uint8 * pReadData, uint8 len)
{
  macSpiRamFifoAccess(ramAddr, pReadData, len, RAM_READ);
}


/**************************************************************************************************
 * @fn          macSpiWriteRamUint16
 *
 * @brief       Write unsigned 16-bit value to radio RAM.
 *
 * @param       ramAddr    - radio RAM address
 * @param       pWriteData - pointer to data to write
 * @param       len        - length of data in bytes
 *
 * @return      none
 **************************************************************************************************
 */
void macSpiWriteRamUint16(uint16 ramAddr, uint16 data)
{
  uint8 littleEndianValue[2];

  littleEndianValue[0] = data & 0xFF;
  littleEndianValue[1] = data >> 8;

  macSpiRamFifoAccess(ramAddr , &littleEndianValue[0], sizeof(uint16), RAM_WRITE);
}


/**************************************************************************************************
 * @fn          macSpiWriteFifo
 *
 * @brief       Write data to radio FIFO.
 *
 * @param       ramAddr    - radio FIFO address
 * @param       pWriteData - pointer to data to write
 * @param       len        - length of data in bytes
 *
 * @return      none
 **************************************************************************************************
 */
void macSpiWriteFifo(uint8 regAddr, uint8 * pWriteData, uint8 len)
{
  macSpiRamFifoAccess((uint16)(regAddr | FIFO_ACCESS_TAG_BIT), pWriteData, len, FIFO_WRITE);
}


/**************************************************************************************************
 * @fn          macSpiReadFifo
 *
 * @brief       Read data from radio FIFO.
 *
 * @param       ramAddr    - radio FIFO address
 * @param       pReadData  - pointer for storing read data
 * @param       len        - length of data in bytes
 *
 * @return      none
 **************************************************************************************************
 */
void macSpiReadFifo(uint8 regAddr, uint8 * pReadData, uint8 len)
{
  macSpiRamFifoAccess((uint16)(regAddr | FIFO_ACCESS_TAG_BIT), pReadData, len, FIFO_READ);
}


/*=================================================================================================
 * @fn          macSpiRamFifoAccess
 *
 * @brief       Access radio FIFO or RAM.
 *
 * @param       addr       - FIFO register address or RAM address, a special bit in the upper
 *                           byte differentiates FIFO or RAM access
 * @param       pData      - pointer for data to read or write
 * @param       len        - length of data in bytes
 * @param       readBit    - dual purpose parameter serves as flag and bit value
 *                           0=write operation, for read must have the correct bit "or" value
 *                           for forming address (see code for details)
 *
 * @return      none
 *=================================================================================================
 */
void macSpiRamFifoAccess(uint16 addr, uint8 * pData, uint8 len, uint8 readBit)
{
  halMacSpiIntState_t s;

  MAC_ASSERT(macSpiRadioPower & MAC_SPI_RADIO_POWER_OSC_ON);  /* oscillator must be on */
  
  /*-------------------------------------------------------------------------------
   *  If there is nothing to do, exit now.
   */
  if (len == 0)  return;

  /*-------------------------------------------------------------------------------
   *  Disable interrupts that call SPI functions.
   */
  HAL_MAC_SPI_ENTER_CRITICAL_SECTION(s);

  /*-------------------------------------------------------------------------------
   *  Turn chip select "off" and then "on" to clear any current SPI access.
   */
  HAL_MAC_SPI_SET_CHIP_SELECT_OFF();
  HAL_MAC_SPI_SET_CHIP_SELECT_ON();

  /*-------------------------------------------------------------------------------
   *  Main loop.  If the SPI access is interrupted, execution comes back to
   *  the start of this loop.  Loop exits when nothing left to transfer.
   *  (Note: previous test guarantees at least one byte to transfer.)
   */
  do
  {
    /*-------------------------------------------------------------------------------
     *  Test to see if this is a FIFO access.  The address parameter includes a
     *  special unused bit to specify RAM or FIFO access.
     */
    if (addr & FIFO_ACCESS_TAG_BIT)
    {
      /*-------------------------------------------------------------------------------
       *  This is a FIFO access.
       * -----------------------
       *
       *  The lower byte of the 16-bit address parameter holds the register address.
       *  The value 'readBit' is or'ed in to get the value to get the formatted address
       *  byte.
       *
       *  The value 'readBit' does double duty as a flag and bit value.  Here it is
       *  used as a bit value.  The calling routine has set 'readBit' to the proper
       *  value for or'ing into the address byte.
       *
       *  Send FIFO register address.  Wait for SPI access to complete.
       */
      HAL_MAC_SPI_WRITE_BYTE((addr & 0xff) | readBit);
      HAL_MAC_SPI_WAIT_DONE();
    }
    else
    {
      /*-------------------------------------------------------------------------------
       *  This is a RAM access.
       * ----------------------
       *
       *  Send first RAM address byte.  Wait for SPI access to complete.
       *
       *  The RAM address is a 9-bit value that is mapped across two bytes.  The first
       *  address byte is formed by taking the lower seven bits of the raw address
       *  and or'ing the RAM/Register bit into the top bit.
       *
       *   1st byte of RAM address:
       *   ---------------------------------------------------------
       *   |   1  |  A6  |  A5  |  A4  |  A3  |  A2  |  A1  |  A0  |
       *   ---------------------------------------------------------
       *      ^
       *      RAM/Register bit: 0=register access, 1=RAM access
       */
      HAL_MAC_SPI_WRITE_BYTE((addr & RAM_ADDR_FIRST_BYTE_MASK) | RAM_NOT_REGISTER_BIT);
      HAL_MAC_SPI_WAIT_DONE();

      /*-------------------------------------------------------------------------------
       *  Send second RAM address byte.  Wait for SPI access to complete.
       *
       *  The second address byte is formed by shifting the raw 16-bit address one bit
       *  to the right.  This positions A8 and A7 in the upper two bits of the lower
       *  byte.  After this, all bits apart from these upper address bits, are masked off.
       *  Finally, the read/write bit is or'ed to get the formatted value.
       *
       *  The value 'readBit' does double duty as a flag and bit value.  Here it is
       *  used as a bit value.  The calling routine has set 'readBit' to the proper
       *  value for or'ing into the address byte.
       *
       *   2nd byte of RAM address:               x=don't care
       *   ---------------------------------------------------------
       *   |  A8  |  A7  |  rw  |  x   |  x   |  x   |  x   |  x   |
       *   ---------------------------------------------------------
       *                    ^
       *                    RAM read/write bit: 0=write+read, 1=read
       */
      HAL_MAC_SPI_WRITE_BYTE(((addr >> 1) & RAM_ADDR_SECOND_BYTE_MASK) | readBit);
      HAL_MAC_SPI_WAIT_DONE();
    }

    /*-------------------------------------------------------------------------------
     *  Inner loop.  This loop executes as long as the SPI access is not interrupted.
     *  Loop completes when nothing left to transfer.
     *  (Note: previous test guarantees at least one byte to transfer.)
     */
    do
    {
      /*-------------------------------------------------------------------------------
       *  Send byte to write.  If this is a read-only operation, the radio will
       *  ignore this value so it's OK to send whatever happens to be at this pointer.
       */
      HAL_MAC_SPI_WRITE_BYTE(*pData);

      /*-------------------------------------------------------------------------------
       *  Use idle time.  Perform increment/decrement operations before pending on
       *  completion of SPI access.
       *
       *  If this is a RAM access, increment the address.  For all accesses decrement
       *  the length counter.  Wait for SPI access to complete.
       */
      if (!(addr & FIFO_ACCESS_TAG_BIT))
      {
        addr++;
      }
      len--;
      HAL_MAC_SPI_WAIT_DONE();

      /*-------------------------------------------------------------------------------
       *  SPI data register holds data just read.  If this is a read, store the value
       *  into memory.  If this is a write, data is junk, ignore it.
       *
       *  The value 'readBit' does double duty as a flag and bit value.  Here it is
       *  used as a flag.  A non-zero value indicates a read operation.
       */
      if (readBit)
      {
        *pData = HAL_MAC_SPI_READ_BYTE();
      }

      /*-------------------------------------------------------------------------------
       *  At least one byte of data has transferred.  Briefly enable (and then disable)
       *  interrupts that call SPI functions.  This provides a window for any timing
       *  critical interrupts that might be pending.
       *
       *  To improve latency, take care of pointer increment within the interrupt
       *  enabled window.
       */
      HAL_MAC_SPI_EXIT_CRITICAL_SECTION(s);
      pData++;
      HAL_MAC_SPI_ENTER_CRITICAL_SECTION(s);

      /*-------------------------------------------------------------------------------
       *  If chip select is "off" the SPI access was interrupted.  In this case,
       *  turn back on chip select and break to the main loop.  The main loop will
       *  resend the RAM address and pick up where the access was interrupted.
       */
      if (HAL_MAC_SPI_CHIP_SELECT_IS_OFF())
      {
        HAL_MAC_SPI_SET_CHIP_SELECT_ON();
        break;
      }

    /*-------------------------------------------------------------------------------
     */
    } while (len); /* inner loop */
  } while (len);   /* main loop */

  /*-------------------------------------------------------------------------------
   *  Turn off chip select and re-enable interrupts that use SPI.
   */
  HAL_MAC_SPI_SET_CHIP_SELECT_OFF();
  HAL_MAC_SPI_EXIT_CRITICAL_SECTION(s);
}



/**************************************************************************************************
 *                                  Compile Time Integrity Checks
 **************************************************************************************************
 */
#if (MAC_SPI_RADIO_POWER_VREG_ON & MAC_SPI_RADIO_POWER_OSC_ON)
#error "ERROR!  Non-unique bit values for SPI radio power states."
#endif


/**************************************************************************************************
*/
