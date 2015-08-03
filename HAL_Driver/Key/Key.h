/*******************************************************************************
* Copyright (c) 2105,�����Զ����Ƽ����޹�˾
 *            All rights reserved.
* �������ƣ�
* �� �� �ţ�1.0
* �� �� �ˣ�л�ֹ�
* ����ʱ�䣺2015-3-12
* �� �� ��:
* �޸�ʱ��:
* �޸�˵��:
* ��    ����
********************************************************************************/
 
 
#ifndef _KEY_H_
#define _KEY_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*******************************************************************************
*                                  ͷ  ��  ��
********************************************************************************/
 
/*******************************************************************************
*                                  ��  ��  ��
********************************************************************************/ 
//-�Ȱ��������ϵİ���Ҳ������ң�����İ���-
typedef enum {Key_Remote, Key_Local, Key_Open, Key_Shut, KeyOnBoard_Num, 
              Key_Inc = KeyOnBoard_Num, Key_Dec, Key_Set, Key_Back, Key_Up, Key_Down, Key_Num}Key_Enum;

//-��������-
#define KEY_MAX_NUM                     KeyOnBoard_Num

//-����״̬-
#define KEY_UNPRESSED                   0         //-����δ����-
#define KEY_PRESSED                     1         //-��������-

#define LONG_KEY                        5         //-����-
#define SHORT_KEY                       2         //-�̼�-
#define KEY_NONE                        0 

//-����ʱ���밴��״̬-
#define LONG_KEY_TIME                   300         //-������ʱ�� ��λ: KeyActivationTimeSet�����õ�����-
#define SHORT_KEY_TIME                  10          //-�̰���ʱ�� ��λ: ͬ��-
#define Key_Pressed_Max_Time            25000       //-�������µ����ʱ�� ��λ:ͬ��-


/*******************************************************************************
*                                  ȫ�ֺ���(����)����
********************************************************************************/

extern unsigned char KeyPressed[Key_Num];   
extern unsigned char PressedType[Key_Num];
unsigned char KeyStateRead(int KeyIndex);
void KeyMsgGet(unsigned char *pKey, unsigned char *Long_Short);
void KeyActivationTimeSet(void);

void KeyInit(void);
void KeyActivationTimeSet(void);

void Task_Key(void);
#ifdef __cplusplus
}
#endif
 
#endif /* _KEY_H_ */
 
 
/*************************************END OF FILE*******************************/


