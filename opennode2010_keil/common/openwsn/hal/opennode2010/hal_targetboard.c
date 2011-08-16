
#include "../hal_configall.h"
#include "../hal_foundation.h"
#include "../hal_digitio.h"
#include "../hal_cpu.h"
#include "../hal_mcu.h"
#include "../hal_targetboard.h"
#include "../hal_uart.h"

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
    mcu_init();

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
    hal_disable_interrupts();
}

void target_reset()
{
    // @todo: save configurations
    cpu_reset();
}

