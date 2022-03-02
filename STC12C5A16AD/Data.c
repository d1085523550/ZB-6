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

void DATA_Que(uchar *i) //接收到有效数据，分类并处理
{
	uchar idata j[5] = {0xfd, 0x03, 0x90, 0x10, 0xa0};
	switch (i[0])
	{
	case 0x01: //设置参数
		IapEraseSector(0x2000); //扇区擦除
		IapEraseSector(0x2200);
		IapEraseSector(0x2400);
		IapEraseSector(0x2600);
		IapEraseSector(0x2800);
		IapEraseSector(0x2a00);
		IapEraseSector(0x2c00);
		IapEraseSector(0x2e00);
		IapProgramuchar(0x2000, i[1]); //电机数
		IapProgramuchar(0x2001, i[2]); //扩展口电机/定位选择
		UART_Send(j, 5);
		break;
	
	case 0x10  0x1f) //设置定位参数
	{
		i[0] = i[0] - 0x10;
		IapProgramuchar(0x2010 + i[0], i[1]); //定位1 端口选择 2010-201F
		IapProgramuchar(0x2020 + i[0], i[2]); //定位2 端口选择 2020-202F
		UART_Send(j, 5);
	}
	else if (i[0] == 0x20) //程序段总数
	{
		IapProgramuchar(0x2030, i[1]); //程序0段总段数
		IapProgramuchar(0x2031, i[2]); //程序1段总段数
		IapProgramuchar(0x2032, i[3]); //程序2段总段数
		UART_Send(j, 5);
	}
	else if (i[0] == 0x21) //程序0段写入
	{
		uchar k, l;
		for (k = 0, l = 19; k < 19; k++, l--)
		{
			IapProgramuchar((i[1] + 1) * 19 - l + 0x2100, i[k + 2]); //循环写入数据 2100-25F9里
		}
		UART_Send(j, 5);
	}
	else if (i[0] == 0x22) //程序1段写入
	{
		uchar k, l;
		for (k = 0, l = 19; k < 19; k++, l--)
		{
			IapProgramuchar((i[1] + 1) * 19 - l + 0x2600, i[k + 2]); //循环写入数据 2600-2AF9里
		}
		UART_Send(j, 5);
	}
	else if (i[0] == 0x23) //程序2段写入
	{
		uchar k, l;
		for (k = 0, l = 19; k < 19; k++, l--)
		{
			IapProgramuchar((i[1] + 1) * 19 - l + 0x2b00, i[k + 2]); //循环写入数据 2b00-2FF9里
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
			IAP_CONTR = 0X20;
		}
	}
	else if (i[0] == 0x32 || i[0] == 0x35) //单段程序启动
	{
		uchar j;
		for (j = 0; j < 18; j++)
		{
			P_Rom[j] = i[j + 1];
		}
	}
	else if (i[0] == 0x33) //程序位置返回
	{
	}
	else if (i[0] == 0x65) //下载完成 并重启
	{
		if (i[1] == 0x10)
		{
			IAP_CONTR = 0X20; //重启单片机
		}
	}
	else if (i[0] == 0xa0) //上传标志
	{
	}
	else if (i[0] == 0x90) //数据是否正确
	{
	}
}
