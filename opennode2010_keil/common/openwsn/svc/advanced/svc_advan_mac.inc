#include "svc_configall.h"
#include "svc_foundation.h"

#ifdef __cplusplus
extern "C"{
#endif

    uint8 send( void * object, TiIoBuf * iobuf, uint8 option );
uintx       aloha_broadcast( TiAloha * mac, TiOpenFrame * opf, uint8 option );
uintx       aloha_recv( TiAloha * mac, TiOpenFrame * opf, uint8 option );
void        aloha_evolve( void * macptr, TiEvent * e );


typedef struct{
    void * object;
    send
    broadcast
    recv
    evolve
    setpanid
    setlocaladdress
    setremoteaddress
    setchannel
    sleep
    wakeup 
    installnotifier/setlistener
}TiOpenMacInterface;



#ifdef __cplusplus
}
#endif
