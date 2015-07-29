/*******************************************************************************
* Copyright (c) 2014,xxx��˾
 *            All rights reserved.
* �������ƣ�
* �� �� �ţ�1.0
* �� �� �ˣ�л�ֹ�
* ����ʱ�䣺2015-5-29
* �� �� ��:
* �޸�ʱ��:
* �޸�˵��:
* ��    ����
********************************************************************************/


#ifndef _SOFTTIMER_H_
#define _SOFTTIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*                                  ͷ  ��  ��
********************************************************************************/
#include "Config.h"


/*******************************************************************************
*                                  ��  ��  ��
********************************************************************************/
//-����ʵ����������޸�(��λ:ms)-
#define Delay_Unit                SoftTimerUnit
#define Delay_2MiliSecond_Factor  (1)        
#define Delay_Second_Factor       (Division(1000, Delay_Unit))
#define Delay_Minute_Factor       (Division(60000, Delay_Unit))     


#define IsTimeOut(Counter)        (Counter < 0)
#define SetTimer(Counter, Value)  (Counter = Value)
#define StopTimer(Counter)        (Counter = 0)

/*******************************************************************************
*                                  ȫ�ֱ�������
********************************************************************************/
extern volatile unsigned char F_Delay2MiliSec;
extern volatile unsigned char F_Delay1Sec;
extern volatile unsigned char F_Delay1Min;

/*******************************************************************************
*                                  ȫ�ֺ�������
********************************************************************************/
void InitTimer(void);
unsigned char InsertTimer(signed long *Count);
unsigned char DeleteTimer(signed long *Count);
void TimerProcess(void);
void Task_Timer(void);

#ifdef __cplusplus
}
#endif

#endif /* _SOFTTIMER_H_ */


/*************************************END OF FILE*******************************/



