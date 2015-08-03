/*******************************************************************************
* Copyright (c) 2105,威鹏自动化科技有限公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-3-12
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：
********************************************************************************/
 
 
#ifndef _KEY_H_
#define _KEY_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*******************************************************************************
*                                  头  文  件
********************************************************************************/
 
/*******************************************************************************
*                                  宏  定  义
********************************************************************************/ 
//-既包括主板上的按键也包含了遥控器的按键-
typedef enum {Key_Remote, Key_Local, Key_Open, Key_Shut, KeyOnBoard_Num, 
              Key_Inc = KeyOnBoard_Num, Key_Dec, Key_Set, Key_Back, Key_Up, Key_Down, Key_Num}Key_Enum;

//-按键总数-
#define KEY_MAX_NUM                     KeyOnBoard_Num

//-按键状态-
#define KEY_UNPRESSED                   0         //-按键未按下-
#define KEY_PRESSED                     1         //-按键按下-

#define LONG_KEY                        5         //-长键-
#define SHORT_KEY                       2         //-短键-
#define KEY_NONE                        0 

//-按键时间与按键状态-
#define LONG_KEY_TIME                   300         //-长按键时间 单位: KeyActivationTimeSet被调用的周期-
#define SHORT_KEY_TIME                  10          //-短按键时间 单位: 同上-
#define Key_Pressed_Max_Time            25000       //-按键按下的最大时间 单位:同上-


/*******************************************************************************
*                                  全局函数(变量)声明
********************************************************************************/

extern unsigned char KeyPressed[Key_Num];   
extern unsigned char PressedType[Key_Num];
unsigned char KeyStateRead(int KeyIndex);
void KeyMsgGet(unsigned char *pKey, unsigned char *Long_Short);
void KeyActivationTimeSet(void);

void KeyInit(void);
void KeyActivationTimeSet(void);

void Task_Key(void);
#ifdef __cplusplus
}
#endif
 
#endif /* _KEY_H_ */
 
 
/*************************************END OF FILE*******************************/


