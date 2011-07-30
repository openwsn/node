 
/* *************************************************************************************************** */ 
/*               crt.s                                                            */ 
/*                                                                                                                        */ 
/*                       Assembly Language Startup Code for Atmel AT91SAM7S256                                */ 
/*                                                                                                                        */ 
/*                                                                                                                 */ 
/*                                                                                                                        */ 
/*                                                                                                                       */ 
/* Author:  James P Lynch      May 12, 2007                                                                         */ 
/* *************************************************************************************************** */ 
 
/* Stack Sizes */ 
.set  UND_STACK_SIZE, 0x00000010    /* stack for "undefined instruction" interrupts is 16 bytes    */ 
.set  ABT_STACK_SIZE, 0x00000010    /* stack for "abort" interrupts is 16 bytes  */ 
.set  FIQ_STACK_SIZE, 0x00000080      /* stack for "FIQ" interrupts  is 128 bytes  */ 
.set  IRQ_STACK_SIZE, 0X00000080    /* stack for "IRQ" normal interrupts is 128 bytes  */ 
.set  SVC_STACK_SIZE, 0x00000080    /* stack for "SVC" supervisor mode is 128 bytes */ 
 
/* Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs (program status registers)  */ 
.set  ARM_MODE_USR, 0x10                /* Normal User Mode  */ 
.set  ARM_MODE_FIQ, 0x11                 /* FIQ Processing Fast Interrupts Mode  */ 
.set  ARM_MODE_IRQ, 0x12                  /* IRQ Processing Standard Interrupts Mode  */ 
.set  ARM_MODE_SVC, 0x13                /* Supervisor Processing Software Interrupts Mode */ 
.set  ARM_MODE_ABT, 0x17                  /* Abort Processing memory Faults Mode */ 
.set  ARM_MODE_UND, 0x1B                /* Undefined Processing Undefined Instructions Mode */ 
.set  ARM_MODE_SYS, 0x1F                  /* System Running Priviledged Operating System Tasks  Mode */ 
.set  I_BIT, 0x80                         /* when I bit is set, IRQ is disabled (program status registers) */ 
.set  F_BIT, 0x40                        /* when F bit is set, FIQ is disabled (program status registers) */ 
 
/* Addresses and offsets of AIC and PIO  */ 
.set  AT91C_BASE_AIC, 0xFFFFF000     /* (AIC) Base Address  */ 
.set  AT91C_PIOA_CODR, 0xFFFFF434    /* (PIO) Clear Output Data Register  */ 
.set  AT91C_AIC_IVR, 0xFFFFF100      /* (AIC) IRQ Interrupt Vector Register */ 
.set  AT91C_AIC_FVR, 0xFFFFF104      /* (AIC) FIQ Interrupt Vector Register */ 
.set  AIC_IVR, 256            /* IRQ Vector Register offset from base above */ 
.set  AIC_FVR, 260            /* FIQ Vector Register offset from base above */ 
.set  AIC_EOICR, 304       /* End of Interrupt Command Register  */ 
 
  
 
 
 
 
 请参考原文档获得代码
 从pdf拷出来全乱了
 
 
 
 
 
 
 
 
 
/* identify all GLOBAL symbols  */ 
global _vec_reset      
global _vec_undef      
global _vec_swi       
global _vec_pabt       
global _vec_dabt       
global _vec_rsv       
global _vec_irq       
vec_fiq 
T91F_Irq_Handler 
 AT91F_Fiq_Handler 
 AT91F_Default_FIQ_handler 
 AT91F_Default_IRQ_handler 
global AT91F_Spurious_handler 
 AT91F_Dabt_Handler 
global AT91F_Pabt_Handler 
global AT91F_Undef_Handler 
.
.
.
.
.
.
.
.global _
.global A
.global
.global
.global
.
.global
.
.
 
  
    93 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 