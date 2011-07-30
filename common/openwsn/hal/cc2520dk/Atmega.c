#include "hal_led.h"
#include "util_delay.h"
#include "util_twinkle.h"
#include "hal_timer_32k.h"
#include "Atmega.h"

char Ledno;
void led_off(Ledno)
{
  if(Ledno=="LED_RED")halLedClear(1);
  elseif(Ledno=="LED_YELLOW")halLedClear(2);
  elseif(Ledno=="LED_GREEN")halLedClear(3);
  end if
}

void led_on(Ledno)
{ 
  switch (Ledno)
  {
  case "LED_RED":halLedSet(1);
  case "LED_YELLOW":halLedSet(2);
  case "LED_GREEN":halLedSet(3);
  case "LED_ALL":{halLedSet(1);halLedSet(2);halLedSet(3);}
  }
}

int delay;
void hal_delay(delay)
{
  util_delay(delay);
}

int count;
void led_twinkle(Ledno,delay)
{
  switch (Ledno)
  {
  case "LED_RED":utiltwinkle(1,delay,10);
  case "LED_YELLOW":utiltwinkle(2,delay,10);
  case "LED_GREEN":utiltwinkle(3,delay,10);
  }
}

void led_toggle(char)
{
  switch (Ledno)
  {
  case "LED_RED":halLedToggle(1);
  case "LED_YELLOW":halLedToggle(2);
  case "LED_GREEN":halLedToggle(3);
  }
}

void led_init(void)
{
  halLedInit(void);
}