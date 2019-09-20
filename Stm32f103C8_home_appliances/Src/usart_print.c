#include "usart_print.h"

int ConvertFromInt(char* str, int a)
{
	int leng = sprintf(str, "%d", a);
	return leng;
}

int ConvertFromFloat(char* str, float a)
{
	memset(str, '\0', sizeof(str) * sizeof(char)); 
	int leng = sprintf(str, "%3.2f", a);
	str[leng] = '\0';
	return leng;
}

int ConvertFromIntForTime(char* str, int a)
{
	int leng = sprintf(str, "%02d", a);
	return leng;
}
//void U_Print_Char(USART_TypeDef * USARTx, char *s)
//{
//	while (*s)
//	{
//		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
//		USART_SendData(USARTx, *s);
//		USART_ClearFlag(USARTx, USART_FLAG_TXE);
//		*s++;
//	}
//}

//void U_Print_float(USART_TypeDef * USARTx, float a)
//{
//	char temp[20] = "";
//	int i;
//	int leng = sprintf(temp, "%3.2f", a);
//	for (i = 0; i < leng; i++)
//	{
//			while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
//			USART_SendData(USARTx,temp[i]);
//			USART_ClearFlag(USARTx, USART_FLAG_TXE);
//	}
//}
