/*******************************************************************************
* Copyright (c) 2105,�����Զ����Ƽ����޹�˾
 *            All rights reserved.
* �������ƣ�
* �� �� �ţ�1.0
* �� �� �ˣ�л�ֹ�
* ����ʱ�䣺2015-3-23
* �� �� ��:
* �޸�ʱ��:
* �޸�˵��:
* ��    ������ֲ��ע��
*           1��LED_onoff�����ʵ�ֺ���.
*           2��SystemUI_Led��Ҫ�������Ե���.
*           3��Led�����ֺ�LED����֮��Ķ�Ӧ��ϵ.
*           4����˸��T = (Count / Interval_Count * T0),����T0ΪSystemUI_Led�����õ�����
********************************************************************************/
/*******************************************************************************
*                                    ͷ  ��  ��
********************************************************************************/
#include "stm32f0xx_conf.h"
#include "stm32f0xx_it.h"
#include "UI.h"

/*******************************************************************************
*                               �ļ��ڲ�ʹ�ú궨��
********************************************************************************/
#define Interval_Count       UI_TICK_MS    
#define UI_Delay             1    //-��λ:SystemUI_Led�����õ�����-

/*******************************************************************************
*                                 ȫ�ֺ���(����)����
********************************************************************************/


#if (Led_Module_Enable == 1)
/****************************************************************************
 LED������״̬�Ȳ����Ľṹ�塣
****************************************************************************/
struct LED_Control LED[LED_Number];

/****************************************************************************
 LED���غ����Ľṹ�����飬���LED���������仯���޸Ĵ˽ṹ�塣
****************************************************************************/
const struct LED_OnOff LED_onoff[LED_Number] = 
{
    {LedOn, LedOff},
    {LedOn, LedOff},
};
#endif    /*-Led_Module_Enable == 1-*/

/*******************************************************************************
*                                 ��̬����(����)����
********************************************************************************/


#if (Led_Module_Enable > 0)
/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void UI_LED_On(struct LED_Control *SLED)
{
	SLED->Model = LED_LightEver;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void UI_LED_Off(struct LED_Control *SLED)
{
	SLED->Model = LED_SlakeEver;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
 �� �� ����static void SystemUI_Buzzer(void)
 ��    ������
 �� �� ֵ����
 �� �� �ߣ��ž���
 �������ڣ�2012-07-26
 ��    ����BUZZERɨ�躯������ʱɨ��Buzzer״̬����������Ӧ�Ķ�����
******************�޸���ʷ***************************************************
 
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
 �� �� ����void UI_Buzz(unsigned short OnTime, unsigned short OffTime, 
         :              unsigned short BuzzNumber)
 ��    ����unsigned short OnTime        ����ʱ��ms
         ��unsigned short OffTime       ���ʱ��ms
         ��unsigned short BuzzNumber    ���д���
 �� �� ֵ����
 �� �� �ߣ��ž���
 �������ڣ�2012-07-26
 ��    ��������������ָ������
******************�޸���ʷ***************************************************
 
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
 �� �� ����static void SystemUITimeOut_IRQHandler(void)
 ��    ������
 �� �� ֵ����
 �� �� �ߣ��ž���
 �������ڣ�2012-07-26
 ��    ����UI��ʱɨ���жϷ�����
******************�޸���ʷ***************************************************
 
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
 �� �� ����static void SetUITimer(unsigned int ms)
 ��    ������
 �� �� ֵ����
 �� �� �ߣ��ž���
 �������ڣ�2012-07-26
 ��    ��������UI��ʱɨ���������øú�������Ҫȷ��SystemCoreClock������ȷ��
         �������Ѿ����ϵͳʱ�ӵĳ�ʼ����
******************�޸���ʷ***************************************************
 
****************************************************************************/
static void SetUITimer(unsigned int ms)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    TIM_DeInit( UI_TIMER );
 
	UI_TIMER_ClockEnable();

    TIM_TimeBaseStructure.TIM_Period = ms*10;
	TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock/10000;	            //��������Ϊ0.1ms
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
 �� �� ����void SystemUI_Init(void)
 ��    ������
 �� �� ֵ����
 �� �� �ߣ��ž���
 �������ڣ�2012-07-26
 ��    ����UI��ʼ����������LED,KEY,Buzzer�˿ڵĳ�ʼ������ʱ���ĳ�ʼ���ȣ��ú�
         ����������ϵͳʱ�����á��Ͳ���ϵͳ��ʼ��֮��
******************�޸���ʷ***************************************************
 
****************************************************************************/
void SystemUI_Init(void)
{
	int i;
	SetUITimer(UI_TICK_MS);    //����Timer�ж�����

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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void UI_Init(void)
{
#if ((Led_Module_Enable > 0) || (Buzzer_Module_Enable > 0))
    SystemUI_Init();
#endif    /*-(Led_Module_Enable > 0) || (Buzzer_Module_Enable > 0)-*/
}


/*************************************END OF FILE*******************************/

