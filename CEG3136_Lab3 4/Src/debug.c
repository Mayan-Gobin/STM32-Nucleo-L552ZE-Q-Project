#include "stm32l5xx.h"

//printf() support via ITM/SWV
int __io_putchar (int c){
	ITM_SendChar(c);
	return c;
}
