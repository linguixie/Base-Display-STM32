/*******************************************************************************
* Copyright (c) 2105,�����Զ����Ƽ����޹�˾
 *            All rights reserved.
* �������ƣ�
* �� �� �ţ�1.0
* �� �� �ˣ�л�ֹ�
* ����ʱ�䣺2015-3-16
* �� �� ��:
* �޸�ʱ��:
* �޸�˵��:
* ��    ����
********************************************************************************/
/*******************************************************************************
*                                    ͷ  ��  ��
********************************************************************************/
#include "Output.h"
#include "UI.h"
#include "SoftTimer.h"
#include "Valve.h"

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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void OutputInit(void)
{
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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

    /*-����ʵ����������޸�-*/
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

    if ((Device.Error.ErrorByte != 0) || (Valve.Error.ErrorByte != 0))
    {
        UI_LED_On(Led_Error);
    }
    else if (Device.Status.ESDStatus == ESD_Valid)
    { 
        if (ESDStatusChanged == 1)
        {
            UI_LED_FlashEver(Led_Error, 300, 300);
        }
    }
    else
    {
        UI_LED_Off(Led_Error);
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void Task_Output(void)
{
    AssistantOutput();
}

 
/*************************************END OF FILE*******************************/


