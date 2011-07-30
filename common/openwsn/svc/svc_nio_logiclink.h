
#include "../rtl/rtl_ieee802frame2.h"

/**
 * IEEE802.2 Logic Link Layer Frame
 *
 * LLC PDU Format
 *  +-----------------+-----------------+------------------+----------------+
 *  | DSAP Address 1B | SSAP Address 1B | Control 1B or 2B | Information nB |
 *  +-----------------+-----------------+------------------+----------------+
 * 
 *  DSAP Address: destination service access point address field
 *  SSAP Address: source service access point address field
 *  Control: 16bits include sequence numbering, and 8bits for formats that do not
 *  Information: information field
 *  
 * reference
 *  IEEE 802.11 specification(version 1998), http://wenku.baidu.com/view/102219360b4c2e3f5727633b.html
 */

#define LLC_DSAP(buf) (buf)[0]
#define LLC_SSAP
#define LLC_CONTROL
#define LLC_PAYLOAD(buf) 

#define LLC_SET_DSAP(buf,addr) ((buf)[0] = (addr))
#define LLC_SET_SSAP(buf,addr) ((buf)[1] = (addr))
#define LLC_SET_CONTROL
#define LLC_SET_PAYLOAD(buf) 

llc_construct
llc_destroy
llc_open
llc_close
llc_send
llc_recv






