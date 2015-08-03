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
* ��    ������ֲ��ע��:
*           1��KeyActivationTimeSet�����ж��б�����,��������Խ��Խ��.
*           2�������ʵ������޸ı�ʾ���̼��ĺ�.
*           3��Task_Key�豻���ϵ���,��ʵʱ��ⰴ��״̬.
*           4��Ҫ�ر����ִ���һֱ��ѹ�����Ǵ���1�λ��Ǳ�ʾһֱ��Ч(��ֲ�ص��ע����)
*           5��ȱ��:�������һֱ��ѹ��ʾ1����Ч.��������������ļ��ʱ�����,�򰴼���
*                   ѹ��,����ֱ�����������ʱ��Ż�����������Ч.
********************************************************************************/
/*******************************************************************************
*                                    ͷ  ��  ��
********************************************************************************/
#include "stm32f0xx_conf.h"
#include "GPIO.h"
#include "Key.h"
#include "Infrared.h"
#include "UI.h"

#include "Valve.h"
#include "Display.h"
/*******************************************************************************
*                               �ļ��ڲ�ʹ�ú궨��
********************************************************************************/
 

/*******************************************************************************
*                                 ȫ�ֺ���(����)����
********************************************************************************/

//-�������¼�ʱ�� ���ڼ�¼�������µ�ʱ�� ����ʱ���жϰ����ǳ��������Ƕ̰���
// ����û�а���ʱ�ü�ʱ��ʼ��Ϊ0-
static volatile unsigned int  KeyActivationTime[Key_Num] = {0};

//-������δ��Ӧǰ�ذ��²��Ҹ���Чֻ����һ��-
static volatile unsigned char  keyScanEn[Key_Num]         = {0};

//--
unsigned char KeyPressed[Key_Num] = {KEY_UNPRESSED};   
unsigned char PressedType[Key_Num] = {KEY_NONE};

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
void KeyInit(void)
{
    int i = 0;

    for(i = 0; i < Key_Num; i++)
    {
       keyScanEn[i] = 1;
    }
}


/*******************************************************************************
* ��������: IRKeyMsgGet
* ��������: ��ȡ����ң����������Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
unsigned char IRKeyStateRead(int KeyIndex)
{
    unsigned int KeyPressed = 0;
    unsigned int Code = 0;

    Code = GetIRKeyCode();

    switch(KeyIndex)
    {
    case Key_Inc:
        KeyPressed = (Code == Code_Open);
        break;
    case Key_Dec:
        KeyPressed = (Code == Code_Shut);
        break;
    case Key_Set:
        KeyPressed = (Code == Code_Set);
        break;
    case Key_Back:
        KeyPressed = (Code == Code_Back);
        break;
    case Key_Up:
        KeyPressed = (Code == Code_Up);
        break;
    case Key_Down:
        KeyPressed = (Code == Code_Down);
        break;
    default:
        break;
    }

    if (KeyPressed)
    {
        return KEY_PRESSED;
    }
    else
    {
        return KEY_UNPRESSED;
    }
}


/*******************************************************************************
* ��������: KeyMsgGet
* ��������: ��ȡ������Ϣ
* �������: KeyIndex,����������.�μ�ö�ٱ���Key_Enum
* �������: ��
* �� �� ֵ: ��
* ��    ��: �͵�ƽ��ʾ����
*******************************************************************************/
unsigned char KeyStateRead(int KeyIndex)
{
    unsigned int KeyPressed = {0};

    switch(KeyIndex)
    {
    case Key_Remote:
        KeyPressed = Key_RemoteRead();
        break;
    case Key_Local:
        KeyPressed = Key_LocalRead();
        break;
    case Key_Open:
        KeyPressed = Key_OpenRead();
        break;    
    case Key_Shut:
        KeyPressed = Key_CloseRead();
        break; 
    default:
        break;
    }
 
    if (KeyIndex >= KeyOnBoard_Num)
    {
        KeyPressed = !IRKeyStateRead(KeyIndex);
    }

    if (!KeyPressed)
    {
        return KEY_PRESSED;
    }
    else
    {
        return KEY_UNPRESSED;
    }

}


/*******************************************************************************
* ��������: IRKeyMsgGet
* ��������: ��ȡ����ң����������Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void IRKeyMsgGet(unsigned char *pKey, unsigned char *Long_Short)
{
    int i = 0; 
    int j = 0;

    for (i = KeyOnBoard_Num; i < Key_Num; i++)
    {
        if ((1 == keyScanEn[i]) && (KeyActivationTime[i] >= LONG_KEY_TIME))
        {
            if (i == Key_Set)
            {
                j++;
            }

            KeyActivationTime[i] = 0;
            keyScanEn[i]         = 0;
          
            pKey[i] = KEY_PRESSED;
            Long_Short[i] = LONG_KEY;
    
            UI_Buzz(500, 300, 1);
        }
        else if ((1 == keyScanEn[i]) && (KeyStateRead(i) == KEY_UNPRESSED) 
                 && (KeyActivationTime[i] >= SHORT_KEY_TIME))
        {
            if (i == Key_Set)
            {
                j++;
            }
            KeyActivationTime[i] = 0;
            keyScanEn[i]         = 0;
          
            pKey[i] = KEY_PRESSED;
            Long_Short[i] = SHORT_KEY;

            UI_Buzz(300, 300, 1);

        }
        //-����-
        else if ((KeyStateRead(i) == KEY_UNPRESSED) && (KeyActivationTime[i] < SHORT_KEY_TIME))
        {
            if (i == Key_Set)
            {
                j++;
            }
            KeyActivationTime[i] = 0;
          
            pKey[i] = KEY_UNPRESSED;
            Long_Short[i] = KEY_NONE;
        }
        else
        {
            if ((i == Key_Inc) || (i == Key_Dec))
            {

                /*-ֻ�����ֳ�ģʽ�º�����λ�����������²�����һֱ��ѹ��ʾ����һֱ��Ч-
                  -��������,һֱ��ѹֻ����һ��-*/
                if ((Device.CommMode.CommModeBits.Local == 1) || (PageFunctionIndex == Page_AdjustZeroAction_ID) 
                    || (PageFunctionIndex == Page_AdjustFullAction_ID))
                {
                }
                else
                { 
                    pKey[i] = KEY_UNPRESSED;
                    Long_Short[i] = KEY_NONE;
                }
            }
            else
            { 
                pKey[i] = KEY_UNPRESSED;
                Long_Short[i] = KEY_NONE;
            }
        }
    }
}


/*******************************************************************************
* ��������: KeyMsgGet
* ��������: ��ȡ������Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void KeyMsgGet(unsigned char *pKey, unsigned char *Long_Short)
{
    int i = 0; 

    for (i = 0; i < KeyOnBoard_Num; i++)
    {
        if ((1 == keyScanEn[i]) && (KeyActivationTime[i] >= LONG_KEY_TIME))
        {
            KeyActivationTime[i] = 0;
            keyScanEn[i]         = 0;
          
            pKey[i] = KEY_PRESSED;
            Long_Short[i] = LONG_KEY;
    
            UI_Buzz(500, 300, 1);
        }
        else if ((1 == keyScanEn[i]) && (KeyStateRead(i) == KEY_UNPRESSED) 
                 && (KeyActivationTime[i] >= SHORT_KEY_TIME))
        {
            KeyActivationTime[i] = 0;
            keyScanEn[i]         = 0;
          
            pKey[i] = KEY_PRESSED;
            Long_Short[i] = SHORT_KEY;

            UI_Buzz(300, 300, 1);

        }
        //-����-
        else if ((KeyStateRead(i) == KEY_UNPRESSED) && (KeyActivationTime[i] < SHORT_KEY_TIME))
        {
            KeyActivationTime[i] = 0;
          
            pKey[i] = KEY_UNPRESSED;
            Long_Short[i] = KEY_NONE;
        }
        else
        {
            if ((i == Key_Open) || (i == Key_Shut))
            {
                /*-ֻ�����ֳ�ģʽ�º�����λ�����������²�����һֱ��ѹ��ʾ����һֱ��Ч-
                  -��������,һֱ��ѹֻ����һ��-*/
                if ((Device.CommMode.CommModeBits.Local == 1) || (PageFunctionIndex == Page_AdjustZeroAction_ID) 
                    || (PageFunctionIndex == Page_AdjustFullAction_ID))
                {
                }
                else
                { 
                    pKey[i] = KEY_UNPRESSED;
                    Long_Short[i] = KEY_NONE;
                }
            }
        }
    }

    IRKeyMsgGet(pKey, Long_Short);
}


/*******************************************************************************
* ��������: KeyActivationTimeSet
* ��������: ��������ʱ������
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void KeyActivationTimeSet(void)
{
    int i = 0;

    for (i = 0; i < Key_Num; i++)
    {
        //-�������¼�ʱ������-
        if ((KeyStateRead(i) == KEY_PRESSED) && (1 == keyScanEn[i]))
        {
            if (KeyActivationTime[i] < Key_Pressed_Max_Time)
            {
                KeyActivationTime[i]++;
            }
        }
        if (KeyStateRead(i) == KEY_UNPRESSED)
        {
            keyScanEn[i] = 1;
        }
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void Task_Key(void)
{
    //-�������ҽ����������KeyPressed�����PressedType������-
    KeyMsgGet(&KeyPressed[0], &PressedType[0]);
}

 
/*************************************END OF FILE*******************************/


