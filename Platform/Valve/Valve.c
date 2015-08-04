/*******************************************************************************
* Copyright (c) 2015,xxx公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-3-28
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：
********************************************************************************/
/*******************************************************************************
*                                    头  文  件
********************************************************************************/
#include "Valve.h"
#include "Math.h"
#include "SoftTimer.h"
#include "Config.h"


/*******************************************************************************
*                               文件内部使用宏定义
********************************************************************************/
#define Action_Delay     (Multiplication(Device.Para.MaxActionTime, Delay_Second_Factor))

#define BigLoadTime       3
#define BigLoad_Delay    (Multiplication(BigLoadTime, Delay_Second_Factor))

#define JustRunTime       3
#define JustRun_Delay    (Multiplication(JustRunTime, Delay_Second_Factor))

//-阀门回退时间(单位:毫秒)-
#define MotorTime        100
//#define Motor_Delay     (Multiplication(MotorTime, Delay_Second_Factor))
#define Motor_Delay     (Division(MotorTime, Delay_MiliSecond_Factor))

/*******************************************************************************
*                                 全局函数(变量)声明
********************************************************************************/
Device_T              Device;
Valve_T               Valve;

Valve_T *ValveArray[ValveNum];

const unsigned char Counter_Max[Counter_Num] = 
{ 8, 8, 8,
  8, 8, 8, 
  32, 8
};

unsigned int ShutInertiaBuf[6] = {6, 6, 6, 6, 6, 6};
unsigned int OpenInertiaBuf[6] = {6, 6, 6, 6, 6, 6};
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
void ValveInit(void)
{
    int i = 0;
    int j = 0;
    Valve_T *pValve;

    ValveArray[0] = &Valve;

    for (i = 0; i < ValveNum; i++)
    {
        //-固定部分-
        pValve = ValveArray[i];

        pValve->Operation.Operation              = Operation_None;
        pValve->Operation.DstOpening             = 0;

        pValve->Action.PreAction                 = Action_None;
        pValve->Action.Action                    = Action_None;

        pValve->Adjust.Adjust0.Adjust0Byte       = 0;
        pValve->Adjust.Adjust1.Adjust1Byte       = 0;

        pValve->AdjustInfo.AdjustInfoByte        = 0;

        pValve->Status.StatusByte                = 0x00;         
        pValve->Error.ErrorByte                  = 0x00;
        pValve->Flag.FlagByte                    = 0;

        for (j = 0; j < Counter_Num; j++)
        {
            pValve->Counter.CounterArray[j] = 0;
        }

        //-变化部分-
        pValve->MiscInfo.ActionTimer             = 0;
        pValve->MiscInfo.BigLoadTimer            = 0;
        pValve->MiscInfo.JustStopTimer           = 0;
        pValve->MiscInfo.MotorTimer              = 0;
        pValve->MiscInfo.CurOpening              = 0;

    }

    //-固定部分-
    Device.Operation.PreOperation                = Operation_None;
    Device.Operation.Operation                   = Operation_Run;

    Device.Action.PreAction                      = Action_None;
    Device.Action.Action                         = Action_None;

    Device.WorkMode.CurWorkMode                  = WorkMode_None;
    Device.CurCommMode.CommModeByte              = 0;
    Device.DstCommMode.CommModeByte              = 0;

    Device.Status.Status                         = 0;

    Device.Error.ErrorByte                       = 0;
    Device.Flag.FlagByte                         = 0;

    Device.MiscInfo.NeedReset                    = 0;

    //-变化部分-
    Device.Status.ESDStatus                      = ESDStatus_Invalid;

    //-和面板交互的参数可以不初始化,因为在读取EEPROM时会初始化-
    Device.Para.RemoteType                       = RemoteType_Regulate;
    Device.Para.ESDDisplayEnable                 = ESDDisplay_Enable;
    Device.Para.CurrentDisplayEnable             = CurrentDisplay_Disable;
    Device.Para.CurrentDecimalBits               = CurrentDecimalBits_One;
    Device.Para.MaxActionTime                    = 0;

    Device.Input.Remote                          = Input_Invalid;
    Device.Input.Local                           = Input_Invalid;
    Device.Input.LocalOpen                       = Input_Invalid;
    Device.Input.LocalShut                       = Input_Invalid;
    Device.Input.LocalStop                       = Input_Invalid;                    
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
        //-!!!注意和DeviceProcess中的不同,仅当IsFirstAction == 1时才改变Action-
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
        //-当Device错误清除时,应该回到Run状态-
        if (Device.Error.ErrorByte == 0)
        {
            //--
            pDevice->Action.Action = Action_Run;
            break;
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ValveTimer(Device_T *pDevice, Valve_T *pValve)
{
    if (IsTimeOut(pValve->MiscInfo.BigLoadTimer) == 1)
    {
        pValve->Flag.FlagBits.F_BigLoad = 0;
    }

    if (IsTimeOut(pValve->MiscInfo.JustStopTimer) == 1)
    {
        pValve->Flag.FlagBits.F_JustStop = 0;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ValveProcess(Device_T *pDevice, Valve_T *pValve)
{
    unsigned char PreAction = Action_None;
    unsigned char IsFirstAction = 0;

    if (pValve->Action.PreAction != pValve->Action.Action)
    {
        PreAction = pValve->Action.PreAction;

        pValve->Action.PreAction = pValve->Action.Action;
        IsFirstAction = 1;
    }
    
    switch(pValve->Action.Action)
    {
    case Action_Open:
        if ((pValve->Error.ErrorByte & 0xAA) != 0)
        {
            pValve->Action.Action = Action_Stop;
            //-因为如果不用goto语句,如果点动模式下按住开阀,则到位后仍然不会停止-
            goto Stop;
        }
        if (pValve->Status.StatusBits.OpenLimit == 1)   
        {
            pValve->Action.Action = Action_Stop;
            goto Stop;
        }
        if (IsFirstAction)
        {
            //-开阀,清除关阀相关错误-
            Valve.Error.ErrorByte &= 0xAA;

            //-启动定时器-
            SetTimer(pValve->MiscInfo.MotorTimer, Motor_Delay);
            InsertTimer(&(pValve->MiscInfo.MotorTimer));
        }
        if (IsTimeOut(pValve->MiscInfo.MotorTimer) == 1)
        {
            DeleteTimer(&(pValve->MiscInfo.MotorTimer));
            pValve->Action.Action = Action_DelayOpen;
        }
#if Motor_Enable
        Motor.Status.Runing = 1;
#endif
        break;
    case Action_DelayOpen:
        if ((pValve->Error.ErrorByte & 0xAA) != 0)
        {
            pValve->Action.Action = Action_Stop;
            //-因为如果不用goto语句,如果点动模式下按住开阀,则到位后仍然不会停止-
            goto Stop;
        }
        if (pValve->Status.StatusBits.OpenLimit == 1)   
        {
            pValve->Action.Action = Action_Stop;
            goto Stop;
        }
        if (IsFirstAction)
        {
            Valve.Flag.FlagBits.F_BigLoad  = 1;
            Valve.Flag.FlagBits.F_JustStop = 0;
            Valve.Flag.FlagBits.F_JustRun  = 1;

            Valve.Error.ErrorBits.OpenTimeout = 0;
            Valve.MiscInfo.RunTmpOpening = Valve.MiscInfo.CurOpening;
            Valve.Counter.CounterArray[Counter_CheckRealRun] = Counter_Max[Counter_CheckRealRun];

            //-启动定时器-
            SetTimer(pValve->MiscInfo.BigLoadTimer, BigLoad_Delay);
            InsertTimer(&(pValve->MiscInfo.BigLoadTimer));

            SetTimer(pValve->MiscInfo.ActionTimer, Action_Delay);
            InsertTimer(&(pValve->MiscInfo.ActionTimer));
        }
        if (IsTimeOut(pValve->MiscInfo.ActionTimer) == 1)
        {
            pValve->Error.ErrorBits.OpenTimeout = 1;
            pValve->Action.Action = Action_Stop;
            goto Stop;
        }
        pValve->Status.StatusBits.Opening = 1;
#if Motor_Enable
        Motor.Status.Runing = 1;
#endif
        break;
    case Action_Shut:
        if ((pValve->Error.ErrorByte & 0x55) != 0)
        {
            pValve->Action.Action = Action_Stop;
            goto Stop;
        }
        if (pValve->Status.StatusBits.ShutLimit == 1)   
        {
            pValve->Action.Action = Action_Stop;
            goto Stop;
        }
        if (IsFirstAction)
        {
            //-开阀,清除关阀相关错误-
            Valve.Error.ErrorByte &= 0x55;

            //-启动定时器-
            SetTimer(pValve->MiscInfo.MotorTimer, Motor_Delay);
            InsertTimer(&(pValve->MiscInfo.MotorTimer));
        }
        if (IsTimeOut(pValve->MiscInfo.MotorTimer) == 1)
        {
            DeleteTimer(&(pValve->MiscInfo.MotorTimer));
            pValve->Action.Action = Action_DelayShut;
        }
#if Motor_Enable
        Motor.Status.Runing = 1;
#endif
        break;
    case Action_DelayShut:
        if ((pValve->Error.ErrorByte & 0x55) != 0)
        {
            pValve->Action.Action = Action_Stop;
            goto Stop;
        }
        if (pValve->Status.StatusBits.ShutLimit == 1)   
        {
            pValve->Action.Action = Action_Stop;
            goto Stop;
        }
        if (IsFirstAction)
        {
            Valve.Flag.FlagBits.F_BigLoad  = 1;
            Valve.Flag.FlagBits.F_JustStop = 0;
            Valve.Flag.FlagBits.F_JustRun  = 1;

            Valve.Error.ErrorBits.ShutTimeout = 0;                 //-!!此处和Action_Open不同-
            Valve.MiscInfo.RunTmpOpening = Valve.MiscInfo.CurOpening;
            Valve.Counter.CounterArray[Counter_CheckRealRun] = Counter_Max[Counter_CheckRealRun];

            //-启动定时器-
            SetTimer(pValve->MiscInfo.BigLoadTimer, BigLoad_Delay);
            InsertTimer(&(pValve->MiscInfo.BigLoadTimer));

            SetTimer(pValve->MiscInfo.ActionTimer, Action_Delay);
            InsertTimer(&(pValve->MiscInfo.ActionTimer));
        }
        if (IsTimeOut(pValve->MiscInfo.ActionTimer) == 1)
        {
            pValve->Error.ErrorBits.ShutTimeout = 1;
            pValve->Action.Action = Action_Stop;
            goto Stop;
        }
        pValve->Status.StatusBits.Shutting = 1;
#if Motor_Enable
        Motor.Status.Runing = 1;
#endif
        break;
Stop:
    case Action_Stop:
         if (IsFirstAction)
        {
            if (PreAction == Action_DelayOpen)
            {
                Valve.Flag.FlagBits.F_JustOpen = 1;
            }
            else if (PreAction == Action_DelayShut)
            {
                Valve.Flag.FlagBits.F_JustOpen = 0;
            }
            Valve.Flag.FlagBits.F_BigLoad = 1;
            Valve.Flag.FlagBits.F_JustRun = 0;
            Valve.Flag.FlagBits.F_JustStop = 1;

            Valve.MiscInfo.StopOutputOpening = Valve.MiscInfo.CurOpening;
            Valve.MiscInfo.StopTmpOpening    = Valve.MiscInfo.CurOpening;

            Valve.Counter.CounterArray[Counter_CheckRealStop] = Counter_Max[Counter_CheckRealStop];

            //-定时器操作-
            SetTimer(pValve->MiscInfo.BigLoadTimer, BigLoad_Delay);
            InsertTimer(&(pValve->MiscInfo.BigLoadTimer));

            SetTimer(pValve->MiscInfo.JustStopTimer, BigLoad_Delay);
            InsertTimer(&(pValve->MiscInfo.BigLoadTimer));

            DeleteTimer(&(pValve->MiscInfo.ActionTimer));
        }
        pValve->Status.StatusBits.Opening = 0;
        pValve->Status.StatusBits.Shutting = 0;
#if Motor_Enable
        //-只有电机不由上下限启动时才停止-
        if (Motor.Flag.F_MotorRuningByLimit == 0)
        {
            Motor.Status.Runing = 0;
        }
#endif
        break;
    case Action_NoLimitOpen:
        if (IsFirstAction)
        {
        }
        if ((pValve->Error.ErrorByte & 0x55) != 0)
        {
            pValve->Action.Action = Action_NoLimitStop;
            break;
        }
        pValve->Status.StatusBits.Opening = 1;
        break;
    case Action_NoLimitShut:
        if (IsFirstAction)
        {
        }
        if ((pValve->Error.ErrorByte & 0xAA) != 0)
        {
            pValve->Action.Action = Action_NoLimitStop;
            break;
        }
        pValve->Status.StatusBits.Shutting = 1;
        break;
    case Action_NoLimitStop:
        pValve->Status.StatusBits.Opening  = 0;
        pValve->Status.StatusBits.Shutting = 0;
        break;
    default:
        break;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void EnableDeviceReset(Device_T *pDevice)
{
    pDevice->MiscInfo.NeedReset = 1;
    
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void EnableDeviceRun(Device_T *pDevice)
{
    pDevice->Action.Action = Action_Run;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void EnableDeviceStop(Device_T *pDevice)
{
    pDevice->Action.Action = Action_Stop;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void EnableValveOpen(Valve_T *pValve)
{
    if (pValve->Action.PreAction != Action_Open)
    {
        pValve->Action.Action = Action_Open;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void EnableValveShut(Valve_T *pValve)
{
    if (pValve->Action.PreAction != Action_Shut)
    {
        pValve->Action.Action = Action_Shut;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void EnableValveNoLimitOpen(Valve_T *pValve)
{
    pValve->Action.Action = Action_NoLimitOpen;
}



/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void EnableValveNoLimitShut(Valve_T *pValve)
{
    pValve->Action.Action = Action_NoLimitShut;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void EnableValveNoLimitStop(Valve_T *pValve)
{
    pValve->Action.Action = Action_NoLimitStop;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void EnableValveStop(Valve_T *pValve)
{
    pValve->Action.Action = Action_Stop;
}
 

/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
unsigned char GetValveStatus(Valve_T *pValve)
{
    return pValve->Action.Action;
}


/*************************************END OF FILE*******************************/


