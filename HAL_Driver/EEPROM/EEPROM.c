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
 描    述：使用3个STM32内部Flash的Page，实现一个1k容量的虚拟EEPROM，该程序对
           外的读取接口完全与EEPROM的访问方法，可以屏蔽底层的操作。使用两个
		   page轮流存储数据，当数据存储完成后再修改标志位，可以有效的防止
		   写入EEPROM时掉电，造成数据丢失。
 注    意：该种方法每次写入EEPROM时都会擦除一次Flash页面，故不能频繁修改，
           仅用于存储关系系统正常运行的关键参数。
********************************************************************************/
/*******************************************************************************
*                                    头  文  件
********************************************************************************/
#include "Valve.h"
#include "EEPROM.h"
#include "stm32f0xx_flash.h"

/****************************************************************************
 函 数 名：STM_EEPROM_PrePage
 参    数：无
 返 回 值：当前EEPROM有效page页 
 创 建 者：张军杰
 创建日期：2011-06-01
 描    述：返回当前使用的page页

******************修改历史***************************************************
 
****************************************************************************/
unsigned int STM_EEPROM_PrePage(void)
{
	unsigned int falg;

	falg = *(unsigned int *)STM32_EEPROM_ADDR_FLAG;     //读取标志位

	if(falg == PAGE1)
		return PAGE1;								   //page1有效

	return PAGENO;                                     //page0有效
}

/****************************************************************************
 函 数 名：STM_EEPROM_Write
 参    数：unsigned char *data  需写入数据的缓冲区首地址
           unsigned int Addr    目标EEPROM地址
		   unsigned int len     写入数据长度
 返 回 值：0                    写入失败
           1                    写入成功 
 创 建 者：张军杰
 创建日期：2011-06-01
 描    述：实现对模拟EEPROM的写入操作

******************修改历史***************************************************
 
****************************************************************************/
unsigned int STM_EEPROM_Write(unsigned char *data, unsigned int Addr, 
                              unsigned int len)
{
	unsigned int RealAddrP;
	unsigned int RealAddrN;	
	unsigned short tem;
	int i,j;

	if((Addr+len) > STM32_EEPROM_PAGESIZE)
		return 0;       //操作范围超出EEPROM容量

	FLASH_Unlock();

	//取得当前有效页面的地址和轮流到下次的页面地址
	if(STM_EEPROM_PrePage() == PAGE1)
	{
		RealAddrP = STM32_EEPROM_ADDR_PAGE1;
		RealAddrN = STM32_EEPROM_ADDR_PAGE0;
	}
	else
	{
		RealAddrP = STM32_EEPROM_ADDR_PAGE0;
		RealAddrN = STM32_EEPROM_ADDR_PAGE1;
	}
		
	//擦除将要使用到的页面
	FLASH_ErasePage(RealAddrN);

	i = 0;
	j = 0;
	while((i+1) < Addr)
	{
		tem = *(unsigned short*)(RealAddrP+i);
		FLASH_ProgramHalfWord(RealAddrN+i,tem);
		i += 2;
	}

	if(i < Addr)
	{
		tem = *(unsigned char*)(RealAddrP+i) + data[j] * 256;
		FLASH_ProgramHalfWord(RealAddrN+i,tem);
		i += 2;
		j += 1;
	}

	while((i+1) < (Addr+len))
	{
		tem = data[j] + data[j+1] * 256;
		FLASH_ProgramHalfWord(RealAddrN+i,tem);
		i += 2;
		j += 2;
	}

	if(i < (Addr+len))
	{
		tem = *(unsigned char*)(RealAddrP+i+1)*256 + data[j];
		FLASH_ProgramHalfWord(RealAddrN+i,tem);
		i += 2;
	}

	while(i < STM32_EEPROM_PAGESIZE)
	{
		tem = *(unsigned short*)(RealAddrP+i);
		FLASH_ProgramHalfWord(RealAddrN+i,tem);
		i += 2;
	}


	//设置另一个页面有效
	
	if(STM_EEPROM_PrePage() == PAGE1)
	{
		FLASH_ErasePage(STM32_EEPROM_ADDR_FLAG);
		FLASH_ProgramWord(STM32_EEPROM_ADDR_FLAG, PAGE0);
	}
	else
	{
		FLASH_ErasePage(STM32_EEPROM_ADDR_FLAG);
		FLASH_ProgramWord(STM32_EEPROM_ADDR_FLAG, PAGE1);
	}

	FLASH_Lock();
	  
	return 1;
}

/****************************************************************************
 函 数 名：STM_EEPROM_Read
 参    数：unsigned char *data  读取数据存储的缓冲区首地址
           unsigned int Addr    目标EEPROM地址
		   unsigned int len     读取数据长度
 返 回 值：0                    读取失败
           1                    读取成功 
 创 建 者：张军杰
 创建日期：2011-06-01
 描    述：实现对模拟EEPROM的读取操作

******************修改历史***************************************************
 
****************************************************************************/
unsigned int STM_EEPROM_Read(unsigned char *data, unsigned int Addr,
                             unsigned int len)
{
	unsigned char *RealAddr;
	int i;

	if((Addr+len) > STM32_EEPROM_PAGESIZE)
		return 0;       //操作范围超出EEPROM容量

	//取得当前有效页面的地址
	if(STM_EEPROM_PrePage() == PAGE1)
		RealAddr = (unsigned char *)STM32_EEPROM_ADDR_PAGE1;
	else
		RealAddr = (unsigned char *)STM32_EEPROM_ADDR_PAGE0;
		
	//虚拟EEPROM实际对应的Flash地址
	RealAddr += Addr;
	
	//读取数据到缓冲区
	for(i=0;i<len;i++)
	{
		data[i] = RealAddr[i];
	}  
	return 1;
}


/*********************************************************************************************
*函数名    : void LoadPrtintPara(void)
*功能      : 加载EEPROM中保存的信息，如果当前EEPROM中存储信息无效，在加载默认配置
*输入      : 无
           :
*返回      : 无
*备注      : 暂时用不到此函数
*日期      : 2015-6-26
**********************************************************************************************
*修改内容  :
*修改人    :
*日期      :
*结果      :
*********************************************************************************************/
void LoadParaFromEEPROM(void)
{
    unsigned int KeyValue = 0;
    unsigned int TempData = 0;

	STM_EEPROM_Read((unsigned char *)&KeyValue, EEPROM_KEY_ADDR, sizeof(KeyValue));

    if (KeyValue != EEPROM_KEY_VALUE)
    {
        TempData = EEPROM_KEY_VALUE;
        STM_EEPROM_Write((unsigned char *)&TempData, EEPROM_KEY_ADDR, sizeof(TempData));

        Device.Para.LanguageType = Default_LanguageType;
        STM_EEPROM_Write((unsigned char *)&Device.Para.LanguageType, EEPROM_ADDR_LANGUAGETYPE, sizeof(Device.Para.LanguageType));
    }
    else
    {
        STM_EEPROM_Read((unsigned char *)&Device.Para.LanguageType, EEPROM_ADDR_LANGUAGETYPE, sizeof(Device.Para.LanguageType));
    }
}


 
/*************************************END OF FILE*******************************/

