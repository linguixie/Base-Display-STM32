/*******************************************************************************
* Copyright (c) 2105,威鹏自动化科技有限公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-6-22
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：此源文件中处理非菜单页面下的按键处理.菜单页面下的按键处理请参见Display.c
********************************************************************************/
/*******************************************************************************
*                                    头  文  件
********************************************************************************/
#include "UI.h"
#include "SoftTimer.h"
#include "Valve.h"
#include "Input.h"
#include "Key.h"
#include "Display.h"
#include "Includes.h"

/*******************************************************************************
*                               文件内部使用宏定义
********************************************************************************/
#define Input_Delay       1

/*******************************************************************************
*                                 全局函数(变量)声明
********************************************************************************/

/*******************************************************************************
*                                 静态函数(变量)声明
********************************************************************************/
signed long InputTimer = 0;


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void LocalCheck(void)
{
    static signed char Count_H = 10;
    static signed char Count_L = 10;

    if((KeyStateRead(Key_Local) == KEY_PRESSED))        
    {
        Count_L = 10;

        if (Device.Input.Local == True)
        {
            return;
        }
        if (--Count_H <= 0)
        {
            Device.Input.Local = True;

            if (IsInMenu() == 1)
            {
                switch(PageFunctionIndex)
                {
                case Page_AdjustZero_ID:
                case Page_AdjustFull_ID:

                    Device.Flag.FlagBits.IsInLocalAdjust = 1;
                    break;
                case Page_DeadZone_ID:
                case Page_AdjustOutput4mA_ID:
                case Page_AdjustOutput20mA_ID:
                case Page_ShutCurrent_ID:
                case Page_OpenCurrent_ID:
                case Page_MaxActionTime_ID:
                
                    Device.Flag.FlagBits.IsInDigitAdjust = 1;
                    Device.Flag.FlagBits.IsInLocalAdjust = 1;
                    break;
                default:
                    Device.DstCommMode.CommModeBits.Local = 1;
                    EnterMenu(0);
                    break;
                }
            }
            else
            {
                Device.DstCommMode.CommModeBits.Local = 1;
                EnterMenu(0);
            }
        }

    }
    else
    {
        Count_H = 10;

        if (Device.Input.Local == False)
        {
            return;
        }
        if (--Count_L <= 0)
        {
            Device.Input.Local = False;

            if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
            {
                Device.Flag.FlagBits.IsInLocalAdjust = 0;
                Device.Flag.FlagBits.IsInDigitAdjust = 0;
            }
            else
            {
                Device.DstCommMode.CommModeBits.Local = 0;
            }
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
void RemoteCheck(void)
{
    static signed char Count_H = 10;
    static signed char Count_L = 10;

    if((KeyStateRead(Key_Remote) == KEY_PRESSED))        
    {
        Count_L = 10;

        if (Device.Input.Remote == True)
        {
            return;
        }
        if (--Count_H <= 0)
        {
            Device.Input.Remote = True;

            Device.DstCommMode.CommModeBits.Remote = 1;
            EnterMenu(0);
        }

    }
    else
    {
        Count_H = 10;

        if (Device.Input.Remote == False)
        {
            return;
        }
        if (--Count_L <= 0)
        {
            Device.Input.Remote = False;

            Device.DstCommMode.CommModeBits.Remote = 0;
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
void OpenCheck(void)
{
    static signed char Count_H = 10;
    static signed char Count_L = 10;

    if (IsInMenu() == 0)
    {
        if (Device.CurCommMode.CommModeBits.Local == 1)
        {
            if((KeyStateRead(Key_Open) == KEY_PRESSED) || (KeyStateRead(Key_Inc) == KEY_PRESSED))        
            {
                Count_L = 10;

                if (Device.Input.LocalOpen == True)
                {
                    return;
                }
                if (--Count_H <= 0)
                {
                    Device.Input.LocalOpen = True;
                }

            }
            else
            {
                Count_H = 10;

                if (Device.Input.LocalOpen == False)
                {
                    return;
                }
                if (--Count_L <= 0)
                {
                    Device.Input.LocalOpen = False;
                }
            }
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
void ShutCheck(void)
{
    static signed char Count_H = 10;
    static signed char Count_L = 10;

    if (IsInMenu() == 0)
    {
        if (Device.CurCommMode.CommModeBits.Local == 1)
        {
            if((KeyStateRead(Key_Shut) == KEY_PRESSED) || (KeyStateRead(Key_Dec) == KEY_PRESSED))        
            {
                Count_L = 10;

                if (Device.Input.LocalShut == True)
                {
                    return;
                }
                if (--Count_H <= 0)
                {
                    Device.Input.LocalShut = True;
                }

            }
            else
            {
                Count_H = 10;

                if (Device.Input.LocalShut == False)
                {
                    return;
                }
                if (--Count_L <= 0)
                {
                    Device.Input.LocalShut = False;
                }
            }
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
void StopCheck(void)
{
    static signed char Count_H = 10;
    static signed char Count_L = 10;

    if (IsInMenu() == 0)
    {
        if (Device.CurCommMode.CommModeBits.Local == 1)
        {
            if(KeyStateRead(Key_Back) == KEY_PRESSED)        
            {
                Count_L = 10;

                if (Device.Input.LocalStop == True)
                {
                    return;
                }
                if (--Count_H <= 0)
                {
                    Device.Input.LocalStop = True;
                }

            }
            else
            {
                Count_H = 10;

                if (Device.Input.LocalStop == False)
                {
                    return;
                }
                if (--Count_L <= 0)
                {
                    Device.Input.LocalStop = False;
                }
            }
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
void InputInit(void)
{
    InsertTimer(&InputTimer);
    SetTimer(InputTimer, Input_Delay);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Task_Input(void)
{
    static unsigned char Count = 0;

    if (IsTimeOut(InputTimer) == 0)
    {
        return;
    }
    SetTimer(InputTimer, Input_Delay);

    switch(Count++)
    {
    case 0:
        LocalCheck();        //-必须首先判断现场还是远程-
        RemoteCheck();
        break;
    case 1:
        OpenCheck();
        ShutCheck();
        StopCheck();

        if (IsInMenu() == 0)
        {
            if (Device.Input.LocalOpen == True)
            {
                Valve.Operation.Operation = Operation_Open;
            }
            else if (Device.Input.LocalShut == True)
            {
                Valve.Operation.Operation = Operation_Shut;
            }
            else if (Device.Input.LocalStop == True)
            {
                if (Valve.Status.StatusBits.Opening == 1)
                {
                    Valve.Operation.Operation = Operation_Shut;
                }
                else if (Valve.Status.StatusBits.Shutting == 1)
                {
                    Valve.Operation.Operation = Operation_Open;
                }
                else
                {
                }
            }
            else
            {
                Valve.Operation.Operation = Operation_None;
            } 
        }
        Count = 0;
        break;
    default:
        break;
    }
}


 
/*************************************END OF FILE*******************************/

