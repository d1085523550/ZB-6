#include "automation.h"

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
