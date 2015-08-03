/*******************************************************************************
* Copyright (c) 2014,xxx公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-5-29
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：
********************************************************************************/


#ifndef _SOFTTIMER_H_
#define _SOFTTIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*                                  头  文  件
********************************************************************************/
#include "Config.h"


/*******************************************************************************
*                                  宏  定  义
********************************************************************************/
//-根据实际情况进行修改(单位:ms)-
#define Delay_Unit                2            //-延时的单位,ms-
#define Delay_MiliSecond_Factor   Delay_Unit   //-延时单位为毫秒是,需要!!除以的因子-
#define Delay_Second_Factor       500          //-延时单位为秒时,需要乘以的因子-
#define Delay_Minute_Factor       30000       


#define IsTimeOut(Counter)        (Counter < 0)
#define SetTimer(Counter, Value)  (Counter = Value)
#define StopTimer(Counter)        (Counter = 0)

/*******************************************************************************
*                                  全局变量声明
********************************************************************************/
extern volatile unsigned char F_Delay2MiliSec;
extern volatile unsigned char F_Delay1Sec;
extern volatile unsigned char F_Delay1Min;

/*******************************************************************************
*                                  全局函数声明
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



