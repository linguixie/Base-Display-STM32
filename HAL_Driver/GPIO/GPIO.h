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
 
 
#ifndef _GPIO_H_
#define _GPIO_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*******************************************************************************
*                                  头  文  件
********************************************************************************/
#include "stm32f0xx_conf.h"
 
/*******************************************************************************
*                                  宏  定  义
********************************************************************************/
/*-Buzzer-*/
#define Buzzer_Pin                GPIO_Pin_7
#define Buzzer_Port               GPIOF 
#define Buzzer_Set()              (Buzzer_Port->BSRR = Buzzer_Pin)
#define Buzzer_Clr()              (Buzzer_Port->BRR = Buzzer_Pin)

 /*-Communication-*/
 #define COMM_CS_Pin               GPIO_Pin_4
 #define COMM_CS_Port              GPIOB
 #define COMM_CSSet()              (COMM_CS_Port->BSRR = COMM_CS_Pin)
 #define COMM_CSClr()              (COMM_CS_Port->BRR = COMM_CS_Pin)
 
 #define COMM_MISO_Pin             GPIO_Pin_5
 #define COMM_MISO_Port            GPIOB
 //#define COMM_MISOGet()            (GPIO_ReadInputDataBit(COMM_Port, COMM_MISO_Pin))
 #define COMM_MISOGet()            (COMM_MISO_Port->IDR & COMM_MISO_Pin)
 
 #define COMM_MOSI_Pin             GPIO_Pin_3
 #define COMM_MOSI_Port            GPIOB
 #define COMM_MOSISet()            (COMM_MOSI_Port->BSRR = COMM_MOSI_Pin)
 #define COMM_MOSIClr()            (COMM_MOSI_Port->BRR = COMM_MOSI_Pin)
 
 #define COMM_CLK_Pin              GPIO_Pin_13
 #define COMM_CLK_Port             GPIOC
 #define COMM_CLKSet()             (COMM_CLK_Port->BSRR = COMM_CLK_Pin)
 #define COMM_CLKClr()             (COMM_CLK_Port->BRR = COMM_CLK_Pin)
 //#define COMM_CLKGet()             (GPIO_ReadOutputDataBit(COMM_Port, COMM_CLK_Pin))
#define COMM_CLKGet()             (COMM_CLK_Port->ODR & COMM_CLK_Pin)


/*-Infrared-*/
#define InfraredRecv_Pin              GPIO_Pin_13 
#define InfraredRecv_EXTI_PortSource  EXTI_PortSourceGPIOB
#define InfraredRecv_EXTI_PinSource   EXTI_PinSource13
#define InfraredRecv_EXTI_Line        EXTI_Line13
#define InfraredRecv_EXTI_IRQ         EXTI4_15_IRQn
#define InfraredRecv_EXTI_IRQHandler  EXTI4_15_IRQHandler
#define InfraredRecv_EXTI_Priority    1

//-悬空, 未用到-
#define InfraredSend_Pin              GPIO_Pin_9
#define InfraredSend_PinSource        GPIO_PinSource9
#define InfraredSend_AF               GPIO_AF_0
#define Infrared_Port                 GPIOB
 

/*-Key-*/
#define Key_Remote_Pin            GPIO_Pin_9
#define Key_Local_Pin             GPIO_Pin_12
#define Key_Close_Pin             GPIO_Pin_11
#define Key_Open_Pin              GPIO_Pin_10
#define Key_Port                  GPIOA
//-!!!注意读取到的值为0和!0(而不是0和1)-
#define Key_RemoteRead()          (Key_Port->IDR & Key_Remote_Pin)
#define Key_LocalRead()           (Key_Port->IDR & Key_Local_Pin)
#define Key_CloseRead()           (Key_Port->IDR & Key_Close_Pin)
#define Key_OpenRead()            (Key_Port->IDR & Key_Open_Pin)


/*-LED-*/
 #define Led_OpenLimit_Pin         GPIO_Pin_8
 #define Led_OpenLimit_Port        GPIOA
 
 #define Led_ShutLimit_Pin         GPIO_Pin_15
 #define Led_ShutLimit_Port        GPIOB
#define Led_OpenLimitSet()        (Led_OpenLimit_Port->BSRR = Led_OpenLimit_Pin)
#define Led_OpenLimitClr()        (Led_OpenLimit_Port->BRR = Led_OpenLimit_Pin)
#define Led_ShutLimitSet()        (Led_ShutLimit_Port->BSRR = Led_ShutLimit_Pin)
#define Led_ShutLimitClr()        (Led_ShutLimit_Port->BRR = Led_ShutLimit_Pin)


/*-Lcd-*/
//-控制信号(RES/CS/DC/RD/WR)-
#define Lcd_RES_Pin              GPIO_Pin_10
#define Lcd_RES_Port             GPIOB
#define Lcd_RESSet()            (Lcd_RES_Port->BSRR = Lcd_RES_Pin)
#define Lcd_RESClr()            (Lcd_RES_Port->BRR = Lcd_RES_Pin)

#define Lcd_CS_Pin               GPIO_Pin_11
#define Lcd_CS_Port              GPIOB
#define Lcd_CSSet()             (Lcd_CS_Port->BSRR = Lcd_CS_Pin)
#define Lcd_CSClr()             (Lcd_CS_Port->BRR = Lcd_CS_Pin)

#define Lcd_DC_Pin               GPIO_Pin_2
#define Lcd_DC_Port              GPIOB
#define Lcd_DCSet()             (Lcd_DC_Port->BSRR = Lcd_DC_Pin)
#define Lcd_DCClr()             (Lcd_DC_Port->BRR = Lcd_DC_Pin)

#define Lcd_RD_Pin               GPIO_Pin_0
#define Lcd_RD_Port              GPIOB
#define Lcd_RDSet()             (Lcd_RD_Port->BSRR = Lcd_RD_Pin)
#define Lcd_RDClr()             (Lcd_RD_Port->BRR = Lcd_RD_Pin)

#define Lcd_WR_Pin               GPIO_Pin_1
#define Lcd_WR_Port              GPIOB
#define Lcd_WRSet()             (Lcd_WR_Port->BSRR = Lcd_WR_Pin)
#define Lcd_WRClr()             (Lcd_WR_Port->BRR = Lcd_WR_Pin)

//-并行总线-
#define Lcd_D0_Pin               GPIO_Pin_0
#define Lcd_D1_Pin               GPIO_Pin_1
#define Lcd_D2_Pin               GPIO_Pin_2
#define Lcd_D3_Pin               GPIO_Pin_3
#define Lcd_D4_Pin               GPIO_Pin_4
#define Lcd_D5_Pin               GPIO_Pin_5
#define Lcd_D6_Pin               GPIO_Pin_6
#define Lcd_D7_Pin               GPIO_Pin_7
#define Lcd_Data_Port            GPIOA
#define Lcd_WriteDataBus(Byte)   (Lcd_Data_Port->ODR = (Byte & 0xFF) | (Lcd_Data_Port->ODR & 0xFF00))
//#define Lcd_WriteDataBus(Byte)   (GPIO_Write(Lcd_Data_Port, (GPIO_ReadOutputData(Lcd_Data_Port) & 0xFF00) | (Byte & 0xFF)))
#define Lcd_ReadDataBus()        ((Lcd_Data_Port->IDR) & 0xFF)


/*-外部字库芯片-*/
#define ROM_CLK_Pin               GPIO_Pin_15
#define ROM_CLK_Port              GPIOC
#define ROM_CLKSet()              (ROM_CLK_Port->BSRR = ROM_CLK_Pin)
#define ROM_CLKClr()              (ROM_CLK_Port->BRR = ROM_CLK_Pin)

#define ROM_CS_Pin                GPIO_Pin_14
#define ROM_CS_Port               GPIOC
#define ROM_CSSet()               (ROM_CS_Port->BSRR = ROM_CS_Pin)
#define ROM_CSClr()               (ROM_CS_Port->BRR = ROM_CS_Pin)

#define ROM_MOSI_Pin              GPIO_Pin_0
#define ROM_MOSI_Port             GPIOF
#define ROM_MOSISet()             (ROM_MOSI_Port->BSRR = ROM_MOSI_Pin)
#define ROM_MOSIClr()             (ROM_MOSI_Port->BRR = ROM_MOSI_Pin)

#define ROM_MISO_Pin              GPIO_Pin_1
#define ROM_MISO_Port             GPIOF
//-!!!注意读取到的值为0和!0(而不是0和1)-
//#define ROM_MISOGet()             (ROM_MISO_Port->IDR & ROM_MISO_Pin)
#define ROM_MISOGet()             (GPIO_ReadInputDataBit(ROM_MISO_Port, ROM_MISO_Pin))


/*******************************************************************************
*                                  全局函数(变量)声明
********************************************************************************/
void GpioInit(void); 

#ifdef __cplusplus
}
#endif
 
#endif /* _GPIO_H_ */
 
 
/*************************************END OF FILE*******************************/

