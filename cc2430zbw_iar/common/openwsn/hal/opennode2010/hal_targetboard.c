/** 
 * @history
 * - modified by zhangwei on 2011.09.10
 *   add rtl_init() call in target_init(). So the developer can enjoy some pre-initialized
 *   components in their own code segments. This means you needn't call rtl_init()
 *   in your own programs from now on.
 */
 
#include "../hal_configall.h"
#include "../hal_foundation.h"
#include "../hal_digitio.h"
#include "../hal_cpu.h"
#include "../hal_mcu.h"
#include "../hal_targetboard.h"
#include "../hal_uart.h"
#include "../hal_debugio.h"
#include "../hal_assert.h"
#include "../../rtl/rtl_debugio.h"

/**
 * @fn halBoardInit
 * @brief
 * Initialize the hardware target board. After this initialization, you can run your 
 * application or startup the operating system(OS).
 *
 * @param   none
 * @return  none
 */
void target_init(void)
{
    /* @todo I think you should place disable interrupts here. */

	hal_init( NULL, NULL );
    mcu_init();
    hal_disable_interrupts();
    
    rtl_init( (void*)dbio_open(9600), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, 
        hal_assert_report );

	/*
    MCU_IO_OUTPUT(HAL_BOARD_IO_LED_1_PORT, HAL_BOARD_IO_LED_1_PIN, 0);
    MCU_IO_OUTPUT(HAL_BOARD_IO_LED_2_PORT, HAL_BOARD_IO_LED_2_PIN, 0);
    MCU_IO_OUTPUT(HAL_BOARD_IO_LED_3_PORT, HAL_BOARD_IO_LED_3_PIN, 0);
    MCU_IO_OUTPUT(HAL_BOARD_IO_LED_4_PORT, HAL_BOARD_IO_LED_4_PIN, 0);

    MCU_IO_INPUT(HAL_BOARD_IO_BTN_1_PORT, HAL_BOARD_IO_BTN_1_PIN, MCU_IO_TRISTATE);
    MCU_IO_INPUT(HAL_BOARD_IO_BTN_2_PORT, HAL_BOARD_IO_BTN_2_PIN, MCU_IO_TRISTATE);

    halLcdSpiInit();
    halLcdInit();
    halAssyInit();
				  
	*/
}

void target_reset()
{
    // @todo: save configurations
    cpu_reset();
}

