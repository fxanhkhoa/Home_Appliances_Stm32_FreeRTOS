#ifndef USART_PRINT_H
#define USART_PRINT_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
	 
int ConvertFromInt(char* str, int a);
int ConvertFromFloat(char* str, float a);
int ConvertFromIntForTime(char* str, int a);
//void U_Print(USART_TypeDef * USARTx, int a);
//void U_Print_Char(USART_TypeDef * USARTx, char *s);
//void U_Print_float(USART_TypeDef * USARTx, float a);
//void U_Print_Raw_Int(UART_HandleTypeDef *huart, uint8_t *a);

#ifdef __cplusplus
}
#endif
	 
#endif
