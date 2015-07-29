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
 
 
#ifndef _INFRARED_H_
#define _INFRARED_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*******************************************************************************
*                                  ͷ  ��  ��
********************************************************************************/
 
 
/*******************************************************************************
*                                  ��  ��  ��
********************************************************************************/
//-����-
#define Min_Period                        4
#define Max_Period                        25
#define Mid_Period                        17
#define Code_BitNum                       12
#define Edge_Num                          12
#define Mask_0_1                          0xF8
#define Code_Open                         0x0520
#define Code_Shut                         0x0510
#define Code_Set                          0x0508
#define Code_Back                         0x0504
#define Code_Up                           0x0502
#define Code_Down                         0x0501
 

#define InfraredRecvTimerClockEnable()    (RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE))
#define InfraredRecvTimer                 TIM3
#define InfraredRecvTimerIRQ              TIM3_IRQn
#define InfraredRecvTimerIRQHandler       TIM3_IRQHandler
#define InfraredRecvTimerPriority         2
#define InfraredRecvTimerFreq             8000   

#define InfraredRecv_EXTI_ClockEnable()   (RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE))
#define InfraredRecv_EXTI_LineConfig()    (SYSCFG_EXTILineConfig(InfraredRecv_EXTI_PortSource, InfraredRecv_EXTI_PinSource))
#define InfraredRecv_EXTI_Trigger_Mode     EXTI_Trigger_Rising


#define InfraredSendModulationTimerClockEnable()    (RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE))
#define InfraredSendModulationTimer       TIM16
#define InfraredSendModulationTimerFreq   1000
#define InfraredSendModulationTimerDuty   50
#define InfraredSendModulationChannelInit TIM_OC1Init
#define InfraredSendModulationChannelSet  TIM_SetCompare1

#define InfraredSendCarrierTimerClockEnable()       (RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE))
#define InfraredSendCarrierTimer          TIM17  
#define InfraredSendCarrierTimerFreq      38000
#define InfraredSendCarrierTimerDuty      75
#define InfraredSendCarrierChannelInit    TIM_OC1Init
#define InfraredSendCarrierChannelSet     TIM_SetCompare1

/*******************************************************************************
*                                  ȫ�ֱ�������
********************************************************************************/

 
/*******************************************************************************
*                                  ȫ�ֺ�������
********************************************************************************/
void InfraredInit(void);
void InfraredRecvTimerIRQHandler(void);
void Task_Infrared(void); 
unsigned int GetIRKeyCode(void);

#ifdef __cplusplus
}
#endif
 
#endif /* _INFRARED_H_ */
 
 
/*************************************END OF FILE*******************************/


