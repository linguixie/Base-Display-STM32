/*******************************************************************************
* Copyright (c) 2105,�����Զ����Ƽ����޹�˾
 *            All rights reserved.
* �������ƣ�
* �� �� �ţ�1.0
* �� �� �ˣ�л�ֹ�
* ����ʱ�䣺2015-3-21
* �� �� ��:
* �޸�ʱ��:
* �޸�˵��:
 ��    ����ʹ��3��STM32�ڲ�Flash��Page��ʵ��һ��1k����������EEPROM���ó����
           ��Ķ�ȡ�ӿ���ȫ��EEPROM�ķ��ʷ������������εײ�Ĳ�����ʹ������
		   page�����洢���ݣ������ݴ洢��ɺ����޸ı�־λ��������Ч�ķ�ֹ
		   д��EEPROMʱ���磬������ݶ�ʧ��
 ע    �⣺���ַ���ÿ��д��EEPROMʱ�������һ��Flashҳ�棬�ʲ���Ƶ���޸ģ�
           �����ڴ洢��ϵϵͳ�������еĹؼ�������
********************************************************************************/
/*******************************************************************************
*                                    ͷ  ��  ��
********************************************************************************/
#include "Valve.h"
#include "EEPROM.h"
#include "stm32f0xx_flash.h"

/****************************************************************************
 �� �� ����STM_EEPROM_PrePage
 ��    ������
 �� �� ֵ����ǰEEPROM��Чpageҳ 
 �� �� �ߣ��ž���
 �������ڣ�2011-06-01
 ��    �������ص�ǰʹ�õ�pageҳ

******************�޸���ʷ***************************************************
 
****************************************************************************/
unsigned int STM_EEPROM_PrePage(void)
{
	unsigned int falg;

	falg = *(unsigned int *)STM32_EEPROM_ADDR_FLAG;     //��ȡ��־λ

	if(falg == PAGE1)
		return PAGE1;								   //page1��Ч

	return PAGENO;                                     //page0��Ч
}

/****************************************************************************
 �� �� ����STM_EEPROM_Write
 ��    ����unsigned char *data  ��д�����ݵĻ������׵�ַ
           unsigned int Addr    Ŀ��EEPROM��ַ
		   unsigned int len     д�����ݳ���
 �� �� ֵ��0                    д��ʧ��
           1                    д��ɹ� 
 �� �� �ߣ��ž���
 �������ڣ�2011-06-01
 ��    ����ʵ�ֶ�ģ��EEPROM��д�����

******************�޸���ʷ***************************************************
 
****************************************************************************/
unsigned int STM_EEPROM_Write(unsigned char *data, unsigned int Addr, 
                              unsigned int len)
{
	unsigned int RealAddrP;
	unsigned int RealAddrN;	
	unsigned short tem;
	int i,j;

	if((Addr+len) > STM32_EEPROM_PAGESIZE)
		return 0;       //������Χ����EEPROM����

	FLASH_Unlock();

	//ȡ�õ�ǰ��Чҳ��ĵ�ַ���������´ε�ҳ���ַ
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
		
	//������Ҫʹ�õ���ҳ��
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


	//������һ��ҳ����Ч
	
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
 �� �� ����STM_EEPROM_Read
 ��    ����unsigned char *data  ��ȡ���ݴ洢�Ļ������׵�ַ
           unsigned int Addr    Ŀ��EEPROM��ַ
		   unsigned int len     ��ȡ���ݳ���
 �� �� ֵ��0                    ��ȡʧ��
           1                    ��ȡ�ɹ� 
 �� �� �ߣ��ž���
 �������ڣ�2011-06-01
 ��    ����ʵ�ֶ�ģ��EEPROM�Ķ�ȡ����

******************�޸���ʷ***************************************************
 
****************************************************************************/
unsigned int STM_EEPROM_Read(unsigned char *data, unsigned int Addr,
                             unsigned int len)
{
	unsigned char *RealAddr;
	int i;

	if((Addr+len) > STM32_EEPROM_PAGESIZE)
		return 0;       //������Χ����EEPROM����

	//ȡ�õ�ǰ��Чҳ��ĵ�ַ
	if(STM_EEPROM_PrePage() == PAGE1)
		RealAddr = (unsigned char *)STM32_EEPROM_ADDR_PAGE1;
	else
		RealAddr = (unsigned char *)STM32_EEPROM_ADDR_PAGE0;
		
	//����EEPROMʵ�ʶ�Ӧ��Flash��ַ
	RealAddr += Addr;
	
	//��ȡ���ݵ�������
	for(i=0;i<len;i++)
	{
		data[i] = RealAddr[i];
	}  
	return 1;
}


/*********************************************************************************************
*������    : void LoadPrtintPara(void)
*����      : ����EEPROM�б������Ϣ�������ǰEEPROM�д洢��Ϣ��Ч���ڼ���Ĭ������
*����      : ��
           :
*����      : ��
*��ע      : ��ʱ�ò����˺���
*����      : 2015-6-26
**********************************************************************************************
*�޸�����  :
*�޸���    :
*����      :
*���      :
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

