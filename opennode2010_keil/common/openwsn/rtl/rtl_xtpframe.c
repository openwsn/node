#include "rtl_configall.h"
#include "rtl_foundation.h"
#include "rtl_xtpframe.h"
#include "rtl_iobuf.h"

/*******************************************************************************
 * rtl_xtpvar
 *
 * @modified by zhangwei on 2010.05.01
 *	- first created
 * @modified by yanshixing on 2010.05.09
 *  - finish the implement, not test.
 ******************************************************************************/

bool xtp_frame_checksum_enabled(TiXtpFrame * xtpframe)
{
	char * pc = iobuf_ptr(xtpframe);	//get the data point in xtpframe.
	return ( (pc[1]&0x80) >>7 );		//refer to the document for the location

}

uint8 xtp_frame_encodetype(TiXtpFrame * xtpframe)
{
	char * pc = iobuf_ptr(xtpframe);	//get the data point in xtpframe.
	return ( (pc[1]&0x60) >>5 );		//refer to the document for the location
}

uint16 xtp_frame_length(TiXtpFrame * xtpframe)
{
	char * pc = iobuf_ptr(xtpframe);	//get the data point in xtpframe.
	return ( (pc[2]) | ((pc[1]&0xFF)<<8) );	//length is 16 bit, high byte first and the highest 4 bits are reserved.
}

/*char * xtp_frame_data(TiXtpFrame * xtpframe)
{
	char * pc = iobuf_ptr(xtpframe);	//get the data point in xtpframe.
	return &pc[4];		//refer to the document for the location

}*/

char * xtp_frame_dataptr(TiXtpFrame * xtpframe)
{
	char * pc = iobuf_ptr(xtpframe);	//get the data point in xtpframe.
	return &pc[3];		//refer to the document for the location

}

uint16 xtp_frame_checksum(TiXtpFrame * xtpframe)
{
	char * pc = xtp_frame_dataptr(xtpframe);	//get the data point in xtpframe.
	uint16 len = xtp_frame_length(xtpframe);
	return ( (pc[len-2]<<8)|(pc[len-1]) );	//高位在前，即pc[len-2]

}


void xtp_frame_set_checksum_enabled(TiXtpFrame * xtpframe)
{
	char * pc = iobuf_ptr(xtpframe);	//get the data point in xtpframe.
	pc[1] = (pc[1]|0x80);		//refer to the document for the location

}

void xtp_frame_set_encodetype(TiXtpFrame * xtpframe, uint8 type)
{
	char * pc = iobuf_ptr(xtpframe);	//get the data point in xtpframe.
	switch( type )
	{
		case 0://00 means raw data(no encoding).
			pc[1] = (pc[1]&0x9F);
			break;
		case 1:
			pc[1] = (pc[1]&0xBF);
			pc[1] = (pc[1]|0x20);
			break;//01 means compact encoding
		case 2://10 means the binary coding with escape characters.
			pc[1] = (pc[1]&0xDF);
			pc[1] = (pc[1]|0x40);
			break;
		case 3://11 means text based Base64 encoding
			pc[1] = (pc[1]|0x60);
			break;
	}
}
void xtp_frame_set_length(TiXtpFrame * xtpframe, uint16 len)
{
	char * pc = iobuf_ptr(xtpframe);	//get the data point in xtpframe.
	pc[2] = (len&0x00FF);
	pc[1] = len>>8;
}
void xtp_frame_set_data(TiXtpFrame * xtpframe, char * mem, uint16 memsize)
{
	char * pc = xtp_frame_dataptr(xtpframe);
	memmove(pc, mem, memsize);
}
void xtp_frame_set_checksum(TiXtpFrame * xtpframe, uint16 checksum)
{
	char * pc = xtp_frame_dataptr(xtpframe);	//get the data point in xtpframe.
	uint16 len = xtp_frame_length(xtpframe);
	pc[len-1] = ( checksum&0x00FF );
	pc[len-2] = (checksum>>8)&0x00FF;
}
