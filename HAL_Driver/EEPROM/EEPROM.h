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
* ��    ����
********************************************************************************/
 
 
#ifndef _EEPROM_H_
#define _EEPROM_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*******************************************************************************
*                                  ͷ  ��  ��
********************************************************************************/
 
/*******************************************************************************
*                                  ��  ��  ��
********************************************************************************/
#define  STM32_EEPROM_ADDR        0x0800F400   //61K��ʹ��62 63 64
#define  STM32_EEPROM_PAGESIZE    0x400        //ҳ��С  1K

#define  STM32_EEPROM_ADDR_START  STM32_EEPROM_ADDR   //����EEPROMʹ��Flash���׵�ַ
#define  STM32_EEPROM_ADDR_PAGE0  STM32_EEPROM_ADDR_START
                                               //page0��ַ
#define  STM32_EEPROM_ADDR_PAGE1  (STM32_EEPROM_ADDR_START+STM32_EEPROM_PAGESIZE)
                                               //page1��ַ
#define  STM32_EEPROM_ADDR_FLAG   (STM32_EEPROM_ADDR_START+STM32_EEPROM_PAGESIZE*2)
                                               //��־λ��ַ
#define  PAGE0                    0x12345678   //page0��Ч
#define  PAGE1                    0x87654321   //page1��Ч
#define  PAGENO                   0xffffffff   //û��page��Ч


typedef enum
{
    Language_CN,
    Language_EN
}Language_TypeDef;


/*-EEPROM��ַ����-*/
#define EEPROM_KEY_ADDR                 0x00          //-EEPROM��ֵ����ʼ��ַ-
#define EEPROM_KEY_VALUE                0x12345678    //-EEPROM�ļ�ֵ- 

#define EEPROM_ADDR_LANGUAGETYPE        0x04


/*-EEPROM�б���Ĭ��ֵ-*/
#define Default_LanguageType            Language_EN
/*******************************************************************************
*                                  ȫ�ֺ���(����)����
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


