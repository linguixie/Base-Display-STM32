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

 
/*******************************************************************************
*                                  宏  定  义
********************************************************************************/
/*-Buzzer-*/
#define Buzzer_Pin                GPIO_Pin_11
#define Buzzer_Port               GPIOB 
#define Buzzer_Set()              (Buzzer_Port->BSRR = Buzzer_Pin)
#define Buzzer_Clr()              (Buzzer_Port->BRR = Buzzer_Pin)

/*-Communication-*/
#define COMM_CS_Pin               GPIO_Pin_5
#define COMM_CSSet()              (COMM_Port->BSRR = COMM_CS_Pin)
#define COMM_CSClr()              (COMM_Port->BRR = COMM_CS_Pin)

#define COMM_MISO_Pin             GPIO_Pin_4
//#define COMM_MISOGet()            (GPIO_ReadInputDataBit(COMM_Port, COMM_MISO_Pin))
#define COMM_MISOGet()            (COMM_Port->IDR & COMM_MISO_Pin)

#define COMM_MOSI_Pin             GPIO_Pin_7
#define COMM_MOSISet()            (COMM_Port->BSRR = COMM_MOSI_Pin)
#define COMM_MOSIClr()            (COMM_Port->BRR = COMM_MOSI_Pin)

#define COMM_CLK_Pin              GPIO_Pin_6
#define COMM_CLKSet()             (COMM_Port->BSRR = COMM_CLK_Pin)
#define COMM_CLKClr()             (COMM_Port->BRR = COMM_CLK_Pin)
//#define COMM_CLKGet()             (GPIO_ReadOutputDataBit(COMM_Port, COMM_CLK_Pin))
#define COMM_CLKGet()             (COMM_Port->ODR & COMM_CLK_Pin)

#define COMM_Port                 GPIOB


/*-Infrared-*/
#define InfraredRecv_Pin              GPIO_Pin_13 
#define InfraredRecv_EXTI_PortSource  EXTI_PortSourceGPIOB
#define InfraredRecv_EXTI_PinSource   EXTI_PinSource13
#define InfraredRecv_EXTI_Line        EXTI_Line13
#define InfraredRecv_EXTI_IRQ         EXTI4_15_IRQn
#define InfraredRecv_EXTI_IRQHandler  EXTI4_15_IRQHandler
#define InfraredRecv_EXTI_Priority    1

#define InfraredSend_Pin              GPIO_Pin_9
#define InfraredSend_PinSource        GPIO_PinSource9
#define InfraredSend_AF               GPIO_AF_0
#define Infrared_Port                 GPIOB
 
/*-Key-*/
#define Key_Remote_Pin            GPIO_Pin_8
#define Key_Local_Pin             GPIO_Pin_9
#define Key_Next_Pin              GPIO_Pin_10
#define Key_OK_Pin                GPIO_Pin_11
#define Key_ESC_Pin               GPIO_Pin_12
#define Key_Port                  GPIOA
//-!!!注意读取到的值为0和!0(而不是0和1)-
#define Key_RemoteRead()          (Key_Port->IDR & Key_Remote_Pin)
#define Key_LocalRead()           (Key_Port->IDR & Key_Local_Pin)
#define Key_NextRead()            (Key_Port->IDR & Key_Next_Pin)
#define Key_OKRead()              (Key_Port->IDR & Key_OK_Pin)
#define Key_ESCRead()             (Key_Port->IDR & Key_ESC_Pin)


/*-LED-*/
#define Led_OpenLimit_Pin         GPIO_Pin_1
#define Led_ShutLimit_Pin         GPIO_Pin_2
#define Led_Error_Pin             GPIO_Pin_10
#define Led_Port                  GPIOB
#define Led_OpenLimitSet()        (Led_Port->BSRR = Led_OpenLimit_Pin)
#define Led_OpenLimitClr()        (Led_Port->BRR = Led_OpenLimit_Pin)
#define Led_ShutLimitSet()        (Led_Port->BSRR = Led_ShutLimit_Pin)
#define Led_ShutLimitClr()        (Led_Port->BRR = Led_ShutLimit_Pin)
#define Led_ErrorSet()            (Led_Port->BSRR = Led_Error_Pin)
#define Led_ErrorClr()            (Led_Port->BRR = Led_Error_Pin)


/*-OLED-*/
//-控制信号(RES/CS/DC/RD/WR)-
#define OLed_RES_Pin              GPIO_Pin_0
#define OLed_RES_Port             GPIOF
#define OLed_RESSet()            (OLed_RES_Port->BSRR = OLed_RES_Pin)
#define OLed_RESClr()            (OLed_RES_Port->BRR = OLed_RES_Pin)

#define OLed_CS_Pin               GPIO_Pin_15
#define OLed_CS_Port              GPIOC
#define OLed_CSSet()             (OLed_CS_Port->BSRR = OLed_CS_Pin)
#define OLed_CSClr()             (OLed_CS_Port->BRR = OLed_CS_Pin)

#define OLed_DC_Pin               GPIO_Pin_14
#define OLed_DC_Port              GPIOC
#define OLed_DCSet()             (OLed_DC_Port->BSRR = OLed_DC_Pin)
#define OLed_DCClr()             (OLed_DC_Port->BRR = OLed_DC_Pin)

#define OLed_RD_Pin               GPIO_Pin_13
#define OLed_RD_Port              GPIOC
#define OLed_RDSet()             (OLed_RD_Port->BSRR = OLed_RD_Pin)
#define OLed_RDClr()             (OLed_RD_Port->BRR = OLed_RD_Pin)

#define OLed_WR_Pin               GPIO_Pin_1
#define OLed_WR_Port              GPIOF
#define OLed_WRSet()             (OLed_WR_Port->BSRR = OLed_WR_Pin)
#define OLed_WRClr()             (OLed_WR_Port->BRR = OLed_WR_Pin)

//-并行总线-
#define OLed_D0_Pin               GPIO_Pin_0
#define OLed_D1_Pin               GPIO_Pin_1
#define OLed_D2_Pin               GPIO_Pin_2
#define OLed_D3_Pin               GPIO_Pin_3
#define OLed_D4_Pin               GPIO_Pin_4
#define OLed_D5_Pin               GPIO_Pin_5
#define OLed_D6_Pin               GPIO_Pin_6
#define OLed_D7_Pin               GPIO_Pin_7
#define OLed_Data_Port            GPIOA
//#define OLed_WriteDataBus(Byte)   (OLed_Data_Port->ODR = Byte & 0xFF)
#define OLed_WriteDataBus(Byte)   (GPIO_Write(OLed_Data_Port, (GPIO_ReadOutputData(OLed_Data_Port) & 0xFF00) | (Byte & 0xFF)))


/*-外部字库芯片-*/
#define ROM_CLK_Pin               GPIO_Pin_6
#define ROM_CLK_Port              GPIOF
#define ROM_CLKSet()              (ROM_CLK_Port->BSRR = ROM_CLK_Pin)
#define ROM_CLKClr()              (ROM_CLK_Port->BRR = ROM_CLK_Pin)

#define ROM_CS_Pin                GPIO_Pin_7
#define ROM_CS_Port               GPIOF
#define ROM_CSSet()               (ROM_CS_Port->BSRR = ROM_CS_Pin)
#define ROM_CSClr()               (ROM_CS_Port->BRR = ROM_CS_Pin)

#define ROM_MOSI_Pin              GPIO_Pin_15
#define ROM_MOSI_Port             GPIOA
#define ROM_MOSISet()             (ROM_MOSI_Port->BSRR = ROM_MOSI_Pin)
#define ROM_MOSIClr()             (ROM_MOSI_Port->BRR = ROM_MOSI_Pin)

#define ROM_MISO_Pin              GPIO_Pin_3
#define ROM_MISO_Port             GPIOB
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

