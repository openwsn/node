not tested


#include "rtl_crc.h"

#ifdef CONFIG_CRC_TEST_STANDALONE
#include <stdio.h>

int main (int argc, char **argv)
{
int len = strlen(argv[1]), idx;
unsigned char msg[128];
unsigned short v16;
unsigned long v32;

    if( len > sizeof(msg) - sizeof(unsigned long) )
        len = sizeof(msg) - sizeof(unsigned);

    memcpy (msg, argv[1], len);

    //  CRC32 test:

    v32 = ~crc32_calc (msg, len, ~0U);
    printf("32 bit CRC: %08lx", v32);

    //  append the inverted the crc and recompute

    for( idx = 0; idx < sizeof(unsigned long); idx++ )
        msg[len + idx] = v32 >> idx * 8;

    v32 = crc32_calc (msg, len + sizeof(unsigned), ~0UL);

    //  this should yield the well known CRC32 residue value:
    //  0xdebb20e3

    printf("   RESIDUE: %08lx\n", v32);

    //  CRC16 test:

    v16 = ~crc16_calc (msg, len, ~0U);
    printf("16 bit CRC: %04lx    ", v16);

    for( idx = 0; idx < sizeof(unsigned short); idx++ )
        msg[len + idx] = v16 >> idx * 8;

    v16 = crc16_calc (msg, len + sizeof(unsigned short), 0xffff);

    //  this should yield the well known CRC16 residue value:
    //  0xf0b8

    printf("   RESIDUE: %04lx\n", v16);
}
#endif
