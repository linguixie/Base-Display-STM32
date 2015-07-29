/*******************************************************************************
* Copyright (c) 2015,威鹏自动化科技有限公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-6-11
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：
********************************************************************************/
/*******************************************************************************
*                                    头  文  件
********************************************************************************/
#include "stm32f0xx_conf.h"
#include "stm32f0xx_it.h"
#include "SoftTimer.h"
#include "GPIO.h"
#include "Infrared.h"
#include "Config.h"

/*******************************************************************************
*                               文件内部使用宏定义
********************************************************************************/
#define Pulse_Max_Num              255

 //-红外帧延时间(单位:毫秒)-
#define InfraredFrameTime          100       
#define InfraredFrameDelay         (Division(InfraredFrameTime, Delay_2MiliSecond_Factor))
#define InfraredIdleTime           160
#define InfraredIdleDelay          (Division(InfraredIdleTime, Delay_2MiliSecond_Factor))
/*******************************************************************************
*                                 全局函数(变量)声明
********************************************************************************/
//-保存所有高低电平宽度(单位:InfraredRecvTimer的周期,例如=1,表示脉冲宽度为1个T)-
unsigned int PulseWidth[Pulse_Max_Num] = {0};
unsigned int g_PulseWidthIndex = 0;
__IO unsigned int TimeCount = 0;

/*******************************************************************************
*                                 静态函数(变量)声明
********************************************************************************/
unsigned char g_InfraredRecving = 0;

//-判断帧结束的定时器-
signed   long InfraredFrameTimer = 0;
//-帧空闲定时器(为了屏蔽红外遥控器连发2帧)-
signed   long InfraredIdleTimer  = -1;

//--
__IO unsigned char F_RecvOneIRFrame = 0;

//--
unsigned int IRKeyCode = 0;

/*******************************************************************************
* 函数名称:    StartInfraredRecv
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void StartInfraredRecv(void)
{
    //TimeCount = 0;
    //TIM_Cmd(InfraredRecvTimer, ENABLE);
}


/*******************************************************************************
* 函数名称:    EndInfraredRecv
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void EndInfraredRecv(void)
{
    //TIM_Cmd(InfraredRecvTimer, DISABLE);
}


/*******************************************************************************
* 函数名称:    InfraredRecvInit
* 函数功能:    初始化红外接收定时器
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
static void InfraredRecvInit(void)
{
    unsigned int Scaler = 0;
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    EXTI_InitTypeDef   EXTI_InitStructure;
#if DebugTmp
    TIM_OCInitTypeDef        TIM_OCInitStructure;
#endif

    /*红外接收管脚配置成外部边沿中断*/
    InfraredRecv_EXTI_ClockEnable();
    InfraredRecv_EXTI_LineConfig();

    EXTI_InitStructure.EXTI_Line = InfraredRecv_EXTI_Line;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = InfraredRecv_EXTI_Trigger_Mode;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = InfraredRecv_EXTI_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPriority = InfraredRecv_EXTI_Priority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    /*-接收定时器-*/
    Scaler = SystemCoreClock / InfraredRecvTimerFreq;
    InfraredRecvTimerClockEnable();
    TIM_DeInit(InfraredRecvTimer);
    
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_Period = 100 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = Scaler / 100 - 1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(InfraredRecvTimer, &TIM_TimeBaseStructure);

#if DebugTmp
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 100 / 3;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
    InfraredSendModulationChannelInit(InfraredRecvTimer, &TIM_OCInitStructure);
#endif

    NVIC_InitStructure.NVIC_IRQChannel = InfraredRecvTimerIRQ;
    NVIC_InitStructure.NVIC_IRQChannelPriority = InfraredRecvTimerPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearFlag(InfraredRecvTimer, TIM_FLAG_Update);
    TIM_ITConfig(InfraredRecvTimer, TIM_IT_Update, ENABLE);
#if DebugTmp
    TIM_Cmd(InfraredRecvTimer, ENABLE);
#endif
    TIM_Cmd(InfraredRecvTimer, ENABLE);
}


/*******************************************************************************
* 函数名称:    InfraredSendInit
* 函数功能:    初始化红外发送
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
static void InfraredSendInit(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef        TIM_OCInitStructure;

    unsigned int ModulationFreq = 0;
    unsigned int CarrierFreq = 0;

    unsigned int ModulationDuty = 0;
    unsigned int CarrierDuty = 0;

    ModulationFreq = (SystemCoreClock / InfraredSendModulationTimerFreq) - 1;
    ModulationDuty   = (((unsigned int) InfraredSendModulationTimerDuty * ModulationFreq) / 100);

    CarrierFreq = (SystemCoreClock / InfraredSendCarrierTimerFreq) - 1;
    CarrierDuty   = (((unsigned int) InfraredSendCarrierTimerDuty * CarrierFreq) / 100);

    //-Modulation Timer-
    InfraredSendModulationTimerClockEnable();
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_Period = ModulationFreq;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(InfraredSendModulationTimer, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ModulationDuty;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
    InfraredSendModulationChannelInit(InfraredSendModulationTimer, &TIM_OCInitStructure);


    //-Carrier Timer-
    InfraredSendCarrierTimerClockEnable();
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_Period = CarrierFreq;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(InfraredSendCarrierTimer, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = CarrierDuty;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
    InfraredSendCarrierChannelInit(InfraredSendCarrierTimer, &TIM_OCInitStructure);
}


/*******************************************************************************
* 函数名称:    InfraredSendEnable
* 函数功能:    红外发送使能
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void InfraredSendEnable(void)
{
    TIM_Cmd(InfraredSendModulationTimer, ENABLE);
    TIM_CtrlPWMOutputs(InfraredSendModulationTimer, ENABLE);
    TIM_Cmd(InfraredSendCarrierTimer, ENABLE);
    TIM_CtrlPWMOutputs(InfraredSendCarrierTimer, ENABLE);
}


/*******************************************************************************
* 函数名称:    SetInfraredModulationTimer
* 函数功能:    设置Modulation定时器的频率和占空比
* 输入参数:    Freq,频率
*              Duty,占空比
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void SetInfraredModulationTimer(unsigned int Freq, unsigned int Duty)
{
    TIM_SetAutoreload(InfraredSendModulationTimer, Freq);
    InfraredSendModulationChannelSet(InfraredSendModulationTimer, Duty);
}


/*******************************************************************************
* 函数名称:    SetInfraredCarrierTimer
* 函数功能:    设置Carrier定时器的频率和占空比
* 输入参数:    Freq,频率
*              Duty,占空比
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void SetInfraredCarrierTimer(unsigned int Freq, unsigned int Duty)
{
    TIM_SetAutoreload(InfraredSendCarrierTimer, Freq);
    InfraredSendCarrierChannelSet(InfraredSendCarrierTimer, Duty);
}



/*******************************************************************************
* 函数名称:    InfraredInit
* 函数功能:    初始化红外
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void InfraredInit(void)
{
    InsertTimer(&InfraredFrameTimer);
    InsertTimer(&InfraredIdleTimer);

    InfraredRecvInit();
    InfraredSendInit();
}


/*******************************************************************************
* 函数名称:    InfraredRecv_EXTI_IRQHandler
* 函数功能:    红外接收外部中断服务函数
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void InfraredRecv_EXTI_IRQHandler(void)
{
    int i = 0;
    unsigned int TimeElapsed = 0;
    static unsigned char Count = 0;

    if (EXTI_GetITStatus(InfraredRecv_EXTI_Line) != RESET)
    {
        EXTI_ClearITPendingBit(InfraredRecv_EXTI_Line);

        SetTimer(InfraredFrameTimer, InfraredFrameDelay);
    
        if (g_InfraredRecving == 0)
        {
            g_InfraredRecving = 1;
            g_PulseWidthIndex = 0;

            StartInfraredRecv();
            //-首次根据波形图可以得知是0-
            PulseWidth[g_PulseWidthIndex++] = 0;
        }
        else
        {
            EndInfraredRecv();

            TimeElapsed = TimeCount;
            if (TimeElapsed > Min_Period && TimeElapsed < Max_Period)
            {
                if (TimeElapsed > Mid_Period)
                {
                    PulseWidth[g_PulseWidthIndex++] = 1;
                }
                else
                {
                    PulseWidth[g_PulseWidthIndex++] = 0;
                }
            }
            else if (TimeElapsed > 230)
            {
            Count++;
            g_InfraredRecving = 1;
            g_PulseWidthIndex = 0;

            StartInfraredRecv();
            //-首次根据波形图可以得知是0-
            PulseWidth[g_PulseWidthIndex++] = 0;
            }
            if (g_PulseWidthIndex == Edge_Num)
            {
                F_RecvOneIRFrame = 1;
                IRKeyCode = 0;
                for (i = 0; i < Edge_Num; i++)
                {
                    if (PulseWidth[i] == 1)
                    {
                        IRKeyCode |= 1 << Edge_Num - 1 - i;
                    }
                }
            }
            else
            {
                StartInfraredRecv();
            }
        }
    }

    TimeCount = 0;
}



/*******************************************************************************
* 函数名称:    InfraredTimerIRQHandler
* 函数功能:    红外定时器中断服务函数
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void InfraredRecvTimerIRQHandler(void)
{
    TimeCount++;
    TIM_ClearFlag(InfraredRecvTimer, TIM_FLAG_Update);
}


/*******************************************************************************
* 函数名称:    GetIRKeyCode
* 函数功能:    返回红外遥控器的按键码字
* 输入参数:    
* 输出参数:    无
* 返 回 值:    红外遥控器的按键码字
*******************************************************************************/
unsigned int GetIRKeyCode(void)
{
    return IRKeyCode;
}

 
/*******************************************************************************
* 函数名称:    Task_Infrared
* 函数功能:    红外数据的处理
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Task_Infrared(void)
{
    //-延时时间到,认为当前帧结束-
    if ((g_InfraredRecving == 1) && (IsTimeOut(InfraredFrameTimer) == 1))
    {  
        F_RecvOneIRFrame = 1;
    } 

    if (F_RecvOneIRFrame ==  1)
    {  
        F_RecvOneIRFrame = 0;
        g_InfraredRecving = 0;  
        //EndInfraredRecv();

        //-无效帧-
        if (g_PulseWidthIndex < Edge_Num)
        {
        }
        else
        {
            SetTimer(InfraredIdleTimer, InfraredIdleDelay); 
        }
    }

    if (IsTimeOut(InfraredIdleTimer) == 1)
    {
        IRKeyCode = 0;
        return;
    }
}

/*************************************END OF FILE*******************************/

