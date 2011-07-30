//----------------------------------------------------//
//-------Institute Of  Computing Technology-----------//
//------------Chinese Academic  Science---------------//
//-----÷–π˙ø∆—ß‘∫º∆À„ºº ı—–æøÀ˘œ»Ω¯≤‚ ‘ºº ı µ—È “-----//
//----------------------------------------------------//

/**
 * www.wsn.net.cn
 * @copyright:nbicc_lpp
 * @data:2005.11.22
 * @version:0.0.1
 * @updata:$
 *
 */
#include "led.h"

uint8_t LedsC_ledsOn;



//-----------------------------------------------------œ‘ æled«˝∂
uint8_t Leds_init(void)

{
  { hal_atomic_t _nesc_atomic = hal_atomic_start();

    {
      LedsC_ledsOn = 0;
      HAL_MAKE_RED_LED_OUTPUT();
      HAL_MAKE_YELLOW_LED_OUTPUT();
      HAL_MAKE_GREEN_LED_OUTPUT();
      HAL_SET_RED_LED_PIN();
      HAL_SET_YELLOW_LED_PIN();
      HAL_SET_GREEN_LED_PIN();
    }

    hal_atomic_end(_nesc_atomic); }
  return SUCCESS;
}
uint8_t Leds_yellowOn(void)
{
  
  { hal_atomic_t _nesc_atomic = hal_atomic_start();

    {
      HAL_CLR_YELLOW_LED_PIN();
      LedsC_ledsOn |= LedsC_YELLOW_BIT;
    }
    hal_atomic_end(_nesc_atomic); }
  return SUCCESS;
}
uint8_t Leds_yellowOff(void)

{
    { hal_atomic_t _nesc_atomic = hal_atomic_start();
    {
      HAL_SET_YELLOW_LED_PIN();
      LedsC_ledsOn &= ~LedsC_YELLOW_BIT;
    }
    hal_atomic_end(_nesc_atomic); }
  return SUCCESS;
}

uint8_t Leds_yellowToggle(void)
{
  uint8_t rval;
  { hal_atomic_t _nesc_atomic = hal_atomic_start();
    {
      if (LedsC_ledsOn & LedsC_YELLOW_BIT) {
        rval = Leds_yellowOff();
        }
      else {
        rval = Leds_yellowOn();
        }
    }
    hal_atomic_end(_nesc_atomic); }
  return rval;
}

uint8_t Leds_greenOn(void)

{
    { hal_atomic_t _nesc_atomic = hal_atomic_start();
    {
      HAL_CLR_GREEN_LED_PIN();
      LedsC_ledsOn |= LedsC_GREEN_BIT;
    }
    hal_atomic_end(_nesc_atomic); }
  return SUCCESS;
}

uint8_t Leds_greenOff(void)
{
    { hal_atomic_t _nesc_atomic = hal_atomic_start();
    {
      HAL_SET_GREEN_LED_PIN();
      LedsC_ledsOn &= ~LedsC_GREEN_BIT;
    }
    hal_atomic_end(_nesc_atomic); }
  return SUCCESS;
}
uint8_t Leds_greenToggle(void)

{
  uint8_t rval;
  { hal_atomic_t _nesc_atomic = hal_atomic_start();
    {
      if (LedsC_ledsOn & LedsC_GREEN_BIT) {
        rval = Leds_greenOff();
        }
      else {
        rval = Leds_greenOn();
        }
    }
    hal_atomic_end(_nesc_atomic); }
  return rval;
}

uint8_t Leds_redOn(void)
{
   { hal_atomic_t _nesc_atomic = hal_atomic_start();
    {
      HAL_CLR_RED_LED_PIN();
      LedsC_ledsOn |= LedsC_RED_BIT;
    }
    hal_atomic_end(_nesc_atomic); }
  return SUCCESS;
}

uint8_t Leds_redOff(void)
{
   { hal_atomic_t _nesc_atomic = hal_atomic_start();
    {
      HAL_SET_RED_LED_PIN();
      LedsC_ledsOn &= ~LedsC_RED_BIT;
    }
    hal_atomic_end(_nesc_atomic); }
  return SUCCESS;
}

uint8_t Leds_redToggle(void)
{
  uint8_t rval;
  { hal_atomic_t _nesc_atomic = hal_atomic_start();
    {
      if (LedsC_ledsOn & LedsC_RED_BIT) {
        rval = Leds_redOff();
        }
      else {
        rval = Leds_redOn();
        }
    }
    hal_atomic_end(_nesc_atomic); }
  return rval;
}

