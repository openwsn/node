#ifndef _HAL_UNIQUEID_H_8243_
#define _HAL_UNIQUEID_H_8243_

void uniqueid_open();
void uniqueid_close();
uint8 uniqueid_get( char * buf, uint8 size );
uint8 uniqueid_set( char * buf, uint8 len );

#endif /* _HAL_UNIQUEID_H_8243_ */
