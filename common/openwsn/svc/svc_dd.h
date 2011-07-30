#ifndef _SVC_DD_4768_
#define _SVC_DD_4768_

dd_construct
dd_destroy
/* DD Stage1: interest broadcast */
dd_broadcast / dd_write

/* DD Stage2: read data from the network */
dd_read

/* DD Stage3: path reinforce */
dd_reinforce

dd_retrieve = {{dd_broadcast}, dd_read, {dd_reinforce}}

#endif
