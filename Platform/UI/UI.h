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
* ��    ����
********************************************************************************/
 
 
#ifndef _UI_H_
#define _UI_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*******************************************************************************
*                                  ͷ  ��  ��
********************************************************************************/
#include "Led.h"
#include "Buzzer.h"
/*******************************************************************************
*                                  ��  ��  ��
********************************************************************************/
#define UI_TIMER                      TIM15 
#define UI_TIMER_ClockEnable()        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE); 
#define UI_TIMER_SOURCE               TIM15_IRQn 
#define SystemUITimeOut_IRQHandler    TIM15_IRQHandler
#define UI_TICK_MS                    50      //UI�ĵδ�ʱ�� 50ms
#define UI_TIMER_Priority             3

#define LED_LightEver    0x01    //����
#define LED_SlakeEver    0x02	 //Ϩ��
#define LED_Flash        0x03	 //��˸
#define LED_FlashEver    0x04	 //һֱ��˸ֱ����һ������
#define LED_Invalid		 0x05    //������

#define LED_ON           0x01    //��
#define LED_OFF          0x00    //��

#define BUZZER_ON        0x01    //����
#define BUZZER_OFF       0x00    //ֹͣ

/*-����ʵ����������޸�-*/
#define Led_ShutLimit            (&LED[LED_SHUTLIMIT])
#define Led_OpenLimit            (&LED[LED_OPENLIMIT])
 
/*--*/
#define UI_Led_Flash_Fast(LedIndex)    (UI_LED_FlashEver(LedIndex, 300, 300))
#define UI_Led_Flash_Slow(LedIndex)    (UI_LED_FlashEver(LedIndex, 1000, 1000))

/*--*/
#define UI_Buzz_Long()           (UI_Buzz(500, 300, 1))
#define UI_Buzz_Short()          (UI_Buzz(300, 300, 1))
/*******************************************************************************
*                                  ȫ�ֱ�������
********************************************************************************/
typedef  void (*Function)(int);

/****************************************************************************
 ���������ƽṹ��
****************************************************************************/
struct BUZZER_Control
{
	unsigned short OnTime;	        //����ʱ��
	unsigned short OffTime;	        //��϶ʱ��
	unsigned char BuzzerNum;		//���д���
	unsigned char Status;           //��ǰ״̬
	signed int Counter;             //������
};

/****************************************************************************
 LED���ƽṹ��
****************************************************************************/
struct LED_Control
{
	unsigned char Model;		   //ģʽ
	unsigned short LightTime;	   //����ʱ�� ��˸ģʽ��Ч
	unsigned short SlakeTime;	   //Ϩ��ʱ�� ��˸ģʽ��Ч
	unsigned char FlashNum;		   //��˸����
	unsigned char Status;          //��ǰ״̬
	signed int Counter;            //������
};

/****************************************************************************
 LED���غ����ṹ��
****************************************************************************/
struct LED_OnOff
{
    Function On;
    Function Off;
}; 


extern struct LED_Control LED[LED_Number];
/*******************************************************************************
*                                  ȫ�ֺ�������
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


