#ifndef _APL_IEEE802FRAME154_DUMP_H_4873_
#define _APL_IEEE802FRAME154_DUMP_H_4873_

/* application layer's frame dump */

#include "openwsn/rtl/rtl_configall.h"
#include "openwsn/rtl/rtl_foundation.h"
#include "openwsn/rtl/rtl_frame.h"
#include "openwsn/rtl/rtl_debugio.h"

#define CONFIG_ASCII_OUTPUT
#undef CONFIG_ASCII_OUTPUT

void ieee802frame154_dump( TiFrame * f );

#endif
