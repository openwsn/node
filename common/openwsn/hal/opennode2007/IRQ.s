;/****************************************Copyright (c)**************************************************
;**                               Guangzou ZLG-MCU Development Co.,LTD.
;**                                      graduate school
;**                                 http://www.zlgmcu.com
;**
;**--------------File Info-------------------------------------------------------------------------------
;** File Name: IRQ.s
;** Last modified Date:  2004-06-14
;** Last Version: 1.1
;** Descriptions: The irq handle that what allow the interrupt nesting. 
;**
;**------------------------------------------------------------------------------------------------------
;** Created By: Chenmingji
;** Created date:   2004-09-17
;** Version: 1.0
;** Descriptions: First version
;**
;**------------------------------------------------------------------------------------------------------
;** Modified by:
;** Modified date:
;** Version:
;** Descriptions:
;**
;********************************************************************************************************/


NoInt       EQU 0x80
 
USR32Mode   EQU 0x10
SVC32Mode   EQU 0x13
SYS32Mode   EQU 0x1f
IRQ32Mode   EQU 0x12
FIQ32Mode   EQU 0x11

    CODE32

    AREA    IRQ,CODE,READONLY

    MACRO
$IRQ_Label HANDLER $IRQ_Exception_Function

        EXPORT  $IRQ_Label                      ; The label for exports 输出的标号
        IMPORT  $IRQ_Exception_Function         ; The imported labels 引用的外部标号

$IRQ_Label
        SUB     LR, LR, #4                      ; Calculate the returning address 计算返回地址
        STMFD   SP!, {R0-R3, R12, LR}           ; Protects the task environments 保存任务环境
        MRS     R3, SPSR                        ; Protects the status variable 保存状态
        STMFD   SP, {R3,LR}^                    ; Protects SPSR and SP in user status, Notice: DO NOT write back.保存SPSR和用户状态的SP,注意不能回写
                                                ; If the SP is written back, it should be adjusted to its appropriate value later.如果回写的是用户的SP，所以后面要调整SP
        NOP
        SUB     SP, SP, #4*2

        MSR     CPSR_c, #(NoInt | SYS32Mode)    ; Switch to the System Mode 切换到系统模式 
       
        BL      $IRQ_Exception_Function         ; call the C interrupt handler funtion 调用c语言的中断处理程序

        MSR     CPSR_c, #(NoInt | IRQ32Mode)    ; Switch bak to IRQ mode 切换回irq模式
        LDMFD   SP, {R3,LR}^                    ; Recover SPSR and SP in user status, Notic: DO NOT write back. 恢复SPSR和用户状态的SP,注意不能回写
                                                ; If the SP is written back, it should be adjusted to its appropriate value later.如果回写的是用户的SP，所以后面要调整SP
        MSR     SPSR_cxsf, R3
        ADD     SP, SP, #4*2                    ; 

        LDMFD   SP!, {R0-R3, R12, PC}^          ;
    MEND

;/* 以下添加中断句柄，用户根据实际情况改变 */
;/* Add interrupt handler here，user could change it as needed */

;Timer0_Handler  HANDLER Timer0

    END
;/*********************************************************************************************************
;**                            End Of File
;********************************************************************************************************/
