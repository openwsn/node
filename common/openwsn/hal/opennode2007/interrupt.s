;******************************************************************************
; Functions: ARMDisableInterrupts, ARMEnableInterrupts
; Purpose:  Disable and enable IRQ and FIQ preserving current CPU mode.
; Processing:
;    Push the cpsr onto the stack
;    Disable IRQ and FIQ interrupts
;    Return
; Parameters: void
; Outputs:  None
; Returns:  void
; Notes:
;   (1) Can be called from SVC mode to protect Critical Sections.
;   (2) Do not use these calls at interrupt level,中断级的屏蔽由两条指令完成.
;   (3) Used in pairs within the same function level;
;   (4) Will restore interrupt state when called; i.e., if interrupts
;       are disabled when DisableInt is called, interrupts will still
;       still be disabled when the matching EnableInt is called.
;   (5) Uses the method described by Labrosse as "Method 2".
;   (6) 注意，编译优化时应选择minimum，否则该函数可能出错
;
; @modified by openwsn on 20070411
;	revision today.
;******************************************************************************

; @modified by zhangwei on 200903
; i remember the original source code is very simple. there's no AREA description. 
; however, i encountered (Fatal) A1355U: A label was found which was in no AREA
; so I add the following two lines and the END in the last line

    ;CODE32
    AREA    IRQ,CODE,READONLY
    
    EXPORT     ARMDisableInterrupts
ARMDisableInterrupts
    MRS    r0, cpsr
    STMFD    sp!, {r0}             	; push current PSR
    ORR    r0, r0, #0xc0
    MSR    cpsr_c, r0         		; disable IRQ Int s
    MOV    pc, lr   ;return


    EXPORT     ARMEnableInterupts
ARMEnableInterupts
    LDMFD    sp!, {r0}              ; pop current PSR
    MSR    cpsr_c, r0               ; restore original cpsr
    MOV    pc, lr ;return

    END