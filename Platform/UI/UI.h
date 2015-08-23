/*******************************************************************************
* Copyright (c) 2105,威鹏自动化科技有限公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-3-23
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：
********************************************************************************/
 
 
#ifndef _UI_H_
#define _UI_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*******************************************************************************
*                                  头  文  件
********************************************************************************/
#include "Led.h"
#include "Buzzer.h"
/*******************************************************************************
*                                  宏  定  义
********************************************************************************/
#define UI_TIMER                      TIM15 
#define UI_TIMER_ClockEnable()        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE); 
#define UI_TIMER_SOURCE               TIM15_IRQn 
#define SystemUITimeOut_IRQHandler    TIM15_IRQHandler
#define UI_TICK_MS                    50      //UI的滴答时间 50ms
#define UI_TIMER_Priority             3

#define LED_LightEver    0x01    //常亮
#define LED_SlakeEver    0x02	 //熄灭
#define LED_Flash        0x03	 //闪烁
#define LED_FlashEver    0x04	 //一直闪烁直到下一个动作
#define LED_Invalid		 0x05    //不操作

#define LED_ON           0x01    //亮
#define LED_OFF          0x00    //灭

#define BUZZER_ON        0x01    //鸣叫
#define BUZZER_OFF       0x00    //停止

/*-根据实际情况进行修改-*/
#define Led_ShutLimit            (&LED[LED_SHUTLIMIT])
#define Led_OpenLimit            (&LED[LED_OPENLIMIT])
 
/*--*/
#define UI_Led_Flash_Fast(LedIndex)    (UI_LED_FlashEver(LedIndex, 300, 300))
#define UI_Led_Flash_Slow(LedIndex)    (UI_LED_FlashEver(LedIndex, 1000, 1000))

/*--*/
#define UI_Buzz_Long()           (UI_Buzz(500, 300, 1))
#define UI_Buzz_Short()          (UI_Buzz(300, 300, 1))
/*******************************************************************************
*                                  全局变量声明
********************************************************************************/
typedef  void (*Function)(int);

/****************************************************************************
 蜂鸣器控制结构体
****************************************************************************/
struct BUZZER_Control
{
	unsigned short OnTime;	        //鸣叫时间
	unsigned short OffTime;	        //间隙时间
	unsigned char BuzzerNum;		//鸣叫次数
	unsigned char Status;           //当前状态
	signed int Counter;             //计数器
};

/****************************************************************************
 LED控制结构体
****************************************************************************/
struct LED_Control
{
	unsigned char Model;		   //模式
	unsigned short LightTime;	   //亮灯时间 闪烁模式有效
	unsigned short SlakeTime;	   //熄灭时间 闪烁模式有效
	unsigned char FlashNum;		   //闪烁次数
	unsigned char Status;          //当前状态
	signed int Counter;            //计数器
};

/****************************************************************************
 LED开关函数结构体
****************************************************************************/
struct LED_OnOff
{
    Function On;
    Function Off;
}; 


extern struct LED_Control LED[LED_Number];
/*******************************************************************************
*                                  全局函数声明
********************************************************************************/
#if (Led_Module_Enable > 0)
void UI_LED_Flash(struct LED_Control *SLED, unsigned short LightTime, 
                  unsigned short SlakeTime, unsigned char FlashNum);
void UI_LED_FlashEver(struct LED_Control *SLED, unsigned short LightTime, 
                      unsigned short SlakeTime);
void UI_LED_On(struct LED_Control *SLED);
void UI_LED_Off(struct LED_Control *SLED);
#endif    /*-(Led_Module_Enable > 0)-*/

#if (Buzzer_Module_Enable > 0)
void UI_Buzz(unsigned short OnTime, unsigned short OffTime, 
          unsigned short BuzzNumber);
#endif    /*-(Buzzer_Module_Enable > 0)-*/

void SystemUITimeOut_IRQHandler(void);

void UI_Init(void);
#ifdef __cplusplus
}
#endif
 
#endif /* _UI_H_ */
 
 
/*************************************END OF FILE*******************************/


