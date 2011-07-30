
#ifndef _BOOT_H_8945
#define _BOOT_H_8945

/*******************************************************************************
 * @author zhangwei on 20061015
 * this is the system boot loader
 * 
 * his module SHOULD be an fully independent boot loader to boot the system on 
 * specific MCU. this module can be shared in most applications and even RTOS-based
 * application, because it will be effective before the RTOS started. usually,
 * it will initialize the MCU interrupt and UART0 to boot the application. after
 * booting, the application can decide whether to change these settings or not.
 * 
 * @modified by zhangwei on 20061015
 * first created
 * 
 ******************************************************************************/ 
 
 
// this module contains the source code on how to boot a ARM7-based system

extern int main(void);
void sysinit( void );

#endif