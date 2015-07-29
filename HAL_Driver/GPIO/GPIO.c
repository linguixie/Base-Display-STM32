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
* 其    他：集中设置所有IO口
********************************************************************************/
/*******************************************************************************
*                                    头  文  件
********************************************************************************/
#include "stm32f0xx_conf.h"
#include "Config.h"
#include "GPIO.h"

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
* 函数名称:    GpioInit
* 函数功能:    对所有涉及到的IO口进行初始化
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void GpioInit(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;    

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC
                          | RCC_AHBPeriph_GPIOF, ENABLE);

    /*-Buzzer-*/
    GPIO_InitStructure.GPIO_Pin   = Buzzer_Pin ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
    GPIO_Init(Buzzer_Port, &GPIO_InitStructure);


    /*-Communication-*/
    //-CS-
    GPIO_InitStructure.GPIO_Pin   = COMM_CS_Pin ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
    GPIO_Init(COMM_Port, &GPIO_InitStructure); 
    COMM_CSClr();
    //-MISO-  
    GPIO_InitStructure.GPIO_Pin  = COMM_MISO_Pin ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; 
    GPIO_Init(COMM_Port, &GPIO_InitStructure); 
    //-MOSI-   
    GPIO_InitStructure.GPIO_Pin   = COMM_MOSI_Pin ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
    GPIO_Init(COMM_Port, &GPIO_InitStructure);
    COMM_MOSIClr();
    //-CLK-
    GPIO_InitStructure.GPIO_Pin   = COMM_CLK_Pin ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
    GPIO_Init(COMM_Port, &GPIO_InitStructure);
    COMM_CLKClr();


    /*-Infrared-*/
    //-Recv-
    GPIO_InitStructure.GPIO_Pin  = InfraredRecv_Pin ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
    GPIO_Init(Infrared_Port, &GPIO_InitStructure);
    //-Send-
    GPIO_InitStructure.GPIO_Pin   = InfraredSend_Pin ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
    GPIO_Init(Infrared_Port, &GPIO_InitStructure);
    //-复用-
    GPIO_PinAFConfig(Infrared_Port, InfraredSend_PinSource, InfraredSend_AF);


    /*-Key-*/
    //-Remote-
#if DebugTmp
    RCC_MCOConfig(RCC_MCOSource_SYSCLK, RCC_MCOPrescaler_1);
    GPIO_InitStructure.GPIO_Pin = Key_Remote_Pin;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(Key_Port, &GPIO_InitStructure); 
    GPIO_PinAFConfig(Key_Port, GPIO_PinSource8, GPIO_AF_0);    
#else
    GPIO_InitStructure.GPIO_Pin  = Key_Remote_Pin ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ; 
    GPIO_Init(Key_Port, &GPIO_InitStructure);
#endif

    //-Local-
    GPIO_InitStructure.GPIO_Pin  = Key_Local_Pin ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ; 
    GPIO_Init(Key_Port, &GPIO_InitStructure);   
    //-Next- 
    GPIO_InitStructure.GPIO_Pin  = Key_Next_Pin ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ; 
    GPIO_Init(Key_Port, &GPIO_InitStructure);
    //-OK-
    GPIO_InitStructure.GPIO_Pin  = Key_OK_Pin ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ; 
    GPIO_Init(Key_Port, &GPIO_InitStructure);
    //-ESC-
    GPIO_InitStructure.GPIO_Pin  = Key_ESC_Pin ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ; 
    GPIO_Init(Key_Port, &GPIO_InitStructure);


    /*-LED-*/
    //-OpenLimit-
    GPIO_InitStructure.GPIO_Pin   = Led_OpenLimit_Pin ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
    GPIO_Init(Led_Port, &GPIO_InitStructure); 
    //-ShutLimit-
    GPIO_InitStructure.GPIO_Pin   = Led_ShutLimit_Pin ;
    GPIO_Init(Led_Port, &GPIO_InitStructure); 
    //-Error-
    GPIO_InitStructure.GPIO_Pin   = Led_Error_Pin ;
    GPIO_Init(Led_Port, &GPIO_InitStructure);     


    /*-OLED-*/
    //-RES-
    GPIO_InitStructure.GPIO_Pin   = OLed_RES_Pin ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(OLed_RES_Port, &GPIO_InitStructure); 
    OLed_RESSet();
    //-CS-
    GPIO_InitStructure.GPIO_Pin   = OLed_CS_Pin ;
    GPIO_Init(OLed_CS_Port, &GPIO_InitStructure); 
    OLed_CSSet();
    //-DC-
    GPIO_InitStructure.GPIO_Pin   = OLed_DC_Pin ;
    GPIO_Init(OLed_DC_Port, &GPIO_InitStructure); 
    OLed_DCSet();
    //-RD-
    GPIO_InitStructure.GPIO_Pin   = OLed_RD_Pin ;
    GPIO_Init(OLed_RD_Port, &GPIO_InitStructure); 
    OLed_RDSet();
    //-WR-
    GPIO_InitStructure.GPIO_Pin   = OLed_WR_Pin ;
    GPIO_Init(OLed_WR_Port, &GPIO_InitStructure); 
    OLed_WRSet();
    //-D0-D7-
#if DebugTmp
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_1);   

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7 ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_5); 
#else
    GPIO_InitStructure.GPIO_Pin   = OLed_D0_Pin ;
    GPIO_Init(OLed_Data_Port, &GPIO_InitStructure);
 
    GPIO_InitStructure.GPIO_Pin   = OLed_D1_Pin ;
    GPIO_Init(OLed_Data_Port, &GPIO_InitStructure); 

    GPIO_InitStructure.GPIO_Pin   = OLed_D2_Pin ;
    GPIO_Init(OLed_Data_Port, &GPIO_InitStructure);
 
    GPIO_InitStructure.GPIO_Pin   = OLed_D3_Pin ;
    GPIO_Init(OLed_Data_Port, &GPIO_InitStructure);
 
    GPIO_InitStructure.GPIO_Pin   = OLed_D4_Pin ;
    GPIO_Init(OLed_Data_Port, &GPIO_InitStructure);
 
    GPIO_InitStructure.GPIO_Pin   = OLed_D5_Pin ;
    GPIO_Init(OLed_Data_Port, &GPIO_InitStructure); 

    GPIO_InitStructure.GPIO_Pin   = OLed_D6_Pin ;
    GPIO_Init(OLed_Data_Port, &GPIO_InitStructure); 

    GPIO_InitStructure.GPIO_Pin   = OLed_D7_Pin ;
    GPIO_Init(OLed_Data_Port, &GPIO_InitStructure);
    OLed_WriteDataBus(0x00);
#endif
 

    /*-Font Chip-*/ 
    //-CS-
    GPIO_InitStructure.GPIO_Pin   = ROM_CS_Pin ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
    GPIO_Init(ROM_CS_Port, &GPIO_InitStructure);
    ROM_CSSet();
    //-MISO-  
    GPIO_InitStructure.GPIO_Pin  = ROM_MISO_Pin ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ; 
    GPIO_Init(ROM_MISO_Port, &GPIO_InitStructure); 
    //-MOSI-   
    GPIO_InitStructure.GPIO_Pin   = ROM_MOSI_Pin ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
    GPIO_Init(ROM_MOSI_Port, &GPIO_InitStructure); 
    ROM_MOSISet();
    //-CLK-
    GPIO_InitStructure.GPIO_Pin   = ROM_CLK_Pin ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
    GPIO_Init(ROM_CLK_Port, &GPIO_InitStructure);
    ROM_CLKSet();
}

 
/*************************************END OF FILE*******************************/

