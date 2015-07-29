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
* 其    他：
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
void ModeSwitch(void)
{
    Device.Error.ErrorByte      = 0x00;
    Valve.Error.ErrorByte       = 0x00; 
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void LocalCheck(void)
{
    static unsigned char PreControlMode = 0;
    unsigned char ControlModeChanged = 0;

    if (PreControlMode != Device.Mode.ControlMode)
    {
        PreControlMode = Device.Mode.ControlMode;
        ControlModeChanged = 1;
    }

    if(KeyPressed[Key_Local] == KEY_PRESSED)        
    {
        EnterMenu(0);
        Device.Mode.ControlModeBits.Local = 1;

        if (ControlModeChanged == 1)
        {
             ModeSwitch();
        }
    }
    else
    {
        Device.Mode.ControlModeBits.Local = 0;

        if ((Device.Mode.ControlModeBits.Remote == 0) && (Device.Mode.ControlModeBits.Local == 0))
        {
            EnterMenu(1);
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
    static unsigned char PreControlMode = 0;
    unsigned char ControlModeChanged = 0;

    if (PreControlMode != Device.Mode.ControlMode)
    {
        PreControlMode = Device.Mode.ControlMode;
        ControlModeChanged = 1;
    }

    if(KeyPressed[Key_Remote] == KEY_PRESSED)        
    {
        EnterMenu(0);
        Device.Mode.ControlModeBits.Remote = 1;

        if (ControlModeChanged == 1)
        {
            ModeSwitch();
        }
    }
    else
    {
        Device.Mode.ControlModeBits.Remote = 0;

        if ((Device.Mode.ControlModeBits.Remote == 0) && (Device.Mode.ControlModeBits.Local == 0))
        {
            EnterMenu(1);
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
    static signed char Count_H = 0;
    static signed char Count_L = 0;

    if (IsInMenu() == 0)
    {
        if (Device.Mode.ControlModeBits.Local == 1)
        {
            if((KeyStateRead(Key_ESC) == KEY_PRESSED) || (KeyStateRead(Key_Open) == KEY_PRESSED))        
            {
                Count_L = 10;

                if (Device.Input.Open == True)
                {
                    return;
                }
                if (--Count_H <= 0)
                {
                    Device.Input.Open = True;
                }

            }
            else
            {
                Count_H = 10;

                if (Device.Input.Open == False)
                {
                    return;
                }
                if (--Count_L <= 0)
                {
                    Device.Input.Open = False;
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
    static signed char Count_H = 0;
    static signed char Count_L = 0;

    if (IsInMenu() == 0)
    {
        if (Device.Mode.ControlModeBits.Local == 1)
        {
            if((KeyStateRead(Key_Next) == KEY_PRESSED) || (KeyStateRead(Key_Shut) == KEY_PRESSED))        
            {
                Count_L = 10;

                if (Device.Input.Shut == True)
                {
                    return;
                }
                if (--Count_H <= 0)
                {
                    Device.Input.Shut = True;
                }

            }
            else
            {
                Count_H = 10;

                if (Device.Input.Shut == False)
                {
                    return;
                }
                if (--Count_L <= 0)
                {
                    Device.Input.Shut = False;
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
    static signed char Count_H = 0;
    static signed char Count_L = 0;

    if (IsInMenu() == 0)
    {
        if (Device.Mode.ControlModeBits.Local == 1)
        {
            if(KeyStateRead(Key_OK) == KEY_PRESSED)        
            {
                Count_L = 10;

                if (Device.Input.Stop == True)
                {
                    return;
                }
                if (--Count_H <= 0)
                {
                    Device.Input.Stop = True;
                }

            }
            else
            {
                Count_H = 10;

                if (Device.Input.Stop == False)
                {
                    return;
                }
                if (--Count_L <= 0)
                {
                    Device.Input.Stop = False;
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
        OpenCheck();
        ShutCheck();
        StopCheck();

        if (IsInMenu() == 0)
        {
            if (Device.Input.Open == True)
            {
                Valve.Operation.Operation = Operation_Open;
            }
            else if (Device.Input.Shut == True)
            {
                Valve.Operation.Operation = Operation_Shut;
            }
            else if (Device.Input.Stop == True)
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

