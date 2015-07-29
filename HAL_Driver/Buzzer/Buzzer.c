/*******************************************************************************
* Copyright (c) 2015,�����Զ����Ƽ����޹�˾
 *            All rights reserved.
* �������ƣ�
* �� �� �ţ�1.0
* �� �� �ˣ�л�ֹ�
* ����ʱ�䣺2015-6-4
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
#include "GPIO.h"
#include "Buzzer.h"

/*******************************************************************************
*                               �ļ��ڲ�ʹ�ú궨��
********************************************************************************/
 

/*******************************************************************************
*                                 ȫ�ֺ���(����)����
********************************************************************************/
 

/*******************************************************************************
*                                 ��̬����(����)����
********************************************************************************/

/*******************************************************************************
* ��������:    BuzzerInit
* ��������:    ��ʼ��������
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void BuzzerInit(void)
{
    unsigned int Scaler = 0;
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    Scaler = SystemCoreClock / BuzzerFreq / 2;

    BuzzerTimerClockEnable();
    TIM_DeInit(BuzzerTimer);
    
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_Period = 100;
    TIM_TimeBaseStructure.TIM_Prescaler = (Scaler / 100) - 1;

    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(BuzzerTimer, &TIM_TimeBaseStructure);

    NVIC_InitStructure.NVIC_IRQChannel = BuzzerIRQ;
    NVIC_InitStructure.NVIC_IRQChannelPriority = BuzzerTimerPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearFlag(BuzzerTimer, TIM_FLAG_Update);
    TIM_ITConfig(BuzzerTimer, TIM_IT_Update, ENABLE);
}


/*******************************************************************************
* ��������:    BuzzerOn
* ��������:    ʵ�ַ�����������
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void BuzzerOn(void)
{
    TIM_Cmd(BuzzerTimer, ENABLE);
}


/*******************************************************************************
* ��������:    BuzzerOn
* ��������:    �رշ�����
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void BuzzerOff(void)
{
    TIM_Cmd(BuzzerTimer, DISABLE);
    TIM_ClearFlag(BuzzerTimer, TIM_FLAG_Update);
    Buzzer_Clr();
}


/*******************************************************************************
* ��������:    BuzzerOn
* ��������:    �رշ�����
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void BuzzerTimerIRQHander(void)
{
    TIM_ClearFlag(BuzzerTimer, TIM_FLAG_Update);
    GPIO_WriteBit(Buzzer_Port, Buzzer_Pin, (BitAction)((1-GPIO_ReadOutputDataBit(Buzzer_Port, Buzzer_Pin))));
}

 
/*************************************END OF FILE*******************************/


