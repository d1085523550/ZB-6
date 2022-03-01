#include "HardWare.h"

void TIM_Init() // T0定时器
{
    AUXR |= 0x80;      //定时器时钟1T模式
    TMOD &= 0xF0;      //设置定时器模式
    TMOD |= 0x01;      //设置定时器模式
    TL0 = T0Time;      //设置定时初值
    TH0 = T0Time >> 8; //设置定时初值
    TF0 = 0;           //清除TF0标志
    TR0 = 1;           //定时器0开始计时
    ET0 = 1;
    EA = 1;
}

void UartInit(void) // 115200bps@24.000MHz
{
    PCON |= 0x80; //使能波特率倍速位SMOD
    SCON = 0x50;  // 8位数据,可变波特率
    AUXR |= 0x04; //独立波特率发生器时钟为Fosc,即1T
    BRT = 0xF3;   //设定独立波特率发生器重装值
    AUXR |= 0x01; //串口1选择独立波特率发生器为波特率发生器
    AUXR |= 0x10; //启动独立波特率发生器
}

void Delay_MN() //模拟串口发送间隔时间 9600 104us
{
    unsigned char i, j;
    _nop_();
    _nop_();
    i = 3;
    j = 105;
    do
    {
        while (--j)
            ;
    } while (--i);
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

void Test_Send(uchar i) //发送测试数据
{
    SBUF = i;
    TI = 0;
    while (!TI)
        ;
}

void KeyTrigger() //按键触发
{
    if (XD1 == 0 && XD2 == 1)
    {
        F_ROM_Set = 2;
    }
    else if (XD1 == 1 && XD2 == 0)
    {
        F_ROM_Set = 3;
    }
    else
    {
        F_ROM_Set = 1;
    }
    if (F_ROM_Set != F_ROM_Temp && F_ROM_Temp != 0)
    {
        XD_Run = 1;
        if (XD_Delay >= 200)
        {
            IAP_CONTR = 0x20; //完全重启单片机
        }
    }
    else
    {
        F_ROM_Temp = F_ROM_Set;
    }
    if (YK == 0)
    {
        YK_Delay = 100;
        while (YK_Delay)
        {
            if (YK == 1)
            {
                YK_Delay = 0;
            }
            else if (YK_Delay < 5 && YK == 0)
            {
                if (F_Step == P_Step)
                {
                    WaitTime = 0;
                }
                F_Run = 1;
            }
        }
    }

    if ((HW == 0) && (F_Run == 0) && (P_RomFlag == 0))
    {
        HW_Delay = 200;
        while (HW_Delay)
        {
            if (HW == 1)
            {
                HW_Delay = 0;
            }
            else if (HW_Delay <= 5 && HW == 0 && F_Run == 0 && P_RomFlag == 0)
            {
                F_Run = 1;
                temp1 = 0;
            }
        }
    }
}
