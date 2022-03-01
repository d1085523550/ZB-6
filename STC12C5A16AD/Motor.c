#include "Motor.h"

void MotorInit()
{
    P0M0 = 0xFF;
    P0M1 = 0;
    P1M0 = 0;
    P1M1 = 0xFF;
    P2M0 = 0;
    P2M1 = 0;
    P3M0 = 0;
    P3M1 = 0;
    P_Run = 1;
    P_Step = 1;
    M_D = IapReaduchar(0x2001); //扩展口配置
    (M_D & 0x80 / 0x80) ? ((M_D1 = 0), (P2M0 |= 0x01)) : ((M_D1 = 1), (P2M1 |= 0x01));
    (M_D & 0x40 / 0x40) ? ((M_D2 = 0), (P2M0 |= 0x02)) : ((M_D2 = 1), (P2M1 |= 0x02));
    (M_D & 0x20 / 0x20) ? ((M_D3 = 0), (P2M0 |= 0x04)) : ((M_D3 = 1), (P2M1 |= 0x04));
    (M_D & 0x10 / 0x10) ? ((M_D4 = 0), (P2M0 |= 0x08)) : ((M_D4 = 1), (P2M1 |= 0x08));
    (M_D & 0x08 / 0x08) ? ((M_D5 = 0), (P2M0 |= 0x10)) : ((M_D5 = 1), (P2M1 |= 0x10));
    (M_D & 0x04 / 0x04) ? ((M_D6 = 0), (P2M0 |= 0x20)) : ((M_D6 = 1), (P2M1 |= 0x20));
    (M_D & 0x02 / 0x02) ? ((M_D7 = 0), (P2M0 |= 0x40)) : ((M_D7 = 1), (P2M1 |= 0x40));
    (M_D & 0x01 / 0x01) ? ((M_D8 = 0), (P2M0 |= 0x80)) : ((M_D8 = 1), (P2M1 |= 0x80));
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

void PositioningSignal()
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
}

void Motor_Set() //电机启停设置
{
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
