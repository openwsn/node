

/***********************************************************************************
  Filename:     util.c
    
  Description:  Utility library
 
***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/

#include "hal_configall.h"
#include "hal_foundation.h"
#include "util.h"
//#include "hal_uart.h"
#include "string.h"

/***********************************************************************************
* LOCAL VARIABLES
*/
static char pValueToTextBuffer[12]; // Length of -2147483648 + terminator

/***********************************************************************************
* GLOBAL FUNCTIONS
*/

/***********************************************************************************
* @fn      printStr
*
* @brief   Print a zero-terminated string on the console
*
* @param   sz - string to send to the console
*
* @return  none
*/
void printStr(char* sz)
{
/*  
    char lineFeed = '\r';  
    halUartWrite((uint8*)sz, (uint16) strlen(sz));
    // if last character is newline, add carriage return
    if(*(sz+(strlen(sz)-1)) == '\n')
        halUartWrite((uint8*)&lineFeed,1);  
*/		
}


/***********************************************************************************
* @fn      printStrW
*
* @brief   Print a string padded with blanks.
*
* @param   sz - string to display
*          len - total length of the string
*
* @return  none
*/
void printStrW(char *sz, uint8 nBytes)
{
/*
    printStr((char *)sz);
    if (strlen(sz)<nBytes) {
        nBytes-= strlen(sz);
        while(--nBytes>0)
            printChar(' ');
    }
*/	
}


/***********************************************************************************
* @fn      printChar
* 
* @brief   Print a character on the console.
*
* @param   c - character to print
*
* @return  none
*/
void printChar(char c)
{ 
/*
    halUartWrite((uint8*)&c, 1);
*/	
}


/***********************************************************************************
* @fn      printHex
*
* @brief   Print an integer as hexadecimal.
*
* @param   v - the value to be displayed
*          s - size (8,16,32) | bit 7 to omit 'h' at the end
*
* @return  none
*/
void printHex(uint32 v, uint8 s)
{
/*
    char buf[16];
    uint8 w;
    
    w= (s&~NO_HEX_INDICATION)>>2;        // String length
    
    if (s&NO_HEX_INDICATION) {
        buf[w]= '\0';
    } else {
        buf[w]= 'h';       // HEX
        buf[w+1]= '\0';    // String terminator
    }
    
    // Convert number
    do {
        w--;
        buf[w]= "0123456789ABCDEF"[v&0xF];
        v>>= 4;
    } while(w>0);
    
    // Print
    printStr(buf);
*/	
}


/***********************************************************************************
* @fn      printKvpHex
*
* @brief   Print a Key-Value pair hexadecimal
*
* @param   szKey - the value to be displayed
*          v - the value to be displayed
*
* @return  none
*/
void printKvpHex(char *szKey, uint16 v)
{
/*
    printStrW(szKey, KVP_ALIGN);
    printStr(": ");
    printHex(v,16);
    printChar('\n');
    printChar('\r');
*/	
}


/***********************************************************************************
* @fn      printKvpInt
*
* @brief   Print a Key-Value pair hexadeciaml.
* 
* @param   szKey - the value to be displayed
*
*          v - the value to be displayed
*
* @return  none
*/
void printKvpInt(char *szKey, uint32 v)
{
/*
    printStrW(szKey, KVP_ALIGN);
    printStr(": ");
    printInt(v);
    printChar('\n');
    printChar('\r');
*/	
}


/***********************************************************************************
* @fn      printInt
*
* @brief   Print a 32-bit integer as decimal.
*
* @param   v - the value to be displayed
*
* @return  none
*/
void printInt(uint32 v)
{
/*
    uint32 iDiv;
    BOOL bLeadingZero;
    
    // Initialise
    iDiv= 1000000000;
    bLeadingZero= TRUE;
    
    // For each digit
    while (iDiv>0) {
        uint8 vDigit;
        
        // Current digit
        vDigit= (v / iDiv);
        
        // First digit to be printed
        if (vDigit>0) {	
            bLeadingZero= FALSE;
        }
        
        if (!bLeadingZero) {
            printChar( '0' + vDigit );
        }
        
        // Next digit
        v= v % iDiv;
        iDiv= iDiv/10;
    }
    
    // Special case zero
    if (bLeadingZero) {
        printChar('0');
    }
*/    
}


/***********************************************************************************
* @fn          convInt32ToText
*
* @brief       Converts 32 bit int to text
*
* @param       int32 value
*          
* @return      char* - pointer to text buffer which is a file scope allocated array
*/
char* convInt32ToText(int32 value) {
    char *pLast = pValueToTextBuffer;
    char *pFirst;
    char last;
    
    // Record the sign of the value
    uint8 negative = (value < 0);
    value = ABS(value);
    
    // Print the value in the reverse order
    do {
        *(pLast++) = '0' + (value % 10);
        value /= 10;
    } while (value);
    
    // Add the '-' when the number is negative, and terminate the string
    if (negative) *(pLast++) = '-';
    *(pLast--) = 0x00;
    
    // Now reverse the string
    pFirst = pValueToTextBuffer;
    while (pLast > pFirst) {
        last = *pLast;
        *(pLast--) = *pFirst;
        *(pFirst++) = last;
    }
    
    return pValueToTextBuffer;
} 


/***********************************************************************************
* @fn          min
*
* @brief       Return minimum of two values
*
* @param       uint8 v1 - value 1
*              uint8 v2 - value 2
*
* @return      uint8 - minimum of two values
*/
/*
uint8 min(uint8 v1, uint8 v2)
{
    if(v1 < v2)
        return v1;
    else return v2;
}
*/
