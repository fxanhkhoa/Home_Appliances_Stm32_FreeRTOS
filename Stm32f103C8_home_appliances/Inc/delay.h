#ifndef DELAY_H
#define DELAY_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
	
#define DELAY_US_TIM	TIM2
#define DELAY_MS_TIM  TIM2
	 
void Delay_us (uint16_t nTime);
void Delay_ms (uint16_t nTime);

#ifdef __cplusplus
}
#endif
	 
#endif
