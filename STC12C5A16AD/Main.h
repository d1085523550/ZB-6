#ifndef __MAIN_H__
#define __MAIN_H__
#define uchar unsigned char
#define uint unsigned int

//库函数
#include <STC12C5A60S2.H>
#include "intrins.h"

//头文件
#include "EEPROM_Iap.H"
#include "HardWare.h"
#include "Motor.h"
#include "State.h"
#include "Data.h"
//#include "Automation.h"

#define FOSC 24000000L                  //晶振频率
#define T0Time 65536 - FOSC / 12 / 100  // T0时钟
#define T1Time 65536 - FOSC / 12 / 1000 // T1时钟

sbit MN_RTX = P3 ^ 5;   //模拟串口 发送口
sbit HW = P3 ^ 3;       //触发口
sbit YK = P3 ^ 2;       //遥控触发口
sbit Power12V = P3 ^ 4; // 12V电源 低有效时开通
sbit XD1 = P3 ^ 6;      //程序选段口1
sbit XD2 = P3 ^ 7;      //程序选段口2

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

//全局变量声明
extern bit F_Run, P_Run, F_Flag, T_Flag, XD_Run;
extern uchar temp1;
extern uchar P_RomFlag;
extern uchar F_Step, P_Step;
extern uchar idata R_Data[24], R_Num, R_CL;
extern uchar P_Rom[19];
extern unsigned long idata WaitTime; //单段运行时间
extern uchar M_D;                    //扩展口选择
extern uchar F_Motor[16];            //电机状态
extern uchar DW_Flag[16];            //定位状态
extern uchar idata DW_XZ[33];        //定位选择
extern uchar DW[17];                 //定位绑定
extern uchar T_ROM_D0;               //程序段0 总段数
extern uchar T_ROM_D1;               //程序段1 总段数
extern uchar T_ROM_D2;               //程序段2 总段数
extern uchar F_ROM_Set;              //程序段选择设置
extern uchar F_ROM_Temp;             //程序段切换检测
extern uint T_Step;                  //上传步骤计数
extern uchar T_YZ;                   //上传程序验证
extern uchar SendCRC;                //数据上传效验码
extern uint YK_Delay;                //遥控延时
extern uint HW_Delay;                //触发延时
extern uint XD_Delay;                //选段延时

#endif