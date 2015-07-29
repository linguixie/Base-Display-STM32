/*******************************************************************************
* Copyright (c) 2015,�����Զ����Ƽ����޹�˾
 *            All rights reserved.
* �������ƣ�
* �� �� �ţ�1.0
* �� �� �ˣ�л�ֹ�
* ����ʱ�䣺2015-6-11
* �� �� ��:
* �޸�ʱ��:
* �޸�˵��:
* ��    ����
********************************************************************************/
/*******************************************************************************
*                                    ͷ  ��  ��
********************************************************************************/
#include "stm32f0xx_conf.h"
#include "stm32f0xx_it.h"
#include "SoftTimer.h"
#include "GPIO.h"
#include "Infrared.h"
#include "Config.h"

/*******************************************************************************
*                               �ļ��ڲ�ʹ�ú궨��
********************************************************************************/
#define Pulse_Max_Num              255

 //-����֡��ʱ��(��λ:����)-
#define InfraredFrameTime          100       
#define InfraredFrameDelay         (Division(InfraredFrameTime, Delay_2MiliSecond_Factor))
#define InfraredIdleTime           160
#define InfraredIdleDelay          (Division(InfraredIdleTime, Delay_2MiliSecond_Factor))
/*******************************************************************************
*                                 ȫ�ֺ���(����)����
********************************************************************************/
//-�������иߵ͵�ƽ���(��λ:InfraredRecvTimer������,����=1,��ʾ������Ϊ1��T)-
unsigned int PulseWidth[Pulse_Max_Num] = {0};
unsigned int g_PulseWidthIndex = 0;
__IO unsigned int TimeCount = 0;

/*******************************************************************************
*                                 ��̬����(����)����
********************************************************************************/
unsigned char g_InfraredRecving = 0;

//-�ж�֡�����Ķ�ʱ��-
signed   long InfraredFrameTimer = 0;
//-֡���ж�ʱ��(Ϊ�����κ���ң��������2֡)-
signed   long InfraredIdleTimer  = -1;

//--
__IO unsigned char F_RecvOneIRFrame = 0;

//--
unsigned int IRKeyCode = 0;

/*******************************************************************************
* ��������:    StartInfraredRecv
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void StartInfraredRecv(void)
{
    //TimeCount = 0;
    //TIM_Cmd(InfraredRecvTimer, ENABLE);
}


/*******************************************************************************
* ��������:    EndInfraredRecv
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void EndInfraredRecv(void)
{
    //TIM_Cmd(InfraredRecvTimer, DISABLE);
}


/*******************************************************************************
* ��������:    InfraredRecvInit
* ��������:    ��ʼ��������ն�ʱ��
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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

    /*������չܽ����ó��ⲿ�����ж�*/
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


    /*-���ն�ʱ��-*/
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
* ��������:    InfraredSendInit
* ��������:    ��ʼ�����ⷢ��
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    InfraredSendEnable
* ��������:    ���ⷢ��ʹ��
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void InfraredSendEnable(void)
{
    TIM_Cmd(InfraredSendModulationTimer, ENABLE);
    TIM_CtrlPWMOutputs(InfraredSendModulationTimer, ENABLE);
    TIM_Cmd(InfraredSendCarrierTimer, ENABLE);
    TIM_CtrlPWMOutputs(InfraredSendCarrierTimer, ENABLE);
}


/*******************************************************************************
* ��������:    SetInfraredModulationTimer
* ��������:    ����Modulation��ʱ����Ƶ�ʺ�ռ�ձ�
* �������:    Freq,Ƶ��
*              Duty,ռ�ձ�
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void SetInfraredModulationTimer(unsigned int Freq, unsigned int Duty)
{
    TIM_SetAutoreload(InfraredSendModulationTimer, Freq);
    InfraredSendModulationChannelSet(InfraredSendModulationTimer, Duty);
}


/*******************************************************************************
* ��������:    SetInfraredCarrierTimer
* ��������:    ����Carrier��ʱ����Ƶ�ʺ�ռ�ձ�
* �������:    Freq,Ƶ��
*              Duty,ռ�ձ�
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void SetInfraredCarrierTimer(unsigned int Freq, unsigned int Duty)
{
    TIM_SetAutoreload(InfraredSendCarrierTimer, Freq);
    InfraredSendCarrierChannelSet(InfraredSendCarrierTimer, Duty);
}



/*******************************************************************************
* ��������:    InfraredInit
* ��������:    ��ʼ������
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void InfraredInit(void)
{
    InsertTimer(&InfraredFrameTimer);
    InsertTimer(&InfraredIdleTimer);

    InfraredRecvInit();
    InfraredSendInit();
}


/*******************************************************************************
* ��������:    InfraredRecv_EXTI_IRQHandler
* ��������:    ��������ⲿ�жϷ�����
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
            //-�״θ��ݲ���ͼ���Ե�֪��0-
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
            //-�״θ��ݲ���ͼ���Ե�֪��0-
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
* ��������:    InfraredTimerIRQHandler
* ��������:    ���ⶨʱ���жϷ�����
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void InfraredRecvTimerIRQHandler(void)
{
    TimeCount++;
    TIM_ClearFlag(InfraredRecvTimer, TIM_FLAG_Update);
}


/*******************************************************************************
* ��������:    GetIRKeyCode
* ��������:    ���غ���ң�����İ�������
* �������:    
* �������:    ��
* �� �� ֵ:    ����ң�����İ�������
*******************************************************************************/
unsigned int GetIRKeyCode(void)
{
    return IRKeyCode;
}

 
/*******************************************************************************
* ��������:    Task_Infrared
* ��������:    �������ݵĴ���
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void Task_Infrared(void)
{
    //-��ʱʱ�䵽,��Ϊ��ǰ֡����-
    if ((g_InfraredRecving == 1) && (IsTimeOut(InfraredFrameTimer) == 1))
    {  
        F_RecvOneIRFrame = 1;
    } 

    if (F_RecvOneIRFrame ==  1)
    {  
        F_RecvOneIRFrame = 0;
        g_InfraredRecving = 0;  
        //EndInfraredRecv();

        //-��Ч֡-
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

