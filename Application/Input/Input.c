/*******************************************************************************
* Copyright (c) 2105,�����Զ����Ƽ����޹�˾
 *            All rights reserved.
* �������ƣ�
* �� �� �ţ�1.0
* �� �� �ˣ�л�ֹ�
* ����ʱ�䣺2015-6-22
* �� �� ��:
* �޸�ʱ��:
* �޸�˵��:
* ��    ������Դ�ļ��д���ǲ˵�ҳ���µİ�������.�˵�ҳ���µİ���������μ�Display.c
********************************************************************************/
/*******************************************************************************
*                                    ͷ  ��  ��
********************************************************************************/
#include "UI.h"
#include "SoftTimer.h"
#include "Valve.h"
#include "Input.h"
#include "Key.h"
#include "Display.h"
#include "Includes.h"

/*******************************************************************************
*                               �ļ��ڲ�ʹ�ú궨��
********************************************************************************/
#define Input_Delay       1

/*******************************************************************************
*                                 ȫ�ֺ���(����)����
********************************************************************************/

/*******************************************************************************
*                                 ��̬����(����)����
********************************************************************************/
signed long InputTimer = 0;


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void InputInit(void)
{
    InsertTimer(&InputTimer);
    SetTimer(InputTimer, Input_Delay);
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
        LocalCheck();        //-���������ж��ֳ�����Զ��-
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

