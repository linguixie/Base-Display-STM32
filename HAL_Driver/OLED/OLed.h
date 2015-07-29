/*******************************************************************************
* Copyright (c) 2015,�����Զ����Ƽ����޹�˾
 *            All rights reserved.
* �������ƣ�
* �� �� �ţ�1.0
* �� �� �ˣ�л�ֹ�
* ����ʱ�䣺2015-6-4
* �� �� ��:
* �޸�ʱ��:
* �޸�˵��:
* ��    ����
********************************************************************************/
 
 
#ifndef _OLED_H_
#define _OLED_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*******************************************************************************
*                                  ͷ  ��  ��
********************************************************************************/
 
 
/*******************************************************************************
*                                  ��  ��  ��
********************************************************************************/
#define PercentIndex             10

#define Display_Buf_Size         256
#define Size_Per_Line            256

#define CN_Character             0
#define EN_Character             1

#define CN_Character_Size        32
#define EN_Character_Size        16

#define CN_X_SIZE                16
#define CN_Y_SIZE                16

#define EN_X_SIZE_Shift_Count    3
#define EN_X_SIZE                8
#define EN_Y_SIZE                16



#define SIZE        16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	

//-�ո�GBK����-
#define GBK_Space        0xA3A0
//-�Զ�������Ƿ���,����ΪGB2312Ԥ��������-
#define GBK_Triangle     0xAAA0
/*******************************************************************************
*                                  ȫ�ֱ�������
********************************************************************************/
extern unsigned char SpecicalCharacter[11][4][16];
extern unsigned char g_DisplayBuf[Display_Buf_Size];
 
/*******************************************************************************
*                                  ȫ�ֺ�������
********************************************************************************/
void OLedInit(void);
void OLED_Reset(void);
void OLED_Refresh(int CharacterLineIndex);
void GetDotData(unsigned int Code, unsigned char CharacterType, unsigned char *FontBuf, unsigned char *FontBufLen);
void SwapFontBuf2DisplayBuf(unsigned char *FontBuf, unsigned FontBufLen, unsigned char *DisplayBuf, int DisplayBufStartIndex);
void OLED_ShowString(unsigned  char x, unsigned char y, unsigned char *chr);
 
#ifdef __cplusplus
}
#endif
 
#endif /* _OLED_H_ */
 
 
/*************************************END OF FILE*******************************/


