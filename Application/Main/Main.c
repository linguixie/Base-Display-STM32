/*******************************************************************************
* Copyright (c) 2015,威鹏自动化科技有限公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-6-4
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：
********************************************************************************/
/*******************************************************************************
*                                    头  文  件
********************************************************************************/
#include "Lcd.h"
#include "GPIO.h"
#include "Key.h"
#include "SysTick.h"
#include "UI.h"
#include "Math.h"
#include "SoftTimer.h"
#include "Infrared.h"
#include "Display.h"
#include "Valve.h"
#include "Input.h"
#include "Output.h"
#include "EEPROM.h"
#include "Watchdog.h"
#include "Communication.h"
/*******************************************************************************
*                               文件内部使用宏定义
********************************************************************************/
 //-上电延时间(单位:秒)-
#define PowerOnTime              2       
#define PowerOn_Delay           (Multiplication(PowerOnTime, Delay_Second_Factor))


/*******************************************************************************
*                                 全局函数(变量)声明
********************************************************************************/


/*******************************************************************************
*                                 静态函数(变量)声明
********************************************************************************/
signed long PowerOnTimer = 0;


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void SystemConfig(void)
{
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
static void HardwareInit(void)
{ 
    InitTimer();
    SysTickInit();
    GpioInit();
    KeyInit();
    UI_Init();
    InfraredInit();
    LcdInit();
    WatchdogInit();
}

/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void PlatformInit(void)
{
    ValveInit();   
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
static void ApplicationInit(void)
{
    DisplayInit();
    CommunicationInit();
    InputInit();
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
static void PowerOnDeal(void)
{
    SetTimer(PowerOnTimer, PowerOn_Delay);
    InsertTimer(&PowerOnTimer);

    UI_Buzz(300, 300, 1);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
int main (void)
{
    HardwareInit();
    PlatformInit();
    ApplicationInit();

    PowerOnDeal();
  
#if DebugTmp
    InfraredSendEnable();
#endif

    while(1)
    {
        Feed_Watchdog();          //-喂狗-
        LcdReset();
        Task_Input();             //-输入-
        Task_Communication();     //-通讯-
        Task_Key();               //-按键-
        Task_Display();           //-显示-
        Task_Infrared();          //-红外- 
        Task_Output(); 
    }

    return 0;
}
/*************************************END OF FILE*******************************/
