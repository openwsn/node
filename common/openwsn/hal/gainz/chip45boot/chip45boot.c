//
// chip45boot.c
// ------------
//
// chip45boot .srec bootloader for Atmel AVR microcontrollers
//
// Copyright 2006 by Erik Lins, www.chip45.com
//
// The bootloader can receive a Motorola s-record file through USART.
// An interactive mode can be entered after reset and provides simple
// commands.
//
// Usage
// -----
// The bootloader waits a short while for receiving an 'i' from USART.
// (In a terminal program, hold the 'i' key pressed during reset.)
// If so, a prompt "> " will be shown.
// Available commands are:
// 'f' starts the flash programming (the character f is echoed plus an '\r')
//     Now an s-record file (see above) can be uploaded to the controller.
//     During programming a line of '.' is shown, each '.' marks the end of
//     a valid s-record line.
//     When programming has been completed successfully 'OK' is shown and the
//     bootloader starts again.
//     Possible errors during programming are:
//     'BC' wrong byte count in s-record
//     'CS' wrong checksum in s-record
//     'PC' wrong page count, i.e. s-record number of data bytes are not an
//          integer factor of flash page size
// 'g' starts the application, i.e. interrupt vector table is rearranged to
//     bottom of flash and bootloader jumps to 0x0000
// 'e' starts the programming of the EEPROM (not yet implemented)
//     This command is only available if you #define EEPROM_CODE!
//     It increases bootloader code size above 1k, keep this in mind when
//     setting the fuse bits for boot block size!
//
// S-Record upload with terminal program
// -------------------------------------
// Any terminal program can be used for uploading the .srec file to
// the MCU, e.g. HyperTerminal, which comes with any Windows system or
// TeraTermPro (http://hp.vector.co.jp/authors/VA002416/teraterm.html),
// which I prefer. Since the bootloader receives an s-record (one line
// of the file), then programs the s-record to flash, then receives the
// next s-record, programs it and so on, it is necessary to delay the 
// transmission of each line for the time the bootloader needs to
// program the flash page. For an ATmega8 (64 byte flash page size) running
// at 14.7456MHz a delay of 100msec after each line is sufficient. I will do
// further tests and add the experiences in this text.
// To set this delay with the terminal program do this:
// TeraTermPro: Menu Setup -> Serial Port -> Transmit Delay -> "100" msec/line
// HyperTerminal: Datei -> Eigenschaften -> Einstellungen ->
// ASCII-Konfiguration -> Zeilenverzögerung "100" Millisekunden
//
// S-record file criteria:
// -----------------------
// The maximum number of data bytes per s-record (each line of the
// s-record file) is 64. avr-objcopy usually generates s-record files
// with 32 data bytes / s-record -> good for all AVRs!
// The data bytes of the s-record must not be larger than the flash
// page size of the target AVR MCU, but must be an integer factor of
// the flash page size (e.g. 64 byte flash page size and 16, 32 or 64
// data bytes per s-record). Since all major AVRs have at least 64 bytes
// flash page size and common s-records have 32 data bytes per record,
// everything should fine most of the time!
//
// In case you need to generate an s-record file from the normally used
// Intel hex file, srec_cat will do the job. srec_cat comes with WinAVR:
//
// srec_cat input.hex -Intel -Output output.srec -Motorola
//
// You can specify the line length for the s-records:
// srec_cat input.hex -Intel -Output output.srec -Motorola -Line_Length 142
// (the line length of 142 results in a byte count of 64, which is the maximum)
//
// Cheers, ER!K ;-)
//

// Subversion keywords
//
// $Rev: 15 $
// $Date: 2006-05-24 12:04:07 +0200 (Mi, 24 Mai 2006) $
// $Author: linserik $
//


// includes
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/boot.h>
#include <util/delay.h>


// include additional MCU definitions to allow compiling
// for different MCUs from same source code
#include "mcudefs.h"


// definitions

// #define this to include code for programming the EEPROM
// without eeprom code, size of bootloader is < 1k
// with eeprom code, size is > 1k
// #undef EEPROM_CODE
#define EEPROM_CODE

// CPU frequency is normally defined in the Makefile
#ifndef F_CPU
#define F_CPU 14745600
#endif

// TRUE and FALSE
#define TRUE  1
#define FALSE 0

// UART baudrate
#define BAUDRATE 115200


// function prototypes
void uartPutChar(char);  // put a character over USART
char uartGetChar(void);  // read (wait for) a character from USART
void uartPutHex(uint8_t);  // print hex byte over USART
uint8_t hexCharToInt(char);  // convert hex character to unsigned integer
uint8_t hexByteToInt(char, char);  // convert hex byte (two characters) to unsigned integer
uint8_t parseSrecBuffer(void);  // parse the current s-record in the buffer
void writeBufferToFlash(void);  // write the current s-record into flash
void (*startApplication)( void ) = 0x0000;  // pointer to application at flash start


// global variables
uint8_t selectFlashProgramming = FALSE;
char receiveBuffer[150];
char *receiveBufferPointer, *tmpBuffer;
uint8_t receiveBufferFull = FALSE;
uint8_t bufferOverflow = FALSE;
uint8_t bootloaderEnableFlag = FALSE;
uint32_t srecAddress = 0;
uint32_t writeBaseAddress = 0;
int8_t flashPageSizeCounter = 0;
uint8_t srecEndOfFile = FALSE;
#ifdef EEPROM_CODE
uint8_t selectEepromProgramming = FALSE;
char eepromWriteBuffer[SPM_PAGESIZE+2];  // SPM_PAGESIZE used as eeprom buffer size
#endif


//
// interrupt routine called when a character was received from USART
//
ISR(myUSART_ReceiveCompleteVect) {

    char c = myUDR;  // save the character

    if((c != '\r') && (c != '\n'))
	*receiveBufferPointer++ = c;  // write character to the buffer

    // if end of line is reached, set buffer full flag
    // this starts s-record processing in the main program
    if((c == '\r') || (c == '\n')) {
	receiveBufferFull = TRUE;
    }
}


//
// main function
//
int main(void) {

    char c;

    // init USART
    myUBRRH = (F_CPU/(BAUDRATE*16L)-1) >> 8;          // calculate baudrate and set high byte
    myUBRRL = (uint8_t)(F_CPU/(BAUDRATE*16L)-1);      // and low byte
    myUCSRB = _BV(myTXEN)|_BV(myRXEN);                // enable transmitter and receiver
    myUCSRC = myURSEL | _BV(myUCSZ1) | _BV(myUCSZ0);  // 8 bit character size, 1 stop bit, no parity

    // enable USART receive interrupt
    myUCSRB |= _BV(myRXCIE);

    // the bootloader may be activated either if
    // the character 'i' (interactive mode) was received from USART
    // or the flash is (still) empty

    // poll USART receive complete flag 64k times to catch the 'i' reliably
    uint16_t loop = 0;
    do {
	if(bit_is_set(myUCSRA, myRXC))
	    if(myUDR == 'i')
		bootloaderEnableFlag = TRUE;
    } while(--loop);

    // test if flash is empty (i.e. flash content is 0xff)
    if(pgm_read_byte_near(0x0000) == 0xFF) {
	bootloaderEnableFlag = TRUE;  // set enable flag
    }

    // check enable flag and start application if FALSE
    if(!bootloaderEnableFlag) {
	startApplication();
    }


    //
    // now the bootloader code begins
    //

 prompt:

    // move interrupt vector table to boot loader area
    GICR = _BV(IVCE);
    GICR = _BV(IVSEL);
 
    // welcome message and prompt
#ifndef EEPROM_CODE
    PGM_P welcome = { "\rchip45boot[flash]\r> " };
#else
    PGM_P welcome = { "\rchip45boot[flash][eeprom]\r> " };
#endif
    for(c=0; welcome[c] != '\0'; ++c) {
	uartPutChar(welcome[c]);
    }

    // loop until a valid character is received
    do {

	c = uartGetChar();  // read a character

	if(c == 'f') {  // 'f' selects flash programming
	    uartPutChar('f');
	    uartPutChar('\r');
	    selectFlashProgramming = TRUE;  // set flag
	}

	if(c == 'g') {  // 'g' starts the application
	    uartPutChar('g');
	    uartPutChar('\r');
	    GICR = _BV(IVCE);    // relocate interrupt vector table
	    GICR = 0;            // to bottom of flash
	    startApplication();  // and jumpt to 0x0000
	}

#ifdef EEPROM_CODE
	if(c == 'e') {  // 'e' selects eeprom programming
	    uartPutChar('e');
	    uartPutChar('\r');
	    selectEepromProgramming = TRUE;  // set flag
	}
#endif

    } while( !selectFlashProgramming
#ifdef EEPROM_CODE
	     && !selectEepromProgramming
#endif
	    );


    // enable interrupts
    sei();

    // set receive buffer pointer to first buffer
    receiveBufferPointer = (char *)receiveBuffer;
    flashPageSizeCounter = SPM_PAGESIZE;  // preset page counter to flash page size
    srecAddress = 0;  // reset s-record address
    receiveBufferFull = FALSE;  // the buffer is not full at beginning
    writeBaseAddress = 0;

    // endless loop
    while(1) {

	// if buffer is full, parse the buffer and write to flash
	if(receiveBufferFull) {
	    cli();  // disable interrupts
	    parseSrecBuffer();  // parse the s-record in buffer
	    writeBufferToFlash();  // write to flash

	    // was an end-of-file s-record found?
	    if(srecEndOfFile) {
		uartPutChar('O');
		uartPutChar('K');
		uartPutChar('\r');
		srecEndOfFile = FALSE;
		goto prompt;  // start bootloader again
	    }
	    receiveBufferFull = FALSE;  // reset full flag
	    receiveBufferPointer = (char *)receiveBuffer;  // reset buffer pointer to start of buffer
	    sei();  // enable interrupts
	}
    }
}


//
// parse the s-record in the current buffer
// check s-record type, extract the address, calculate checksum, etc.
//
uint8_t parseSrecBuffer() {

    uint8_t srecBytecount, srecChecksum, srecType;
    uint8_t tmpAddress;
    char hi, lo;
    char c;
    uint16_t i, w;
    char *thisBuffer;

    // make a pointer to the receive buffer
    thisBuffer = (char *)receiveBuffer;

    // check if current buffer is a data record (starts with S1, S2 or S3)
    if(*thisBuffer++ == 'S' ) {

	// get s-record type
	srecType = hexCharToInt(*thisBuffer++);

	// only process the record in case it's a data record
	if((srecType == 1) || (srecType == 2) || (srecType == 3)) {

	    // get the byte count
	    hi = *thisBuffer++;
	    lo = *thisBuffer++;
	    srecBytecount = hexByteToInt(hi, lo);
	    // one could directly put *thisBuffer++ into the arguments,
	    // but the arguments are put on stack last first, i.e. the
	    // lo character is fetched from the *thisBuffer first and
	    // this changes lo and hi character! Using seperate variables
	    // hi and lo is more clear and readable than changing the 
	    // sequence in the hexByteToInt function.

	    // check if byte count is larger than 0x43, i.e. we have more
	    // than 64 bytes in the record -> not allowed
	    if(srecBytecount > (0x43 + srecType - 1)) {
		uartPutChar('B');
		uartPutChar('C');
		uartPutChar('\r');
		return FALSE;
	    }
	    srecChecksum = srecBytecount;  // add byte count to checksum

	    // extract the address depending of s-record type
	    for(i = 0; i <= srecType; ++i) {
		
		hi = *thisBuffer++;
		lo = *thisBuffer++;
		tmpAddress = hexByteToInt(hi, lo);  // get next address byte
		srecAddress <<= 8;  // shift existing address one byte left
		srecAddress += tmpAddress;  // add new lower address byte
		srecChecksum += tmpAddress;  // add address portion to checksum
	    }

	    // read all data bytes
	    for(i = 0; i < (srecBytecount - 3 + (srecType - 1)); i += 2) {

		if(selectFlashProgramming) {
		    // assemble a 16 bit little endian data word and calculate checksum
		    hi = *thisBuffer++;
		    lo = *thisBuffer++;
		    c = hexByteToInt(hi, lo);
		    srecChecksum += c;
		    w = c;
		    hi = *thisBuffer++;
		    lo = *thisBuffer++;
		    c = hexByteToInt(hi, lo);
		    srecChecksum += c;
		    w += c << 8;
		
		    // write word to flash write buffer
		    boot_page_fill(srecAddress + i, w);
		}

#ifdef EEPROM_CODE
		if(selectEepromProgramming) {
		    hi = *thisBuffer++;
		    lo = *thisBuffer++;
		    c = hexByteToInt(hi, lo);
		    srecChecksum += c;
		    eepromWriteBuffer[srecAddress + i] = c;
		    hi = *thisBuffer++;
		    lo = *thisBuffer++;
		    c = hexByteToInt(hi, lo);
		    srecChecksum += c;
		    eepromWriteBuffer[srecAddress + i + 1] = c;
		}
#endif

		if((flashPageSizeCounter -= 2) < 0) {
		    uartPutChar('P');
		    uartPutChar('S');
		    uartPutChar('\r');
		    return FALSE;
		}
	    }

	    // get checksum and compare to 0xff
	    hi = *thisBuffer++;
	    lo = *thisBuffer++;
	    srecChecksum += hexByteToInt(hi, lo);  
	    if(srecChecksum != 0xff) {
		uartPutChar('C');
		uartPutChar('S');
		uartPutChar('\r');
		return FALSE;
	    }

	    uartPutChar('.');  // some progress indication
	}

	// check if end of file record
	if((srecType == 9) || (srecType == 8) || (srecType == 7)) {
	    srecEndOfFile = TRUE;
	}
    }

    return TRUE;
}

    
//
// write buffer to flash
//
void writeBufferToFlash() {

    uint8_t sreg;

    sreg = SREG;  // save status register

    // check if either page size counter is zero (i.e. buffer is full)
    // or end of file was reached (i.e. the previously received
    // bytes must be written to flash)
    if((flashPageSizeCounter == 0) || (srecEndOfFile == TRUE)){

	if(selectFlashProgramming) {
	    eeprom_busy_wait();  // wait for eeprom not busy

	    boot_page_erase(writeBaseAddress);  // do a page erase
	    boot_spm_busy_wait();  // wait for page erase done

	    boot_page_write(writeBaseAddress);  // do a page write
	    boot_spm_busy_wait();  // wait for write completed

	    boot_rww_enable();  // reenable rww section again
	}

#ifdef EEPROM_CODE
	if(selectEepromProgramming) {

	    eeprom_busy_wait();

	    uartPutHex((writeBaseAddress & 0xff00) >> 8);
	    uartPutHex((writeBaseAddress & 0x00ff));

	    eeprom_write_block(eepromWriteBuffer, (void *)writeBaseAddress, SPM_PAGESIZE);
	}
#endif

	flashPageSizeCounter = SPM_PAGESIZE;  // set byte counter to correct value
	writeBaseAddress += SPM_PAGESIZE;
    }

    SREG = sreg;  // restore status register
}


//
// convert a hex number character into 4 bit unsigned integer
//
uint8_t hexCharToInt(char c) {

    // test if character is letter or number
    if(c <= '9')
	return (c - '0');
    else
	return (c - 'A' + 0xa);

}


//
// convert a hex byte (two characters) into 8 bit unsigned integer
//
uint8_t hexByteToInt(char hi, char lo) {

    return ( (hexCharToInt(hi) << 4) + hexCharToInt(lo) );  // return the unsigned integer
}


// set to #if 1 for testing and debugging,
// this function is not used during normal operation
#if 1
//
// print a hex number (byte, 2 characters) over USART
//
void uartPutHex(uint8_t i) {
    uint8_t hi, lo;

    hi = (i & 0xf0) >> 4;
    lo = i & 0x0f;

    if((hi >= 0xa) && (hi <= 0xf))
	uartPutChar(hi + 'A' - 10);
    else
	uartPutChar(hi + '0');

    if((lo >= 0xa) && (lo <= 0xf))
	uartPutChar(lo + 'A' - 10);
    else
	uartPutChar(lo + '0');

}
#endif

//
// send a character through the USART
//
void uartPutChar(char c) {

    if(c == '\r')                            // if character is 'new line', then
	uartPutChar('\n');                  // send an additional 'carriage return'
    loop_until_bit_is_set(myUCSRA, myUDRE);  // wait until transmit buffer is empty
    myUDR = c;                               // write character to transmit buffer
}


//
// receive a character from the USART
//
char uartGetChar(void) {

    loop_until_bit_is_set(myUCSRA, myRXC);  // wait until character is received
    return myUDR;                           // return the character
}


//
// end of file chip45boot.c
////////////////////////////////////////////////////////////
