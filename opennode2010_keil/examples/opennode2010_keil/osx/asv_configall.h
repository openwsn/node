#ifndef _ASV_CONFIGALL_4674_
#define _ASV_CONFIGALL_4674_
/******************************************************************************* 
 * asv_configall.h
 * This is the configuration file for the whole application. It should always be
 * the first file to be included in all other source files and header files.
 * Thus only the configuration macros can be placed in this file.
 * 
 * @author zhangwei(TongJi Univeristy) on 20091106
 *	- first created
 ******************************************************************************/

/* The macro set the application into the debug mode. In the debugging mode, you can
 * check additional output through the RS232 communication port. 
 * 
 * In debugging mode, the assertions(hal_assert, rtl_assert, or others) will also 
 * take effective.
 */
#define CONFIG_DEBUG

#endif

