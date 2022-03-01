#include "STC89C5xRC.H"
#include "EEPROM_ISP.H"
#define uchar unsigned char
#define uint unsigned int

//默认为 6倍速
#define FOSC 11059200L                //晶振频率
#define BAUD 9600                     //波特率
#define T0Time 65536 - FOSC / 6 / 100 //T0时钟
#define T1Time 65536 - FOSC / 6 / 100 //T1时钟
sbit MN_RTX = P3 ^ 5;                 //模拟串口 发送口
sbit HW = P3 ^ 3;                     //触发口
sbit YK = P3 ^ 2;                     //遥控触发口
sbit Power12V = P3 ^ 4;               //12V电源 低有效时开通
sbit XD1 = P3 ^ 6;                    //程序选段口1
sbit XD2 = P3 ^ 7;                    //程序选段口2

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

bit F_Run, P_Run, F_Flag, T_Flag, YK_Run, HW_Run, XD_Run;
uchar P_RomFlag;
uchar temp1;
uchar F_Step, P_Step;
uchar idata R_Data[24], R_Num, R_CL;
uchar P_Rom[19];
uint idata WaitTime;   //单段运行时间
uchar M_D;             //扩展口选择
uchar F_Motor[16];     //电机状态
uchar DW_Flag[16];     //定位状态
uchar idata DW_XZ[33]; //定位选择
uchar DW[17];          //定位绑定
uchar T_ROM_D0;        //程序段0 总段数
uchar T_ROM_D1;        //程序段1 总段数
uchar T_ROM_D2;        //程序段2 总段数
uchar F_ROM_Set;       //程序段选择设置
uchar F_ROM_Temp;      //程序段切换检测
uint T_Step;           //上传步骤计数
uchar T_YZ;            //上传程序验证
uchar SendCRC;         //数据上传效验码
uchar YK_Delay;        //遥控延时
uchar HW_Delay;        //触发延时
uchar XD_Delay;        //选段延时

void Delay() //软件延时
{
    uchar i, j, k = 10;
    while (k--)
    {
        i = 180;
        while (i--)
        {
            j = 73;
            while (j--)
                ;
        }
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

void UART_Int() //串口初始化
{
    SCON = 0x50;
    TH2 = RCAP2H = (65536 - (FOSC / 16 / BAUD)) / 256;
    TL2 = RCAP2L = (65536 - (FOSC / 16 / BAUD)) % 256;
    T2CON = 0x34; //T2 Run
    ES = 1;
}

void TIM_Int() //定时器初始化
{
    TMOD = 0x11; //T1,T2为16位模式 10MS
    TH0 = T0Time >> 8;
    TL0 = T0Time;
    //TH1 = T1Time >> 8;
    //TL1 = T1Time;
    TF0 = 0;
    //TF1 = 0;
    ET0 = 1;
    //ET1 = 1;
    TR0 = 1;
    //TR1 = 1;
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

// void MusicVol() //音量调节
// {
//     uchar code Music_Vol[10] = {0x7e, 0xff, 0x06, 0x06, 0x00, 0x00, 0x1e, 0xfe, 0xf7, 0xef};
//     UART_MN(Music_Vol, 10);
// }

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
        F_Step = IspReaduchar(0x2030 + F_ROM_Set - 1);
        switch (F_ROM_Set)
        {
        case 1:
            M_Rom[16] = IspReaduchar(9 * (n - 1) + 0x2200);
            M_Rom[17] = IspReaduchar(9 * (n - 1) + 0x2200 + 1);
            M_Rom[18] = IspReaduchar(9 * (n - 1) + 0x2200 + 2);
            for (num1 = 0, num2 = 1; num1 < 8; num1++, num2 *= 2)
            {
                M_Rom[num1] = (M_Rom[16] & num2) / num2 + (M_Rom[17] & num2) / num2 * 2 + (M_Rom[18] & num2) / num2 * 3;
            }
            M_Rom[16] = IspReaduchar(9 * (n - 1) + 0x2200 + 3);
            M_Rom[17] = IspReaduchar(9 * (n - 1) + 0x2200 + 4);
            M_Rom[18] = IspReaduchar(9 * (n - 1) + 0x2200 + 5);
            for (num1 = 8, num2 = 1; num1 < 16; num1++, num2 *= 2)
            {
                M_Rom[num1] = (M_Rom[16] & num2) / num2 + (M_Rom[17] & num2) / num2 * 2 + (M_Rom[18] & num2) / num2 * 3;
            }

            M_Rom[16] = IspReaduchar(9 * (n - 1) + 0x2200 + 6);
            M_Rom[17] = IspReaduchar(9 * (n - 1) + 0x2200 + 7);
            M_Rom[18] = IspReaduchar(9 * (n - 1) + 0x2200 + 8);
            break;
        case 2:
            M_Rom[16] = IspReaduchar(9 * (n - 1) + 0x2800);
            M_Rom[17] = IspReaduchar(9 * (n - 1) + 0x2800 + 1);
            M_Rom[18] = IspReaduchar(9 * (n - 1) + 0x2800 + 2);
            for (num1 = 0, num2 = 1; num1 < 8; num1++, num2 *= 2)
            {
                M_Rom[num1] = (M_Rom[16] & num2) / num2 + (M_Rom[17] & num2) / num2 * 2 + (M_Rom[18] & num2) / num2 * 3;
            }
            M_Rom[16] = IspReaduchar(9 * (n - 1) + 0x2800 + 3);
            M_Rom[17] = IspReaduchar(9 * (n - 1) + 0x2800 + 4);
            M_Rom[18] = IspReaduchar(9 * (n - 1) + 0x2800 + 5);
            for (num1 = 8, num2 = 1; num1 < 16; num1++, num2 *= 2)
            {
                M_Rom[num1] = (M_Rom[16] & num2) / num2 + (M_Rom[17] & num2) / num2 * 2 + (M_Rom[18] & num2) / num2 * 3;
            }
            M_Rom[16] = IspReaduchar(9 * (n - 1) + 0x2800 + 6);
            M_Rom[17] = IspReaduchar(9 * (n - 1) + 0x2800 + 7);
            M_Rom[18] = IspReaduchar(9 * (n - 1) + 0x2800 + 8);
            break;
        case 3:
            M_Rom[16] = IspReaduchar(9 * (n - 1) + 0x2E00);
            M_Rom[17] = IspReaduchar(9 * (n - 1) + 0x2E00 + 1);
            M_Rom[18] = IspReaduchar(9 * (n - 1) + 0x2E00 + 2);
            for (num1 = 0, num2 = 1; num1 < 8; num1++, num2 *= 2)
            {
                M_Rom[num1] = (M_Rom[16] & num2) / num2 + (M_Rom[17] & num2) / num2 * 2 + (M_Rom[18] & num2) / num2 * 3;
            }
            M_Rom[16] = IspReaduchar(9 * (n - 1) + 0x2E00 + 3);
            M_Rom[17] = IspReaduchar(9 * (n - 1) + 0x2E00 + 4);
            M_Rom[18] = IspReaduchar(9 * (n - 1) + 0x2E00 + 5);
            for (num1 = 8, num2 = 1; num1 < 16; num1++, num2 *= 2)
            {
                M_Rom[num1] = (M_Rom[16] & num2) / num2 + (M_Rom[17] & num2) / num2 * 2 + (M_Rom[18] & num2) / num2 * 3;
            }
            M_Rom[16] = IspReaduchar(9 * (n - 1) + 0x2E00 + 6);
            M_Rom[17] = IspReaduchar(9 * (n - 1) + 0x2E00 + 7);
            M_Rom[18] = IspReaduchar(9 * (n - 1) + 0x2E00 + 8);
            break;
        }
    }
    return M_Rom;
}

void Data_Int() //需要初始的值
{
    P_Run = 1;
    P_Step = 1;
    M_D = IspReaduchar(0x2001); //扩展口配置
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

void DATA_FL(uchar *i) //处理有效数据，分类并处理
{
    uchar idata j[5] = {0xfd, 0x03, 0x90, 0x10, 0xa0};
    if (i[0] == 0x01) //设置参数
    {
        TR0 = 0;
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
        j[3] = i[3];
        j[4] = j[0] + j[1] + j[2] + j[3];
        UART_Send(j, 5);
    }
    else if (i[0] >= 0x10 && i[0] <= 0x1f) //设置定位参数
    {
        i[0] = i[0] - 0x10;
        IspProgramuchar(0x2010 + i[0], i[1]); //定位1 端口选择 2010-201F
        IspProgramuchar(0x2020 + i[0], i[2]); //定位2 端口选择 2020-202F
        j[3] = i[3];
        j[4] = j[0] + j[1] + j[2] + j[3];
        UART_Send(j, 5);
    }
    else if (i[0] == 0x20) //程序段总数
    {
        IspProgramuchar(0x2030, i[1]); //程序0段总段数
        IspProgramuchar(0x2031, i[2]); //程序1段总段数
        IspProgramuchar(0x2032, i[3]); //程序2段总段数
        j[3] = i[4];
        j[4] = j[0] + j[1] + j[2] + j[3];
        UART_Send(j, 5);
    }
    else if (i[0] == 0x21 && i[1] < 170) //程序0段写入
    {
        uchar k, l;
        for (k = 0, l = 9; k < 9; k++, l--)
        {
            IspProgramuchar((i[1] + 1) * 9 - l + 0x2200, i[k + 2]); //循环写入数据 2200-27FF里
        }
        j[3] = i[11];
        j[4] = j[0] + j[1] + j[2] + j[3];
        UART_Send(j, 5);
    }
    else if (i[0] == 0x22 && i[1] < 170) //程序1段写入
    {
        uchar k, l;
        for (k = 0, l = 9; k < 9; k++, l--)
        {
            IspProgramuchar((i[1] + 1) * 9 - l + 0x2800, i[k + 2]); //循环写入数据 2800-2DFF里
        }
        j[3] = i[11];
        j[4] = j[0] + j[1] + j[2] + j[3];
        UART_Send(j, 5);
    }
    else if (i[0] == 0x23 && i[1] < 170) //程序2段写入
    {
        uchar k, l;
        for (k = 0, l = 9; k < 9; k++, l--)
        {
            IspProgramuchar((i[1] + 1) * 9 - l + 0x2e00, i[k + 2]); //循环写入数据 2E00-33FF里
        }
        j[3] = i[11];
        j[4] = j[0] + j[1] + j[2] + j[3];
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
            ISP_CONTR = 0X60; //重启单片机
        }
    }
    else if (i[0] == 0x32) //单段程序启动
    {
        uchar j;
        for (j = 0; j < 19; j++)
        {
            P_Rom[j] = i[j + 1];
        }
        P_Run = 1;
        F_Run = 1;
        F_Step = 1;
        P_Step = 1;
        P_RomFlag = 1;
    }
    else if (i[0] == 0x35) //在线程序启动
    {
        uchar j;
        for (j = 0; j < 19; j++)
        {
            P_Rom[j] = i[j + 1];
        }
        P_Run = 1;
        F_Run = 1;
        F_Step = 1;
        P_Step = 1;
        P_RomFlag = 2;
    }
    else if (i[0] == 0x65) //下载完成 并重启
    {
        if (i[1] == 0x10)
        {
            ISP_CONTR = 0x60; //完全重启单片机
        }
    }
    else if (i[0] == 0xa0) //上传标志
    {
        if (i[1] == 1)
        {
            T_Step = 1;
            F_Flag = 1;
            T_YZ = 1;
        }
    }
    else if (i[0] == 0x90) //数据是否正确
    {
        if (i[1] == SendCRC && F_Flag == 1)
        {
            T_YZ = 1;
            T_Step++;
        }
    }
}

void Data_CL() //接收数据并处理
{
    uchar idata SUM = 0xFD, num, i[22];
    while (R_CL)
    {
        for (num = 1; num < R_Data[1] + 1; num++)
        {
            SUM = SUM + R_Data[num];
        }
        if (SUM == R_Data[num])
        {
            for (num = 0; num < R_Data[1]; num++)
            {
                i[num] = R_Data[num + 2];
            }
            DATA_FL(i);
        }
        R_CL--;
    }
}

void Send_Data(uint n) //数据上传
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
        SendCRC = i[4];
    }
    else if (n == 2)
    {
        i[0] = 0xfd;
        i[1] = 5;
        i[2] = 0x64;
        T_ROM_D0 = IspReaduchar(0x2030);
        if (T_ROM_D0 == 0)
        {
            T_ROM_D0 = 1;
            i[3] = 1;
        }
        else
        {
            i[3] = T_ROM_D0;
        }
        T_ROM_D1 = IspReaduchar(0x2031);
        if (T_ROM_D1 == 0)
        {
            T_ROM_D1 = 1;
            i[4] = 1;
        }
        else
        {
            i[4] = T_ROM_D1;
        }
        T_ROM_D2 = IspReaduchar(0x2032);
        if (T_ROM_D2 == 0)
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
        SendCRC = i[6];
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
        T_YZ = 0;
        SendCRC = i[5];
    }
    else if (n < 19 + T_ROM_D0)
    {
        uchar l, m;
        i[0] = 0xfd;
        i[1] = 12;
        i[2] = 0x61;
        i[3] = n - 18;
        for (l = 0, m = 9; l < 9; l++, m--)
        {
            i[l + 4] = IspReaduchar(i[3] * 9 - m + 0x2200);
        }
        i[13] = i[0] + i[1] + i[2] + i[3] + i[4] + i[5] + i[6] + i[7] + i[8] + i[9] + i[10] + i[11] + i[12];
        UART_Send(i, 14);
        T_YZ = 0;
        SendCRC = i[13];
    }
    else if (n < 19 + T_ROM_D0 + T_ROM_D1)
    {
        uchar l, m;
        i[0] = 0xfd;
        i[1] = 12;
        i[2] = 0x62;
        i[3] = n - 18 - T_ROM_D0;
        for (l = 0, m = 9; l < 9; l++, m--)
        {
            i[l + 4] = IspReaduchar(i[3] * 9 - m + 0x2800);
        }
        i[13] = i[0] + i[1] + i[2] + i[3] + i[4] + i[5] + i[6] + i[7] + i[8] + i[9] + i[10] + i[11] + i[12];
        UART_Send(i, 14);
        T_YZ = 0;
        SendCRC = i[13];
    }
    else if (n < 19 + T_ROM_D0 + T_ROM_D1 + T_ROM_D2)
    {
        uchar l, m;
        i[0] = 0xfd;
        i[1] = 12;
        i[2] = 0x63;
        i[3] = n - 18 - T_ROM_D0 - T_ROM_D1;
        for (l = 0, m = 9; l < 9; l++, m--)
        {
            i[l + 4] = IspReaduchar(i[3] * 9 - m + 0x2e00);
        }
        i[13] = i[0] + i[1] + i[2] + i[3] + i[4] + i[5] + i[6] + i[7] + i[8] + i[9] + i[10] + i[11] + i[12];
        UART_Send(i, 14);
        T_YZ = 0;
        SendCRC = i[13];
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
        F_Flag = 0;
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
        //Test_Send(*(M_Rom + 17));
        //Test_Send(*(M_Rom + 18));
        WaitTime = *(M_Rom + 17) * 256 + *(M_Rom + 18);
        P_Run = 0;
    }
    if (temp1 >= 10)
    {
        temp1 = 0;
        if (WaitTime == 0)
        {
            P_Step++;
            P_Run = 1;
            if (P_Step > F_Step)
            {
                P_Step = 1;
                F_Run = 0;
                if (P_RomFlag == 1)
                {
                    uchar i;
                    for (i = 0; i < 16; i++)
                    {
                        F_Motor[i] = 0;
                    }
                    P_RomFlag = 0;
                }
                else if (P_RomFlag == 2)
                {
                    uchar i[5] = {0xfd, 0x03, 0x71, 0x01, 0x72};
                    UART_Send(i, 5);
                    P_RomFlag = 0;
                }
            }
        }
        else
        {
            WaitTime--;
            //Test_Send(P_Step);
        }
    }
}

void KeyTrigger() //按键触发
{
    if ((XD1 == 1) || (XD2 == 1))
    {
        if ((XD1 == 0) && (XD2 == 1))
        {
            F_ROM_Set = 2;
        }
        else if ((XD1 == 1) && (XD2 == 0))
        {
            F_ROM_Set = 3;
        }
        else
        {
            F_ROM_Set = 1;
        }
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
            XD_Run = 0;
            ISP_CONTR = 0x60; //完全重启单片机
        }
    }
    else
    {
        F_ROM_Temp = F_ROM_Set;
        XD_Run = 0;
    }
    if (YK == 0)
    {
        YK_Run = 1;
        if (YK_Delay >= 100)
        {
            YK_Run = 0;
            if (YK == 0)
            {
                if (F_Step == P_Step)
                {
                    WaitTime = 0;
                }
                F_Run = 1;
            }
        }
    }
    else
    {
        YK_Run = 0;
    }
    if ((HW == 0) && (F_Run == 0) && (P_RomFlag == 0))
    {
        HW_Run = 1;
        if (HW_Delay >= 200)
        {
            HW_Run = 0;
            if ((HW == 0) && (F_Run == 0) && (P_RomFlag == 0))
            {
                F_Run = 1;
                temp1 = 0;
            }
        }
    }
    else
    {
        YK_Run = 0;
    }
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
        if (F_Flag)
        {
            if (T_YZ)
            {
                Send_Data(T_Step);
                temp1 = 0;
            }
            else
            {
                if (temp1 > 100)
                {
                    T_YZ = 1;
                    temp1 = 0;
                }
            }
        }
        else
        {
            KeyTrigger();
            if (F_Run)
            {
                ProgramRun();
                Motor_Set();
            }
            if (T_Flag)
            {
                Send_MotorDW_Flag();
            }
        }
        Data_CL();
    }
}

void T0_Isr() interrupt 1
{
    TH0 = T0Time >> 8;
    TL0 = T0Time;
    TF0 = 0;
    temp1++;
    HW_Run ? (HW_Delay++) : (HW_Delay = 0);
    YK_Run ? (YK_Delay++) : (YK_Delay = 0);
    XD_Run ? (XD_Delay++) : (XD_Delay = 0);
}

/*
void T1_Isr() interrupt 3
{
    TH1 = T1Time >> 8;
    TL1 = T1Time;
    TF1 = 0;
}
*/

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