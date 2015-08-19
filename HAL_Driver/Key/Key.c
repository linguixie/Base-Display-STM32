/*******************************************************************************
* Copyright (c) 2015,威鹏自动化科技有限公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-6-4
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：移植需注意:
*           1、KeyActivationTimeSet需在中断中被调用,调用周期越短越好.
*           2、需根据实际情况修改表示长短键的宏.
*           3、Task_Key需被不断调用,以实时监测按键状态.
*           4、要特别区分处理一直按压究竟是处理1次还是表示一直有效(移植重点关注部分)
*           5、缺点:如果按键一直按压表示1次有效.但是如果长按键的检测时间过长,则按键按
*                   压后,必须直到长按键检测时间才会认作按键有效.
*           6、本源文件处理进入菜单后的按键.非菜单界面下的按键请参照Input.c
********************************************************************************/
/*******************************************************************************
*                                    头  文  件
********************************************************************************/
#include "stm32f0xx_conf.h"
#include "GPIO.h"
#include "Key.h"
#include "Infrared.h"
#include "UI.h"

#include "SoftTimer.h"
#include "Valve.h"
#include "Display.h"
/*******************************************************************************
*                               文件内部使用宏定义
********************************************************************************/
 //-数字快速变化延时(单位:毫秒)-
#define DigitChangeTime              100      
#define DigitChange_Delay           (Division(DigitChangeTime, Delay_MiliSecond_Factor))

/*******************************************************************************
*                                 全局函数(变量)声明
********************************************************************************/
signed long DigitChangeTimer = 0;


//-按键按下计时器 用于记录按键按下的时间 根据时间判断按键是长按键还是短按键
// 按键没有按下时该计时器始终为0-
static volatile unsigned int  KeyActivationTime[Key_Num] = {0};

//-按键在未响应前重按下并且复有效只处理一次-
static volatile unsigned char  keyScanEn[Key_Num]         = {0};

//--
unsigned char KeyPressed[Key_Num] = {KEY_UNPRESSED};   
unsigned char PressedType[Key_Num] = {KEY_NONE};

/*******************************************************************************
*                                 静态函数(变量)声明
********************************************************************************/


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void KeyInit(void)
{
    int i = 0;

    for(i = 0; i < Key_Num; i++)
    {
       keyScanEn[i] = 1;
    }

    InsertTimer(&DigitChangeTimer);
}


/*******************************************************************************
* 函数名称: IRKeyMsgGet
* 函数功能: 获取红外遥控器按键消息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
unsigned char IRKeyStateRead(int KeyIndex)
{
    unsigned int KeyPressed = 0;
    unsigned int Code = 0;

    Code = GetIRKeyCode();

    switch(KeyIndex)
    {
    case Key_Inc:
        KeyPressed = (Code == Code_Open);
        break;
    case Key_Dec:
        KeyPressed = (Code == Code_Shut);
        break;
    case Key_Set:
        KeyPressed = (Code == Code_Set);
        break;
    case Key_Back:
        KeyPressed = (Code == Code_Back);
        break;
    case Key_Up:
        KeyPressed = (Code == Code_Up);
        break;
    case Key_Down:
        KeyPressed = (Code == Code_Down);
        break;
    default:
        break;
    }

    if (KeyPressed)
    {
        return KEY_PRESSED;
    }
    else
    {
        return KEY_UNPRESSED;
    }
}


/*******************************************************************************
* 函数名称: KeyMsgGet
* 函数功能: 获取按键消息
* 输入参数: KeyIndex,按键的索引.参见枚举变量Key_Enum
* 输出参数: 无
* 返 回 值: 无
* 其    他: 低电平表示按下
*******************************************************************************/
unsigned char KeyStateRead(int KeyIndex)
{
    unsigned int KeyPressed = {0};

    switch(KeyIndex)
    {
    case Key_Remote:
        KeyPressed = Key_RemoteRead();
        break;
    case Key_Local:
        KeyPressed = Key_LocalRead();
        break;
    case Key_Open:
        KeyPressed = Key_OpenRead();
        break;    
    case Key_Shut:
        KeyPressed = Key_CloseRead();
        break; 
    default:
        break;
    }
 
    if (KeyIndex >= KeyOnBoard_Num)
    {
        KeyPressed = !IRKeyStateRead(KeyIndex);
    }

    if (!KeyPressed)
    {
        return KEY_PRESSED;
    }
    else
    {
        return KEY_UNPRESSED;
    }

}


/*******************************************************************************
* 函数名称: IRKeyMsgGet
* 函数功能: 获取红外遥控器按键消息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void IRKeyMsgGet(unsigned char *pKey, unsigned char *Long_Short)
{
    int i = 0; 

    for (i = KeyOnBoard_Num; i < Key_Num; i++)
    {
        if ((1 == keyScanEn[i]) && (KeyActivationTime[i] >= LONG_KEY_TIME))
        {
            KeyActivationTime[i] = 0;
            keyScanEn[i]         = 0;
          
            SetTimer(DigitChangeTimer, DigitChange_Delay);          

            pKey[i] = KEY_PRESSED;
            Long_Short[i] = LONG_KEY;
    
            UI_Buzz_Long();
        }
        else if ((1 == keyScanEn[i]) && (KeyStateRead(i) == KEY_UNPRESSED) 
                 && (KeyActivationTime[i] >= SHORT_KEY_TIME))
        {
            KeyActivationTime[i] = 0;
            keyScanEn[i]         = 0;
          
            SetTimer(DigitChangeTimer, DigitChange_Delay);

            pKey[i] = KEY_PRESSED;
            Long_Short[i] = SHORT_KEY;

            UI_Buzz_Short();

        }
        //-抖动-
        else if ((KeyStateRead(i) == KEY_UNPRESSED) && (KeyActivationTime[i] < SHORT_KEY_TIME))
        {
            KeyActivationTime[i] = 0;
          
            pKey[i] = KEY_UNPRESSED;
            Long_Short[i] = KEY_NONE;
        }
        else if (keyScanEn[i] == 0)
        {
            if ((i == Key_Inc) || (i == Key_Dec))
            {
                /*-注意和板子按键处理不同:红外按键对于数字的加减无需切换至现场
                  -*/
                switch(PageFunctionIndex)
                {
                case Page_AdjustZero_ID:
                case Page_AdjustFull_ID:
                    break;

                case Page_DeadZone_ID:
                case Page_AdjustOutput4mA_ID:
                case Page_AdjustOutput20mA_ID:
                case Page_ShutCurrent_ID:
                case Page_OpenCurrent_ID:
                case Page_MaxActionTime_ID:
                    if (IsTimeOut(DigitChangeTimer) == 1)
                {
                        SetTimer(DigitChangeTimer, DigitChange_Delay);

                        pKey[i] = KEY_PRESSED;
                        Long_Short[i] = SHORT_KEY;
                }
                else
                { 
                    pKey[i] = KEY_UNPRESSED;
                    Long_Short[i] = KEY_NONE;
                }
                    break;

                default:
                    pKey[i] = KEY_UNPRESSED;
                    Long_Short[i] = KEY_NONE;
                    break;
                }
            }
            else
            { 
                pKey[i] = KEY_UNPRESSED;
                Long_Short[i] = KEY_NONE;
            }
        }
        else
        {
            pKey[i] = KEY_UNPRESSED;
            Long_Short[i] = KEY_NONE;
        }
    }
}


/*******************************************************************************
* 函数名称: KeyMsgGet
* 函数功能: 获取按键消息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void KeyMsgGet(unsigned char *pKey, unsigned char *Long_Short)
{
    int i = 0; 

    for (i = Key_Open; i < KeyOnBoard_Num; i++)
    {
        if ((1 == keyScanEn[i]) && (KeyActivationTime[i] >= LONG_KEY_TIME))
        {
            KeyActivationTime[i] = 0;
            keyScanEn[i]         = 0;
          
            SetTimer(DigitChangeTimer, DigitChange_Delay);

            pKey[i] = KEY_PRESSED;
            Long_Short[i] = LONG_KEY;
    
            UI_Buzz_Long();
        }
        else if ((1 == keyScanEn[i]) && (KeyStateRead(i) == KEY_UNPRESSED) 
                 && (KeyActivationTime[i] >= SHORT_KEY_TIME))
        {
            KeyActivationTime[i] = 0;
            keyScanEn[i]         = 0;
          
            SetTimer(DigitChangeTimer, DigitChange_Delay);

            pKey[i] = KEY_PRESSED;
            Long_Short[i] = SHORT_KEY;

            UI_Buzz_Short();

        }
        //-抖动-
        else if ((KeyStateRead(i) == KEY_UNPRESSED) && (KeyActivationTime[i] < SHORT_KEY_TIME))
        {
            KeyActivationTime[i] = 0;
          
            pKey[i] = KEY_UNPRESSED;
            Long_Short[i] = KEY_NONE;
        }
        else if (keyScanEn[i] == 0)
        {
            if ((i == Key_Open) || (i == Key_Shut))
            {
                /*-只有在现场模式下和无限位开、关情形下才允许一直按压表示按键一直有效-
                  -其他情形,一直按压只处理一次-*/
                if ((Device.Flag.FlagBits.IsInLocalAdjust == 1) && (Device.Flag.FlagBits.IsInDigitAdjust == 0))
                {
                }
                else if ((Device.Flag.FlagBits.IsInLocalAdjust == 1) && (Device.Flag.FlagBits.IsInDigitAdjust == 1))
                {
                    if (IsTimeOut(DigitChangeTimer) == 1)
                    {
                        SetTimer(DigitChangeTimer, DigitChange_Delay);

                        pKey[i] = KEY_PRESSED;
                        Long_Short[i] = SHORT_KEY;
                    }
                    else
                {
                        pKey[i] = KEY_UNPRESSED;
                        Long_Short[i] = KEY_NONE;
                    }
                }
                else
                { 
                    pKey[i] = KEY_UNPRESSED;
                    Long_Short[i] = KEY_NONE;
                }
            }
        }
        else
        {
            pKey[i] = KEY_UNPRESSED;
            Long_Short[i] = KEY_NONE;
        }
    }

    IRKeyMsgGet(pKey, Long_Short);
}


/*******************************************************************************
* 函数名称: KeyActivationTimeSet
* 函数功能: 按键按下时间设置
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void KeyActivationTimeSet(void)
{
    int i = 0;

    for (i = 0; i < Key_Num; i++)
    {
        //-按键按下计时器递增-
        if ((KeyStateRead(i) == KEY_PRESSED) && (1 == keyScanEn[i]))
        {
            if (KeyActivationTime[i] < Key_Pressed_Max_Time)
            {
                KeyActivationTime[i]++;
            }
        }
        if (KeyStateRead(i) == KEY_UNPRESSED)
        {
            keyScanEn[i] = 1;
        }
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Task_Key(void)
{
    //-读按键且将结果保存在KeyPressed数组和PressedType数组中-
    KeyMsgGet(&KeyPressed[0], &PressedType[0]);
}

 
/*************************************END OF FILE*******************************/


