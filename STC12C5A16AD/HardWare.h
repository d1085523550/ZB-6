#ifndef __HARDWARE_H__
#define __HARDWARE_H__
#include "Main.h"

void TIM_Init();
void UartInit();
void UART_MN(uchar *dat, uchar n);
void UART_Send(uchar *dat, uchar n);
void Test_Send(uchar i);
void KeyTrigger();
#endif