/*******************************************************************************
* Copyright (c) 2015,xxx��˾
 *            All rights reserved.
* �������ƣ�
* �� �� �ţ�1.0
* �� �� �ˣ�л�ֹ�
* ����ʱ�䣺2015-3-26
* �� �� ��:
* �޸�ʱ��:
* �޸�˵��:
* ��    ����
********************************************************************************/
 
 
#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*******************************************************************************
*                                  ͷ  ��  ��
********************************************************************************/


/*******************************************************************************
*                                  ��  ��  ��
********************************************************************************/
#define CommFreq                 8000
#define CommTimerClockEnable()   (RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE))
#define CommTimer                TIM1
#define CommIRQ                  TIM1_BRK_UP_TRG_COM_IRQn
#define CommTimerIRQHander       TIM1_BRK_UP_TRG_COM_IRQHandler
#define CommTimerPriority        1


/*------Э����ض���------*/
typedef enum
{
    Sync_Frame,
    Normal_Frame,
    Adjust_Frame,
    Setting_Frame
}FrameType_TypeDef;


typedef enum
{
    Frame_AdjustZero,
    Frame_AdjustFull,
    Frame_AdjustInput4mA,
    Frame_AdjustInput20mA,
    Frame_Invlid
}Adjust_FrameTypeDef;


//-����֡-
#define NormalFrame_IDCode                  0xC3
#define ReplyNormalFrame_IDCode             0xA3

#define ExtraNormalFrame_IDCode             0xC2
#define ReplyExtraNormalFrame_IDCode        0xA2

//-�궨֡-
#define AdjustFrame_IDCode                  0xC5
#define ReplyAdjustFrame_IDCode             0xA5

#define ExtraAdjustFrame_IDCode             0xC4
#define ReplyExtraAdjustFrame_IDCode        0xA4


//-����֡-
#define SettingFrame_IDCode                 0xC6
#define ReplySettingFrame_IDCode            0xA6

#define ExtraSettingFrame_IDCode            0xC7
#define ReplyExtraSettingFrame_IDCode       0xA7

//-ͬ��֡-
#define SyncFrame_IDCode                    0xA6
#define ExtraSyncFrame_IDCode               0xA7

//-֡���Ⱥ�����-
#define Frame_Size                          10       //-��Լ�й涨֡���̶�Ϊ10�ֽ�-
#define RecognizeIndex                      0        //-ʶ������֡���ֽ�����-
#define RecognizeIndex_2                    1        //-�ڶ�ʶ����(�е�֡������ʶ����)-
#define CodeIndex                           1        //-��������֡���ֽ�����-
#define CheckSumIndex                       9        //-У�����֡���ֽ�����-

 
 //-�ȴ�����ظ���ʱ��(��λ:����)-
#define WaitReplyTime                       400       
#define WaitReply_Delay                     (Division(WaitReplyTime, Delay_2MiliSecond_Factor))

/*******************************************************************************
*                                  ȫ�ֱ�������
********************************************************************************/
extern unsigned char RecvBuf[Frame_Size];
extern unsigned char SendBuf[Frame_Size];
extern unsigned char F_SendBufValid;

extern unsigned char F_WaitAdjustFrame;
extern unsigned char F_Disconnect;
extern signed long WaitReplyTimer;
extern unsigned char CurrentAdjustType;
/*******************************************************************************
*                                  ȫ�ֺ�������
********************************************************************************/
void CommunicationInit(void);
void SetFrameType(unsigned char Type);
void Task_Communication(void);
#ifdef __cplusplus
}
#endif
 
#endif /* _COMMUNICATION_H_ */
 
 
/*************************************END OF FILE*******************************/

