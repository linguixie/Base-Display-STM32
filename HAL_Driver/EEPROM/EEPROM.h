/*******************************************************************************
* Copyright (c) 2105,威鹏自动化科技有限公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-3-21
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：
********************************************************************************/
 
 
#ifndef _EEPROM_H_
#define _EEPROM_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*******************************************************************************
*                                  头  文  件
********************************************************************************/
 
/*******************************************************************************
*                                  宏  定  义
********************************************************************************/
#define  STM32_EEPROM_ADDR        0x0800F400   //61K起，使用62 63 64
#define  STM32_EEPROM_PAGESIZE    0x400        //页大小  1K

#define  STM32_EEPROM_ADDR_START  STM32_EEPROM_ADDR   //虚拟EEPROM使用Flash的首地址
#define  STM32_EEPROM_ADDR_PAGE0  STM32_EEPROM_ADDR_START
                                               //page0地址
#define  STM32_EEPROM_ADDR_PAGE1  (STM32_EEPROM_ADDR_START+STM32_EEPROM_PAGESIZE)
                                               //page1地址
#define  STM32_EEPROM_ADDR_FLAG   (STM32_EEPROM_ADDR_START+STM32_EEPROM_PAGESIZE*2)
                                               //标志位地址
#define  PAGE0                    0x12345678   //page0有效
#define  PAGE1                    0x87654321   //page1有效
#define  PAGENO                   0xffffffff   //没有page有效


typedef enum
{
    Language_CN,
    Language_EN
}Language_TypeDef;


/*-EEPROM地址分配-*/
#define EEPROM_KEY_ADDR                 0x00          //-EEPROM键值的起始地址-
#define EEPROM_KEY_VALUE                0x12345678    //-EEPROM的键值- 

#define EEPROM_ADDR_LANGUAGETYPE        0x04


/*-EEPROM中变量默认值-*/
#define Default_LanguageType            Language_EN
/*******************************************************************************
*                                  全局函数(变量)声明
********************************************************************************/
unsigned int STM_EEPROM_Write(unsigned char *data, unsigned int Addr, 
                              unsigned int len);

unsigned int STM_EEPROM_Read(unsigned char *data, unsigned int Addr, 
                              unsigned int len);
void LoadParaFromEEPROM(void);
#ifdef __cplusplus
}
#endif
 
#endif /* _EEPROM_H_ */
 
 
/*************************************END OF FILE*******************************/


