#ifndef _RTL_FFT1D_H_7843_
#define _RTL_FFT1D_H_7843_

#include "rtl_configall.h"
#include "rtl_foundation.h"

/** 
 * fast fourier transform 
 */
int16 fft1d_n8( int8 * input, uint16 len, int8 * output, uint16 size );

/**
 * inverse fast fourier transform
 */
int16 ifft1d_n8( int8 * input, uint16 len, int8 * output, uint16 size );

int16 fft1d_n16( int16 * input, uint16 len, int16 * output, uint16 size );
int16 ifft1d_n16( int16 * input, uint16 len, int16 * output, uint16 size );

#endif
