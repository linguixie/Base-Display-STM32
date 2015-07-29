/*******************************************************************************
* Copyright (c) 2105,威鹏自动化科技有限公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-6-26
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：
********************************************************************************/
/*******************************************************************************
*                                    头  文  件
********************************************************************************/
#include "Watchdog.h"
#include "stm32f0xx_iwdg.h"

/*******************************************************************************
*                               文件内部使用宏定义
********************************************************************************/
 

/*******************************************************************************
*                                 全局函数(变量)声明
********************************************************************************/
__IO uint32_t LsiFreq = 40000;

/*******************************************************************************
*                                 静态函数(变量)声明
********************************************************************************/


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void WatchdogInit(void)
{
    if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
    {
        /* IWDGRST flag set */
    
        /* Clear reset flags */
        RCC_ClearFlag();
    }
    else
    {
        /* IWDGRST flag is not set */
    }

    /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
       dispersion) */
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  
    /* IWDG counter clock: LSI/32 */
    IWDG_SetPrescaler(IWDG_Prescaler_32);
  
    /* Set counter reload value to obtain 250ms IWDG TimeOut.
       Counter Reload Value = 250ms/IWDG counter clock period
                            = 250ms / (LSI/32)
                            = 0.25s / (LsiFreq/32)
                            = LsiFreq/(32 * 4)
                            = LsiFreq/128
     */
    IWDG_SetReload(LsiFreq/128);
  
    /* Reload IWDG counter */
    IWDG_ReloadCounter();
  
    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
}



/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Feed_Watchdog(void)
{
    /* Reload IWDG counter */
    IWDG_ReloadCounter(); 
}

 
/*************************************END OF FILE*******************************/

