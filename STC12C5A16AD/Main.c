#include "Main.h"

//全局变量定义
bit F_Run, P_Run, F_Flag, T_Flag, XD_Run;
uchar temp1;
uchar P_RomFlag;
uchar F_Step, P_Step;
uchar idata R_Data[24], R_Num, R_CL;
uchar P_Rom[19];
unsigned long idata WaitTime;
uchar M_D;
uchar F_Motor[16];
uchar DW_Flag[16];
uchar idata DW_XZ[33];
uchar DW[17];
uchar T_ROM_D0;
uchar T_ROM_D1;
uchar T_ROM_D2;
uchar F_ROM_Set;
uchar F_ROM_Temp;
uint T_Step;
uint T_Step_Temp;            
uchar T_YZ;
uchar T_Num;
uint YK_Delay;
uint HW_Delay;
uint XD_Delay;
//////////////////////////////////////////////////////

void HardwareInit() //硬件初始
{
	TIM_Init();
	UartInit();
}

void DataInit() //数据初始化
{
	P_Run = 1;
	P_Step = 1;
	MotorInit();
}

void main()
{
	HardwareInit();
	DataInit();
	while (1)
	{
		MainCycle();
	}
}

void T0_Isr() interrupt 1 //时钟
{
	TH0 = T0Time >> 8;
	TL0 = T0Time;
	TF0 = 0;
	temp1++;
	if (HW_Delay > 0)
		HW_Delay--;
	if (YK_Delay > 0)
		YK_Delay--;
	XD_Run ? (XD_Delay++) : (XD_Delay = 0);
	if (F_Run == 1 && P_Run == 0 && WaitTime != 0)
	{
		WaitTime--;
	}
}