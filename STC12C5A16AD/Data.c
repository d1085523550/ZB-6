#include "Data.h"

void Uart_Isr() interrupt 4
{
	if (RI)
	{
		RI = 0;
		R_Data[R_Num] = SBUF;
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
		else
		{
			R_Num = 0;
		}
	}
}

void DATA_CL() //接收到的数据进行效验处理
{
	uchar idata SUM = 0xFD, num, i[13];
	if (R_CL)
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
			i[0] = 0xFD;
			i[1] = 0x03;
			i[2] = 0xA0;
			i[3] = 0x00;
			i[4] = 0xA0;
			UART_Send(i, 5);
		}
		R_CL--;
	}
}

void DATA_Que(uchar *i) //接收到有效数据，分类并处理
{
	uchar idata j[5] = {0xFD, 0x03, 0xA0, 0x10, 0xB0};
	uchar k, l;
	switch (i[0])
	{
	case 0x00: //握手信号
		F_Run = 0;
		j[3] = 0xF0;
		j[4] = 0x90;
		UART_Send(j, 5);
		IapEraseSector(0x2000); //扇区擦除
		IapEraseSector(0x2200);
		IapEraseSector(0x2400);
		IapEraseSector(0x2600);
		IapEraseSector(0x2800);
		IapEraseSector(0x2a00);
		IapEraseSector(0x2c00);
		IapEraseSector(0x2e00);
		IapEraseSector(0x3000);
		IapEraseSector(0x3200);
		j[3] = 0x10;
		j[4] = 0xb0;
		UART_Send(j, 5);
		break;
	case 0x01:						   //设置参数
		IapProgramuchar(0x2001, i[2]); //扩展口电机/定位选择
		UART_Send(j, 5);
		break;
	case 0x02: //设置定位参数
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:
	case 0x08:
	case 0x09:
	case 0x0A:
	case 0x0B:
	case 0x0C:
	case 0x0D:
	case 0x0E:
	case 0x0F:
	case 0x10:
	case 0x11:
		IapProgramuchar(0x2000 + i[0], i[1]); //定位1 端口选择 2002-2011
		IapProgramuchar(0x2010 + i[0], i[2]); //定位2 端口选择 2012-2021
		UART_Send(j, 5);
		break;
	case 0x12: //动作名称
	case 0x13:
	case 0x14:
	case 0x15:
	case 0x16:
	case 0x17:
	case 0x18:
	case 0x19:
	case 0x1A:
	case 0x1B:
	case 0x1C:
	case 0x1D:
	case 0x1E:
	case 0x1F:
	case 0x20:
	case 0x21:
		for (k = 0; k < 12; k++)
		{
			IapProgramuchar(0x2022 + (i[0] - 0x12) * 12 + k, i[k + 1]);
		}
		UART_Send(j, 5);
		break;
	case 0x22:						   //程序段总数
		IapProgramuchar(0x20E2, i[1]); //程序0段总段数
		IapProgramuchar(0x20E3, i[2]); //程序1段总段数
		IapProgramuchar(0x20E4, i[3]); //程序2段总段数
		UART_Send(j, 5);
		break;
	case 0x23: //程序0段写入
		for (k = 0, l = 9; k < 9; k++, l--)
		{
			IapProgramuchar((i[1] + 1) * 9 - l + 0x2100, i[k + 2]); //循环写入数据 2100-25F9里
		}
		UART_Send(j, 5);
		break;
	case 0x24: //程序1段写入
		for (k = 0, l = 9; k < 9; k++, l--)
		{
			IapProgramuchar((i[1] + 1) * 9 - l + 0x2755, i[k + 2]); //循环写入数据 2600-2AF9里
		}
		UART_Send(j, 5);
		break;
	case 0x25: //程序2段写入
		for (k = 0, l = 9; k < 9; k++, l--)
		{
			IapProgramuchar((i[1] + 1) * 9 - l + 0x2DAA, i[k + 2]); //循环写入数据 2b00-2FF9里
		}
		UART_Send(j, 5);
		break;
	case 0x26: //下载完成 并重启
		if (i[1] == 0xF0)
		{
			IAP_CONTR = 0X20; //重启单片机
		}
		break;
	case 0x30: //电机定位状态返回
		if (i[1] == 1)
		{
			T_Flag = 1;
		}
		else
		{
			T_Flag = 0;
		}
		break;
	case 0x31: //强制停止
		if (i[1] == 1)
		{
			IAP_CONTR = 0X20;
		}
		break;
	case 0x32: //单段程序启动
	case 0x35:
		for (k = 0; k < 18; k++)
		{
			P_Rom[k] = i[k + 1];
		}
		break;
	case 0x33: //程序位置返回
		break;
	case 0x2B: //上传标志
		if (i[1] == 1)
		{
			T_YZ = 1;
		}
		else
		{
			i[0] = 0xFD;
			i[1] = 0x03;
			i[2] = 0xA0;
			i[3] = 0x00;
			i[4] = 0xA0;
			UART_Send(i, 5);
		}
		break;
	case 0xA0: //数据验证
	{
		if (i[1] == 0x10)
		{
			T_Step++;
			T_Step_Temp = T_Step;
		}
		else if (i[1] == 0x00)
		{
			T_Step_Temp = T_Step;
		}
		else if (i[1] == 0x90)
		{
			T_Num++;
			T_Step_Temp = T_Step;
		}
	}
	default:
		i[0] = 0xFD;
		i[1] = 0x03;
		i[2] = 0xA0;
		i[3] = 0x00;
		i[4] = 0xA0;
		UART_Send(i, 5);
		break;
	}
}

void SendData() //上传数据
{
	uchar idata i[16], k, l;
	i[0] = 0xfd;
	i[2] = 0x4f + T_Step_Temp;
	switch (T_Step_Temp)
	{
	case 1:
		i[1] = 0x03;
		i[2] = 0x50;
		i[3] = IapReaduchar(0x2001);
		i[4] = i[0] + i[1] + i[2] + i[3];
		UART_Send(i, 5);
		break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
		i[1] = 0x04;
		i[3] = IapReaduchar(0x2000 + T_Step_Temp);
		i[4] = IapReaduchar(0x2010 + T_Step_Temp);
		i[5] = i[0] + i[1] + i[2] + i[3] + i[4];
		UART_Send(i, 6);
		break;
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
	case 33:
		i[1] = 14;
		for (k = 0; k < 12; k++)
		{
			i[k + 3] = IapReaduchar(0x2022 + (T_Step_Temp - 18) * 12 + k);
		}
		i[15] = i[0] + i[1] + i[2] + i[3] + i[4] + i[5] + i[6] + i[7] + i[8] + i[9] + i[10] + i[11] + i[12] + i[13] + i[14];
		UART_Send(i, 16);
		break;
	case 34:
		i[1] = 12;
		i[3] = T_Num;
		for (k = 0, l = 9; k < 9; k++, l--)
		{
			i[k + 4] = IapReaduchar((T_Num + 1) * 9 - l + 0x2100); //循环写入数据 2100-25F9里
		}
		i[13] = i[0] + i[1] + i[2] + i[3] + i[4] + i[5] + i[6] + i[7] + i[8] + i[9] + i[10] + i[11] + i[12];
		UART_Send(i, 14);
		if (T_Num >= T_ROM_D0)
		{
			i[1] = 0x03;
			i[2] = 0xa0;
			i[3] = 0xff;
			i[4] = i[0] + i[1] + i[2] + i[3];
			UART_Send(i, 5);
		}
		break;
	case 35:
		i[1] = 12;
		i[3] = T_Num;
		for (k = 0, l = 9; k < 9; k++, l--)
		{
			i[k + 4] = IapReaduchar((T_Num + 1) * 9 - l + 0x2755); //循环写入数据 2755-2DA9里
		}
		i[13] = i[0] + i[1] + i[2] + i[3] + i[4] + i[5] + i[6] + i[7] + i[8] + i[9] + i[10] + i[11] + i[12];
		UART_Send(i, 14);
		if (T_Num >= T_ROM_D1)
		{
			i[1] = 0x03;
			i[2] = 0xa0;
			i[3] = 0xff;
			i[4] = i[0] + i[1] + i[2] + i[3];
			UART_Send(i, 5);
		}
		break;
	case 36:
		i[1] = 12;
		i[3] = T_Num;
		for (k = 0, l = 9; k < 9; k++, l--)
		{
			i[k + 4] = IapReaduchar((T_Num + 1) * 9 - l + 0x2DAA); //循环写入数据 2DAA-33FE里
		}
		i[13] = i[0] + i[1] + i[2] + i[3] + i[4] + i[5] + i[6] + i[7] + i[8] + i[9] + i[10] + i[11] + i[12];
		UART_Send(i, 14);
		if (T_Num >= T_ROM_D2)
		{
			i[1] = 0x03;
			i[2] = 0xa0;
			i[3] = 0xff;
			i[4] = i[0] + i[1] + i[2] + i[3];
			UART_Send(i, 5);
		}
		break;
	case 37:
		i[1] = 0x03;
		i[2] = 0x74;
		i[3] = T_ROM_D0;
		i[4] = T_ROM_D1;
		i[5] = T_ROM_D2;
		i[6] = i[0] + i[1] + i[2] + i[3] + i[4] + i[5];
		UART_Send(i, 7);
		break;
	case 38:
		i[1] = 0x03;
		i[2] = 0x75;
		i[3] = 0x10;
		i[4] = 0x75;
		i[5] = i[0] + i[1] + i[2] + i[3] + i[4];
		UART_Send(i, 7);
		break;
	case 39:
		T_YZ = 0;
		break;
	}
	T_Step_Temp = 0;
}
