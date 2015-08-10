/*******************************************************************************
* Copyright (c) 2015,威鹏自动化科技有限公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-6-5
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：
********************************************************************************/
 
 
#ifndef _CONFIG_H_
#define _CONFIG_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*******************************************************************************
*                                  头  文  件
********************************************************************************/
#include "Math.h"
#include "SysTick.h"
 
/*******************************************************************************
*                                  宏  定  义
********************************************************************************/
#define DebugTmp   0      //-临时调试代码(中间调试代码宏)-


/*-SoftTimer时基(单位:ms)-*/
#define SoftTimerUnit    (Division(1000, SYSTEM_TICK_FREQ))           
 
/*******************************************************************************
*                                  全局变量声明
********************************************************************************/
#define USE_Chinese              1

 
/*******************************************************************************
*                                  全局函数声明
********************************************************************************/
 
#ifdef __cplusplus
}
#endif
 
#endif /* _CONFIG_H_ */
 
 
/*************************************END OF FILE*******************************/


