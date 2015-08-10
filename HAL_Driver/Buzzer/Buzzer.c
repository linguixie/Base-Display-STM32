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
* 其    他：
********************************************************************************/
/*******************************************************************************
*                                    头  文  件
********************************************************************************/
#include "stm32f0xx_conf.h"
#include "stm32f0xx_it.h"
#include "GPIO.h"
#include "Buzzer.h"

/*******************************************************************************
*                               文件内部使用宏定义
********************************************************************************/
 

/*******************************************************************************
*                                 全局函数(变量)声明
********************************************************************************/
 

/*******************************************************************************
*                                 静态函数(变量)声明
********************************************************************************/

/*******************************************************************************
* 函数名称:    BuzzerInit
* 函数功能:    初始化蜂鸣器
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    BuzzerOn
* 函数功能:    实现蜂鸣器的鸣叫
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void BuzzerOn(void)
{
    TIM_Cmd(BuzzerTimer, ENABLE);
}


/*******************************************************************************
* 函数名称:    BuzzerOn
* 函数功能:    关闭蜂鸣器
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void BuzzerOff(void)
{
    TIM_Cmd(BuzzerTimer, DISABLE);
    TIM_ClearFlag(BuzzerTimer, TIM_FLAG_Update);
    Buzzer_Clr();
}


/*******************************************************************************
* 函数名称:    BuzzerOn
* 函数功能:    关闭蜂鸣器
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void BuzzerTimerIRQHander(void)
{
    TIM_ClearFlag(BuzzerTimer, TIM_FLAG_Update);
    GPIO_WriteBit(Buzzer_Port, Buzzer_Pin, (BitAction)((1-GPIO_ReadOutputDataBit(Buzzer_Port, Buzzer_Pin))));
}

 
/*************************************END OF FILE*******************************/


