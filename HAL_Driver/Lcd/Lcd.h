/*******************************************************************************
* Copyright (c) 2015,xxx公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-3-16
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：
********************************************************************************/
 
 
#ifndef _LCD_H_
#define _LCD_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*******************************************************************************
*                                  头  文  件
********************************************************************************/
#include "GPIO.h"
 
/*******************************************************************************
*                                  宏  定  义
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

//-空格GBK内码-
#define GBK_Space        0xA3A0
//-自定义的三角符号,码字为GB2312预留的码字-
#define GBK_Triangle     0xAAA0
/*******************************************************************************
*                                  全局变量声明
********************************************************************************/
extern unsigned char SpecicalCharacter[11][4][16];
extern unsigned char g_DisplayBuf[Display_Buf_Size];

/*******************************************************************************
*                                  全局函数声明
********************************************************************************/
void LcdInit(void);
void LcdReset(void);
void ClearScreen(void);
void LcdGPIOInit(void);
void LcdRefresh(int CharacterLineIndex);
void SwapFontBuf2DisplayBuf(unsigned char *FontBuf, unsigned FontBufLen, unsigned char *DisplayBuf, int DisplayBufStartIndex);
void GetDotData(unsigned int Code, unsigned char CharacterType, unsigned char *FontBuf, unsigned char *FontBufLen);
#ifdef __cplusplus
}
#endif
 
#endif /* _LCD_H_ */
 
 
/*************************************END OF FILE*******************************/


