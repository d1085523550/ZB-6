#include "STC89C5xRC.H"
#include "EEPROM_ISP.H"
#define uchar unsigned char
#define uint unsigned int

//默认为 6倍速
#define FOSC 11059200L	//晶振频率
#define BAUD 57600		//波特率
sbit MN_RTX = P3 ^ 5;	//模拟串口 发送口
sbit HW = P3 ^ 3;		//触发口
sbit YK = P3 ^ 2;		//遥控触发口
sbit Power12V = P3 ^ 4; //12V电源 低有效时开通
sbit XD1 = P3 ^ 6;		//程序选段口1
sbit XD2 = P3 ^ 7;		//程序选段口2

/////////////////////////////IO脚位/////////////////////////////
sbit Motor1 = P0 ^ 0; //电机
sbit Motor2 = P0 ^ 1;
sbit Motor3 = P0 ^ 2;
sbit Motor4 = P0 ^ 3;
sbit Motor5 = P0 ^ 4;
sbit Motor6 = P0 ^ 5;
sbit Motor7 = P0 ^ 6;
sbit Motor8 = P0 ^ 7;
sbit M_D1 = P2 ^ 7; //电机或者定位 可选
sbit M_D2 = P2 ^ 6;
sbit M_D3 = P2 ^ 5;
sbit M_D4 = P2 ^ 4;
sbit M_D5 = P2 ^ 3;
sbit M_D6 = P2 ^ 2;
sbit M_D7 = P2 ^ 1;
sbit M_D8 = P2 ^ 0;
sbit DW1 = P1 ^ 0; //定位
sbit DW2 = P1 ^ 1;
sbit DW3 = P1 ^ 2;
sbit DW4 = P1 ^ 3;
sbit DW5 = P1 ^ 4;
sbit DW6 = P1 ^ 5;
sbit DW7 = P1 ^ 6;
sbit DW8 = P1 ^ 7;

bit F_Run, P_Run, T_Flag, P_RomFlag, T_BD, T_SendDataFlag, T_YZ;
uchar F_Motor[16]; //电机状态
uchar DW_Flag[16]; //定位状态
uchar DW[17];
uchar M_D;
uchar temp1, temp2;
uchar F_Set, P_Set, F_CXD;
uchar P_Rom[18], T_ROM_num, T_ROM_D0, T_ROM_D1, T_ROM_D2;
uchar idata DW_XZ[33];
uchar idata R_Data[24], R_Num, R_CL;
uint WaitRes, T_YZ_num;
uint idata WaitTime;

void Delay()
{
	uchar i = 5, j;
	while (i--)
	{
		j = 50;
		while (j--)
			;
	}
}
void Delay_MN() //模拟串口发送间隔时间 9600 104us
{
	uchar i;
	i = 90;
	while (--i)
		;
}

void UART_MN(uchar *dat, uchar n) //模拟串口发送数据
{
	uchar i;
	for (i = 0; i < n; i++)
	{
		bit UartData1, UartData2, UartData3, UartData4, UartData5, UartData6, UartData7, UartData8;
		EA = 0;
		UartData1 = dat[i] & 0x01;
		UartData2 = dat[i] & 0x02;
		UartData3 = dat[i] & 0x04;
		UartData4 = dat[i] & 0x08;
		UartData5 = dat[i] & 0x10;
		UartData6 = dat[i] & 0x20;
		UartData7 = dat[i] & 0x40;
		UartData8 = dat[i] & 0x80;
		MN_RTX = 0;
		Delay_MN();
		MN_RTX = UartData1;
		Delay_MN();
		MN_RTX = UartData2;
		Delay_MN();
		MN_RTX = UartData3;
		Delay_MN();
		MN_RTX = UartData4;
		Delay_MN();
		MN_RTX = UartData5;
		Delay_MN();
		MN_RTX = UartData6;
		Delay_MN();
		MN_RTX = UartData7;
		Delay_MN();
		MN_RTX = UartData8;
		Delay_MN();
		MN_RTX = 1;
		Delay_MN();
		EA = 1;
	}
}

void UART_Send(uchar *dat, uchar n) //串口发送
{
	uchar i;
	for (i = 0; i < n; i++)
	{
		SBUF = dat[i];
		TI = 0;
		while (!TI)
			;
	}
}

void Music(uchar num) //播放音乐
{
	uchar idata Music_Selection[10] = {0x7e, 0xff, 0x06, 0x12, 0x00, 0x00, 0x01, 0xfe, 0xf7, 0xef}; //选择播发 MP3文件夹
	Music_Selection[6] = num;
	Music_Selection[7] = ((0xffff - (Music_Selection[1] + Music_Selection[2] + Music_Selection[3] + Music_Selection[4] + Music_Selection[5] + Music_Selection[6]) + 1) >> 8);
	Music_Selection[8] = ((0xffff - (Music_Selection[1] + Music_Selection[2] + Music_Selection[3] + Music_Selection[4] + Music_Selection[5] + Music_Selection[6]) + 1) & 0xff);
	UART_MN(Music_Selection, 10);
}

void MusicVol()
{
	uchar code Music_Vol[10] = {0x7e, 0xff, 0x06, 0x06, 0x00, 0x00, 0x1e, 0xfe, 0xf7, 0xef}; //音量调节
	UART_MN(Music_Vol, 10);
}

uchar *M_ROM(uchar n)
{
	uchar idata M_Rom[19], num1, num2;
	if (P_RomFlag)
	{
		for (num1 = 0; num1 < 19; num1++)
		{
			M_Rom[num1] = P_Rom[num1];
		}
	}
	else
	{
		switch (F_CXD)
		{
		case 1:
			for (num1 = 0, num2 = 19; num1 < 19; num1++, num2--)
			{
				M_Rom[num1] = IspReaduchar(19 * n - num2 + 0x2200);
			}
			break;
		case 2:
			for (num1 = 0, num2 = 19; num1 < 19; num1++, num2--)
			{
				M_Rom[num1] = IspReaduchar(19 * n - num2 + 0x2600);
			}
			break;
		case 3:
			for (num1 = 0, num2 = 19; num1 < 19; num1++, num2--)
			{
				M_Rom[num1] = IspReaduchar(19 * n - num2 + 0x2b00);
			}
			break;
		}
	}
	return M_Rom;
}

void DATA_Int() //需要初始的值
{
	M_D1 = 0;
	M_D2 = 0;
	M_D3 = 0;
	M_D4 = 0;
	M_D5 = 0;
	M_D6 = 0;
	M_D7 = 0;
	M_D8 = 0;
	Power12V = 0;
	P_Run = 1;
	F_Run = 0;
	P_Set = 1;
	R_CL = 0;
	T_Flag = 0;
	P_RomFlag = 0;
	T_BD = 0;
	T_YZ = 0;
	T_YZ_num = 1;
	T_SendDataFlag = 0;
	R_Num = 0;
	M_D = IspReaduchar(0x2001);
	(M_D & 0x80 / 0x80) ? (M_D1 = 0) : (M_D1 = 1);
	(M_D & 0x40 / 0x40) ? (M_D2 = 0) : (M_D2 = 1);
	(M_D & 0x20 / 0x20) ? (M_D3 = 0) : (M_D3 = 1);
	(M_D & 0x10 / 0x10) ? (M_D4 = 0) : (M_D4 = 1);
	(M_D & 0x08 / 0x08) ? (M_D5 = 0) : (M_D5 = 1);
	(M_D & 0x04 / 0x04) ? (M_D6 = 0) : (M_D6 = 1);
	(M_D & 0x02 / 0x02) ? (M_D7 = 0) : (M_D7 = 1);
	(M_D & 0x01 / 0x01) ? (M_D8 = 0) : (M_D8 = 1);
	DW_XZ[0] = IspReaduchar(0x2010);
	DW_XZ[1] = IspReaduchar(0x2020);
	DW_XZ[2] = IspReaduchar(0x2011);
	DW_XZ[3] = IspReaduchar(0x2021);
	DW_XZ[4] = IspReaduchar(0x2012);
	DW_XZ[5] = IspReaduchar(0x2022);
	DW_XZ[6] = IspReaduchar(0x2013);
	DW_XZ[7] = IspReaduchar(0x2023);
	DW_XZ[8] = IspReaduchar(0x2014);
	DW_XZ[9] = IspReaduchar(0x2024);
	DW_XZ[10] = IspReaduchar(0x2015);
	DW_XZ[11] = IspReaduchar(0x2025);
	DW_XZ[12] = IspReaduchar(0x2016);
	DW_XZ[13] = IspReaduchar(0x2026);
	DW_XZ[14] = IspReaduchar(0x2017);
	DW_XZ[15] = IspReaduchar(0x2027);
	DW_XZ[16] = IspReaduchar(0x2018);
	DW_XZ[17] = IspReaduchar(0x2028);
	DW_XZ[18] = IspReaduchar(0x2019);
	DW_XZ[19] = IspReaduchar(0x2029);
	DW_XZ[20] = IspReaduchar(0x201a);
	DW_XZ[21] = IspReaduchar(0x202a);
	DW_XZ[22] = IspReaduchar(0x201b);
	DW_XZ[23] = IspReaduchar(0x202b);
	DW_XZ[24] = IspReaduchar(0x201c);
	DW_XZ[25] = IspReaduchar(0x202c);
	DW_XZ[26] = IspReaduchar(0x201d);
	DW_XZ[27] = IspReaduchar(0x202d);
	DW_XZ[28] = IspReaduchar(0x201e);
	DW_XZ[29] = IspReaduchar(0x202e);
	DW_XZ[30] = IspReaduchar(0x201f);
	DW_XZ[31] = IspReaduchar(0x202f);
}

void Send_MotorDW_Flag() //返回电机及定位的状态
{
	uchar i[7];
	i[0] = 0xfd;
	i[1] = 0x05;
	i[2] = 0x70;
	i[3] = (Motor1 * 128) | (Motor2 * 64) | (Motor3 * 32) | (Motor4 * 16) | (Motor5 * 8) | (Motor6 * 4) | (Motor7 * 2) | (Motor8 * 1);
	i[4] = (DW1 * 0x80) | (DW2 * 0x40) | (DW3 * 0x20) | (DW4 * 0x10) | (DW5 * 0x08) | (DW6 * 0x04) | (DW7 * 0x02) | (DW8 * 0x01);
	i[5] = (M_D1 * 0x80) | (M_D2 * 0x40) | (M_D3 * 0x20) | (M_D4 * 0x10) | (M_D5 * 0x08) | (M_D6 * 0x04) | (M_D7 * 0x02) | (M_D8 * 0x01);
	i[6] = i[0] + i[1] + i[2] + i[3] + i[4] + i[5];
	UART_Send(i, 7);
}

void Send_Data(uchar n) //数据上传
{
	uchar idata i[24];
	if (n == 1)
	{
		i[0] = 0xfd;
		i[1] = 3;
		i[2] = 0x50;
		i[3] = IspReaduchar(0x2001);
		i[4] = i[0] + i[1] + i[2] + i[3];
		UART_Send(i, 5);
		T_YZ = 0;
	}
	else if (n == 2)
	{
		i[0] = 0xfd;
		i[1] = 3;
		i[2] = 0x64;
		T_ROM_D0 = IspReaduchar(0x2030);
		if (T_ROM_D0 == 0 || T_ROM_D0 > 68)
		{
			T_ROM_D0 = 1;
			i[3] = 1;
		}
		else
		{
			i[3] = T_ROM_D0;
		}
		T_ROM_D1 = IspReaduchar(0x2031);
		if (T_ROM_D1 == 0 || T_ROM_D1 > 68)
		{
			T_ROM_D1 = 1;
			i[4] = 1;
		}
		else
		{
			i[4] = T_ROM_D1;
		}
		T_ROM_D2 = IspReaduchar(0x2032);
		if (T_ROM_D2 == 0 || T_ROM_D2 > 68)
		{
			T_ROM_D2 = 1;
			i[5] = 1;
		}
		else
		{
			i[5] = T_ROM_D2;
		}
		i[6] = i[0] + i[1] + i[2] + i[3] + i[4] + i[5];
		UART_Send(i, 7);
		T_YZ = 0;
	}
	else if (n < 19)
	{
		i[0] = 0xfd;
		i[1] = 0x04;
		i[2] = 0x51 + n - 3;
		i[3] = IspReaduchar(0x2010 + n - 3);
		i[4] = IspReaduchar(0x2020 + n - 3);
		i[5] = i[0] + i[1] + i[2] + i[3] + i[4];
		UART_Send(i, 6);
		Delay();
		T_YZ = 0;
	}
	else if (n < 19 + T_ROM_D0)
	{
		uchar l, m;
		i[0] = 0xfd;
		i[1] = 22;
		i[2] = 0x61;
		i[3] = n - 18;
		for (l = 0, m = 19; l < 19; l++, m--)
		{
			i[l + 4] = IspReaduchar(i[3] * 19 - m + 0x2100);
		}
		if (i[4] > 4)
		{
			for (l = 0; l < 19; l++)
			{
				i[l + 4] = 0;
			}
		}
		i[23] = i[0] + i[1] + i[2] + i[3] + i[4] + i[5] + i[6] + i[7] + i[8] + i[9] + i[10] + i[11] + i[12] + i[13] + i[14] + i[15] + i[16] + i[17] + i[18] + i[19] + i[20] + i[21] + i[22];
		UART_Send(i, 24);
		T_YZ = 0;
	}
	else if (n < 19 + T_ROM_D0 + T_ROM_D1)
	{
		uchar l, m;
		i[0] = 0xfd;
		i[1] = 22;
		i[2] = 0x61;
		i[3] = n - 18 - T_ROM_D0;
		for (l = 0, m = 19; l < 19; l++, m--)
		{
			i[l + 4] = IspReaduchar(i[3] * 19 - m + 0x2100);
		}
		if (i[4] > 4)
		{
			for (l = 0; l < 19; l++)
			{
				i[l + 4] = 0;
			}
		}
		i[23] = i[0] + i[1] + i[2] + i[3] + i[4] + i[5] + i[6] + i[7] + i[8] + i[9] + i[10] + i[11] + i[12] + i[13] + i[14] + i[15] + i[16] + i[17] + i[18] + i[19] + i[20] + i[21] + i[22];
		UART_Send(i, 24);
		T_YZ = 0;
	}
	else if (n < 19 + T_ROM_D0 + T_ROM_D1 + T_ROM_D2)
	{
		uchar l, m;
		i[0] = 0xfd;
		i[1] = 22;
		i[2] = 0x61;
		i[3] = n - 18 - T_ROM_D0 - T_ROM_D1;
		for (l = 0, m = 19; l < 19; l++, m--)
		{
			i[l + 4] = IspReaduchar(i[3] * 19 - m + 0x2100);
		}
		if (i[4] > 4)
		{
			for (l = 0; l < 19; l++)
			{
				i[l + 4] = 0;
			}
		}
		i[23] = i[0] + i[1] + i[2] + i[3] + i[4] + i[5] + i[6] + i[7] + i[8] + i[9] + i[10] + i[11] + i[12] + i[13] + i[14] + i[15] + i[16] + i[17] + i[18] + i[19] + i[20] + i[21] + i[22];
		UART_Send(i, 24);
		T_YZ = 0;
	}
	else if (n == 19 + T_ROM_D0 + T_ROM_D1 + T_ROM_D2)
	{
		i[0] = 0xfd;
		i[1] = 3;
		i[2] = 0x65;
		i[3] = 0x10;
		i[4] = 0x75;
		UART_Send(i, 5);
		T_YZ = 0;
		T_YZ_num = 1;
		T_SendDataFlag = 0;
		TR0 = 1;
		TR1 = 1;
	}
}

void DATA_Que(uchar *i) //接收到有效数据，分类并处理
{
	uchar idata j[5] = {0xfd, 0x03, 0x90, 0x10, 0xa0};
	if (i[0] == 0x01) //设置参数
	{
		IspEraseSector(0x2000); //扇区擦除
		IspEraseSector(0x2200);
		IspEraseSector(0x2400);
		IspEraseSector(0x2600);
		IspEraseSector(0x2800);
		IspEraseSector(0x2a00);
		IspEraseSector(0x2c00);
		IspEraseSector(0x2e00);
		IspProgramuchar(0x2000, i[1]); //电机数
		IspProgramuchar(0x2001, i[2]); //扩展口电机/定位选择
		UART_Send(j, 5);
	}
	else if (i[0] >= 0x10 && i[0] <= 0x1f) //设置定位参数
	{
		i[0] = i[0] - 0x10;
		IspProgramuchar(0x2010 + i[0], i[1]); //定位1 端口选择 2010-201F
		IspProgramuchar(0x2020 + i[0], i[2]); //定位2 端口选择 2020-202F
		UART_Send(j, 5);
	}
	else if (i[0] == 0x20) //程序段总数
	{
		IspProgramuchar(0x2030, i[1]); //程序0段总段数
		IspProgramuchar(0x2031, i[2]); //程序1段总段数
		IspProgramuchar(0x2032, i[3]); //程序2段总段数
		UART_Send(j, 5);
	}
	else if (i[0] == 0x21) //程序0段写入
	{
		uchar k, l;
		for (k = 0, l = 19; k < 19; k++, l--)
		{
			IspProgramuchar((i[1] + 1) * 19 - l + 0x2100, i[k + 2]); //循环写入数据 2100-25F9里
		}
		UART_Send(j, 5);
	}
	else if (i[0] == 0x22) //程序1段写入
	{
		uchar k, l;
		for (k = 0, l = 19; k < 19; k++, l--)
		{
			IspProgramuchar((i[1] + 1) * 19 - l + 0x2600, i[k + 2]); //循环写入数据 2600-2AF9里
		}
		UART_Send(j, 5);
	}
	else if (i[0] == 0x23) //程序2段写入
	{
		uchar k, l;
		for (k = 0, l = 19; k < 19; k++, l--)
		{
			IspProgramuchar((i[1] + 1) * 19 - l + 0x2b00, i[k + 2]); //循环写入数据 2b00-2FF9里
		}
		UART_Send(j, 5);
	}
	else if (i[0] == 0x30) //电机定位状态返回
	{
		if (i[1] == 1)
		{
			T_Flag = 1;
		}
		else
		{
			T_Flag = 0;
		}
	}
	else if (i[0] == 0x31) //强制停止
	{
		if (i[1] == 1)
		{
			ISP_CONTR = 0X20;
		}
	}
	else if (i[0] == 0x32 || i[0] == 0x35) //单段程序启动
	{
		uchar j;
		for (j = 0; j < 18; j++)
		{
			P_Rom[j] = i[j + 1];
		}
		P_Run = 1;
		F_Run = 1;
		F_Set = 1;
		P_Set = 1;
		P_RomFlag = 1;
	}
	else if (i[0] == 0x33) //程序位置返回
	{
		if (i[1] == 1)
		{
			T_BD = 1;
		}
		else
		{
			T_BD = 0;
		}
	}
	else if (i[0] == 0x65) //下载完成 并重启
	{
		if (i[1] == 0x10)
		{
			ISP_CONTR = 0X20; //重启单片机
		}
	}
	else if (i[0] == 0xa0) //上传标志
	{
		if (i[1] == 1 && T_SendDataFlag == 0)
		{
			T_YZ = 1;
			T_SendDataFlag = 1;
			TR0 = 0;
			TR1 = 0;
		}
	}
	else if (i[0] == 0x90) //数据是否正确
	{
		if (i[1] == 0x10 && T_SendDataFlag == 1)
		{
			T_YZ = 1;
			T_YZ_num++;
		}
		else if (i[1] == 0xff && T_SendDataFlag == 1)
		{
			T_YZ = 1;
		}
	}
}

void DATA_CL() //接收到的数据进行效验处理
{
	uchar idata SUM = 0xFD, num, i[21];
	while (R_CL)
	{
		for (num = 1; num < R_Data[1] + 1; num++)
		{
			SUM = SUM + R_Data[num];
		}
		if (SUM == R_Data[num])
		{
			for (num = 0; num < R_Data[1] - 1; num++)
			{
				i[num] = R_Data[num + 2];
			}
			DATA_Que(i);
		}
		else
		{
			i[0] = 0xfd;
			i[1] = 0x03;
			i[2] = 0x90;
			i[3] = 0xff;
			i[4] = 0xa0;
			UART_Send(i, 5);
		}
		R_CL--;
	}
}

void UART_Int()
{
	SCON = 0x50;
	TH2 = RCAP2H = (65536 - (FOSC / 16 / BAUD)) / 256;
	TL2 = RCAP2L = (65536 - (FOSC / 16 / BAUD)) % 256;
	T2CON = 0x34; //T2 Run
	ES = 1;
}

void TIM_Int()
{
	TMOD = 0x11; //T1,T2为16位模式 10MS
	TH0 = (65536 - FOSC / 6 / 100) / 256;
	TL0 = (65536 - FOSC / 6 / 100) % 256;
	TH1 = (65536 - FOSC / 6 / 100) / 256;
	TL1 = (65536 - FOSC / 6 / 100) % 256;
	TF0 = 0;
	TF1 = 0;
	ET0 = 1;
	ET1 = 1;
	TR0 = 1;
	TR1 = 1;
	EA = 1;
}

void main()
{
	WDT_CONTR=0x37;
	TIM_Int();
	UART_Int();
	MusicVol();
	DATA_Int();
	while (1)
	{
		if ((XD1 == 1) || (XD2 == 1))
		{
			if ((XD1 == 0) && (XD2 == 1))
			{
				F_CXD = 2;
				F_Set = IspReaduchar(0x2031);
			}
			else if ((XD1 == 1) && (XD2 == 0))
			{
				F_CXD = 3;
				F_Set = IspReaduchar(0x2032);
			}
			else
			{
				F_CXD = 1;
				F_Set = IspReaduchar(0x2030);
			}
		}
		else
		{
			F_CXD = 1;
			F_Set = IspReaduchar(0x2030);
		}
		if (YK == 0)
		{
			WaitTime = 0;
			F_Run=1;
		}
		if (((HW == 0) || (YK == 0)) && (F_Run == 0))
		{
			F_Run = 1;
			P_RomFlag = 0;
		}
		if (T_SendDataFlag == 1 && T_YZ == 1)
		{
			Send_Data(T_YZ_num);
		}
		if (T_Flag)
		{
			Send_MotorDW_Flag();
			Delay();
		}
		DATA_CL();
	}
}

void T0_Isr() interrupt 1
{
	TH0 = (65536 - FOSC / 6 / 100) / 256;
	TL0 = (65536 - FOSC / 6 / 100) % 256;
	TF0 = 0;
	WDT_CONTR=0x37;
	F_Motor[0] ? (Motor1 = 1) : (Motor1 = 0); //电机控制
	F_Motor[1] ? (Motor2 = 1) : (Motor2 = 0);
	F_Motor[2] ? (Motor3 = 1) : (Motor3 = 0);
	F_Motor[3] ? (Motor4 = 1) : (Motor4 = 0);
	F_Motor[4] ? (Motor5 = 1) : (Motor5 = 0);
	F_Motor[5] ? (Motor6 = 1) : (Motor6 = 0);
	F_Motor[6] ? (Motor7 = 1) : (Motor7 = 0);
	F_Motor[7] ? (Motor8 = 1) : (Motor8 = 0);
	if ((M_D & 0x80) / 0x80 == 1)
	{
		F_Motor[8] ? (M_D1 = 1) : (M_D1 = 0);
	}
	if ((M_D & 0x40) / 0x40 == 1)
	{
		F_Motor[9] ? (M_D2 = 1) : (M_D2 = 0);
	}
	if ((M_D & 0x20) / 0x20 == 1)
	{
		F_Motor[10] ? (M_D3 = 1) : (M_D3 = 0);
	}
	if ((M_D & 0x10) / 0x10 == 1)
	{
		F_Motor[11] ? (M_D4 = 1) : (M_D4 = 0);
	}
	if ((M_D & 0x08) / 0x08 == 1)
	{
		F_Motor[12] ? (M_D5 = 1) : (M_D5 = 0);
	}
	if ((M_D & 0x04) / 0x04 == 1)
	{
		F_Motor[13] ? (M_D6 = 1) : (M_D6 = 0);
	}
	if ((M_D & 0x02) / 0x02 == 1)
	{
		F_Motor[14] ? (M_D7 = 1) : (M_D7 = 0);
	}
	if ((M_D & 0x01) / 0x01 == 1)
	{
		F_Motor[15] ? (M_D8 = 1) : (M_D8 = 0);
	}
	DW[1] = DW1; //定位反馈
	DW[2] = DW2;
	DW[3] = DW3;
	DW[4] = DW4;
	DW[5] = DW5;
	DW[6] = DW6;
	DW[7] = DW7;
	DW[8] = DW8;
	DW[9] = M_D1;
	DW[10] = M_D2;
	DW[11] = M_D3;
	DW[12] = M_D4;
	DW[13] = M_D5;
	DW[14] = M_D6;
	DW[15] = M_D7;
	DW[16] = M_D8;
	if (((DW_Flag[0] == 1) && (DW[DW_XZ[0]] == 0)) || ((DW_Flag[0] == 2) && (DW[DW_XZ[1]] == 0))) //定位监测
	{
		DW_Flag[0] = 0;
		F_Motor[0] = 0;
	}
	if (((DW_Flag[1] == 1) && (DW[DW_XZ[2]] == 0)) || ((DW_Flag[1] == 2) && (DW[DW_XZ[3]] == 0)))
	{
		DW_Flag[1] = 0;
		F_Motor[1] = 0;
	}
	if (((DW_Flag[2] == 1) && (DW[DW_XZ[4]] == 0)) || ((DW_Flag[2] == 2) && (DW[DW_XZ[5]] == 0)))
	{
		DW_Flag[2] = 0;
		F_Motor[2] = 0;
	}
	if (((DW_Flag[3] == 1) && (DW[DW_XZ[6]] == 0)) || ((DW_Flag[3] == 2) && (DW[DW_XZ[7]] == 0)))
	{
		DW_Flag[3] = 0;
		F_Motor[3] = 0;
	}
	if (((DW_Flag[4] == 1) && (DW[DW_XZ[8]] == 0)) || ((DW_Flag[4] == 2) && (DW[DW_XZ[9]] == 0)))
	{
		DW_Flag[4] = 0;
		F_Motor[4] = 0;
	}
	if (((DW_Flag[5] == 1) && (DW[DW_XZ[10]] == 0)) || ((DW_Flag[5] == 2) && (DW[DW_XZ[11]] == 0)))
	{
		DW_Flag[5] = 0;
		F_Motor[5] = 0;
	}
	if (((DW_Flag[6] == 1) && (DW[DW_XZ[12]] == 0)) || ((DW_Flag[6] == 2) && (DW[DW_XZ[13]] == 0)))
	{
		DW_Flag[6] = 0;
		F_Motor[6] = 0;
	}
	if (((DW_Flag[7] == 1) && (DW[DW_XZ[14]] == 0)) || ((DW_Flag[7] == 2) && (DW[DW_XZ[15]] == 0)))
	{
		DW_Flag[7] = 0;
		F_Motor[7] = 0;
	}
	if (((DW_Flag[8] == 1) && (DW[DW_XZ[16]] == 0)) || ((DW_Flag[8] == 2) && (DW[DW_XZ[17]] == 0)))
	{
		DW_Flag[8] = 0;
		F_Motor[8] = 0;
	}
	if (((DW_Flag[9] == 1) && (DW[DW_XZ[18]] == 0)) || ((DW_Flag[9] == 2) && (DW[DW_XZ[19]] == 0)))
	{
		DW_Flag[9] = 0;
		F_Motor[9] = 0;
	}
	if (((DW_Flag[10] == 1) && (DW[DW_XZ[20]] == 0)) || ((DW_Flag[10] == 2) && (DW[DW_XZ[21]] == 0)))
	{
		DW_Flag[10] = 0;
		F_Motor[10] = 0;
	}
	if (((DW_Flag[11] == 1) && (DW[DW_XZ[22]] == 0)) || ((DW_Flag[11] == 2) && (DW[DW_XZ[23]] == 0)))
	{
		DW_Flag[11] = 0;
		F_Motor[11] = 0;
	}
	if (((DW_Flag[12] == 1) && (DW[DW_XZ[24]] == 0)) || ((DW_Flag[12] == 2) && (DW[DW_XZ[25]] == 0)))
	{
		DW_Flag[12] = 0;
		F_Motor[12] = 0;
	}
	if (((DW_Flag[13] == 1) && (DW[DW_XZ[26]] == 0)) || ((DW_Flag[13] == 2) && (DW[DW_XZ[27]] == 0)))
	{
		DW_Flag[13] = 0;
		F_Motor[13] = 0;
	}
	if (((DW_Flag[14] == 1) && (DW[DW_XZ[28]] == 0)) || ((DW_Flag[14] == 2) && (DW[DW_XZ[29]] == 0)))
	{
		DW_Flag[14] = 0;
		F_Motor[14] = 0;
	}
	if (((DW_Flag[15] == 1) && (DW[DW_XZ[30]] == 0)) || ((DW_Flag[15] == 2) && (DW[DW_XZ[31]] == 0)))
	{
		DW_Flag[15] = 0;
		F_Motor[15] = 0;
	}
}

void T1_Isr() interrupt 3
{
	TH1 = (65536 - FOSC / 6 / 100) / 256;
	TL1 = (65536 - FOSC / 6 / 100) % 256;
	TF1 = 0;
	if (F_Run)
	{
		temp1++;
		if (P_Run)
		{
			uchar *M_Rom, num1;
			M_Rom = M_ROM(P_Set);
			for (num1 = 0; num1 < 16; num1++)
			{
				if (*(M_Rom + num1) == 0)
				{
					F_Motor[num1] = 0;
				}
				else if (*(M_Rom + num1) == 1)
				{
					F_Motor[num1] = 1;
				}
				else if (*(M_Rom + num1) == 2)
				{
					DW_Flag[num1] = 1;
				}
				else if (*(M_Rom + num1) == 3)
				{
					DW_Flag[num1] = 2;
				}
			}
			if (*(M_Rom + 16) > 0)
			{
				Music(*(M_Rom + 16));
			}
			WaitTime = *(M_Rom + 17) * 256 + *(M_Rom + 18);
			if (T_BD)
			{
				uchar j[5];
				j[0] = 0xfd;
				j[1] = 0x03;
				j[2] = 0x71;
				j[3] = P_Set;
				j[4] = j[0] + j[1] + j[2] + j[3];
				UART_Send(j, 5);
			}
			P_Run = 0;
		}
		if (temp1 >= 10)
		{
			temp1 = 0;
			WaitTime--;
			if (WaitTime == 0)
			{
				P_Set++;
				P_Run = 1;
				if (P_Set > F_Set)
				{
					P_Set = 1;
					P_Run = 1;
					F_Run = 0;
				}
			}
		}
	}
}

void Uart_Isr() interrupt 4
{
	if (RI)
	{
		R_Data[R_Num] = SBUF;
		RI = 0;
		if (R_Data[0] == 0xFD)
		{
			R_Num++;
			if ((R_Data[1] + 2) == R_Num)
			{
				R_CL++;
				R_Num = 0;
				R_Data[0] = 0;
			}
		}
	}
}