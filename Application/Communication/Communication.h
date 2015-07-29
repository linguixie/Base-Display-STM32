/*******************************************************************************
* Copyright (c) 2015,xxx公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-3-26
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：
********************************************************************************/
 
 
#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*******************************************************************************
*                                  头  文  件
********************************************************************************/


/*******************************************************************************
*                                  宏  定  义
********************************************************************************/
#define CommFreq                 8000
#define CommTimerClockEnable()   (RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE))
#define CommTimer                TIM1
#define CommIRQ                  TIM1_BRK_UP_TRG_COM_IRQn
#define CommTimerIRQHander       TIM1_BRK_UP_TRG_COM_IRQHandler
#define CommTimerPriority        1


/*------协议相关定义------*/
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


//-常显帧-
#define NormalFrame_IDCode                  0xC3
#define ReplyNormalFrame_IDCode             0xA3

#define ExtraNormalFrame_IDCode             0xC2
#define ReplyExtraNormalFrame_IDCode        0xA2

//-标定帧-
#define AdjustFrame_IDCode                  0xC5
#define ReplyAdjustFrame_IDCode             0xA5

#define ExtraAdjustFrame_IDCode             0xC4
#define ReplyExtraAdjustFrame_IDCode        0xA4


//-设置帧-
#define SettingFrame_IDCode                 0xC6
#define ReplySettingFrame_IDCode            0xA6

#define ExtraSettingFrame_IDCode            0xC7
#define ReplyExtraSettingFrame_IDCode       0xA7

//-同步帧-
#define SyncFrame_IDCode                    0xA6
#define ExtraSyncFrame_IDCode               0xA7

//-帧长度和索引-
#define Frame_Size                          10       //-规约中规定帧长固定为10字节-
#define RecognizeIndex                      0        //-识别码在帧的字节索引-
#define RecognizeIndex_2                    1        //-第二识别码(有的帧有两个识别码)-
#define CodeIndex                           1        //-功能码在帧的字节索引-
#define CheckSumIndex                       9        //-校验和在帧的字节索引-

 
 //-等待主板回复延时间(单位:毫秒)-
#define WaitReplyTime                       400       
#define WaitReply_Delay                     (Division(WaitReplyTime, Delay_2MiliSecond_Factor))

/*******************************************************************************
*                                  全局变量声明
********************************************************************************/
extern unsigned char RecvBuf[Frame_Size];
extern unsigned char SendBuf[Frame_Size];
extern unsigned char F_SendBufValid;

extern unsigned char F_WaitAdjustFrame;
extern unsigned char F_Disconnect;
extern signed long WaitReplyTimer;
extern unsigned char CurrentAdjustType;
/*******************************************************************************
*                                  全局函数声明
********************************************************************************/
void CommunicationInit(void);
void SetFrameType(unsigned char Type);
void Task_Communication(void);
#ifdef __cplusplus
}
#endif
 
#endif /* _COMMUNICATION_H_ */
 
 
/*************************************END OF FILE*******************************/

