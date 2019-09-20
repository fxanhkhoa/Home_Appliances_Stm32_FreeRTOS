#include "delay.h"

void Delay_us (uint16_t nTime)
{
  DELAY_US_TIM->CNT = 0;

  while (DELAY_US_TIM->CNT < nTime);
}

void Delay_ms (uint16_t nTime)
{
  DELAY_MS_TIM->CNT = 0;

  while (DELAY_MS_TIM->CNT < nTime);
}
