/*******************************************************************************
* Copyright (c) 2105,威鹏自动化科技有限公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-3-16
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：
********************************************************************************/
/*******************************************************************************
*                                    头  文  件
********************************************************************************/
#include "Output.h"
#include "UI.h"
#include "SoftTimer.h"
#include "Valve.h"

/*******************************************************************************
*                               文件内部使用宏定义
********************************************************************************/


/*******************************************************************************
*                                 全局函数(变量)声明
********************************************************************************/

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
void OutputInit(void)
{
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void AssistantOutput(void)
{
    static unsigned char PreValveStatus = 0xFF;
    static unsigned char PreESDStatus   = 0xFF; 
  
    unsigned ValveStatusChanged = 0;
    unsigned ESDStatusChanged   = 0;   

    if (PreValveStatus != Valve.Status.StatusByte)
    {
        PreValveStatus = Valve.Status.StatusByte;
        ValveStatusChanged = 1;
    }

    if (PreESDStatus != Device.Status.ESDStatus)
    {
        PreESDStatus = Device.Status.ESDStatus;
        ESDStatusChanged = 1;
    }

    /*-根据实际情况进行修改-*/
    if (Valve.Status.StatusBits.Opening == 1)
    {
        if (ValveStatusChanged == 1)
        {
            UI_LED_FlashEver(Led_OpenLimit, 300, 300);
            UI_LED_On(Led_ShutLimit);
        }
    }
    else if (Valve.Status.StatusBits.Shutting == 1)
    {
        if (ValveStatusChanged == 1)
        {
            UI_LED_FlashEver(Led_ShutLimit, 300, 300);
            UI_LED_On(Led_OpenLimit);
        }
    }
    else if (Valve.Status.StatusBits.OpenLimit == 1)
    {
        UI_LED_On(Led_OpenLimit);
        UI_LED_Off(Led_ShutLimit);
    }
    else if (Valve.Status.StatusBits.ShutLimit == 1)
    {
        UI_LED_On(Led_ShutLimit);
        UI_LED_Off(Led_OpenLimit);
    }
    else 
    {
        UI_LED_On(Led_OpenLimit);
        UI_LED_On(Led_ShutLimit); 
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Task_Output(void)
{
    AssistantOutput();
}

 
/*************************************END OF FILE*******************************/


