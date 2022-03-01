#include "EEPROM_Iap.h"

/*----------------------------
关闭IAP
----------------------------*/
void IapIdle()
{
    IAP_CONTR = 0;    //关闭IAP功能
    IAP_CMD = 0;      //清除命令寄存器
    IAP_TRIG = 0;     //清除触发寄存器
    IAP_ADDRH = 0x80; //将地址设置到非IAP区域
    IAP_ADDRL = 0;
}

/*----------------------------
从IAP/IAP/EEPROM区域读取一字节
----------------------------*/
uchar IapReaduchar(uint addr)
{
    uchar dat; //数据缓冲区

    IAP_CONTR = ENABLE_IAP; //使能IAP
    IAP_CMD = CMD_READ;     //设置IAP命令
    IAP_ADDRL = addr;       //设置IAP低地址
    IAP_ADDRH = addr >> 8;  //设置IAP高地址
    EA = 0;
    IAP_TRIG = 0x5a; //写触发命令(0x46)
    IAP_TRIG = 0xa5; //写触发命令(0xb9)
    _nop_();         //等待IAP/IAP/EEPROM操作完成
    dat = IAP_DATA;  //读IAP/IAP/EEPROM数据
    EA = 1;
    IapIdle(); //关闭IAP功能

    return dat; //返回
}

/*----------------------------
写一字节数据到IAP/IAP/EEPROM区域
----------------------------*/
void IapProgramuchar(uint addr, uchar dat)
{
    IAP_CONTR = ENABLE_IAP; //使能IAP
    IAP_CMD = CMD_PROGRAM;  //设置IAP命令
    IAP_ADDRL = addr;       //设置IAP低地址
    IAP_ADDRH = addr >> 8;  //设置IAP高地址
    IAP_DATA = dat;         //写IAP/IAP/EEPROM数据
    EA = 0;
    IAP_TRIG = 0x5a; //写触发命令(0x46)
    IAP_TRIG = 0xa5; //写触发命令(0xb9)
    _nop_();         //等待IAP/IAP/EEPROM操作完成
    EA = 1;
    IapIdle();
}

/*----------------------------
扇区擦除
----------------------------*/
void IapEraseSector(uint addr)
{
    IAP_CONTR = ENABLE_IAP; //使能IAP
    IAP_CMD = CMD_ERASE;    //设置IAP命令
    IAP_ADDRL = addr;       //设置IAP低地址
    IAP_ADDRH = addr >> 8;  //设置IAP高地址
    EA = 0;
    IAP_TRIG = 0x5a; //写触发命令(0x46)
    IAP_TRIG = 0xa5; //写触发命令(0xb9)
    _nop_();         //等待IAP/IAP/EEPROM操作完成
    EA = 1;
    IapIdle();
}