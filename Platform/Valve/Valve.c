/*******************************************************************************
* Copyright (c) 2105,�����Զ����Ƽ����޹�˾
 *            All rights reserved.
* �������ƣ�
* �� �� �ţ�1.0
* �� �� �ˣ�л�ֹ�
* ����ʱ�䣺2015-3-28
* �� �� ��:
* �޸�ʱ��:
* �޸�˵��:
* ��    ����
********************************************************************************/
/*******************************************************************************
*                                    ͷ  ��  ��
********************************************************************************/
#include "Valve.h"
#include "Math.h"
#include "SoftTimer.h"


/*******************************************************************************
*                               �ļ��ڲ�ʹ�ú궨��
********************************************************************************/
#define Action_Delay     (Multiplication(Device.Para.MaxActionTime, 500))

/*******************************************************************************
*                                 ȫ�ֺ���(����)����
********************************************************************************/
Device_T Device;
Valve_T  Valve;

Valve_T *ValveArray[ValveNum];
/*******************************************************************************
*                                 ��̬����(����)����
********************************************************************************/


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void ValveInit(void)
{
    int i = 0;
    Valve_T *pValve;

    ValveArray[0] = &Valve;

    for (i = 0; i < ValveNum; i++)
    {
        pValve = ValveArray[i];
        pValve->Operation.Operation              = Operation_None;
        pValve->Operation.DstOpening             = 0;
        pValve->Adjust.Adjust0.Adjust0Byte       = 0;
        pValve->Adjust.Adjust1.Adjust1Byte       = 0;
        pValve->AdjustInfo.AdjustInfoByte        = 0;
        pValve->Action.PreAction                 = Action_None;
        pValve->Action.Action                    = Action_None;
        pValve->Status.StatusByte                = 0x00;         
        pValve->Error.ErrorByte                  = 0x00;
        pValve->MiscInfo.ActionTimer             = 0;
        pValve->MiscInfo.OpenedTimer             = 0;
        pValve->MiscInfo.ShuttedTimer            = 0;
        pValve->MiscInfo.CurOpening              = 0;
        pValve->MiscInfo.PositionADValue         = 0;
        pValve->MiscInfo.In4_20mA                = 0;
    }

    Device.Operation.PreOperation                = Operation_None;
    Device.Operation.Operation                   = Operation_None;
    Device.Action.PreAction                      = Action_None;
    Device.Action.Action                         = Action_None;
    Device.Mode.ControlMode                      = ControlMode_None;
    Device.Status.Status                         = 0;
    Device.Error.ErrorByte                       = 0x00;

    Device.MiscInfo.RunRemainder                 = 0x00;
    Device.MiscInfo.RestRemainder                = 0x00;
    Device.MiscInfo.WorkTimer                    = 0x00;
    Device.MiscInfo.RestTimer                    = 0x00;
    Device.MiscInfo.NeedReset                    = 0x00;

    Device.Input.Open                            = Input_Invalid;
    Device.Input.Shut                            = Input_Invalid;
    Device.Input.Stop                            = Input_Invalid;

    Device.Mode.ControlModeBits.Local            = 1;
    Device.Para.LocalMode                        = ControlMode_LocalJog;
    Device.Para.RemoteANMode                     = ControlMode_RemoteJog;
    Device.Para.MaxActionTime                    = 0;
    Device.Status.ESDStatus                      = 0;
    Device.Status.CurMode                        = ControlMode_RemoteJog;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void DeviceSched(Device_T *pDevice)
{
    unsigned char IsFirstAction = 0;

    if (pDevice->Operation.PreOperation != pDevice->Operation.Operation)
    {
        pDevice->Operation.PreOperation = pDevice->Operation.Operation;
        IsFirstAction = 1;
    }

    switch(pDevice->Operation.Operation)
    {
    case Operation_Run:
        //-!!!ע���DeviceProcess�еĲ�ͬ,����IsFirstAction == 1ʱ�Ÿı�Action-
        if (IsFirstAction == 1)
        {
            pDevice->Action.Action = Action_Run;
        }
        break;
    case Operation_Stop:
        if (IsFirstAction == 1)
        {
            pDevice->Action.Action = Action_Stop;
        }
        break;
    default:
        break;
    }
}

/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void DeviceProcess(Device_T *pDevice)
{
    int i = 0;
    unsigned char PreAction = Action_None;
    unsigned char IsFirstAction = 0;

    if (pDevice->Action.PreAction != pDevice->Action.Action)
    {
        PreAction = pDevice->Action.PreAction;

        pDevice->Action.PreAction = pDevice->Action.Action;
        IsFirstAction = 1;
    }

    switch(pDevice->Action.Action)
    {
    case Action_Reset:
        if (IsFirstAction == 1)
        {
        }
        if (Device.Error.ErrorByte != 0)
        {
            //--
            pDevice->Action.Action = Action_Stop;
            break;
        }
        Device.Status.Status = Status_Reset;
        break;
    case Action_Run:
        if (IsFirstAction == 1)
        {
        }
        if (Device.Error.ErrorByte != 0)
        {
            //--
            pDevice->Action.Action = Action_Stop;
            break;
        }
        Device.Status.Status = Status_Run;
        break;
    case Action_Stop:
        if (IsFirstAction == 1)
        {
            if (PreAction == Action_Run)
            {
                pDevice->Action.Action = Action_Reset;
                break;
            }
        }   
        
        for (i = 0; i < ValveNum; i++)
        {
            ValveArray[i]->Action.Action = Action_Stop;
        }
        Device.Status.Status = Status_Stop;
        break;
    default:
        break;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void ValveProcess(Device_T *pDevice, Valve_T *pValve)
{
    unsigned char IsFirstAction = 0;

    if (pValve->Action.PreAction != pValve->Action.Action)
    {
        pValve->Action.PreAction = pValve->Action.Action;
        IsFirstAction = 1;
    }
    
    switch(pValve->Action.Action)
    {
    case Action_Open:
        if (IsFirstAction)
        {
            //-�����򿪶�ʱ��-
            SetTimer(pValve->MiscInfo.ActionTimer, Action_Delay);
            InsertTimer(&(pValve->MiscInfo.ActionTimer));
        }
        if ((pValve->Error.ErrorByte & 0x55) != 0)
        {
            pValve->Action.Action = Action_Stop;
            break;
        }
        if (IsTimeOut(pValve->MiscInfo.ActionTimer) == 1)
        {
            pValve->Error.ErrorBits.OpenTimeout = 1;
            pValve->Action.Action = Action_Stop;
            break;
        }
        if (pValve->Status.StatusBits.OpenLimit == 1)   
        {
            pValve->Action.Action = Action_Stop;
            break;
        }
        pValve->Status.StatusBits.Opening = 1;
        break;
    case Action_Shut:
        if (IsFirstAction)
        {
            //-�����򿪶�ʱ��-
            SetTimer(pValve->MiscInfo.ActionTimer, Action_Delay);
            InsertTimer(&(pValve->MiscInfo.ActionTimer));
        }
        if ((pValve->Error.ErrorByte & 0xAA) != 0)
        {
            pValve->Action.Action = Action_Stop;
            break;
        }
        if (IsTimeOut(pValve->MiscInfo.ActionTimer) == 1)
        {
            pValve->Error.ErrorBits.ShutTimeout = 1;
            pValve->Action.Action = Action_Stop;
            break;
        }
        if (pValve->Status.StatusBits.ShutLimit == 1)   
        {
            pValve->Action.Action = Action_Stop;
            break;
        }
        pValve->Status.StatusBits.Shutting = 1;
        break;
    case Action_Stop:
        if (IsFirstAction)
        {
            DeleteTimer(&(pValve->MiscInfo.ActionTimer));
        }
        pValve->Status.StatusBits.Opening = 0;
        pValve->Status.StatusBits.Shutting = 0;
        break;
    default:
        break;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void EnableDeviceReset(Device_T *pDevice)
{
    pDevice->MiscInfo.NeedReset = 1;
    
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void EnableDeviceRun(Device_T *pDevice)
{
    pDevice->Action.Action = Action_Run;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void EnableDeviceStop(Device_T *pDevice)
{
    pDevice->Action.Action = Action_Stop;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void EnableValveOpen(Valve_T *pValve)
{
    pValve->Action.Action = Action_Open;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void EnableValveShut(Valve_T *pValve)
{
    pValve->Action.Action = Action_Shut;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void EnableValveStop(Valve_T *pValve)
{
    pValve->Action.Action = Action_Stop;
}
 
/*************************************END OF FILE*******************************/


