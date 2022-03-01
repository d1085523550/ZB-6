/***********************************************************************************************
对EEPROM进行操作
IspEraseSector(uint addr);                 //擦除某一扇区
IspProgramuchar(uint addr,uchar dat);      //写一字节 参数1为地址 参数2为写入数据
IspReaduchar(uint addr);                   //读一字节 参数1为地址 
************************************************************************************************
STC89C52RC内部EEPROM详细地址表：
第一扇区(参数设置)   第二扇区             第三扇区             第四扇区
起始地址  结束地址   起始地址   结束地址   起始地址   结束地址   起始地址   结束地址
2000h    21FFh     2200h      23FFh     2400h     25FFh      2600h     27FFH
第五扇区            第六扇区             第七扇区              第八扇区
起始地址  结束地址   起始地址   结束地址   起始地址   结束地址   起始地址    结束地址
2800h    29FFh     2A00h      2BFFh     2C00h     2DFFh      2E00h     2FFFh
第九扇区            第十扇区
起始地址  结束地址   起始地址   结束地址
3000h    31ffh      3200h      33ffh
***********************************************************************************************/

#ifndef __EEPROM_ISP_H__
#define __EEPROM_ISP_H__
#include "intrins.h"
#define uchar unsigned char   
#define uint unsigned int

#define CMD_IDLE    0               //空闲模式
#define CMD_READ    1               //ISP字节读命令
#define CMD_PROGRAM 2               //ISP字节编程命令
#define CMD_ERASE   3               //ISP扇区擦除命令
#define ENABLE_ISP  0x81            //if SYSCLK<20MHz
/*----------------------------
关闭ISP
----------------------------*/
void IspIdle()
{
    ISP_CONTR = 0;                  //关闭ISP功能
    ISP_CMD = 0;                    //清除命令寄存器
    ISP_TRIG = 0;                   //清除触发寄存器
    ISP_ADDRH = 0x80;               //将地址设置到非ISP区域
    ISP_ADDRL = 0;
}

/*----------------------------
从ISP/ISP/EEPROM区域读取一字节
----------------------------*/
uchar IspReaduchar(uint addr)
{
    uchar dat;                       //数据缓冲区

    ISP_CONTR = ENABLE_ISP;         //使能ISP
    ISP_CMD = CMD_READ;             //设置ISP命令
    ISP_ADDRL = addr;               //设置ISP低地址
    ISP_ADDRH = addr >> 8;          //设置ISP高地址
		EA=0;
    ISP_TRIG = 0x46;                //写触发命令(0x46)
    ISP_TRIG = 0xb9;                //写触发命令(0xb9)
    _nop_();                        //等待ISP/ISP/EEPROM操作完成
    dat = ISP_DATA;                 //读ISP/ISP/EEPROM数据
		EA=1;
    IspIdle();                      //关闭ISP功能

    return dat;                     //返回
}

/*----------------------------
写一字节数据到ISP/ISP/EEPROM区域
----------------------------*/
void IspProgramuchar(uint addr, uchar dat)
{
    ISP_CONTR = ENABLE_ISP;         //使能ISP
    ISP_CMD = CMD_PROGRAM;          //设置ISP命令
    ISP_ADDRL = addr;               //设置ISP低地址
    ISP_ADDRH = addr >> 8;          //设置ISP高地址
    ISP_DATA = dat;                 //写ISP/ISP/EEPROM数据
		EA=0;
    ISP_TRIG = 0x46;                //写触发命令(0x46)
    ISP_TRIG = 0xb9;                //写触发命令(0xb9)
    _nop_();                        //等待ISP/ISP/EEPROM操作完成
		EA=1;
    IspIdle();
}

/*----------------------------
扇区擦除
----------------------------*/
void IspEraseSector(uint addr)
{
    ISP_CONTR = ENABLE_ISP;         //使能ISP
    ISP_CMD = CMD_ERASE;            //设置ISP命令
    ISP_ADDRL = addr;               //设置ISP低地址
    ISP_ADDRH = addr >> 8;          //设置ISP高地址
    EA=0;
		ISP_TRIG = 0x46;                //写触发命令(0x46)
    ISP_TRIG = 0xb9;                //写触发命令(0xb9)
    _nop_();                        //等待ISP/ISP/EEPROM操作完成
    EA=1;
		IspIdle();
}
#endif