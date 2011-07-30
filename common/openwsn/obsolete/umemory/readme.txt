*************************************************************************************************

This module is very stable and efficient. 
It has been widely used in our Bluetooth stack / profiles / applications and related productions.

If you have any problem or new requirements by using this module, please
send e-mail to zhjwp@hotmail.com

Thanks!

**************************************************************************************************


1. 

fmem.c / fmem.h are used for 'static memory allocation'.
vmem.c / vmem.h are used for 'dynamic memory allocation'.

2. 

To use uMemory module, the only thing you need to do is porting it to your OS.
This release is using APIs of uCOS-II. For other OS, you only need to change file osdep.h.
Although there are many primitives are implemented in this file, only the following ones are used actually:
'DISABLE_INTERRUPT', 'ENABLE_INTERRUPT', 'CRITICAL_CREATE', 'CRITICAL_FREE', 'CRITICAL_ENTER', 'CRITICAL_LEAVE'.


Note:
'DISABLE_INTERRUPT' and  'ENABLE_INTERRUPT' are only used in 'static memory allocation' to enable memory allocation in interrupt ISR context.
If 'static memory allocation' is not used in your application, you don't need to porting them.


3. 

Macro 'CFG_VMEM_BUF_SIZE' defined in file 'vmem.h' defines the total size of RAM space can be used by the memory management module.
Modifying it according to your envirement.


4. 

For the 'dynamic memory allocation', just using 'NEW(size)' to allocate memory and 'FREE' to deallocate memory.