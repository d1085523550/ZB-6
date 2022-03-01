/***********************************************************************************************
对EEPROM进行操作
IapEraseSector(uint addr);                 //擦除某一扇区
IapProgramuchar(uint addr,uchar dat);      //写一字节 参数1为地址 参数2为写入数据
IapReaduchar(uint addr);                   //读一字节 参数1为地址
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
************************************************************************************************
STC12C5A16AD内部EEPROM地址 0000H-B3FFH
STC12C5A60S2内部EEPROM地址 0000H-03FFH
***********************************************************************************************/
#ifndef __EEPROM_IAP_H__
#define __EEPROM_IAP_H__

#include "Main.h"

#define CMD_IDLE 0      //空闲模式
#define CMD_READ 1      // IAP字节读命令
#define CMD_PROGRAM 2   // IAP字节编程命令
#define CMD_ERASE 3     // IAP扇区擦除命令
#define ENABLE_IAP 0x80 // if SYSCLK<40MHz

uchar IapReaduchar(uint addr);
void IapProgramuchar(uint addr, uchar dat);
void IapEraseSector(uint addr);

#endif