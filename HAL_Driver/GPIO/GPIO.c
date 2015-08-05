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
    GPIO_Init(COMM_CS_Port, &GPIO_InitStructure); 
    COMM_CSClr();
    //-MISO-  
    GPIO_InitStructure.GPIO_Pin  = COMM_MISO_Pin ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(COMM_MISO_Port, &GPIO_InitStructure); 
    //-MOSI-   
    GPIO_InitStructure.GPIO_Pin   = COMM_MOSI_Pin ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
    GPIO_Init(COMM_MOSI_Port, &GPIO_InitStructure);
    COMM_MOSISet();
    //-CLK-
    GPIO_InitStructure.GPIO_Pin   = COMM_CLK_Pin ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
    GPIO_Init(COMM_CLK_Port, &GPIO_InitStructure);
    COMM_CLKSet();


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
    GPIO_InitStructure.GPIO_Pin  = Key_Remote_Pin ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ; 
    GPIO_Init(Key_Port, &GPIO_InitStructure);

    //-Local-
    GPIO_InitStructure.GPIO_Pin  = Key_Local_Pin ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ; 
    GPIO_Init(Key_Port, &GPIO_InitStructure);   
    //-Close- 
    GPIO_InitStructure.GPIO_Pin  = Key_Close_Pin ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ; 
    GPIO_Init(Key_Port, &GPIO_InitStructure);
    //-Open-
    GPIO_InitStructure.GPIO_Pin  = Key_Open_Pin ;
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
    GPIO_Init(Led_OpenLimit_Port, &GPIO_InitStructure);
    Led_OpenLimitClr();
    //-ShutLimit-
    GPIO_InitStructure.GPIO_Pin   = Led_ShutLimit_Pin ;
    GPIO_Init(Led_ShutLimit_Port, &GPIO_InitStructure); 
    Led_ShutLimitClr(); 


    /*-Lcd-*/
    //-RES-
    GPIO_InitStructure.GPIO_Pin   = Lcd_RES_Pin ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(Lcd_RES_Port, &GPIO_InitStructure); 
    Lcd_RESSet();
    //-CS-
    GPIO_InitStructure.GPIO_Pin   = Lcd_CS_Pin ;
    GPIO_Init(Lcd_CS_Port, &GPIO_InitStructure); 
    Lcd_CSSet();
    //-DC-
    GPIO_InitStructure.GPIO_Pin   = Lcd_DC_Pin ;
    GPIO_Init(Lcd_DC_Port, &GPIO_InitStructure); 
    Lcd_DCSet();
    //-RD-
    GPIO_InitStructure.GPIO_Pin   = Lcd_RD_Pin ;
    GPIO_Init(Lcd_RD_Port, &GPIO_InitStructure); 
    Lcd_RDSet();
    //-WR-
    GPIO_InitStructure.GPIO_Pin   = Lcd_WR_Pin ;
    GPIO_Init(Lcd_WR_Port, &GPIO_InitStructure); 
    Lcd_WRSet();
    //-D0-D7-
    GPIO_InitStructure.GPIO_Pin   = Lcd_D0_Pin ;
    GPIO_Init(Lcd_Data_Port, &GPIO_InitStructure);
 
    GPIO_InitStructure.GPIO_Pin   = Lcd_D1_Pin ;
    GPIO_Init(Lcd_Data_Port, &GPIO_InitStructure); 

    GPIO_InitStructure.GPIO_Pin   = Lcd_D2_Pin ;
    GPIO_Init(Lcd_Data_Port, &GPIO_InitStructure);
 
    GPIO_InitStructure.GPIO_Pin   = Lcd_D3_Pin ;
    GPIO_Init(Lcd_Data_Port, &GPIO_InitStructure);
 
    GPIO_InitStructure.GPIO_Pin   = Lcd_D4_Pin ;
    GPIO_Init(Lcd_Data_Port, &GPIO_InitStructure);
 
    GPIO_InitStructure.GPIO_Pin   = Lcd_D5_Pin ;
    GPIO_Init(Lcd_Data_Port, &GPIO_InitStructure); 

    GPIO_InitStructure.GPIO_Pin   = Lcd_D6_Pin ;
    GPIO_Init(Lcd_Data_Port, &GPIO_InitStructure); 

    GPIO_InitStructure.GPIO_Pin   = Lcd_D7_Pin ;
    GPIO_Init(Lcd_Data_Port, &GPIO_InitStructure);
    //Lcd_WriteDataBus(0x00);
 

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

