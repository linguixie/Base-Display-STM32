/*******************************************************************************
* Copyright (c) 2015,xxx公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-3-26
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：
********************************************************************************/
/*******************************************************************************
*                                    头  文  件
********************************************************************************/
#include "stm32f0xx_conf.h"
#include "GPIO.h"
#include "Buzzer.h"
#include "Display.h"
#include "Valve.h"
#include "SoftTimer.h"
#include "Communication.h"

/*******************************************************************************
*                               文件内部使用宏定义
********************************************************************************/
#define COMM_CLK_H()             COMM_CLKSet()
#define COMM_CLK_L()             COMM_CLKClr()
#define COMM_CLK_Get()           COMM_CLKGet()

#define COMM_CS_H()              COMM_CSSet()
#define COMM_CS_L()              COMM_CSClr()

#define COMM_MOSI_H()            COMM_MOSISet()
#define COMM_MOSI_L()            COMM_MOSIClr()

#define COMM_MISO_Get()          COMM_MISOGet()

/*******************************************************************************
*                                 全局函数(变量)声明
********************************************************************************/

unsigned char RecvBuf[Frame_Size] = {0};    //-数据接收缓冲-
unsigned char SendBuf[Frame_Size] = {0};    //-数据发送缓冲-

unsigned char F_SYNC                = 0;
unsigned char F_ExtraNormal         = 0;


unsigned char F_WaitAdjustFrame = 0;        //-防止标定帧重复处理的标志-

unsigned char F_SendBufValid = 0;           //-发送缓冲中的数据是否有效-
unsigned char F_Disconnect = 0;

//-无按键延时-
signed long WaitReplyTimer = 0;

unsigned char CurrentAdjustType = Frame_Invlid;

/*******************************************************************************
*                                 静态函数(变量)声明
********************************************************************************/
unsigned char ClockCount = 0;                      //-时钟计数器-
volatile unsigned char F_Sending = 0;              //-正在发送数据帧-
volatile unsigned char F_RegularFrameSending = 0;  //--
volatile unsigned char F_ExtraFrameSending = 1;    //--
unsigned char FrameType = 0;
/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
unsigned char GetCheckSum(unsigned char *pBuf, unsigned int Length)
{
    int i = 0;
    unsigned char CheckSum = 0x00;    //-注意类型应为unsigned char-

    for (i = 0; i < Length; i++)
    {
        CheckSum += pBuf[i];
    }

    return CheckSum;    
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
static void CommTimerInit(void)
{
    unsigned int Scaler = 0;
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    Scaler = SystemCoreClock / CommFreq;

    CommTimerClockEnable();
    TIM_DeInit(CommTimer);
    
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_Period = 100 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = (Scaler / 100) - 1;

    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(CommTimer, &TIM_TimeBaseStructure);

    NVIC_InitStructure.NVIC_IRQChannel = CommIRQ;
    NVIC_InitStructure.NVIC_IRQChannelPriority = CommTimerPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearFlag(CommTimer, TIM_FLAG_Update);
    TIM_ITConfig(CommTimer, TIM_IT_Update, ENABLE);

    TIM_Cmd(CommTimer, ENABLE);
}


/*******************************************************************************
* 函数名称:    CommTimerIRQHander
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void CommTimerIRQHander(void)
{
    unsigned char SendByteIndex = 0;
    unsigned char SendBitIndex = 0;
    unsigned char RecvByteIndex = 0;
    unsigned char RecvBitIndex = 0;

    //-清除中断标志-
    TIM_ClearFlag(CommTimer, TIM_FLAG_Update);

#if 1
    if (ClockCount == 0)    
    {
        F_Sending = 1;

        COMM_CLK_L();
        COMM_CS_L(); 
    }
    else if ((ClockCount >= 1) && (ClockCount < 161))
    {
        if (COMM_CLK_Get() != 0)                               //-显示板发送且读取数据-
        {
            COMM_CLK_L();

            SendByteIndex = (ClockCount + 1) >> 4;                   //-除以16-
            SendBitIndex  = 7 - (((ClockCount + 1) >> 1) & 0x07);    //-对8求余-

            RecvByteIndex = (ClockCount - 1) >> 4;
            RecvBitIndex  = 7 - (((ClockCount - 1) >> 1) & 0x07);
  
            //-发送-
            if ((SendBuf[SendByteIndex] & (1 << SendBitIndex)) == 0)
            {
                COMM_MOSI_L();
            }
            else
            {
                COMM_MOSI_H();
            }

            //-接收-
            if (COMM_MISO_Get() != 0)
            {
                RecvBuf[RecvByteIndex] |= 1 << RecvBitIndex;
            }
            else
            {
                RecvBuf[RecvByteIndex] &= ~(1 << RecvBitIndex);
            }
        }
        else
        {
            COMM_CLK_H();
        }
    }
    else if (ClockCount == 161)
    {
        COMM_CS_H();
        //COMM_CLK = 1;
    }
    else if(ClockCount == 255)
    {
        //-发送第一位-
        if ((SendBuf[0] & (1 << 7)) == 0)
        {
            COMM_MOSI_L();
        }
        else
        {
            COMM_MOSI_H();
        }
    }
    else 
    {
        F_Sending = 0;

        if (F_RegularFrameSending == 1)
        {
            F_RegularFrameSending = 0;
            F_ExtraFrameSending = 1;
        }
        else if (F_ExtraFrameSending == 1)
        {
            F_ExtraFrameSending = 0;
            F_RegularFrameSending = 1;
        }
    }

    ClockCount++;    //-累加后自动溢出,清零-
#endif
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void CommunicationInit(void)
{
    CommTimerInit();
    F_SYNC = 1;
    InsertTimer(&WaitReplyTimer);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void SetFrameType(unsigned char Type)
{
    FrameType = Type;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
unsigned char GetFrameType(void)
{
    return FrameType;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void MakeSyncFrame(void)
{
    unsigned char CheckSum = 0x00;

    /*-组发送帧-*/
    ClearBuf(&SendBuf[0], sizeof(SendBuf));

    //-字节0-
    SendBuf[0] = SyncFrame_IDCode;                           
    /*----------------组帧(根据实际应用,只需修改以下)-------------------------*/


    /*----------------结束----------------------------------------------------*/
    //-字节9-
    CheckSum = GetCheckSum(&SendBuf[0], Frame_Size - 1);
    SendBuf[Frame_Size - 1] = CheckSum;                             
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void MakeExtraSyncFrame(void)
{
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void MakeNormalFrame(void)
{
    int Index = 0;
    unsigned char CheckSum = 0x00;
    unsigned char Command = 0;

    /*-组发送帧-*/
    ClearBuf(&SendBuf[0], sizeof(SendBuf));

    //-字节0-
    SendBuf[0] = NormalFrame_IDCode;                           
    /*----------------组帧(根据实际应用,只需修改以下)-------------------------*/
    Index = 1;

    if (Device.Mode.ControlModeBits.Remote == 1)
    {
        Command |= 0x80;
    }
    else if (Device.Mode.ControlModeBits.Local == 1)
    {
        Command |= 0x40;
    }

    if (Valve.Operation.Operation == Operation_Shut)
    {
        Command |= 0x01;
    }
    else if (Valve.Operation.Operation == Operation_Open)
    {
        Command |= 0x02;
    }
    SendBuf[Index++] = Command;

    /*----------------结束----------------------------------------------------*/
    //-字节9-
    CheckSum = GetCheckSum(&SendBuf[0], Frame_Size - 1);
    SendBuf[Frame_Size - 1] = CheckSum; 
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void MakeExtraNormalFrame(void)
{
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void MakeAdjustFrame(void)
{
    int Index = 0;
    unsigned char CheckSum = 0x00;
    unsigned AdjustRequest[2] = {0};

    /*-组发送帧-*/
    ClearBuf(&SendBuf[0], sizeof(SendBuf));

    //-字节0-
    SendBuf[0] = AdjustFrame_IDCode;                           
    /*----------------组帧(根据实际应用,只需修改以下)-------------------------*/
    Index = 1;

    if (Valve.Adjust.Adjust0.Adjust0Bits.Full == 1)
    {
        AdjustRequest[0] |= 0x02;
    }
    if (Valve.Adjust.Adjust0.Adjust0Bits.Zero == 1)
    {
        AdjustRequest[0] |= 0x01;
    }
    if (Valve.Operation.Operation == Operation_NoLimitOpen)
    {
        Valve.Operation.Operation = Operation_None;
        AdjustRequest[0] |= 0x80;
    }
    else if (Valve.Operation.Operation == Operation_NoLimitShut)
    {
        Valve.Operation.Operation = Operation_None;

        AdjustRequest[0] |= 0x40;
    }
    SendBuf[Index++] = AdjustRequest[0];

    if (Valve.Adjust.Adjust1.Adjust1Bits.IncOutput4mA == 1)
    {
        //-和标定0和100不同,只发1次,下同-
        Valve.Adjust.Adjust1.Adjust1Bits.IncOutput4mA = 0;

        AdjustRequest[1] |= 0x80;
    }
    if (Valve.Adjust.Adjust1.Adjust1Bits.DecOutput4mA == 1)
    {
        Valve.Adjust.Adjust1.Adjust1Bits.DecOutput4mA = 0;

        AdjustRequest[1] |= 0x40;
    }
    if (Valve.Adjust.Adjust1.Adjust1Bits.IncOutput20mA == 1)
    {
        Valve.Adjust.Adjust1.Adjust1Bits.IncOutput20mA = 0;

        AdjustRequest[1] |= 0x20;
    }
    if (Valve.Adjust.Adjust1.Adjust1Bits.DecOutput20mA == 1)
    {
        Valve.Adjust.Adjust1.Adjust1Bits.DecOutput20mA = 0;

        AdjustRequest[1] |= 0x10;
    }
    if (Valve.Adjust.Adjust1.Adjust1Bits.Input20mA == 1)
    {
        //Valve.Adjust.Adjust1.Adjust1Bits.Input20mA = 0;

        AdjustRequest[1] |= 0x02;
    }
    if (Valve.Adjust.Adjust1.Adjust1Bits.Input4mA == 1)
    {
        //Valve.Adjust.Adjust1.Adjust1Bits.Input4mA = 0;

        AdjustRequest[1] |= 0x01;
    }
    SendBuf[Index++] = AdjustRequest[1];


    /*----------------结束----------------------------------------------------*/
    //-字节9-
    CheckSum = GetCheckSum(&SendBuf[0], Frame_Size - 1);
    SendBuf[Frame_Size - 1] = CheckSum; 
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void MakeExtraAdjustFrame(void)
{
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void MakeSettingFrame(void)
{
    int Index = 0;
    unsigned char CheckSum = 0x00;

    unsigned char LocalMode = 0;
    unsigned int ESDMode   = 0;
    unsigned char RemoteANMode = 0;
    unsigned char RemoteIOMode = 0;

    /*-组发送帧-*/
    ClearBuf(&SendBuf[0], sizeof(SendBuf));

    //-字节0-
    SendBuf[0] = SettingFrame_IDCode;                           
    /*----------------组帧(根据实际应用,只需修改以下)-------------------------*/
    Index = 1;
    SendBuf[Index++] = 0xA9;    
    
    if (Device.Para.LocalMode == LocalMode_Jog)
    {
        LocalMode = 0;
    }
    else if (Device.Para.LocalMode == LocalMode_Hold)
    {
        LocalMode = 1;
    }


    if (Device.Para.ErrorFeedBack == IO_NormallyShut)
    {
        LocalMode |= 1 << 3;
    }
    else
    {
        LocalMode &= ~(1 << 3);
    }


    switch(Device.Para.ESDMode)
    {
    case ESD_Disable:
        ESDMode = 0;
        break;
    case ESD_NoAction:
        ESDMode = 1;
        break;
    case ESD_Shut:
        ESDMode = 2;
        break;
    case ESD_Open:
        ESDMode = 3;
        break;
    case ESD_Middle:
        ESDMode = 4;
        break;
    default:
        break;
    }

    LocalMode &= 0x0F;
    ESDMode &= 0x07;
    LocalMode = (ESDMode << 4) + LocalMode;
    SendBuf[Index++] = LocalMode;

    switch(Device.Para.RemoteIOMode)
    {
    case RemoteIOMode_Jog:
        RemoteIOMode = 0;
        break;
    case RemoteIOMode_Hold:
        if (Device.Para.RemoteHold == IO_NormallyOpen)
        {
            RemoteIOMode = 1;
        }
        else
        {
            RemoteIOMode = 2;
        }
        break;
    case RemoteIOMode_SignalOnNoOff:
        RemoteIOMode = 3;
        break;
    case RemoteIOMode_SignalOffNoOn:
        RemoteIOMode = 4;
        break;
    default:
        break;
    }
    SendBuf[Index++] = RemoteIOMode;


    switch(Device.Para.RemoteANMode)
    {
    case RemoteANMode_Invalid:
        RemoteANMode = 0;
        break;
    case RemoteANMode_NoSigKeep:
        RemoteANMode = 0;
        break;
    case RemoteANMode_NoSigShut:
        RemoteANMode = 1;
        break;
    case RemoteANMode_NoSigMid:
        RemoteANMode = 2;
        break;
    case RemoteANMode_NoSigOpen:
        RemoteANMode = 3;
        break;
    default:
        break;
    }
    SendBuf[Index++] = RemoteANMode;

    SendBuf[Index++] = Device.Para.DeadZone;
    SendBuf[Index++] = Device.Para.MaxShutCurrent;
    SendBuf[Index++] = Device.Para.MaxActionTime;
    SendBuf[Index++] = Device.Para.MaxOpenCurrent;

    /*----------------结束----------------------------------------------------*/
    //-字节9-
    CheckSum = GetCheckSum(&SendBuf[0], Frame_Size - 1);
    SendBuf[Frame_Size - 1] = CheckSum; 
}    


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void MakeExtraSettingFrame(void)
{
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void NormalFrameDeal(void)
{
{
    int RecvBufIndex = 0;
 
    unsigned int TmpData = 0;
 
    RecvBufIndex = 1;
 
    //-避免震荡,开度在闭区间:[CurOpening * 2 - 1, CurOpening * 2 + 1],则认为和当前开度一样.下同
    // 考虑边界:当开度为0的时候,TmpData - 1 = 0xFFFF; 判断条件仍然成立 -
    TmpData = Valve.MiscInfo.CurOpening << 1;
    if ((RecvBuf[RecvBufIndex] > (TmpData + 1)) || (RecvBuf[RecvBufIndex] < (TmpData - 1)))
    {
       Valve.MiscInfo.CurOpening = RecvBuf[RecvBufIndex];
       Valve.MiscInfo.CurOpening >>= 1;
    }
    RecvBufIndex++;

    //-目标开度-
    if (RecvBuf[RecvBufIndex] > 250)
    {
        Device.Para.RemoteType = RemoteType_Switch;
    }
    else
    {
        Device.Para.RemoteType = RemoteType_Regulate;

        TmpData = Valve.Operation.DstOpening << 1;
        if ((RecvBuf[RecvBufIndex] > (TmpData + 1)) || (RecvBuf[RecvBufIndex] < (TmpData - 1)))
        {
            Valve.Operation.DstOpening = RecvBuf[RecvBufIndex];
            Valve.Operation.DstOpening >>= 1;
        }
    }
    RecvBufIndex++;

#if 0
    Device.Mode.ControlMode = 0x00;
    switch(RecvBuf[RecvBufIndex])
    {
    case 0x00:
        Device.Mode.ControlModeBits.Bus = 1;
        break;
    case 0x01:
        Device.Mode.ControlModeBits.Analog = 1;
        break;
    case 0x02:
        Device.Para.RemoteIOMode = RemoteIOMode_Jog;
        break;
    case 0x03:
        Device.Para.RemoteIOMode = RemoteIOMode_Hold;
        break;
    case 0x04:
        Device.Para.RemoteIOMode = RemoteIOMode_BiPos;
        break;
    case 0x05:
        Device.Para.LocalMode = LocalMode_Jog;
        break;
    case 0x06:
        Device.Para.LocalMode = LocalMode_Hold;
        break;
    case 0x07:
        break;
    default:
        break;
    } 
    RecvBufIndex++;
#endif
    Device.Status.CurMode = RecvBuf[RecvBufIndex++];


    if ((RecvBuf[RecvBufIndex] & 0x80) != 0)
    {
        Device.Mode.ControlModeBits.Remote = 1;
    }
    else if ((RecvBuf[RecvBufIndex] & 0x40) != 0)
    {
        Device.Mode.ControlModeBits.Local = 1;
    }
    Valve.Status.StatusByte = RecvBuf[RecvBufIndex] & 0x3F;
    RecvBufIndex++;

    Device.Error.ErrorByte = RecvBuf[RecvBufIndex++];
    Valve.Error.ErrorByte = RecvBuf[RecvBufIndex++];
}
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ExtraNormalFrameDeal(void)
{
    if (F_ExtraNormal != 1)
    {
        return;
    }
    F_ExtraNormal = 0;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void AdjustFrameDeal(void)
{
    unsigned int ValvePositionADValue = 0;

    if (F_WaitAdjustFrame == 1)    //-只有在规定的时间内接收,防止重复判断-
    {
        Valve.AdjustInfo.AdjustInfoByte = RecvBuf[1];
    }

    ValvePositionADValue = RecvBuf[3];
    ValvePositionADValue <<= 8;
    ValvePositionADValue += RecvBuf[2];

    Valve.MiscInfo.PositionADValue = ValvePositionADValue;
    Valve.MiscInfo.In4_20mA = RecvBuf[4];
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ExtraAdjustFrameDeal(void)
{
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void SettingFrameDeal(void)
{
    int RecvBufIndex = 0;

    if (F_SYNC != 1)
    {
        return;;
    }
  
    F_SYNC = 0;

    RecvBufIndex = 1;
    if (RecvBuf[RecvBufIndex++] != 0xA9)
    {
        return;
    }


    //-Local Mode-
    if (RecvBuf[RecvBufIndex] & 0x01)
    {
        Device.Para.LocalMode = LocalMode_Hold;
    }
    else
    {
        Device.Para.LocalMode = LocalMode_Jog;
    }


    //-Error Feedback-
    if (RecvBuf[RecvBufIndex] & 0x08)
    {
        Device.Para.ErrorFeedBack = IO_NormallyShut;
    }
    else
    {
        Device.Para.ErrorFeedBack = IO_NormallyOpen;
    }


    //-ESD-
    if (RecvBuf[RecvBufIndex] & 0x80)
    {
        Device.Status.ESDStatus = ESD_Valid;
    }
    else
    {
        Device.Status.ESDStatus = ESD_Invalid;
    }
    switch((RecvBuf[RecvBufIndex]   & 0x70) >> 4)
    {
    case 0:
        Device.Para.ESDMode = ESD_Disable;
        break;
    case 1:
        Device.Para.ESDMode = ESD_NoAction;
        break;
    case 2:
        Device.Para.ESDMode = ESD_Shut;
        break;
    case 3:
        Device.Para.ESDMode = ESD_Open;
        break;
    case 4:
        Device.Para.ESDMode = ESD_Middle;
        break;
    default:
        break;
    }
    RecvBufIndex++;


    //-存在的隐患:如果面板上传的模式为非保持,那么保持是否常开还是常闭不确定.-
    if (RecvBuf[RecvBufIndex] == 1)
    {
        Device.Para.RemoteHold = IO_NormallyOpen;
        Device.Para.RemoteIOMode = RemoteIOMode_Hold;
    }
    else if (RecvBuf[RecvBufIndex] == 2)
    {
        Device.Para.RemoteHold = IO_NormallyShut;
        Device.Para.RemoteIOMode = RemoteIOMode_Hold;
    }
    else
    {
        Device.Para.RemoteIOMode = RecvBuf[RecvBufIndex];
    }
    RecvBufIndex++;
    
    switch(RecvBuf[RecvBufIndex])
    {
    case 0:
        Device.Para.RemoteANMode = RemoteANMode_NoSigKeep;
        break;
    case 1:
        Device.Para.RemoteANMode = RemoteANMode_NoSigShut;
        break;
    case 2:
        Device.Para.RemoteANMode = RemoteANMode_NoSigMid;
        break;
    case 3:
        Device.Para.RemoteANMode = RemoteANMode_NoSigOpen;
        break;
    default:
        break;
    }
    RecvBufIndex++;    


    Device.Para.DeadZone = RecvBuf[RecvBufIndex++];
    Device.Para.MaxShutCurrent = RecvBuf[RecvBufIndex++];
    Device.Para.MaxActionTime = RecvBuf[RecvBufIndex++];
    Device.Para.MaxOpenCurrent = RecvBuf[RecvBufIndex++];
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ExtraSettingFrameDeal(void)
{
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void GetMultiplexFrameTypeStatus(volatile unsigned char *RegularFrame, volatile unsigned char *ExtraFrame)
{
#if 0
    if ((F_RegularFrameSending == 0) && (F_ExtraFrameSending == 0))
    {
        F_ExtraFrameSending = 1;
    }
    if (F_ExtraFrameSending == 0)
    {
        F_RegularFrameSending = 1;
    }
#endif
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Task_SendDataDeal(void)
{
    unsigned char FrameType = 0; 

#if 0
    MakeSettingFrame();
#else
    if (F_SYNC == 1)
    {
        MakeSyncFrame();
    }
    else
    {
        FrameType = GetFrameType();
        switch(FrameType)
        {
        case Normal_Frame:
            MakeNormalFrame();
            Valve.Adjust.Adjust0.Adjust0Byte = 0;
            Valve.Adjust.Adjust1.Adjust1Byte = 0;
            break;
        case Adjust_Frame:
            GetMultiplexFrameTypeStatus(&F_RegularFrameSending, &F_ExtraFrameSending);
            if (F_RegularFrameSending == 1)
            {
                MakeAdjustFrame();
            }
            else if (F_ExtraFrameSending == 1)
            {
                if ((PageFunctionIndex == Page_OpenTerminal_ID) || (PageFunctionIndex == Page_ShutTerminal_ID))// || (PageFunctionIndex == Page_In4mA_ID) || (PageFunctionIndex == Page_In20mA_ID) || (PageFunctionIndex == Page_ANSignal_ID))
                {
                    MakeAdjustFrame();
                }
                else
                {
                    MakeNormalFrame();
                }
            }
            break;
        case Setting_Frame:
            GetMultiplexFrameTypeStatus(&F_RegularFrameSending, &F_ExtraFrameSending);
            if (F_RegularFrameSending == 1)
            {
                MakeSettingFrame();
            }
            else if (F_ExtraFrameSending == 1)
            {
                MakeNormalFrame();
            }
            Valve.Adjust.Adjust0.Adjust0Byte = 0;
            Valve.Adjust.Adjust1.Adjust1Byte = 0;
            break;
        default:
            MakeNormalFrame();
            break;
        }
    }
#endif
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Task_RecvDataDeal(void)
{
    static signed char Count = 0;
    unsigned char CheckSum = 0x00;

    /*-判断校验和-*/
    CheckSum = GetCheckSum(&RecvBuf[0], Frame_Size - 1);
    if (CheckSum != RecvBuf[CheckSumIndex])
    {
        Count++;
        if (Count >= 50)
        {
            //--
            F_Disconnect = 1;
        }
        return;
    }

    switch(RecvBuf[RecognizeIndex])
    {
    case ReplyNormalFrame_IDCode:
        Count = 0;
        F_Disconnect = 0;
        NormalFrameDeal();
        break;
    case ReplyExtraNormalFrame_IDCode:
        Count = 0;
        F_Disconnect = 0;
        ExtraNormalFrameDeal();
        break;
    case ReplyAdjustFrame_IDCode:
        Count = 0;
        F_Disconnect = 0;
        AdjustFrameDeal();
        break;
    case ReplyExtraAdjustFrame_IDCode:
        Count = 0;
        F_Disconnect = 0;
        ExtraAdjustFrameDeal();
        break;
    case ReplySettingFrame_IDCode:
        Count = 0;
        F_Disconnect = 0;
        SettingFrameDeal();
        break;
    case ReplyExtraSettingFrame_IDCode:
        Count = 0;
        F_Disconnect = 0;
        break;
    default:
        Count++;
        if (Count >= 50)
        {
            //--
            F_Disconnect = 1;
        }
        break;
    }

    if (F_WaitAdjustFrame == 1)    //--
    {
        if ((IsTimeOut(WaitReplyTimer) == 1))
        {
            F_WaitAdjustFrame = 0;
    
            if (CurrentAdjustType == Frame_AdjustZero)
            {
                if (Valve.AdjustInfo.AdjustInfoBits.Zero == 1)
                {
                    Show_Page(Page_AdjustZeroInfo_ID);
                    SetTimer(AdjustInfoTimer, AdjustInfo_Delay);
                }
                else
                {
                    Show_Page(Page_Terminal_ID);
                }
            }
            if (CurrentAdjustType == Frame_AdjustFull)
            {
                if (Valve.AdjustInfo.AdjustInfoBits.Full == 1)
                {
                    Show_Page(Page_AdjustFullInfo_ID);
                    SetTimer(AdjustInfoTimer, AdjustInfo_Delay);
                }
                else
                {
                    Show_Page(Page_Terminal_ID);
                }
            }
            if (CurrentAdjustType == Frame_AdjustInput4mA)
            {
                if (Valve.AdjustInfo.AdjustInfoBits.Input4mA == 1)
                {
                    Show_Page(Page_In4mAInfo_ID);
                    SetTimer(AdjustInfoTimer, AdjustInfo_Delay);
                }
                else
                {
                    Show_Page(Page_ANSignal_ID);
                }
            }
            if (CurrentAdjustType == Frame_AdjustInput20mA)
            {
                if (Valve.AdjustInfo.AdjustInfoBits.Input20mA == 1)
                {
                    Show_Page(Page_In20mAInfo_ID);
                    SetTimer(AdjustInfoTimer, AdjustInfo_Delay);
                }
                else
                {
                    Show_Page(Page_ANSignal_ID);
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
void Task_Communication(void)
{
    Task_SendDataDeal();

    Task_RecvDataDeal();
}


 
/*************************************END OF FILE*******************************/

