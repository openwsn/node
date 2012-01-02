#include "hal_configall.h"
#include "hal_foundation.h"

/**
 * @reference
 * - Signature bytes, atmega128 datasheet;
 * - Calibrate byte, atmega128 datasheet;
 * - Standard defined macros( __DATE__ and __TIME__ ) in GNU C Compiler, 
 *   http://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html
 */

#define UNIQURID_SIZE 8

static bool m_init = FALSE;
static char m_uniqueid[UNIQURID_SIZE];

void uniqueid_open()
{
	uint8 i;

	/* This is only a temporary method to generate a unique id by using the pre-defined
	 * macro in C compilers. You should upgrade it to a formal version in your real 
	 * applications. 
	 * 
	 * __DATE__  the date when do the preprocessing
	 * __TIME__  the time when doing the compiling
	 * 
	 * Everytime you compiled the application, a new unique id will be generated. 
	 */
	for (i=0; i<strlen(__TIME__); i++)
		m_uniqueid[i] = __TIME__[i];
		
	return;
}

void uniqueid_close()
{
	return;
}


uint8 uniqueid_get( char * buf, uint8 size )
{
	uint8 i, count;
	
	hal_assert( size >= UNIQURID_SIZE );
	for (i=0; i<UNIQURID_SIZE; i++)
		buf[i] = m_uniqueid[i];
		
	return UNIQURID_SIZE;
}

uint8 uniqueid_set( char * buf, uint8 len )
{
	uint8 i;
	
	if (len > UNIQURID_SIZE)
		len = UNIQURID_SIZE;
		
	for (i=0; i<len; i++)
		m_uniqueid[i] = buf[i];

	for (i=len; i<UNIQURID_SIZE; i++)
		m_uniqueid[i] = 0x00;
		
	return len;
}
