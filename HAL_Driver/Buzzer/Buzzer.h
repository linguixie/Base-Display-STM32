/*******************************************************************************
* Copyright (c) 2105,威鹏自动化科技有限公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-3-16
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：
********************************************************************************/
 
 
#ifndef __BUZZER_H_
#define __BUZZER_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*******************************************************************************
*                                  头  文  件
********************************************************************************/
#include "GPIO.h"
 
/*******************************************************************************
*                                  宏  定  义
********************************************************************************/
#define Buzzer_Module_Enable      1
#define BuzzerFreq                4000

#define BuzzerTimerClockEnable()  (RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE))
#define BuzzerTimer               TIM14
#define BuzzerIRQ                 TIM14_IRQn
#define BuzzerTimerIRQHander      TIM14_IRQHandler
#define BuzzerTimerPriority       1
 
/*******************************************************************************
*                                  全局函数(变量)声明
********************************************************************************/
void BuzzerInit(void);
void BuzzerOn(void);
void BuzzerOff(void);
#ifdef __cplusplus
}
#endif
 
#endif /* __BUZZER_H_ */
 
 
/*************************************END OF FILE*******************************/


