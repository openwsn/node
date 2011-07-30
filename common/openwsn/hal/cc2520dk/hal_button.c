/***********************************************************************************
    Filename:     hal_button.c

    Description:  HAL button control header file

    Copyright 2007 Texas Instruments, Inc.
***********************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////
// INCLUDES
//
#include "hal_board.h"
#include "hal_button.h"


/////////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//


uint8 halButtonPushed(void)
{
    uint8 v;

    v= HAL_BUTTON_NONE;
    if (HAL_BUTTON_1_PUSHED()) {
        HAL_DEBOUNCE(!HAL_BUTTON_1_PUSHED());
        v= HAL_BUTTON_1;
    }

    if (HAL_BUTTON_2_PUSHED()) {
        HAL_DEBOUNCE(!HAL_BUTTON_2_PUSHED());
        v= HAL_BUTTON_2;
    }

    return v;
}

