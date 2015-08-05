/*******************************************************************************
* Copyright (c) 2015,�����Զ����Ƽ����޹�˾
 *            All rights reserved.
* �������ƣ�
* �� �� �ţ�1.0
* �� �� �ˣ�л�ֹ�
* ����ʱ�䣺2015-6-4
* �� �� ��:
* �޸�ʱ��:
* �޸�˵��:
* ��    ����������������IO��
********************************************************************************/
/*******************************************************************************
*                                    ͷ  ��  ��
********************************************************************************/
#include "Config.h"
#include "GPIO.h"

/*******************************************************************************
*                               �ļ��ڲ�ʹ�ú궨��
********************************************************************************/
 

/*******************************************************************************
*                                 ȫ�ֺ���(����)����
********************************************************************************/
 

/*******************************************************************************
*                                 ��̬����(����)����
********************************************************************************/


/*******************************************************************************
* ��������:    GpioInit
* ��������:    �������漰����IO�ڽ��г�ʼ��
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
    //-����-
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

