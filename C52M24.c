#include "STC12C5A60S2.H"
#include "EEPROM_Iap.H"
#define uchar unsigned char
#define uint unsigned int

#define FOSC 24000000L                  //晶振频率
#define BAUD 115200                     //波特率
#define T0Time 65536 - FOSC / 12 / 100  // T0时钟
#define T1Time 65536 - FOSC / 12 / 1000 // T1时钟
sbit MN_RTX = P3 ^ 5;                   //模拟串口 发送口
sbit HW = P3 ^ 3;                       //触发口
sbit YK = P3 ^ 2;                       //遥控触发口
sbit Power12V = P3 ^ 4;                 // 12V电源 低有效时开通
sbit XD1 = P3 ^ 6;                      //程序选段口1
sbit XD2 = P3 ^ 7;                      //程序选段口2

/////////////////////////////IO脚位/////////////////////////////
sbit Motor1 = P0 ^ 0; //电机 P0.0
sbit Motor2 = P0 ^ 1; //电机 P0.1
sbit Motor3 = P0 ^ 2; //电机 P0.2
sbit Motor4 = P0 ^ 3; //电机 P0.3
sbit Motor5 = P0 ^ 4; //电机 P0.4
sbit Motor6 = P0 ^ 5; //电机 P0.5
sbit Motor7 = P0 ^ 6; //电机 P0.6
sbit Motor8 = P0 ^ 7; //电机 P0.7
sbit M_D1 = P2 ^ 7;   //电机或者定位 P2.7
sbit M_D2 = P2 ^ 6;   //电机或者定位 P2.6
sbit M_D3 = P2 ^ 5;   //电机或者定位 P2.5
sbit M_D4 = P2 ^ 4;   //电机或者定位 P2.4
sbit M_D5 = P2 ^ 3;   //电机或者定位 P2.3
sbit M_D6 = P2 ^ 2;   //电机或者定位 P2.2
sbit M_D7 = P2 ^ 1;   //电机或者定位 P2.1
sbit M_D8 = P2 ^ 0;   //电机或者定位 P2.0
sbit DW1 = P1 ^ 0;    //定位信号P1.0
sbit DW2 = P1 ^ 1;    //定位信号P1.1
sbit DW3 = P1 ^ 2;    //定位信号P1.2
sbit DW4 = P1 ^ 3;    //定位信号P1.3
sbit DW5 = P1 ^ 4;    //定位信号P1.4
sbit DW6 = P1 ^ 5;    //定位信号P1.5
sbit DW7 = P1 ^ 6;    //定位信号P1.6
sbit DW8 = P1 ^ 7;    //定位信号P1.7

bit F_Run, P_Run, F_Flag, T_Flag, XD_Run;
uchar temp1;
uchar P_RomFlag;
uchar F_Step, P_Step;
uchar idata R_Data[24], R_Num, R_CL;
uchar P_Rom[19];
unsigned long idata WaitTime; //单段运行时间
uchar M_D;                    //扩展口选择
uchar F_Motor[16];            //电机状态
uchar DW_Flag[16];            //定位状态
uchar idata DW_XZ[33];        //定位选择
uchar DW[17];                 //定位绑定
uchar T_ROM_D0;               //程序段0 总段数
uchar T_ROM_D1;               //程序段1 总段数
uchar T_ROM_D2;               //程序段2 总段数
uchar F_ROM_Set;              //程序段选择设置
uchar F_ROM_Temp;             //程序段切换检测
uint T_Step;                  //上传步骤计数
uchar T_YZ;                   //上传程序验证
uchar SendCRC;                //数据上传效验码
uint YK_Delay;                //遥控延时
uint HW_Delay;                //触发延时
uint XD_Delay;                //选段延时

void Delay() //软件延时
{
    uchar i, j, k = 20;
    while (k--)
    {
        i = 100;
        while (i--)
        {
            j = 249;
            while (j--)
                ;
        }
    }
}

void Delay_MN() //模拟串口发送间隔时间 9600 104us
{
    uchar i;
    _nop_();
    i = 202;
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

void UART_Int() //串口初始化 115200bps@24.000MHz
{
    PCON &= 0x7F; //波特率不倍速
    SCON = 0x50;  // 8位数据,可变波特率
    AUXR |= 0x04; //独立波特率发生器时钟为Fosc,即1T
    BRT = 0xF9;   //设定独立波特率发生器重装值
    AUXR |= 0x01; //串口1选择独立波特率发生器为波特率发生器
    AUXR |= 0x10; //启动独立波特率发生器
}

void TIM_Int() //定时器初始化
{
    TMOD = 0x11; // T1,T2为16位模式 10MS
    TH0 = T0Time >> 8;
    TL0 = T0Time;
    // TH1 = T1Time >> 8;
    // TL1 = T1Time;
    TF0 = 0;
    // TF1 = 0;
    ET0 = 1;
    // ET1 = 1;
    TR0 = 1;
    // TR1 = 1;
    EA = 1;
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

void Music(uchar num) //播放音乐
{
    uchar idata Music_Selection[10] = {0x7e, 0xff, 0x06, 0x12, 0x00, 0x00, 0x01, 0xfe, 0xf7, 0xef}; //选择播发 MP3文件夹
    Music_Selection[6] = num;
    Music_Selection[7] = ((0xffff - (Music_Selection[1] + Music_Selection[2] + Music_Selection[3] + Music_Selection[4] + Music_Selection[5] + Music_Selection[6]) + 1) >> 8);
    Music_Selection[8] = ((0xffff - (Music_Selection[1] + Music_Selection[2] + Music_Selection[3] + Music_Selection[4] + Music_Selection[5] + Music_Selection[6]) + 1) & 0xff);
    UART_MN(Music_Selection, 10);
}

uchar *M_ROM(uchar n) //获取程序段
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
        F_Step = IapReaduchar(0x20E2 + F_ROM_Set - 1);
        switch (F_ROM_Set)
        {
        case 1:
            M_Rom[16] = IapReaduchar(9 * (n - 1) + 0x2100);
            M_Rom[17] = IapReaduchar(9 * (n - 1) + 0x2100 + 1);
            M_Rom[18] = IapReaduchar(9 * (n - 1) + 0x2100 + 2);
            for (num1 = 0, num2 = 1; num1 < 8; num1++, num2 *= 2)
            {
                M_Rom[num1] = (M_Rom[16] & num2) / num2 + (M_Rom[17] & num2) / num2 * 2 + (M_Rom[18] & num2) / num2 * 3;
            }
            M_Rom[16] = IapReaduchar(9 * (n - 1) + 0x2100 + 3);
            M_Rom[17] = IapReaduchar(9 * (n - 1) + 0x2100 + 4);
            M_Rom[18] = IapReaduchar(9 * (n - 1) + 0x2100 + 5);
            for (num1 = 8, num2 = 1; num1 < 16; num1++, num2 *= 2)
            {
                M_Rom[num1] = (M_Rom[16] & num2) / num2 + (M_Rom[17] & num2) / num2 * 2 + (M_Rom[18] & num2) / num2 * 3;
            }

            M_Rom[16] = IapReaduchar(9 * (n - 1) + 0x2100 + 6);
            M_Rom[17] = IapReaduchar(9 * (n - 1) + 0x2100 + 7);
            M_Rom[18] = IapReaduchar(9 * (n - 1) + 0x2100 + 8);
            break;
        case 2:
            M_Rom[16] = IapReaduchar(9 * (n - 1) + 0x2755);
            M_Rom[17] = IapReaduchar(9 * (n - 1) + 0x2755 + 1);
            M_Rom[18] = IapReaduchar(9 * (n - 1) + 0x2755 + 2);
            for (num1 = 0, num2 = 1; num1 < 8; num1++, num2 *= 2)
            {
                M_Rom[num1] = (M_Rom[16] & num2) / num2 + (M_Rom[17] & num2) / num2 * 2 + (M_Rom[18] & num2) / num2 * 3;
            }
            M_Rom[16] = IapReaduchar(9 * (n - 1) + 0x2755 + 3);
            M_Rom[17] = IapReaduchar(9 * (n - 1) + 0x2755 + 4);
            M_Rom[18] = IapReaduchar(9 * (n - 1) + 0x2755 + 5);
            for (num1 = 8, num2 = 1; num1 < 16; num1++, num2 *= 2)
            {
                M_Rom[num1] = (M_Rom[16] & num2) / num2 + (M_Rom[17] & num2) / num2 * 2 + (M_Rom[18] & num2) / num2 * 3;
            }
            M_Rom[16] = IapReaduchar(9 * (n - 1) + 0x2755 + 6);
            M_Rom[17] = IapReaduchar(9 * (n - 1) + 0x2755 + 7);
            M_Rom[18] = IapReaduchar(9 * (n - 1) + 0x2755 + 8);
            break;
        case 3:
            M_Rom[16] = IapReaduchar(9 * (n - 1) + 0x2DAA);
            M_Rom[17] = IapReaduchar(9 * (n - 1) + 0x2DAA + 1);
            M_Rom[18] = IapReaduchar(9 * (n - 1) + 0x2DAA + 2);
            for (num1 = 0, num2 = 1; num1 < 8; num1++, num2 *= 2)
            {
                M_Rom[num1] = (M_Rom[16] & num2) / num2 + (M_Rom[17] & num2) / num2 * 2 + (M_Rom[18] & num2) / num2 * 3;
            }
            M_Rom[16] = IapReaduchar(9 * (n - 1) + 0x2DAA + 3);
            M_Rom[17] = IapReaduchar(9 * (n - 1) + 0x2DAA + 4);
            M_Rom[18] = IapReaduchar(9 * (n - 1) + 0x2DAA + 5);
            for (num1 = 8, num2 = 1; num1 < 16; num1++, num2 *= 2)
            {
                M_Rom[num1] = (M_Rom[16] & num2) / num2 + (M_Rom[17] & num2) / num2 * 2 + (M_Rom[18] & num2) / num2 * 3;
            }
            M_Rom[16] = IapReaduchar(9 * (n - 1) + 0x2DAA + 6);
            M_Rom[17] = IapReaduchar(9 * (n - 1) + 0x2DAA + 7);
            M_Rom[18] = IapReaduchar(9 * (n - 1) + 0x2DAA + 8);
            break;
        }
    }
    return M_Rom;
}

void Data_Int() //需要初始的值
{
    P0M0=0xFF;
    P0M1=0;
    P1M0=0;
    P1M1=0xFF;
    P2M0=0;
    P2M1=0;
    P3M0=0;
    P3M1=0;
    P_Run = 1;
    P_Step = 1;
    M_D = IapReaduchar(0x2001); //扩展口配置
    (M_D & 0x80 / 0x80) ? ((M_D1 = 0),(P2M0|=0x01)) : ((M_D1 = 1),(P2M1|=0x01));
    (M_D & 0x40 / 0x40) ? ((M_D2 = 0),(P2M0|=0x02)) : ((M_D2 = 1),(P2M1|=0x02));
    (M_D & 0x20 / 0x20) ? ((M_D3 = 0),(P2M0|=0x04)) : ((M_D3 = 1),(P2M1|=0x04));
    (M_D & 0x10 / 0x10) ? ((M_D4 = 0),(P2M0|=0x08)) : ((M_D4 = 1),(P2M1|=0x08));
    (M_D & 0x08 / 0x08) ? ((M_D5 = 0),(P2M0|=0x10)) : ((M_D5 = 1),(P2M1|=0x10));
    (M_D & 0x04 / 0x04) ? ((M_D6 = 0),(P2M0|=0x20)) : ((M_D6 = 1),(P2M1|=0x20));
    (M_D & 0x02 / 0x02) ? ((M_D7 = 0),(P2M0|=0x40)) : ((M_D7 = 1),(P2M1|=0x40));
    (M_D & 0x01 / 0x01) ? ((M_D8 = 0),(P2M0|=0x80)) : ((M_D8 = 1),(P2M1|=0x80));
    DW_XZ[0] = IapReaduchar(0x2002);
    DW_XZ[1] = IapReaduchar(0x2012);
    DW_XZ[2] = IapReaduchar(0x2003);
    DW_XZ[3] = IapReaduchar(0x2013);
    DW_XZ[4] = IapReaduchar(0x2004);
    DW_XZ[5] = IapReaduchar(0x2014);
    DW_XZ[6] = IapReaduchar(0x2005);
    DW_XZ[7] = IapReaduchar(0x2015);
    DW_XZ[8] = IapReaduchar(0x2006);
    DW_XZ[9] = IapReaduchar(0x2016);
    DW_XZ[10] = IapReaduchar(0x2007);
    DW_XZ[11] = IapReaduchar(0x2017);
    DW_XZ[12] = IapReaduchar(0x2008);
    DW_XZ[13] = IapReaduchar(0x2018);
    DW_XZ[14] = IapReaduchar(0x2009);
    DW_XZ[15] = IapReaduchar(0x2019);
    DW_XZ[16] = IapReaduchar(0x200A);
    DW_XZ[17] = IapReaduchar(0x201A);
    DW_XZ[18] = IapReaduchar(0x200B);
    DW_XZ[19] = IapReaduchar(0x201B);
    DW_XZ[20] = IapReaduchar(0x200C);
    DW_XZ[21] = IapReaduchar(0x201C);
    DW_XZ[22] = IapReaduchar(0x200D);
    DW_XZ[23] = IapReaduchar(0x201D);
    DW_XZ[24] = IapReaduchar(0x200E);
    DW_XZ[25] = IapReaduchar(0x201E);
    DW_XZ[26] = IapReaduchar(0x200F);
    DW_XZ[27] = IapReaduchar(0x201F);
    DW_XZ[28] = IapReaduchar(0x2010);
    DW_XZ[29] = IapReaduchar(0x2020);
    DW_XZ[30] = IapReaduchar(0x2011);
    DW_XZ[31] = IapReaduchar(0x2021);
}

void Motor_Set() //电机启停设置
{
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
}

void ProgramRun() //运行程序
{
    if (P_Run)
    {
        uchar *M_Rom, num1;
        M_Rom = M_ROM(P_Step);
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
        // Test_Send(*(M_Rom + 17));
        // Test_Send(*(M_Rom + 18));
        WaitTime = *(M_Rom + 17) * 256 + *(M_Rom + 18);
        if (WaitTime > 0)
            WaitTime = WaitTime * 1000;
        P_Run = 0;
    }
    else
    {
        if (WaitTime == 0)
        {
            P_Step++;
            P_Run = 1;
            if (P_Step > F_Step)
            {
                P_Step = 1;
                F_Run = 0;
            }
        }
    }
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
            IAP_CONTR = 0x60; //完全重启单片机
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

void main()
{
    Power12V = 1;
    TIM_Int();
    UART_Int();
    Data_Int();
    Motor_Set();
    Delay();
    Power12V = 0;
    while (1)
    {
        KeyTrigger();
        if (F_Run)
        {
            ProgramRun();
            Motor_Set();
        }
    }
}

void T0_Isr() interrupt 1
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