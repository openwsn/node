//
// mcudefs.h
//
// This file contains register, bit and interrupt vector definitions for
// different processor types. chip45boot uses the myXXXX names for USART
// access and interrupt settings.
// Feel free to add more mcu types to the file!
// 
// (C) E.Lins, www.chip45.com
// 17-May-2006
//

// Subversion keywords
//
// $Rev: 15 $
// $Date: 2006-05-24 12:04:07 +0200 (Mi, 24 Mai 2006) $
// $Author: linserik $
//


#ifndef MCUDEFS
#define MCUDEFS


#ifdef __AVR_ATmega8__
#define myUSART_ReceiveCompleteVect USART_RXC_vect
#define myRXCIE RXCIE
#define myUDR   UDR
#define myUBRRH UBRRH
#define myUBRRL UBRRL
#define myUCSRA UCSRA
#define myUCSRB UCSRB
#define myUCSRC UCSRC
#define myUDRE  UDRE
#define myRXC   RXC
#define myRXEN  RXEN
#define myTXEN  TXEN
#define myUCSZ0 UCSZ0
#define myUCSZ1 UCSZ1
#define myURSEL _BV(URSEL) // bit must be set to one on certain MCU types
#endif

#ifdef __AVR_ATmega128__
#define myUSART_ReceiveCompleteVect USART0_RX_vect
#define myRXCIE RXCIE0
#define myUDR   UDR0
#define myUBRRH UBRR0H
#define myUBRRL UBRR0L
#define myUCSRA UCSR0A
#define myUCSRB UCSR0B
#define myUCSRC UCSR0C
#define myUDRE  UDRE0
#define myRXC   RXC0
#define myRXEN  RXEN0
#define myTXEN  TXEN0
#define myUCSZ0 UCSZ00
#define myUCSZ1 UCSZ01
#define myURSEL 0          // MCU type does not require URSEL bit to be set
#endif

#ifdef __AVR_ATmega168__
#define myUSART_ReceiveCompleteVect USART_RX_vect
#define myRXCIE RXCIE0
#define myUDR   UDR0
#define myUBRRH UBRR0H
#define myUBRRL UBRR0L
#define myUCSRA UCSR0A
#define myUCSRB UCSR0B
#define myUCSRC UCSR0C
#define myUDRE  UDRE0
#define myRXC   RXC0
#define myRXEN  RXEN0
#define myTXEN  TXEN0
#define myUCSZ0 UCSZ00
#define myUCSZ1 UCSZ01
#define myURSEL 0          // MCU type does not require URSEL bit to be set
#endif

#ifdef __AVR_ATmega644__
#define myUSART_ReceiveCompleteVect USART0_RX_vect
#define myRXCIE RXCIE0
#define myUDR   UDR0
#define myUBRRH UBRR0H
#define myUBRRL UBRR0L
#define myUCSRA UCSR0A
#define myUCSRB UCSR0B
#define myUCSRC UCSR0C
#define myUDRE  UDRE0
#define myRXC   RXC0
#define myRXEN  RXEN0
#define myTXEN  TXEN0
#define myUCSZ0 UCSZ00
#define myUCSZ1 UCSZ01
#define myURSEL 0          // MCU type does not require URSEL bit to be set
#endif

#endif  // #ifndef MCUDEFS

// end of file mcudefs.h
