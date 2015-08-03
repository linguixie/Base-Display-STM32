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
* 其    他：移植需注意
*           1、LED_onoff数组的实现函数.
*           2、SystemUI_Led需要被周期性调用.
*           3、Led宏名字和LED数组之间的对应关系.
*           4、闪烁的T = (Count / Interval_Count * T0),其中T0为SystemUI_Led被调用的周期
********************************************************************************/
/*******************************************************************************
*                                    头  文  件
********************************************************************************/
#include "stm32f0xx_conf.h"
#include "stm32f0xx_it.h"
#include "UI.h"

/*******************************************************************************
*                               文件内部使用宏定义
********************************************************************************/
#define Interval_Count       UI_TICK_MS    
#define UI_Delay             1    //-单位:SystemUI_Led被调用的周期-

/*******************************************************************************
*                                 全局函数(变量)声明
********************************************************************************/


#if (Led_Module_Enable == 1)
/****************************************************************************
 LED处理中状态等参数的结构体。
****************************************************************************/
struct LED_Control LED[LED_Number];

/****************************************************************************
 LED开关函数的结构体数组，如果LED数量发生变化，修改此结构体。
****************************************************************************/
const struct LED_OnOff LED_onoff[LED_Number] = 
{
    {LedOn, LedOff},
    {LedOn, LedOff},
};
#endif    /*-Led_Module_Enable == 1-*/

/*******************************************************************************
*                                 静态函数(变量)声明
********************************************************************************/


#if (Led_Module_Enable > 0)
/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void UI_LED_Flash(struct LED_Control *SLED, unsigned short LightTime, 
                  unsigned short SlakeTime, unsigned char FlashNum)
{
	SLED->Status = LED_OFF;
	SLED->LightTime = LightTime;
	SLED->SlakeTime = SlakeTime;
	SLED->FlashNum = FlashNum;
	SLED->Model = LED_Flash;
	SLED->Counter = 0;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void UI_LED_FlashEver(struct LED_Control *SLED, unsigned short LightTime, 
                      unsigned short SlakeTime)
{
	SLED->Status = LED_OFF;
	SLED->LightTime = LightTime;
	SLED->SlakeTime = SlakeTime;
	SLED->Model = LED_FlashEver;
	SLED->Counter = 0;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void UI_LED_On(struct LED_Control *SLED)
{
	SLED->Model = LED_LightEver;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void UI_LED_Off(struct LED_Control *SLED)
{
	SLED->Model = LED_SlakeEver;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void SystemUI_Led(void)
{
    int i;

    for(i=0;i<LED_Number;i++)
	{
		if(LED[i].Model == LED_Invalid)
			continue;
		if(LED[i].Model == LED_LightEver)
		{
			LED_onoff[i].On(i);
			continue;
		}
		if(LED[i].Model == LED_SlakeEver)
		{
			LED_onoff[i].Off(i);
			continue;
		}
		if(LED[i].Model == LED_Flash)
		{
			LED[i].Counter -= Interval_Count;
			if(LED[i].Counter <= 0)
			{
				if(LED[i].Status == LED_OFF)
				{
					LED_onoff[i].On(i);
					LED[i].Counter = LED[i].LightTime;
					LED[i].Status = LED_ON;
				}
				else
				{
					LED_onoff[i].Off(i);
					LED[i].Counter = LED[i].SlakeTime;
					LED[i].Status = LED_OFF;
					LED[i].FlashNum--;
					if(LED[i].FlashNum == 0)
						LED[i].Model = LED_Invalid;
				}
			}
			continue;
		}
		if(LED[i].Model == LED_FlashEver)
		{
			LED[i].Counter -= Interval_Count;
			if(LED[i].Counter <= 0)
			{
				if(LED[i].Status == LED_OFF)
				{
					LED_onoff[i].On(i);
					LED[i].Counter = LED[i].LightTime;
					LED[i].Status = LED_ON;
				}
				else
				{
					LED_onoff[i].Off(i);
					LED[i].Counter = LED[i].SlakeTime;
					LED[i].Status = LED_OFF;
				}
			}
			
		}
	}
}
#endif    /*-(Led_Module_Enable > 0)-*/
 

#if (Buzzer_Module_Enable > 0)
struct BUZZER_Control Buzzer;

/****************************************************************************
 函 数 名：static void SystemUI_Buzzer(void)
 参    数：无
 返 回 值：无
 创 建 者：张军杰
 创建日期：2012-07-26
 描    述：BUZZER扫描函数，定时扫描Buzzer状态，并进行响应的动作。
******************修改历史***************************************************
 
****************************************************************************/
static void SystemUI_Buzzer(void)
{
    Buzzer.Counter -= Interval_Count;

    if(Buzzer.BuzzerNum <= 0)
    {
        return;
    }

    if(Buzzer.Counter <= 0)
    {
        if(Buzzer.Status == BUZZER_OFF)
        {
            BuzzerOn();
            Buzzer.Counter = Buzzer.OnTime;
            Buzzer.Status = BUZZER_ON;
        }
        else
        {
            BuzzerOff();
            Buzzer.Counter = Buzzer.OffTime;
            Buzzer.Status = BUZZER_OFF;
            Buzzer.BuzzerNum--;
        }
    }
			
}


/****************************************************************************
 函 数 名：void UI_Buzz(unsigned short OnTime, unsigned short OffTime, 
         :              unsigned short BuzzNumber)
 参    数：unsigned short OnTime        鸣叫时间ms
         ：unsigned short OffTime       间隔时间ms
         ：unsigned short BuzzNumber    鸣叫次数
 返 回 值：无
 创 建 者：张军杰
 创建日期：2012-07-26
 描    述：蜂鸣器鸣叫指定次数
******************修改历史***************************************************
 
****************************************************************************/
void UI_Buzz(unsigned short OnTime, unsigned short OffTime, 
          unsigned short BuzzNumber)
{
    Buzzer.Status = BUZZER_OFF;
	Buzzer.OnTime = OnTime;
	Buzzer.OffTime = OffTime;
    Buzzer.BuzzerNum = BuzzNumber;
	Buzzer.Counter = 0;
}

#endif   /* Buzzer_Enable > 0  */


/****************************************************************************
 函 数 名：static void SystemUITimeOut_IRQHandler(void)
 参    数：无
 返 回 值：无
 创 建 者：张军杰
 创建日期：2012-07-26
 描    述：UI定时扫描中断服务函数
******************修改历史***************************************************
 
****************************************************************************/
#if ((Led_Module_Enable > 0) || (Buzzer_Module_Enable > 0))
void SystemUITimeOut_IRQHandler(void)
{	
  	TIM_ClearFlag(UI_TIMER, TIM_FLAG_Update);
	
    #if (Led_Module_Enable > 0)
	SystemUI_Led();
    #endif   /* LED_Modle_Enable > 0  */
    
    #if (Buzzer_Module_Enable > 0)
    SystemUI_Buzzer();
    #endif   /* Buzzer_Enable > 0  */
}

/****************************************************************************
 函 数 名：static void SetUITimer(unsigned int ms)
 参    数：无
 返 回 值：无
 创 建 者：张军杰
 创建日期：2012-07-26
 描    述：设置UI定时扫描间隔，调用该函数，需要确认SystemCoreClock参数正确，
         ：并且已经完成系统时钟的初始化。
******************修改历史***************************************************
 
****************************************************************************/
static void SetUITimer(unsigned int ms)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    TIM_DeInit( UI_TIMER );
 
	UI_TIMER_ClockEnable();

    TIM_TimeBaseStructure.TIM_Period = ms*10;
	TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock/10000;	            //计数周期为0.1ms
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(UI_TIMER, &TIM_TimeBaseStructure);
  	
    TIM_ClearFlag(UI_TIMER, TIM_FLAG_Update);

    TIM_ITConfig(UI_TIMER, TIM_IT_Update, ENABLE); 

    NVIC_InitStructure.NVIC_IRQChannel = UI_TIMER_SOURCE;
    NVIC_InitStructure.NVIC_IRQChannelPriority = UI_TIMER_Priority;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(UI_TIMER, ENABLE);
}

/****************************************************************************
 函 数 名：void SystemUI_Init(void)
 参    数：无
 返 回 值：无
 创 建 者：张军杰
 创建日期：2012-07-26
 描    述：UI初始化，包括对LED,KEY,Buzzer端口的初始化，定时器的初始化等，该函
         ：调用须在系统时钟配置、和操作系统初始化之后。
******************修改历史***************************************************
 
****************************************************************************/
void SystemUI_Init(void)
{
	int i;
	SetUITimer(UI_TICK_MS);    //设置Timer中断周期

    #if (Led_Module_Enable > 0)
        LedInit();
        for(i=0;i<LED_Number;i++)
            LED[i].Model = LED_Invalid;
    #endif   /* LED_Modle_Enable > 0  */

    #if (Buzzer_Module_Enable > 0)
        BuzzerInit();
        Buzzer.BuzzerNum = 0;
    #endif   /* Buzzer_Enable > 0  */

    
}
#endif /*  LED_Modle_Enable > 0 || Buzzer_Enable > 0   */

/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void UI_Init(void)
{
#if ((Led_Module_Enable > 0) || (Buzzer_Module_Enable > 0))
    SystemUI_Init();
#endif    /*-(Led_Module_Enable > 0) || (Buzzer_Module_Enable > 0)-*/
}


/*************************************END OF FILE*******************************/

