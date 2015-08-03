/*******************************************************************************
* Copyright (c) 2105,�����Զ����Ƽ����޹�˾
 *            All rights reserved.
* �������ƣ�
* �� �� �ţ�1.0
* �� �� �ˣ�л�ֹ�
* ����ʱ�䣺2015-6-13
* �� �� ��:
* �޸�ʱ��:
* �޸�˵��:
* ��    ����
********************************************************************************/
/*******************************************************************************
*                                    ͷ  ��  ��
********************************************************************************/
#include "Type.h"
#include "Main.h"
#include "Lcd.h"
#include "Key.h"
#include "SoftTimer.h"
#include "Valve.h"
#include "EEPROM.h"
#include "UI.h"
#include "Communication.h"
#include "Display.h"
#include "Lcd.h"
#include "Communication.h"

/*******************************************************************************
*                               �ļ��ڲ�ʹ�ú궨��
********************************************************************************/
#define PasswordByte_0   0x01
#define PasswordByte_1   0x00
#define PasswordByte_2   0x00
#define PasswordByte_3   0x00

#define MaxLockTime      0xFFFF
#define MinLockTime      0x00

//-Ϊ����д����-
#define Space            GBK_Space

//-���ⰴ��,�԰����ķ�װ,V��Virtual-
typedef enum
{
   VKey_Inc,
   VKey_Dec,
   VKey_Set,
   VKey_Back,
   VKey_Up, 
   VKey_Down,
   VKey_Num  
}VKey_TypeDef;

/*******************************************************************************
*                                 ȫ�ֺ���(����)����
********************************************************************************/
unsigned char PageFunctionIndex = Page_PowerOn_ID;
  
unsigned char NeedResetMenuPara = 0;

unsigned char PasswordErrorCount = 0;
unsigned char Password[4] = {0};

//-��������ǰ������ShowNumber��,���Ǻ������ֻᵼ��ջ�ռ䲻��,
// ֻ�ܽ�ԭ���ֲ������ĳ�ȫ�ֱ���.-
unsigned int LongCode[16];

unsigned char IsNeedChangePage = 0;
unsigned char F_StartRecv = 1;

//-�ް�����ʱ-
signed long NoKeyTimer = 0;
//-�궨�ɹ���Ϣ��ʾ��ʱ-
signed long AdjustInfoTimer = 0;
/*******************************************************************************
*                                 ��̬����(����)����
********************************************************************************/
const unsigned short int SeriousErrorArray[8][2][16] = 
{
    //-��λ�쳣(Valve Pos Err!)-
    {{0xB7A7, 0xCEBB, 0xD2EC, 0xB3A3},  {0x56, 0x61, 0x6C, 0x76, 0x65, 0x20, 0x50, 0x6F, 0x73, 0x20, 0x45, 0x72, 0x72, 0x21}},
    //-Reserved-
    {0},
    //-Reserved-
    {0},
    //-Reserved-
    {0},
    //-Reserved-
    {0},
    //-Reserved-
    {0},
    //-Reserved-
    {0},
    //-��Դȱ��(Phase Lack Err!)-
    {{0xB5E7, 0xD4B4, 0xC8B1, 0xCFE0}, {0x50, 0x68, 0x61, 0x73, 0x65, 0x20, 0x4C, 0x61, 0x63, 0x6B, 0x20, 0x45, 0x72, 0x72, 0x21}}
    
};
const unsigned short int SlightErrorArray[8][2][16] = 
{
    //-�����ع���(Open Torque Err!)-
    {{0xB9D8, 0xCFF2, 0xB9FD, 0xBED8}, {0x4F, 0x70, 0x65, 0x6E, 0x20, 0x54, 0x6F, 0x72, 0x71, 0x75, 0x65, 0x20, 0x45, 0x72, 0x72, 0x21}},
    //-�����ع���(Shut Torque Err!)-
    {{0xBFAA, 0xCFF2, 0xB9FD, 0xBED8}, {0x53, 0x68, 0x75, 0x74, 0x20, 0x54, 0x6F, 0x72, 0x71, 0x75, 0x65, 0x20, 0x45, 0x72, 0x72, 0x21}}, 
    //-����ʱ(Shut Timeout!)-
    {{0xB9D8, 0xCFF2, 0xB3AC, 0xCAB1}, {0x53, 0x68, 0x75, 0x74, 0x20, 0x54, 0x69, 0x6D, 0x65, 0x6F, 0x75, 0x74, 0x21}},
    //-����ʱ(Open Timeout!)-
    {{0xBFAA, 0xCFF2, 0xB3AC, 0xCAB1}, {0x4F, 0x70, 0x65, 0x6E, 0x20, 0x54, 0x69, 0x6D, 0x65, 0x6F, 0x75, 0x74, 0x21}},
    //-�������(Shut OverCurrent)-
    {{0xB9D8, 0xCFF2, 0xB9FD, 0xC1F7}, {0x53, 0x68, 0x75, 0x74, 0x20, 0x4F, 0x76, 0x65, 0x72, 0x43, 0x75, 0x72, 0x72, 0x65, 0x6E, 0x74}},
    //-�������(Open OverCurrent)-
    {{0xBFAA, 0xCFF2, 0xB9FD, 0xC1F7}, {0x4F, 0x70, 0x65, 0x6E, 0x20, 0x4F, 0x76, 0x65, 0x72, 0x43, 0x75, 0x72, 0x72, 0x65, 0x6E, 0x74}},
    //-�������(Shut DIR Error!)-
    {{0xB9D8, 0xCFF2, 0xB4ED, 0xCEF3}, {0x53, 0x68, 0x75, 0x74, 0x20, 0x44, 0x49, 0x52, 0x20, 0x45, 0x72, 0x72, 0x6F, 0x72, 0x21}},
    //-�������(Open DIR Error!)-
    {{0xBFAA, 0xCFF2, 0xB4ED, 0xCEF3}, {0x4F, 0x70, 0x65, 0x6E, 0x20, 0x44, 0x49, 0x52, 0x20, 0x45, 0x72, 0x72, 0x6F, 0x72, 0x21}},
};



/*******************************************************************************
*                                 �˵�(ҳ��)��˵��
*1�����л�������ɲ˵�,������ͨ�˵�������˵�.
*2����ͨ�˵�ʾ��:���˵�.
*3������˵�ʾ��:�ϵ�ҳ��.�������������̶��Ĳ˵�.
*4���˵�������Ϊ��ʾ�Ĳ˵�������,��GB2312���ֱ�ʾ.
*5��ע������˵�������:������Դ�ڲ˵�������,Ҳ������ʵ�ʵ�ҳ���и�ֵ��
*6���˵�����3��ģ��:���˵�:    ������ͨ�˵�ʱ�ο�.
*                   ����˵�:  ������"ȷ��"��"����"�Ĳ˵�ʱ�ο���
*                   �ϵ�ҳ��:  �����޽��㡢ֻ��1���˵���Ĳ˵�ʱ�ο�
*7���˵�������ֻ������Ч�ַ�����(����,������)�Ŀո��ر���.
*8���ո�Ĵ���,����Ч�ַ�ǰ��Ŀո���봦��,��������账��.����" �� �� "�е�
    �й���3���ո�,���е�1���ո�͵�2���ո�������0xA3A0
*9��{0}��ʾ�հ��л���ռλ
*10�����˵��ĸ���������������˵���˵,�Ӳ˵�������ڵ����˵�����˵.
*11���˵�ģ��
//------------------ ģ��-------------------------------
const MenuStructure Menu_Template[] = 
{
  //-������       ���˵�                 �Ӳ˵�                   �˵�������-
  //--
  {0,             InvalidMenuID,         InvalidMenuID,           {0},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {1,             InvalidMenuID,         InvalidMenuID,           {0},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {2,             InvalidMenuID,         InvalidMenuID,           {0},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {3,             InvalidMenuID,         InvalidMenuID,           {0},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_Template = {2, 2, Disable, Enable, 8, 4, Multiplex_None, Dummy_Reset};
********************************************************************************/

/*-------------------�ϵ�ҳ��(�������̶��Ĳ˵�)-------------------------------*/
const MenuStructure Menu_PowerOn[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-��ӭʹ��-
  {0,             InvalidMenuID,           InvalidMenuID,         {0xA3A0, 0xA3A0, 0xA3A0, 0xA3A0, 0xBBB6, 0xD3AD, 0xCAB9, 0xD3C3,  0xA3A0, 0xA3A0, 0xA3A0, 0xA3A0},
  Dummy_Special,  StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-�綯����ִ�л���-
  {1,             InvalidMenuID,           InvalidMenuID,         {0xB5E7, 0xB6AF, 0xB7A7, 0xC3C5, 0xD6B4, 0xD0D0, 0xBBFA, 0xB9B9},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-�汾V1.0-
  {2,            InvalidMenuID,           InvalidMenuID,          {0xA3A0, 0xA3A0, 0xB0E6, 0xB1BE, 0xA3BA, 0xA3A0, 0xA3D6, 0xA3B1, 0xA3AE, 0xA3B0, 0xA3A0, 0xA3A0, 0xA3A0, 0xA3A0},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {3,            InvalidMenuID,           InvalidMenuID,          {0},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_PowerOn = {0, Invalid, Disable, Disable, Invalid, 4, Multiplex_None, Dummy_Reset};    


/*------------------ ����ҳ��(�������̶��Ĳ˵�)-------------------------------*/
const MenuStructure Menu_Normal[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-ռλ,������NormalPage_Special�����-
  {0,             InvalidMenuID,    Page_MainMenu_ID,             {0},
  NormalPage_Special0, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-ռλ,������NormalPage_Special�����-
  {1,             InvalidMenuID,           InvalidMenuID,         {0},
  NormalPage_Special1, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-ռλ,������NormalPage_Special�����-
  {2,             InvalidMenuID,           InvalidMenuID,         {0},
  NormalPage_Special2, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-ռλ,������NormalPage_Special�����-
  {3,             InvalidMenuID,           InvalidMenuID,           {0},
  NormalPage_Special3, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_Normal = {0, Invalid, Disable, Disable, Invalid, 4, Multiplex_None, Dummy_Reset};    


/*--------------------���˵�--------------------------------------------------*/
const MenuStructure Menu_Main[] =         
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-�궨ȫ��-
  {0,             Page_Normal_ID,          Page_AdjustZero_ID,    {0xB1EA, 0xB6A8, 0xC8AB, 0xB9D8},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-�궨ȫ��-
  {1,             InvalidMenuID,           Page_AdjustFull_ID,    {0xB1EA, 0xB6A8, 0xC8AB, 0xBFAA},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
  
  //-�ֳ����Ʒ�ʽ-
  {2,             InvalidMenuID,           Page_LocalMode_ID,     {0xCFD6, 0xB3A1, 0xBFD8, 0xD6C6, 0xB7BD, 0xCABD},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
  
  //-Զ�̿���������-
  {3,             InvalidMenuID,           Page_RemoteIOMode_ID,  {0xD4B6, 0xB3CC, 0xBFAA, 0xB9D8, 0xD0CD, 0xC9E8, 0xD6C3},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-Զ�̵���������-
  {4,             InvalidMenuID,           Page_RemoteANMode_ID,  {0xD4B6, 0xB3CC, 0xB5F7, 0xBDDA, 0xD0CD, 0xC9E8, 0xD6C3},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
  
  //-�ڲ�����-
  {5,             InvalidMenuID,           Page_Password_ID,      {0xC4DA, 0xB2BF, 0xB2CE, 0xCAFD},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
  
  //-�˳�����-
  {6,             Page_Normal_ID,          InvalidMenuID,           {0xCDCB, 0xB3F6, 0xC9E8, 0xD6C3},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction}
};   

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_Main = {0, Invalid, Enable, Disable, Invalid, 7, Multiplex_None, StandardMenu_Reset0};    


/*---------------------�궨���-----------------------------------------------*/
const MenuStructure Menu_AdjustZero[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-�ѵ�ǰλ��#0-
  {0,             Page_MainMenu_ID,        InvalidMenuID,         {0xB0D1, 0xB5B1, 0xC7B0, 0xCEBB, 0xD6C3, 0xA3A3},
   AdjustZeroFull_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�궨Ϊ0%-
  {1,             InvalidMenuID,           InvalidMenuID,         {0xA3A0, 0xA3A0, 0xA3A0, 0xA3A0, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B0, 0xA3A5},
   AdjustZeroFull_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //--
  {2,             InvalidMenuID,           InvalidMenuID,         {0},
   AdjustZeroFull_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-ȷ��  ����-
  {3,             Page_MainMenu_ID,        InvalidMenuID,         {0xA3A0, 0xA3A0, 0xA3A0, 0xC8B7, 0xC8CF, 0xA3A0, 0xA3A0, 0xB7B5, 0xBBD8},
   AdjustZeroFull_Special, AdjustZeroFull_SetKey, AdjustZeroFull_UpKey, AdjustZeroFull_DownKey, DummyFunction, DummyFunction},
};   
   
//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_AdjustZero = {3, 3, Disable, Enable, 4, 4, Multiplex_None, StandardMenu_Reset1};    


/*---------------------�궨0%��ʾ��Ϣ---------------------------------------*/
const MenuStructure Menu_AdjustZeroInfo[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //--
  {0,             Page_MainMenu_ID,        InvalidMenuID,         {0},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�Ѱѵ�ǰλ��-
  {1,             InvalidMenuID,           InvalidMenuID,         {0xA3A0, 0xA3A0, 0xD2D1, 0xB0D1, 0xB5B1, 0xC7B0, 0xCEBB, 0xD6C3},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�ɹ��궨Ϊ0%-
  {2,             InvalidMenuID,           InvalidMenuID,         {0xA3A0, 0xA3A0, 0xB3C9, 0xB9A6, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B0, 0xA3A5},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //--
  {3,             Page_MainMenu_ID,        InvalidMenuID,         {0},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};   

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_AdjustZeroInfo = {0, Invalid, Disable, Disable, Invalid, 4, Multiplex_None, Dummy_Reset};


/*---------------------�궨����-----------------------------------------------*/
const MenuStructure Menu_AdjustFull[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-�ѵ�ǰλ��#0-
  {0,             Page_MainMenu_ID,       InvalidMenuID,         {0xB0D1, 0xB5B1, 0xC7B0, 0xCEBB, 0xD6C3, 0xA3A3},
   AdjustZeroFull_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�궨Ϊ100%-
  {1,             InvalidMenuID,           InvalidMenuID,         {0xA3A0, 0xA3A0, 0xA3A0, 0xA3A0, 0xB1EA, 0xB6A8, 0xCEAA,0xA3B1, 0xA3B0, 0xA3B0,0xA3A5},
   AdjustZeroFull_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //--
  {2,             InvalidMenuID,           InvalidMenuID,         {0},
   AdjustZeroFull_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-ȷ��  ����-
  {3,             Page_MainMenu_ID,        InvalidMenuID,         {0xA3A0, 0xA3A0, 0xA3A0, 0xC8B7, 0xC8CF, 0xA3A0, 0xA3A0, 0xB7B5, 0xBBD8},
   AdjustZeroFull_Special, AdjustZeroFull_SetKey, AdjustZeroFull_UpKey, AdjustZeroFull_DownKey, DummyFunction, DummyFunction},
};   

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_AdjustFull = {3, 3, Disable, Enable, 4, 4, Multiplex_None, StandardMenu_Reset1};  


/*---------------------�궨100%��ʾ��Ϣ---------------------------------------*/
const MenuStructure Menu_AdjustFullInfo[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //--
  {0,             Page_MainMenu_ID,        InvalidMenuID,         {0},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�Ѱѵ�ǰλ��-
  {1,             InvalidMenuID,           InvalidMenuID,         {0xA3A0, 0xA3A0, 0xD2D1, 0xB0D1, 0xB5B1, 0xC7B0, 0xCEBB, 0xD6C3},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�ɹ��궨Ϊ100%-
  {2,             InvalidMenuID,           InvalidMenuID,         {0xA3A0, 0xA3A0, 0xB3C9, 0xB9A6, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B1, 0xA3B0, 0xA3B0, 0xA3A5},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //--
  {3,             InvalidMenuID,           InvalidMenuID,         {0},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};   

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_AdjustFullInfo = {0, Invalid, Disable, Disable, Invalid, 4, Multiplex_None, Dummy_Reset};   


/*---------------------�ֳ�����-----------------------------------------------*/
const MenuStructure Menu_LocalMode[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-�ֳ��㶯����-
  {0,             Page_MainMenu_ID,        InvalidMenuID,           {0xCFD6, 0xB3A1, 0xB5E3, 0xB6AF, 0xBFD8, 0xD6C6},
  LocalMode_Special, LocalMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
  
  //-�ֳ����ֿ���-
  {1,             InvalidMenuID,           InvalidMenuID,           {0xCFD6, 0xB3A1, 0xB1A3, 0xB3D6, 0xBFD8, 0xD6C6},
  LocalMode_Special, LocalMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
 
  //-�����ϼ�-
  {2,             Page_MainMenu_ID,        InvalidMenuID,           {0xB7B5, 0xBBD8,0xC9CF, 0xBCB6},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction}
};   

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_LocalMode = {0, Invalid, Enable, Disable, Invalid, 3, Multiplex_None, StandardMenu_Reset0};    


/*---------------------Զ�̿���������-----------------------------------------*/
const MenuStructure Menu_RemoteIOMode[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-Զ�̵㶯����-
  {0,             Page_MainMenu_ID,        InvalidMenuID,         {0xD4B6, 0xB3CC, 0xB5E3, 0xB6AF, 0xBFD8, 0xD6C6},
  RemoteIOMode_Special, RemoteIOMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-Զ�̱���ͣ����-
  {1,             InvalidMenuID,           InvalidMenuID,         {0xD4B6, 0xB3CC, 0xB1A3, 0xB3D6, 0xCDA3, 0xB3A3, 0xBFAA},
  RemoteIOMode_Special, RemoteIOMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-Զ�̱���ͣ����-
  {2,             InvalidMenuID,           InvalidMenuID,         {0xD4B6, 0xB3CC, 0xB1A3, 0xB3D6, 0xCDA3, 0xB3A3, 0xB1D5},
  RemoteIOMode_Special, RemoteIOMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-���ſ����Ź�-
  {3,             InvalidMenuID,           InvalidMenuID,         {0xD3D0, 0xD0C5, 0xBFAA, 0xCEDE, 0xD0C5, 0xB9D8},
  RemoteIOMode_Special, RemoteIOMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-���Ź����ſ�-
  {4,             InvalidMenuID,           InvalidMenuID,         {0xD3D0, 0xD0C5, 0xB9D8, 0xCEDE, 0xD0C5, 0xBFAA},
  RemoteIOMode_Special, RemoteIOMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-�����ϼ�-
  {5,             Page_MainMenu_ID,        InvalidMenuID,         {0xB7B5, 0xBBD8,0xC9CF, 0xBCB6},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction}
};  

//-��6����ʼֵ����˵������,�������ȷ��ֵ- 
MenuPara      MenuPara_RemoteIOMode = {0, Invalid, Enable, Disable, Invalid, 6, Multiplex_None, StandardMenu_Reset0};  


/*---------------------Զ�̵���������-----------------------------------------*/
const MenuStructure Menu_RemoteANMode[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-��������-
  {0,             Page_MainMenu_ID,        Page_DeadZone_ID,        {0xBFD8, 0xD6C6,0xCBC0, 0xC7F8},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-���ű�λ-
  {1,             InvalidMenuID,           InvalidMenuID,           {0xB6AA, 0xD0C5,0xB1A3, 0xCEBB},
  RemoteANMode_Special, RemoteANMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-���Źر�-
  {2,             InvalidMenuID,           InvalidMenuID,           {0xB6AA, 0xD0C5,0xB9D8, 0xB1D5},
  RemoteANMode_Special, RemoteANMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-���ž���-
  {3,             InvalidMenuID,           InvalidMenuID,           {0xB6AA, 0xD0C5,0xBED3, 0xD6D0},
  RemoteANMode_Special, RemoteANMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-���Ŵ�-
  {4,             InvalidMenuID,           InvalidMenuID,           {0xB6AA, 0xD0C5,0xB4F2, 0xBFAA},
  RemoteANMode_Special, RemoteANMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-�����ϼ�-
  {5,             Page_MainMenu_ID,        InvalidMenuID,           {0xB7B5, 0xBBD8,0xC9CF, 0xBCB6},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction}
};  

//-��6����ʼֵ����˵������,�������ȷ��ֵ- 
MenuPara      MenuPara_RemoteANMode = {0, Invalid, Enable, Disable, Invalid, 6, Multiplex_None, StandardMenu_Reset0};    


/*---------------------����---------------------------------------------------*/
//-ע��:����һ�㴦��DeadZone_UpKeyӦ�÷ŵ��˵���1�����ǲ˵���2(�������ϼ�).
//     ���������Ľ���һֱ�̶���"�����ϼ�",�Ӽ��ı�Ĳ˵���1.�����⴦��.
const MenuStructure Menu_DeadZone[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-���ÿ�������-
  {0,             Page_RemoteANMode_ID,    InvalidMenuID,           {0xC9E8, 0xD6C3, 0xBFD8, 0xD6C6, 0xCBC0, 0xC7F8},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-����Ӧ/x.x%-
  {1,             InvalidMenuID,           InvalidMenuID,           {0},
  DeadZone_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-�����ϼ�-
  {2,             Page_RemoteANMode_ID,    InvalidMenuID,           {0xA3A0, 0xA3A0, 0xA3A0, 0xA3A0, 0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6},
  DeadZone_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, DeadZone_IncKey, DeadZone_DecKey},

  //-�л����ֳ��ɵ���-
  {3,             InvalidMenuID,           InvalidMenuID,           {0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB},
  DeadZone_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, AdjustOutput4mA_IncKey, AdjustOutput4mA_DecKey}
};  

//-��6����ʼֵ����˵������,�������ȷ��ֵ- 
MenuPara      MenuPara_DeadZone = {2, 4, Disable, Enable, 8, 4, Multiplex_None, Dummy_Reset};  


/*---------------------��������-----------------------------------------------*/
const MenuStructure Menu_Password[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-����רҵ��Ա����-
  {0,             Page_MainMenu_ID,        InvalidMenuID,           {0xD0E8, 0xD3C9, 0xD7A8, 0xD2B5, 0xC8CB, 0xD4B1, 0xC9E8, 0xD6C3},  
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-����������-
  {1,             InvalidMenuID,           InvalidMenuID,           {0xC7EB, 0xCAE4, 0xC8EB, 0xC3DC, 0xC2EB}, 
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //- X X X X-
  {2,             InvalidMenuID,           Page_InternalPara_ID,    {0xA3A0, 0xA3A0, 0xA3A0, 0xA3A0, 0xA3B0, 0xA3A0, 0xA3B0, 0xA3A0, 0xA3B0, 0xA3A0, 0xA3B0, 0xA3A0},
  Password_Special, Password_SetKey, Password_UpKey, Password_DownKey, Password_IncKey, Password_DecKey},

  //-���������������-
  {3,             InvalidMenuID,           InvalidMenuID,           {0xC3DC, 0xC2EB, 0xB4ED, 0xCEF3, 0xD6D8, 0xD0C2, 0xCAE4, 0xC8EB},
  Password_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction}
};   

 //-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_Password = {2, 4, Disable,Enable, 1, 4, Multiplex_Digit, Dummy_Reset};   


/*---------------------�ڲ�����-----------------------------------------------*/
const MenuStructure Menu_InternalPara[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-����4mA����-
  {0,             Page_MainMenu_ID,        Page_AdjustOutput4mA_ID, {0xB7B4, 0xC0A1, 0xA3A0, 0xA3B4, 0xA3ED, 0xA3C1, 0xD0DE, 0xD5FD},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-����20mA����-
  {1,             InvalidMenuID,           Page_AdjustOutput20mA_ID,{0xB7B4, 0xC0A1,0xA3B2, 0xA3B0, 0xA3ED, 0xA3C1, 0xD0DE, 0xD5FD},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-����4mAУ׼-
  {2,             InvalidMenuID,           Page_AdjustInput4mA_ID,  {0xCAE4, 0xC8EB, 0xA3A0, 0xA3B4, 0xA3ED, 0xA3C1, 0xD0A3, 0xD7BC},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-����20mAУ׼-
  {3,             InvalidMenuID,           Page_AdjustInput20mA_ID, {0xCAE4, 0xC8EB,0xA3B2, 0xA3B0, 0xA3ED, 0xA3C1, 0xD0A3, 0xD7BC},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-���ùض�������-
  {4,             InvalidMenuID,           Page_ShutCurrent_ID,     {0xC9E8, 0xD6C3, 0xB9D8, 0xB6AF, 0xD7F7, 0xB5E7, 0xC1F7},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-���ÿ���������-
  {5,             InvalidMenuID,           Page_OpenCurrent_ID,     {0xC9E8, 0xD6C3, 0xBFAA, 0xB6AF, 0xD7F7, 0xB5E7, 0xC1F7},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-������󿪱�ʱ��-
  {6,             InvalidMenuID,           Page_MaxActionTime_ID,   {0xB7A7, 0xC3C5,0xD7EE, 0xB4F3, 0xBFAA, 0xB1D5, 0xCAB1, 0xBCE4},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-���Ϸ�������ѡ��-
  {7,             InvalidMenuID,           Page_ErrorFeedback_ID,   {0xB9CA, 0xD5CF, 0xB7B4, 0xC0A1, 0xB4A5, 0xB5E3, 0xD1A1, 0xD4F1},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD����-
  {8,             InvalidMenuID,           Page_ESDSetting_ID,      {0x45, 0x53, 0x44, 0xC9E8, 0xD6C3},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-�����ϼ�-
  {9,             Page_MainMenu_ID,        InvalidMenuID,           {0xB7B5, 0xBBD8,0xC9CF, 0xBCB6},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction}
};   

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_InternalPara = {0, Invalid, Enable, Disable, Invalid, 10, Multiplex_None, StandardMenu_Reset0};    


/*---------------------��������4mA����----------------------------------------*/
const MenuStructure Menu_AdjustOutput4mA[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-��������4mA����-
  {0,             Page_InternalPara_ID,    InvalidMenuID,           {0xB7B4, 0xC0A1, 0xB5E7, 0xC1F7, 0xA3B4, 0xA3ED, 0xA3C1, 0xD0DE, 0xD5FD},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {1,             InvalidMenuID,           InvalidMenuID,           {0},
  AdjustOutput4_20mA_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-�����ϼ�-
  {2,             Page_InternalPara_ID,    InvalidMenuID,           {0xA3A0, 0xA3A0, 0xA3A0, 0xA3A0, 0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6},
  AdjustOutput4_20mA_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, AdjustOutput4mA_IncKey, AdjustOutput4mA_DecKey},

  //-�л����ֳ��ɵ���-
  {3,             InvalidMenuID,           InvalidMenuID,           {0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB},
  AdjustOutput4_20mA_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, AdjustOutput4mA_IncKey, AdjustOutput4mA_DecKey}
};  

//-��6����ʼֵ����˵������,�������ȷ��ֵ- 
MenuPara      MenuPara_AdjustOutput4mA = {2, 4, Disable, Enable, 8, 4, Multiplex_Adjust, Dummy_Reset}; 


/*---------------------��������20mA����----------------------------------------*/
const MenuStructure Menu_AdjustOutput20mA[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-��������20mA����-
  {0,             Page_InternalPara_ID,    InvalidMenuID,           {0xB7B4, 0xC0A1, 0xB5E7, 0xC1F7, 0xA3B2, 0xA3B0, 0xA3ED, 0xA3C1, 0xD0DE, 0xD5FD},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-����-
  {1,             InvalidMenuID,           InvalidMenuID,           {0},
  AdjustOutput4_20mA_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-�����ϼ�-
  {2,             Page_InternalPara_ID,    InvalidMenuID,           {0xA3A0, 0xA3A0, 0xA3A0, 0xA3A0, 0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6},
  AdjustOutput4_20mA_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, AdjustOutput20mA_IncKey, AdjustOutput20mA_DecKey}, 

  //-�л����ֳ��ɵ���-
  {3,             InvalidMenuID,           InvalidMenuID,           {0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB},
  AdjustOutput4_20mA_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, AdjustOutput4mA_IncKey, AdjustOutput4mA_DecKey}
};  

//-��6����ʼֵ����˵������,�������ȷ��ֵ- 
MenuPara      MenuPara_AdjustOutput20mA = {2, 4, Disable, Enable, 8, 4, Multiplex_Adjust, Dummy_Reset}; 


/*---------------------�궨����4mA--------------------------------------------*/
const MenuStructure Menu_AdjustInput4mA[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-�ѵ�ǰ�������-
  {0,             Page_InternalPara_ID,  InvalidMenuID,           {0xA3A0, 0xB0D1, 0xB5B1, 0xC7B0, 0xCAE4, 0xC8EB, 0xB5E7, 0xC1F7},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-Լxx.xmA-
  {1,             InvalidMenuID,         InvalidMenuID,           {0, 0, 0, 0, 0xD4BC, 0, 0, 0, 0, 0xA3ED, 0xA3C1},
  AdjustInput4_20mA_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�궨ΪĿ��0%-
  {2,             InvalidMenuID,         InvalidMenuID,           {0xA3A0, 0xB1EA, 0xB6A8, 0xCEAA,0xC4BF, 0xB1EA, 0xA3B0,0xA3A5},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-ȷ��  ����-
  {3,             Page_InternalPara_ID,   Page_AdjustInput4mAInfo_ID,{0xA3A0, 0xA3A0, 0xA3A0, 0xC8B7, 0xC8CF, 0xA3A0, 0xA3A0, 0xB7B5, 0xBBD8},
  Dummy_Special, AdjustInput4_20mA_SetKey, AdjustInput4_20mA_UpKey, AdjustInput4_20mA_DownKey, DummyFunction, DummyFunction},
};   

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_AdjustInput4mA = {3, 3, Disable, Enable, 4, 4, Multiplex_None, StandardMenu_Reset1}; 


/*---------------------��������궨4mA��ʾ��Ϣ---------------------------------------*/
const MenuStructure Menu_AdjustInput4mAInfo[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //--
  {0,             Page_InternalPara_ID,    InvalidMenuID,           {0},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�Ѱѵ�ǰ����-
  {1,             InvalidMenuID,           InvalidMenuID,           {0xA3A0, 0xA3A0, 0xD2D1, 0xB0D1, 0xB5B1, 0xC7B0, 0xB5E7, 0xC1F7},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�ɹ��궨Ϊ0%-
  {2,             InvalidMenuID,           InvalidMenuID,           {0xA3A0, 0xA3A0, 0xB3C9, 0xB9A6, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B0, 0xA3A5},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //--
  {3,             InvalidMenuID,           InvalidMenuID,           {0},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};   

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_AdjustInput4mAInfo = {0, Invalid, Disable, Disable, Invalid, 4, Multiplex_None, Dummy_Reset};


/*---------------------�궨����20mA-------------------------------------------*/
const MenuStructure Menu_AdjustInput20mA[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-�ѵ�ǰ�������-
  {0,             Page_InternalPara_ID,    InvalidMenuID,           {0xA3A0, 0xB0D1, 0xB5B1, 0xC7B0, 0xCAE4, 0xC8EB, 0xB5E7, 0xC1F7},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-Լxx.xmA-
  {1,             InvalidMenuID,           InvalidMenuID,           {0, 0, 0, 0, 0xD4BC, 0, 0, 0, 0, 0xA3ED, 0xA3C1},
  AdjustInput4_20mA_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�궨ΪĿ��100%-
  {2,             InvalidMenuID,           InvalidMenuID,           {0xA3A0, 0xB1EA, 0xB6A8, 0xCEAA,0xC4BF, 0xB1EA, 0xA3B1, 0xA3B0, 0xA3B0,0xA3A5},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-ȷ��  ����-
  {3,             Page_InternalPara_ID,     Page_AdjustInput20mAInfo_ID,{0xA3A0, 0xA3A0, 0xA3A0, 0xC8B7, 0xC8CF, 0xA3A0, 0xA3A0, 0xB7B5, 0xBBD8},
  Dummy_Special, AdjustInput4_20mA_SetKey, AdjustInput4_20mA_UpKey, AdjustInput4_20mA_DownKey, DummyFunction, DummyFunction},
};   

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_AdjustInput20mA = {3, 3, Disable, Enable, 4, 4, Multiplex_None, StandardMenu_Reset1}; 


/*---------------------����20mA�궨��ʾ��Ϣ---------------------------------------*/
const MenuStructure Menu_AdjustInput20mAInfo[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //--
  {0,             Page_InternalPara_ID,    InvalidMenuID,           {0},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�Ѱѵ�ǰ����-
  {1,             InvalidMenuID,           InvalidMenuID,           {0xA3A0, 0xA3A0, 0xD2D1, 0xB0D1, 0xB5B1, 0xC7B0, 0xB5E7, 0xC1F7},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�ɹ��궨Ϊ100%-
  {2,             InvalidMenuID,           InvalidMenuID,           {0xA3A0, 0xA3A0, 0xB3C9, 0xB9A6, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B1, 0xA3B0, 0xA3B0, 0xA3A5},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //--
  {3,             InvalidMenuID,           InvalidMenuID,           {0},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};   

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_AdjustInput20mAInfo = {0, Invalid, Disable, Disable, Invalid, 4, Multiplex_None, Dummy_Reset};   


/*------------------ �ض�������-------------------------------*/
const MenuStructure Menu_ShutCurrent[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-���ù���������-
  {0,             Page_InternalPara_ID,  InvalidMenuID,           {0xC9E8, 0xD6C3, 0xB9D8, 0xCFF2, 0xB6AF, 0xD7F7, 0xB5E7, 0xC1F7},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {1,             InvalidMenuID,         InvalidMenuID,           {0},
  ShutCurrent_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-    �����ϼ�-
  {2,             Page_InternalPara_ID,  InvalidMenuID,           {Space, Space, Space, Space, 0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6},
  ShutCurrent_Special, StandardMenu_Back2Parent, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-�л����ֳ��ɵ���-
  {3,             InvalidMenuID,         InvalidMenuID,           {0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB},
  ShutCurrent_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_ShutCurrent = {2, 4, Disable, Enable, 8, 4, Multiplex_None, Dummy_Reset};


/*------------------ ����������-------------------------------*/
const MenuStructure Menu_OpenCurrent[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-���ÿ���������-
  {0,             Page_InternalPara_ID,  InvalidMenuID,           {0xC9E8, 0xD6C3, 0xBFAA, 0xCFF2, 0xB6AF, 0xD7F7, 0xB5E7, 0xC1F7},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {1,             InvalidMenuID,         InvalidMenuID,           {0},
  ShutCurrent_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-    �����ϼ�-
  {2,             Page_InternalPara_ID,    InvalidMenuID,           {Space, Space, Space, Space, 0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6},
  ShutCurrent_Special, StandardMenu_Back2Parent, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-�л����ֳ��ɵ���-
  {3,             InvalidMenuID,         InvalidMenuID,           {0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB},
  ShutCurrent_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_OpenCurrent = {2, 4, Disable, Enable, 8, 4, Multiplex_None, Dummy_Reset};


/*---------------------������󿪱�ʱ��---------------------------------------*/
const MenuStructure Menu_MaxActionTime[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-������󿪱�ʱ��-
  {0,             Page_InternalPara_ID,    InvalidMenuID,           {0xB7A7, 0xC3C5, 0xD7EE, 0xB4F3, 0xBFAA, 0xB1D5, 0xCAB1, 0xBCE4},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-����ʱ/xxxS-
  {1,             Page_InternalPara_ID,    InvalidMenuID,           {0},
  MaxActionTime_Special, MaxActionTime_SetKey, DummyFunction, DummyFunction, MaxActionTime_IncKey, MaxActionTime_DecKey},

  //-    �����ϼ�-
  {2,             Page_InternalPara_ID,    InvalidMenuID,           {Space, Space, Space, Space, 0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6},
  MaxActionTime_Special, StandardMenu_Back2Parent, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-�л����ֳ��ɵ���-
  {3,             InvalidMenuID,         InvalidMenuID,           {0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB},
  MaxActionTime_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};  

//-��6����ʼֵ����˵������,�������ȷ��ֵ- 
MenuPara      MenuPara_MaxActionTime = {1, 6, Disable, Enable, 6, 4, Multiplex_Digit, Dummy_Reset}; 


/*------------------ ���Ϸ�������-------------------------------*/
const MenuStructure Menu_ErrorFeedback[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-���Ϸ�������ѡ��-
  {0,             Page_InternalPara_ID,  InvalidMenuID,           {0xB9CA, 0xD5CF, 0xB7B4, 0xC0A1, 0xB4A5, 0xB5E3, 0xD1A1, 0xD4F1},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-�й��ϱպ�NO-
  {1,             InvalidMenuID,         InvalidMenuID,           {0xD3D0, 0xB9CA, 0xD5CF, 0xB1D5, 0xBACF, 0x4E, 0x4F},
  ErrorFeedback_Special, ErrorFeedback_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-�й��ϱպ�NC-
  {2,             InvalidMenuID,         InvalidMenuID,           {0xD3D0, 0xB9CA, 0xD5CF, 0xB1D5, 0xBACF, 0x4E, 0x43},
  ErrorFeedback_Special, ErrorFeedback_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-�����ϼ�-
  {3,             Page_InternalPara_ID,  InvalidMenuID,           {0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6},
  Dummy_Special, StandardMenu_Back2Parent, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
};

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_ErrorFeedback = {1, Invalid, Enable, Disable, 8, 4, Multiplex_None, Dummy_Reset};


/*------------------ ESD����-------------------------------*/
const MenuStructure Menu_ESDSetting[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-ESD����-
  {0,             Page_InternalPara_ID,  InvalidMenuID,           {0x45, 0x53, 0x44, 0xBDFB, 0xD3C3},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD��ֹ����-
  {1,             InvalidMenuID,         InvalidMenuID,           {0x45, 0x53, 0x44, 0xBDFB, 0xD6B9, 0xB6AF, 0xD7F7},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD�򿪷���-
  {2,             InvalidMenuID,         InvalidMenuID,           {0x45, 0x53, 0x44, 0xB4F2, 0xBFAA, 0xB7A7, 0xC3C5},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD�رշ���-
  {3,             InvalidMenuID,         InvalidMenuID,           {0x45, 0x53, 0x44, 0xB9D8, 0xB1D5, 0xB7A7, 0xC3C5},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD���м�λ-
  {4,             InvalidMenuID,         InvalidMenuID,           {0x45, 0x53, 0x44, 0xB5BD, 0xD6D0, 0xBCE4, 0xCEBB},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-�����ϼ�-
  {5,             Page_InternalPara_ID,  InvalidMenuID,           {0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6},
  Dummy_Special, StandardMenu_Back2Parent, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
};

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_ESDSetting = {0, 0, Enable, Disable, Invalid, 6, Multiplex_None, Dummy_Reset};


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void DummyFunction(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void Dummy_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex)
{
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void Dummy_Reset(MenuPara *pMenuPara)
{
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void ShowStar(int X_Offset)
{
    unsigned char FontBuf[32] = {0};
    unsigned char FontBufLen = 0;
  
    unsigned int Code = 0;

    //-����"*"- 
    Code = 0xA3AA;
    GetDotData(Code, EN_Character, &FontBuf[0], &FontBufLen);
    SwapFontBuf2DisplayBuf(&FontBuf[0], FontBufLen, &g_DisplayBuf[0], X_Offset);
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void GetSingleNumber(unsigned int Data, unsigned char *pGe, unsigned char *pShi, unsigned char *pBai, unsigned char *pQian, unsigned char *pWan)
{
    signed long TempData = Data;    //-�������ת��,�������-
    
    while((TempData - 10000) >= 0)
    {
        TempData -= 10000;
        (*pWan)++;
    }
    while((TempData - 1000) >= 0)
    {
        TempData -= 1000;
        (*pQian)++;
    }
    while((TempData - 100) >= 0)
    {
        TempData -= 100;
        (*pBai)++;
    }
    while((TempData - 10) >= 0)
    {
        TempData -= 10;
        (*pShi)++;
    }
    *pGe = TempData;  
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void InsertBlock(unsigned int *pCode, int CodeLen, int X_Offset)
{
    int i = 0;
    int j = 0;
    int k = 0;

    unsigned char FontBuf[32] = {0};
    unsigned char FontBufLen = 0;

    unsigned char CharacterType = 0;
    int Character_X_Size = 0;

    for (i = 0; i < CodeLen; i++)
    {
        if (pCode[i] == 0x00)    //-��Ч:����ǿո�����0xA3A0-
            continue;

        if (pCode[i] >= 0xB0A0)
        {
            CharacterType = CN_Character;
            Character_X_Size = CN_X_SIZE;
        }
        else 
        {
            CharacterType = EN_Character;
            Character_X_Size = EN_X_SIZE;
        }

        GetDotData(pCode[i], CharacterType, &FontBuf[0], &FontBufLen);
        
        for (k = 0; k < Character_X_Size; k++)
        {
            for (j = Size_Per_Line / 2 - 1; j >= X_Offset + k; j--)
            { 
                g_DisplayBuf[j] = g_DisplayBuf[j - 1];
            }
    
            for (j = Size_Per_Line - 1; j >= X_Offset + k + (Size_Per_Line / 2); j--)
            { 
                g_DisplayBuf[j] = g_DisplayBuf[j - 1];
            }
        }

        SwapFontBuf2DisplayBuf(&FontBuf[0], FontBufLen, &g_DisplayBuf[0], X_Offset);
        X_Offset += Character_X_Size;
    }
}


/*******************************************************************************
* ��������:    ShowBlock
* ��������:    �����ַ����������ʾ����
* �������:    *pCode,���ֻ���
*              CodeLen,���ֻ��峤��
*              X_Offset,ˮƽƫ��0~128
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void ShowBlock(unsigned int *pCode, int CodeLen, int X_Offset)
{
    int i = 0;

    unsigned char FontBuf[32] = {0};
    unsigned char FontBufLen = 0;

    unsigned char CharacterType = 0;
    int Character_X_Size = 0;

    for (i = 0; i < CodeLen; i++)
    {
        if (pCode[i] == 0x00)    //-��Ч:����ǿո�����0xA3A0-
            continue;

        if (pCode[i] >= 0xB0A0)
        {
            CharacterType = CN_Character;
            Character_X_Size = CN_X_SIZE;
        }
        else 
        {
            CharacterType = EN_Character;
            Character_X_Size = EN_X_SIZE;
        }

        GetDotData(pCode[i], CharacterType, &FontBuf[0], &FontBufLen);
        SwapFontBuf2DisplayBuf(&FontBuf[0], FontBufLen, &g_DisplayBuf[0], X_Offset);
        X_Offset += Character_X_Size;
    }
}


/*******************************************************************************
* ��������:    ShowNumbers
* ��������:    
* �������:    Value,Ҫ��ʾ��ֵ(ȥ��С������ֵ)
*              BitCount,��ʾ��λ��(ȥ��С������λ��,����0.5��λ��Ϊ2)
*              DecimalBitCount,С����λ��(����0.5��λ��Ϊ2)
*              X_Offset,��ʾ��ˮƽƫ��0~128
*              FrontZeroDisplay, = 1,��Ч����ǰ�����Ҳ��ʾ; ������ʾ.
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void ShowNumbers(unsigned int Value, int BitCount, int DecimalBitCount, int X_Offset, int FrontZeroDisplay)
{
    int i = 0;
    int DecimalIndex = 0;

    unsigned char GeValue = 0; 
    unsigned char ShiValue = 0;
    unsigned char BaiValue = 0;
    unsigned char QianValue = 0;
    unsigned char WanValue = 0;

    ClearBuf((unsigned char *)&LongCode, sizeof(LongCode));
    GetSingleNumber(Value, &GeValue, &ShiValue, &BaiValue, &QianValue, &WanValue);

    if (BitCount > 5)    //-������ʾʮ���Ƶ�5λ��-
    {
        return;
    }
    if (DecimalBitCount >= BitCount)
    {
        return;
    }

    //-��-
    if (WanValue == 0)
    {
        if (FrontZeroDisplay == 1)
        {
            LongCode[0] = WanValue + 0xA3B0;
        }
        else
        {
            LongCode[0] = Space;
        }
    }
    else
    {

        LongCode[0] = WanValue + 0xA3B0;
    }

    //-ǧ-
    if ((QianValue == 0) && (WanValue == 0))
    {
        if (FrontZeroDisplay == 1)
        {
            LongCode[1] = QianValue + 0xA3B0;
        }
        else
        {
            LongCode[1] = Space;
        }
    }
    else
    {
        LongCode[1] = QianValue + 0xA3B0;
    }

    //-��-
    if ((BaiValue == 0) && (QianValue == 0) && (WanValue == 0))
    {
        if (FrontZeroDisplay == 1)
        {
            LongCode[2] = BaiValue + 0xA3B0;
        }
        else
        {
            LongCode[2] = Space;
        }
    }
    else 
    {
        LongCode[2] = BaiValue + 0xA3B0;
    }

    //-ʮ-
    if ((ShiValue == 0) && (BaiValue == 0) && (QianValue == 0) && (WanValue == 0))
    {
        if (FrontZeroDisplay == 1)
        {
            LongCode[3] = ShiValue + 0xA3B0;
        }
        else
        {
            LongCode[3] = Space;
        }
    }
    else
    {
        LongCode[3] = ShiValue + 0xA3B0;
    } 

    //-��-
    LongCode[4] = GeValue + 0xA3B0;

    if ((LongCode[3] != Space) || (LongCode[4] != 0xA3B0))
    {
        DecimalIndex = 5 - DecimalBitCount;
        if (LongCode[DecimalIndex - 1] == Space)
        {
            LongCode[DecimalIndex - 1] = 0xA3B0;
        }
        for (i = 4; i >= DecimalIndex; i--)
        {
            if (LongCode[i] == Space)
            {
                LongCode[i] = 0xA3B0;
            }
            
            LongCode[i + 1] = LongCode[i];
        }

        if (DecimalBitCount == 0)
        {
            //-Ҫ�ж���ʾ��λ��-
            ShowBlock(&LongCode[5 - BitCount], BitCount, X_Offset);
        }
        else
        {
            LongCode[DecimalIndex] = 0xA3AE;    //-С����-
    
            //-Ҫ�ж���ʾ��λ��-
            ShowBlock(&LongCode[5 - BitCount], BitCount + 1, X_Offset);
        }
    }
    else
    {
        if (DecimalBitCount == 0)
        {
            ShowBlock(&LongCode[5 - BitCount], BitCount, X_Offset);
        }
        else
        {
            if (FrontZeroDisplay == 1)
            {
                ShowBlock(&LongCode[5 - BitCount], BitCount, X_Offset);
            }
            else
            {
                //-==0ʱ,��ʾ0,����Ҫȥ��С������ռ��λ��-
                ShowBlock(&LongCode[5 - BitCount], BitCount, X_Offset + 8);
            }
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
void ShowInc_Dec(int LineIndex)
{
    int Index = 0;
    int DisplayBufIndex = 0;

    if (LineIndex == 2)
    {
        DisplayBufIndex= 0;
        //-��һ��:����-
        LongCode[Index++] = 0xB4F2;
        LongCode[Index++] = 0xBFAA;
        LongCode[Index++] = 0xD2BB;
        LongCode[Index++] = 0xCFC2;
        LongCode[Index++] = 0x3A;
        LongCode[Index++] = 0xD4F6;
        LongCode[Index++] = 0xB4F3;
        ShowBlock(&LongCode[0], Index, DisplayBufIndex);
    }
    else if (LineIndex == 3)
    {
        DisplayBufIndex= 0;
        //-�ر�һ��:��С-
        LongCode[Index++] = 0xB9D8;
        LongCode[Index++] = 0xB1D5;
        LongCode[Index++] = 0xD2BB;
        LongCode[Index++] = 0xCFC2;
        LongCode[Index++] = 0x3A;
        LongCode[Index++] = 0xBCF5;
        LongCode[Index++] = 0xD0A1;
        ShowBlock(&LongCode[0], Index, DisplayBufIndex);
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void ClearBuf(unsigned char *Buf, int BufLen)
{
    int i = 0;
   
    for (i = 0; i < BufLen; i++)
    {
        Buf[i] = 0x00;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void NormalPage_Special0(const MenuStructure *pMenu, MenuPara *pMenuPara, int Line)
{
    int LineIndex = 0;
    int DisplayBufIndex = 0;

    unsigned int  Code[4] = {0};

    int i = 0;
    int j = 0;
    int Index = 0;
    unsigned char HaveError = 0;
    unsigned char IsMoving  = 0;
    unsigned char Error = 0;
    const unsigned short (*pError)[2][16];

    /*--------------------------------��һ��------------------------------------*/
    LineIndex = 0;
    ClearBuf(&g_DisplayBuf[0], Display_Buf_Size);

    ClearBuf((unsigned char *)&Code[0], sizeof(Code));
    if (Valve.Status.StatusBits.Opening == 1)
    {
        IsMoving = 1; 

        Code[0] = 0xB4F2;    //-��-
        Code[1] = 0xBFAA;    //-��-
        Code[2] = 0xD6D0;    //-��-
    }
    else if(Valve.Status.StatusBits.Shutting == 1)
    {
        IsMoving = 1; 

        Code[0] = 0xB9D8;    //-��-
        Code[1] = 0xB1D5;    //-��-
        Code[2] = 0xD6D0;    //-��-
    }
    else if (Valve.Status.StatusBits.OpenLimit == 1)
    {
        Code[0] = 0xBFAA;    //-��-
        Code[1] = 0xB5BD;    //-��-
        Code[2] = 0xCEBB;    //-λ-     
    }
    else if (Valve.Status.StatusBits.ShutLimit == 1)
    {
        Code[0] = 0xB9D8;    //-��-
        Code[1] = 0xB5BD;    //-��-
        Code[2] = 0xCEBB;    //-λ-
    }
    else 
    {
    }

    DisplayBufIndex = 0;
    ShowBlock(&Code[0], 3, DisplayBufIndex);

    /*-���Ͻǲ�����ʾ���ȼ�:���ش��������ֶ�������״̬���������
      -*/
    ClearBuf((unsigned char *)&LongCode[0], sizeof(LongCode));
    if (F_Disconnect == 1)
    {
        LongCode[Index++] = 0xCDA8;    //-ͨ-
        LongCode[Index++] = 0xD1B6;    //-Ѷ-
        LongCode[Index++] = 0xB6CF;    //-��-
        LongCode[Index++] = 0xC1AC;    //-��-
    }
    else if (Device.Error.ErrorByte != 0)
    {
        HaveError = 1;
    }
    else if (Device.CommMode.CommModeBits.Manual == 1)
    {
        LongCode[Index++] = 0xCAD6;    //-��-
        LongCode[Index++] = 0xC2D6;    //-��-
        LongCode[Index++] = 0xCAD6;    //-��-
        LongCode[Index++] = 0xC2D6;    //-��-
    }
    else if (Device.Status.ESDStatus == ESDStatus_Valid)
    {
        LongCode[Index++] = 0xBDF4;    //-��-
        LongCode[Index++] = 0xBCB1;    //-��-
        LongCode[Index++] = 0xD7B4;    //-״-
        LongCode[Index++] = 0xCCAC;    //-̬-
    }
    else if (Valve.Error.ErrorByte != 0)
    {
        HaveError = 1;
    }
    else if (IsMoving == 1)
    {
        LongCode[Index++] = 0x49;      //-I-
        LongCode[Index++] = 0x3D;      //-=-
        LongCode[Index++] = Space;
        LongCode[Index++] = Space;
        LongCode[Index++] = Space;
        LongCode[Index++] = Space;
        LongCode[Index++] = 0x41;      //-A-

        DisplayBufIndex = 64;
        ShowBlock(&LongCode[0], Index, DisplayBufIndex);

        DisplayBufIndex += 16;
        if (Device.Para.CurrentDecimalBits == CurrentDecimalBits_Two)
        {
            ShowNumbers(Valve.MiscInfo.Current, 3, 2, DisplayBufIndex, 1);
        }
        else
        {
            ShowNumbers(Valve.MiscInfo.Current, 3, 1, DisplayBufIndex, 1);
        }

        LcdRefresh(LineIndex);

        return;    //-ֱ�ӷ���-
    }
    else
    {
    }


    if (HaveError == 1)
    {
        if (Device.Error.ErrorByte != 0)
        {
            pError = &SeriousErrorArray[0];
            Error = Device.Error.ErrorByte;
        }
        else if (Valve.Error.ErrorByte != 0)
        {
            pError = &SlightErrorArray[0];
            Error = Valve.Error.ErrorByte;
        }
        else
        {
        }
        for (i = 7; i >= 0; i--)
        {
            if (Error & (1 << i))
            {
                for (j = 0; j < 16; j++)
                {
                    LongCode[Index++] = pError[i][Device.Para.LanguageType][j];
                }
    
                break;
            }
        }
    }

    DisplayBufIndex = 64;
    ShowBlock(&LongCode[0], Index, DisplayBufIndex);

    LcdRefresh(LineIndex);
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void NormalPage_Special1(const MenuStructure *pMenu, MenuPara *pMenuPara, int Line)
{
    int i = 0;

    int LineIndex = 0;
    int DisplayBufIndex = 0;
    unsigned int  Code[4] = {0};
    signed int CurOpening = 0;
    
    unsigned char DummyData = 0;
    unsigned char BaiValue = 0;
    unsigned char ShiValue = 0;
    unsigned char GeValue =  0;

    /*--------------------------------�ڶ���------------------------------------*/
    LineIndex = 1;
    ClearBuf(&g_DisplayBuf[0], Display_Buf_Size); 

    //-----------��ǰ���������ַ��ϰ벿�ֵĴ���-----------
    CurOpening = Valve.MiscInfo.CurOpening;
    GetSingleNumber(CurOpening, &GeValue, &ShiValue, &BaiValue, &DummyData, &DummyData);
 
    //-��λ-
    DisplayBufIndex = 0;
    if (BaiValue == 0)
    {     
    }
    else
    {
        for (i = 0; i < 16; i++)
        {
            g_DisplayBuf[DisplayBufIndex]  = SpecicalCharacter[BaiValue][0][i];
            g_DisplayBuf[DisplayBufIndex + 128] = SpecicalCharacter[BaiValue][1][i];
            
            DisplayBufIndex++;
        }
    }
    //-ʮλ-
    DisplayBufIndex = 16;
    if ((ShiValue == 0) && (BaiValue == 0))
    {           
    }
    else
    {
        for (i = 0; i < 16; i++)
        {
            g_DisplayBuf[DisplayBufIndex]         = SpecicalCharacter[ShiValue][0][i];
            g_DisplayBuf[DisplayBufIndex + 128] = SpecicalCharacter[ShiValue][1][i];

            DisplayBufIndex++;
        }
    }
    //-��λ-
    DisplayBufIndex = 32;
    for (i = 0; i < 16; i++)
    {
        g_DisplayBuf[DisplayBufIndex]         = SpecicalCharacter[GeValue][0][i];
        g_DisplayBuf[DisplayBufIndex + 128] = SpecicalCharacter[GeValue][1][i];
        
        DisplayBufIndex++;
    }
    //-%-
    DisplayBufIndex = 48;
    for (i = 0; i < 16; i++)
    {
        g_DisplayBuf[DisplayBufIndex]         = SpecicalCharacter[PercentIndex][0][i];
        g_DisplayBuf[DisplayBufIndex + 128] = SpecicalCharacter[PercentIndex][1][i];

        DisplayBufIndex++;
    }

    //-----------����/Ŀ��-----------------
    ClearBuf((unsigned char *)&Code[0], sizeof(Code));
    if ((Device.CommMode.CommModeBits.Remote == 1) && (Device.Para.RemoteType == RemoteType_Regulate))
    {
        if (Valve.Status.StatusBits.NoSignal == 1)
        {
            Code[0] = 0xB6AA;    //-��-  
            Code[1] = 0xD0C5;    //-��-  
        }
        else
        {
            Code[0] = 0xC4BF;    //-Ŀ-  
            Code[1] = 0xB1EA;    //-��-  
        }
    }
    else
    {
    }

    DisplayBufIndex = 96;
    ShowBlock(&Code[0], 2, DisplayBufIndex);

    LcdRefresh(LineIndex);
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void NormalPage_Special2(const MenuStructure *pMenu, MenuPara *pMenuPara, int Line)
{
    int i = 0;

    int LineIndex = 0;
    int DisplayBufIndex = 0;

    unsigned int  Code[4] = {0};

    signed int DstOpening = 0;
    signed int CurOpening = 0;
    
    unsigned char DummyData = 0;
    unsigned char BaiValue = 0;
    unsigned char ShiValue = 0;
    unsigned char GeValue =  0;


    /*--------------------------------������------------------------------------*/
    LineIndex = 2; 
    ClearBuf(&g_DisplayBuf[0], Display_Buf_Size);

    //-----------��ǰ���������ַ��°벿�ֵĴ���-----------
    CurOpening = Valve.MiscInfo.CurOpening;
    GetSingleNumber(CurOpening, &GeValue, &ShiValue, &BaiValue, &DummyData, &DummyData);
    //-��λ-
    DisplayBufIndex = 0;
    if (BaiValue == 0)
    {     
    }
    else
    {
        for (i = 0; i < 16; i++)
        {
            g_DisplayBuf[DisplayBufIndex]  = SpecicalCharacter[BaiValue][2][i];
            g_DisplayBuf[DisplayBufIndex + 128] = SpecicalCharacter[BaiValue][3][i];
            
            DisplayBufIndex++;
        }
    }
    //-ʮλ-
    DisplayBufIndex = 16;
    if ((ShiValue == 0) && (BaiValue == 0))
    {           
    }
    else
    {
        for (i = 0; i < 16; i++)
        {
            g_DisplayBuf[DisplayBufIndex]         = SpecicalCharacter[ShiValue][2][i];
            g_DisplayBuf[DisplayBufIndex + 128] = SpecicalCharacter[ShiValue][3][i];

            DisplayBufIndex++;
        }
    }
    //-��λ-
    DisplayBufIndex = 32;
    for (i = 0; i < 16; i++)
    {
        g_DisplayBuf[DisplayBufIndex]         = SpecicalCharacter[GeValue][2][i];
        g_DisplayBuf[DisplayBufIndex + 128] = SpecicalCharacter[GeValue][3][i];
        
        DisplayBufIndex++;
    }
    //-%-
    DisplayBufIndex = 48;
    for (i = 0; i < 16; i++)
    {
        g_DisplayBuf[DisplayBufIndex]         = SpecicalCharacter[PercentIndex][2][i];
        g_DisplayBuf[DisplayBufIndex + 128] = SpecicalCharacter[PercentIndex][3][i];

        DisplayBufIndex++;
    }

     //-----------�հ�/Ŀ�꿪��-----------------
    BaiValue = 0;
    ShiValue = 0;
    GeValue  = 0;
    DstOpening = (signed int)Valve.Operation.DstOpening;    //-���ǵ����Ȳ��������-
    GetSingleNumber(DstOpening, &GeValue, &ShiValue, &BaiValue, &DummyData, &DummyData);
  
    ClearBuf((unsigned char *)&Code[0], sizeof(Code));
    if ((Device.CommMode.CommModeBits.Remote == 1) && (Device.Para.RemoteType == RemoteType_Regulate))
    {
        if (Valve.Status.StatusBits.NoSignal == 1)
        { 
        }
        else
        {
            if (BaiValue == 0)
            {
                Code[0] = 0xA3A0;              //-�ո�-  
            } 
            else
            {
                Code[0] = 0xA3B0 + BaiValue;    //-��-  
            }
            if ((ShiValue == 0) && (BaiValue == 0))
            {
                Code[1] = 0xA3A0;              //-�ո�-  
            } 
            else
            {
                Code[1] = 0xA3B0 + ShiValue;    //-ʮ-  
            } 
            Code[2] = 0xA3B0 + GeValue;         //-��-  
            Code[3] = 0xA3A5;                   //-%-  
        }

        DisplayBufIndex = 96;
        ShowBlock(&Code[0], 4, DisplayBufIndex);
    }
    else
    {
    }
    LcdRefresh(LineIndex);
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void NormalPage_Special3(const MenuStructure *pMenu, MenuPara *pMenuPara, int Line)
{
    int LineIndex = 0;
    int DisplayBufIndex = 0;

    unsigned int  Code[4] = {0};

    /*--------------------------------������------------------------------------*/
    LineIndex = 3;  
    ClearBuf(&g_DisplayBuf[0], Display_Buf_Size);   

    ClearBuf((unsigned char *)&Code[0], sizeof(Code));
    switch(Device.WorkMode.CurWorkMode)
    {
    case 0:
        Code[0] = 0xD4B6;    //-Զ-  
        Code[1] = 0xB3CC;    //-��-  
        Code[2] = 0xCDA8;    //-ͨ-  
        Code[3] = 0xD0C5;    //-��- 
        break;
    case 1:
        Code[0] = 0xD4B6;    //-Զ-  
        Code[1] = 0xB3CC;    //-��-  
        Code[2] = 0xC4A3;    //-ģ-  
        Code[3] = 0xC4E2;    //-��- 
        break;
    case 2:
        Code[0] = 0xD4B6;    //-Զ-  
        Code[1] = 0xB3CC;    //-��-  
        Code[2] = 0xB5E3;    //-��-  
        Code[3] = 0xB6AF;    //-��- 
        break;
    case 3:
        Code[0] = 0xD4B6;    //-Զ-  
        Code[1] = 0xB3CC;    //-��-  
        Code[2] = 0xB1A3;    //-��-  
        Code[3] = 0xB3D6;    //-��- 
        break;
    case 4:
        Code[0] = 0xD4B6;    //-Զ-  
        Code[1] = 0xB3CC;    //-��-  
        Code[2] = 0xCBAB;    //-˫-  
        Code[3] = 0xCEBB;    //-λ- 
        break;
    case 5:
        Code[0] = 0xCFD6;    //-��-  
        Code[1] = 0xB3A1;    //-��-  
        Code[2] = 0xB5E3;    //-��-  
        Code[3] = 0xB6AF;    //-��- 
        break;
    case 6:
        Code[0] = 0xCFD6;    //-��-  
        Code[1] = 0xB3A1;    //-��-  
        Code[2] = 0xB1A3;    //-��-  
        Code[3] = 0xB3D6;    //-��- 
        break;
    case 7:
        Code[0] = 0xCDA3;    //-ͣ-  
        Code[1] = 0;
        Code[2] = 0;
        Code[3] = 0xD6B9;    //-ֹ-  
        break;
    default:
        break;
    }
   
    DisplayBufIndex = 32;
 
    ShowBlock(&Code[0], 4, DisplayBufIndex);

    LcdRefresh(LineIndex);
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void StandardMenu_Back2Parent(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (pMenu->ParentMenuID != InvalidMenuID)
    {
        IsNeedChangePage = 1;
        Show_Page(pMenu->ParentMenuID);
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void StandardMenu_Set2Child(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (pMenu->ChildMenuID != InvalidMenuID)
    {
        IsNeedChangePage = 1;
        NeedResetMenuPara = 1;
        Show_Page(pMenu->ChildMenuID);
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void StandardMenu_Reset0(MenuPara *pMenuPara)
{
    pMenuPara->RowIndex = 0;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void StandardMenu_Reset1(MenuPara *pMenuPara)
{
    pMenuPara->ColumnIndex = 3;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void StandardMenu_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    //-��׼�˵����1��һ��Ϊ"�����ϼ�"-
    if (pMenuPara->RowIndex == pMenuPara->NumOfRows - 1)
    {
        if (pMenu->ParentMenuID != InvalidMenuID)
        {
            IsNeedChangePage = 1;
            Show_Page(pMenu->ParentMenuID);
        }
    }
    else
    {
        if (pMenu->ChildMenuID != InvalidMenuID)
        {
            NeedResetMenuPara = 1;
            IsNeedChangePage = 1;
            Show_Page(pMenu->ChildMenuID);
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
void StandardMenu_UpKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    pMenuPara->RowIndex--;
    if (pMenuPara->RowIndex < 0)
    {
        pMenuPara->RowIndex = pMenuPara->NumOfRows - 1;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void StandardMenu_DownKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    pMenuPara->RowIndex++;
    if (pMenuPara->RowIndex >= pMenuPara->NumOfRows)
    {
        pMenuPara->RowIndex = 0;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void AdjustZeroFull_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex)
{
    int Index = 0;
    int DisplayBufIndex = 0;

    ClearBuf((unsigned char *)&LongCode[0], sizeof(LongCode));
    if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
    {
        pMenuPara->ColumnReverseSwitch = Disable;
        ClearBuf(&g_DisplayBuf[0], Display_Buf_Size);

        if (LineIndex == 0)
        {
            DisplayBufIndex= 16;
            //-��ǰλ��#xxxx-
            LongCode[Index++] = 0xB5B1;
            LongCode[Index++] = 0xC7B0;
            LongCode[Index++] = 0xCEBB;
            LongCode[Index++] = 0xD6C3;
            LongCode[Index++] = 0x23;
            ShowBlock(&LongCode[0], Index, DisplayBufIndex);

            DisplayBufIndex += 72;
            ShowNumbers(Valve.MiscInfo.PositionValue, 5, 0, DisplayBufIndex, 1);
        }
        else if (LineIndex == 1)
        {
            DisplayBufIndex= 0;
            //-������ť������λ-
            LongCode[Index++] = 0xB2D9;
            LongCode[Index++] = 0xD7F7;
            LongCode[Index++] = 0xD0FD;
            LongCode[Index++] = 0xC5A5;
            LongCode[Index++] = 0xBFC9;
            LongCode[Index++] = 0xCEDE;
            LongCode[Index++] = 0xCFDE;
            LongCode[Index++] = 0xCEBB;
            ShowBlock(&LongCode[0], Index, DisplayBufIndex);
        }
        else if (LineIndex == 2)
        {
            DisplayBufIndex= 0;
            //-��������λ�ã���-
            LongCode[Index++] = 0xB5F7;
            LongCode[Index++] = 0xD5FB;
            LongCode[Index++] = 0xB7A7;
            LongCode[Index++] = 0xC3C5;
            LongCode[Index++] = 0xCEBB;
            LongCode[Index++] = 0xD6C3;
            LongCode[Index++] = 0xA3AC;
            LongCode[Index++] = 0xB7B5;
            ShowBlock(&LongCode[0], Index, DisplayBufIndex);
        }
        else if (LineIndex == 3)
        {
            DisplayBufIndex= 0;
            //-��ֹͣλ��ɱ궨-
            LongCode[Index++] = 0xBBD8;
            LongCode[Index++] = 0xCDA3;
            LongCode[Index++] = 0xD6B9;
            LongCode[Index++] = 0xCEBB;
            LongCode[Index++] = 0xCDEA;
            LongCode[Index++] = 0xB3C9;
            LongCode[Index++] = 0xB1EA;
            LongCode[Index++] = 0xB6A8;
            ShowBlock(&LongCode[0], Index, DisplayBufIndex);
        }
    }
    else
    {
        pMenuPara->ColumnReverseSwitch = Enable;

        DisplayBufIndex = 88;
        if (LineIndex == 0)
        {
            ShowNumbers(Valve.MiscInfo.PositionValue, 5, 0, DisplayBufIndex, 1);
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
void AdjustZeroFull_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (pMenuPara->ColumnIndex == 3)    //-ȷ��-
    {
        if (PageFunctionIndex == Page_AdjustFull_ID)
        {
            Valve.Adjust.Adjust0.Adjust0Bits.Full = 1;
            Valve.Adjust.Adjust0.Adjust0Bits.Zero = 0;

            CurrentAdjustType = Frame_AdjustFull;
        }
        else
        {
            Valve.Adjust.Adjust0.Adjust0Bits.Full = 0;
            Valve.Adjust.Adjust0.Adjust0Bits.Zero = 1;

            CurrentAdjustType = Frame_AdjustZero;
        }

        F_WaitAdjustFrame   = 1;

        SetTimer(WaitReplyTimer, WaitReply_Delay);
    }
    else
    {
        if (pMenu->ParentMenuID != InvalidMenuID)
        {
            IsNeedChangePage = 1;
            Show_Page(pMenu->ParentMenuID);
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
void AdjustZeroFull_UpKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    pMenuPara->ColumnIndex += 6;
    if (pMenuPara->ColumnIndex > 9)
    {
        pMenuPara->ColumnIndex = 3;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void AdjustZeroFull_DownKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    pMenuPara->ColumnIndex -= 6;
    if (pMenuPara->ColumnIndex < 3)
    {
        pMenuPara->ColumnIndex = 9;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void LocalMode_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex)
{
    int DisplayBufIndex = 0;

    if ((LineIndex == 0) && (Device.Para.LocalMode == LocalMode_Jog))
    {
        DisplayBufIndex = 96;
        ShowStar(DisplayBufIndex);
    }
    else if ((LineIndex == 1) && (Device.Para.LocalMode == LocalMode_Hold))
    {
        DisplayBufIndex = 96;
        ShowStar(DisplayBufIndex);
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void LocalMode_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (pMenuPara->RowIndex == 0)    //-�㶯-
    {
        Device.Para.LocalMode = LocalMode_Jog;
    }
    else if (pMenuPara->RowIndex == 1)    //-����-
    {
        Device.Para.LocalMode = LocalMode_Hold;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void RemoteIOMode_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex)
{    
    int DisplayBufIndex = 0;

    if ((LineIndex == 0) )
    {
        if (Device.Para.RemoteIOMode == RemoteIOMode_Jog)
        {
            DisplayBufIndex = 96;
            ShowStar(DisplayBufIndex);
        }
    }
    else if ((LineIndex == 1))
    {
        if (Device.Para.RemoteIOMode == RemoteIOMode_HoldNormallyOpen)
        {
            DisplayBufIndex = 112;
            ShowStar(DisplayBufIndex);
        }
    }
    else if ((LineIndex == 2) )
    {
        if (Device.Para.RemoteIOMode == RemoteIOMode_HoldNormallyShut)
        {
            DisplayBufIndex = 112;
            ShowStar(DisplayBufIndex);
        }
    }
    else if ((LineIndex == 3) )
    {
        if (Device.Para.RemoteIOMode == RemoteIOMode_SignalOnNoOff)
        {
            DisplayBufIndex = 96;
            ShowStar(DisplayBufIndex);
        }
    }
    else if ((LineIndex == 4) )
    {
        if (Device.Para.RemoteIOMode == RemoteIOMode_SignalOffNoOn)
        {
            DisplayBufIndex = 96;
            ShowStar(DisplayBufIndex);
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
void RemoteIOMode_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{    
    if (pMenuPara->RowIndex == 0)
    {
        Device.Para.RemoteIOMode = RemoteIOMode_Jog;
    }
    else if (pMenuPara->RowIndex == 1)
    {
        Device.Para.RemoteIOMode = RemoteIOMode_HoldNormallyOpen;
    }
    else if (pMenuPara->RowIndex == 2)
    {
        Device.Para.RemoteIOMode = RemoteIOMode_HoldNormallyShut;
    }
    else if (pMenuPara->RowIndex == 3)
    {
        Device.Para.RemoteIOMode = RemoteIOMode_SignalOnNoOff;
    }
    else if (pMenuPara->RowIndex == 4)
    {
        Device.Para.RemoteIOMode = RemoteIOMode_SignalOffNoOn;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void RemoteANMode_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex)
{    
    int DisplayBufIndex = 0;

    if ((LineIndex == 1) && (Device.Para.RemoteANMode == RemoteANMode_NoSigKeep))
    {
        DisplayBufIndex = 64;
        ShowStar(DisplayBufIndex);
    }
    else if ((LineIndex == 2) && (Device.Para.RemoteANMode == RemoteANMode_NoSigShut))
    {
        DisplayBufIndex = 64;
        ShowStar(DisplayBufIndex);
    }
    else if ((LineIndex == 3) && (Device.Para.RemoteANMode == RemoteANMode_NoSigMid))
    {
        DisplayBufIndex = 64;
        ShowStar(DisplayBufIndex);
    }
    else if ((LineIndex == 4) && (Device.Para.RemoteANMode == RemoteANMode_NoSigOpen))
    {
        DisplayBufIndex = 64;
        ShowStar(DisplayBufIndex);
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void RemoteANMode_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (pMenuPara->RowIndex == 1)
    {
        Device.Para.RemoteANMode = RemoteANMode_NoSigKeep;
    }
    else if (pMenuPara->RowIndex == 2)
    {
        Device.Para.RemoteANMode = RemoteANMode_NoSigShut;
    }
    else if (pMenuPara->RowIndex == 3)
    {
        Device.Para.RemoteANMode = RemoteANMode_NoSigMid;
    }
    else if (pMenuPara->RowIndex == 4)
    {
        Device.Para.RemoteANMode = RemoteANMode_NoSigOpen;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void DeadZone_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex)
{
    unsigned int Code[5] = {0};

    unsigned int  DeadZone = 0;
    int DisplayBufIndex = 0;
    

    if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
    {
        ClearBuf(&g_DisplayBuf[0], sizeof(g_DisplayBuf));

        pMenuPara->RowIndex = 1;
        pMenuPara->ColumnIndex = 6;
        pMenuPara->ColumnReverseUnit = 5;
    }
    else 
    {
        pMenuPara->RowIndex = 2;
        pMenuPara->ColumnIndex = 4;
        pMenuPara->ColumnReverseUnit = 8;
    }    

    if (LineIndex == 1)
    {
        //-��Χ:1~20-
        if (Device.Para.DeadZone <= 20)
        {
            //-DeadZone = Device.Para.DeadZone * 5-
            DeadZone = (Device.Para.DeadZone << 2) + Device.Para.DeadZone;
        }

        if (DeadZone < 5)
        {
            //-����Ӧ-
            Code[0] = 0xD7D4;
            Code[1] = 0xCACA;
            Code[2] = 0xD3A6;
            DisplayBufIndex = 48;
    
            ShowBlock(&Code[0], 5, DisplayBufIndex);
        }
        else
        {
            DisplayBufIndex = 48;
            ShowNumbers(DeadZone, 3, 1, DisplayBufIndex, 0);
            
            //-%-
            DisplayBufIndex += 32;
            Code[0] = 0x25;
            ShowBlock(&Code[0], 1, DisplayBufIndex);
        }
    }
    else
    {
        if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
        {
            ShowInc_Dec(LineIndex);
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
void DeadZone_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    Device.Para.DeadZone++;

    if (Device.Para.DeadZone > 20)
    {
        Device.Para.DeadZone = 0;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void DeadZone_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (Device.Para.DeadZone <= 0)
    {
        Device.Para.DeadZone = 20;  
    }
    else
    {
        Device.Para.DeadZone--;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void Password_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex)
{
    int i = 0;
    int DisplayBufIndex = 0;
    unsigned int Code = 0;
    unsigned char FontBuf[32] = {0};
    unsigned char FontBufLen = 0;

    if (LineIndex == 2)
    {
        DisplayBufIndex = 4 << EN_X_SIZE_Shift_Count;
        for (i = 0; i < 4; i++)
        {
            Code = Password[i] + 0xA3B0;
            GetDotData(Code, EN_Character, &FontBuf[0], &FontBufLen);
            SwapFontBuf2DisplayBuf(FontBuf, FontBufLen, &g_DisplayBuf[0], DisplayBufIndex);
    
            DisplayBufIndex += EN_X_SIZE + EN_X_SIZE;
        }
    }
    else if (LineIndex == 3)
    {
        if (PasswordErrorCount == 0)
        {
            ClearBuf(&g_DisplayBuf[0], Display_Buf_Size);
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
void Password_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    MenuPara_Password.ColumnIndex += 2;

    //-��Χ[4,6,8,10]-
    if (MenuPara_Password.ColumnIndex > 10)
    {
        MenuPara_Password.ColumnIndex = 4;  

        if ((Password[0] == PasswordByte_0) && (Password[1] == PasswordByte_1) &&  
            (Password[2] == PasswordByte_2) && (Password[3] == PasswordByte_3))        
        {
            if (pMenu->ChildMenuID != 0)
            {
                IsNeedChangePage = 1;
                NeedResetMenuPara = 1;
                Show_Page(pMenu->ChildMenuID);
 
                PasswordErrorCount = 0;
                ClearBuf(&Password[0], 4);
                 
                return;
            }
        }
        else 
        {
            PasswordErrorCount++;

            //-3�δ���ֱ���˳����ò˵�,����������-
            if (PasswordErrorCount >= 3)
            {
                IsNeedChangePage = 1;
                Show_Page(Page_Normal_ID);

                PasswordErrorCount = 0;
                ClearBuf(&Password[0], 4);

                return;
            }
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
void Password_UpKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    MenuPara_Password.ColumnIndex -= 2;

    //-��Χ[4,6,8,10]-
    if (MenuPara_Password.ColumnIndex < 4)
    {
        MenuPara_Password.ColumnIndex = 10;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void Password_DownKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    MenuPara_Password.ColumnIndex += 2;

    //-��Χ[4,6,8,10]-
    if (MenuPara_Password.ColumnIndex > 10)
    {
        MenuPara_Password.ColumnIndex = 4;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void Password_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    int Index = 0;

    if (MenuPara_Password.ColumnIndex == 4)
    {
        Index = 0;
    }
    else if (MenuPara_Password.ColumnIndex == 6)
    {
        Index = 1;
    }
    else if (MenuPara_Password.ColumnIndex == 8)
    {
        Index = 2;
    }
    else 
    {
        Index = 3;
    }

    Password[Index]++;
    if (Password[Index] >= 10)
    {
        Password[Index] = 0;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void Password_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    int Index = 0;

    if (MenuPara_Password.ColumnIndex == 4)
    {
        Index = 0;
    }
    else if (MenuPara_Password.ColumnIndex == 6)
    {
        Index = 1;
    }
    else if (MenuPara_Password.ColumnIndex == 8)
    {
        Index = 2;
    }
    else
    {
        Index = 3;
    }

    if (Password[Index] <= 0)
    {
        Password[Index] = 10;
    }

    Password[Index]--;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void AdjustOutput4_20mA_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex)
{
    int Index  = 0;
    int DisplayBufIndex = 0;

    if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
    {
        ClearBuf(&g_DisplayBuf[0], sizeof(g_DisplayBuf));
        pMenuPara->ColumnReverseSwitch = Disable;

        if (LineIndex == 1)
        {
            DisplayBufIndex= 0;
            //-��ֹͣλ�����ϼ�-
            LongCode[Index++] = 0xD6C1;
            LongCode[Index++] = 0xCDA3;
            LongCode[Index++] = 0xD6B9;
            LongCode[Index++] = 0xCEBB;
            LongCode[Index++] = 0xB7B5;
            LongCode[Index++] = 0xBBD8;
            LongCode[Index++] = 0xC9CF;
            LongCode[Index++] = 0xBCB6;
            ShowBlock(&LongCode[0], Index, DisplayBufIndex);
        }
        else 
        {
            ShowInc_Dec(LineIndex);
        }
    }
    else
    {
        pMenuPara->ColumnReverseSwitch = Enable;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void AdjustOutput4mA_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    Valve.Adjust.Adjust1.Adjust1Bits.IncOutput4mA = 1;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void AdjustOutput4mA_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    Valve.Adjust.Adjust1.Adjust1Bits.DecOutput4mA = 1;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void AdjustOutput20mA_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    Valve.Adjust.Adjust1.Adjust1Bits.IncOutput20mA = 1;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void AdjustOutput20mA_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    Valve.Adjust.Adjust1.Adjust1Bits.DecOutput20mA = 1;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void AdjustInput4_20mA_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex)
{
    unsigned int Code[4] = {0};

    int DisplayBufIndex = 0;
  
    unsigned char DummyData = 0;
    unsigned char GeValue = 0;
    unsigned char ShiValue = 0;
    unsigned char BaiValue = 0;

    GetSingleNumber(Valve.MiscInfo.In4_20mAADValue, &GeValue, &ShiValue, &BaiValue, &DummyData, &DummyData);
    if (BaiValue == 0)
    {
        Code[0] = 0xA3A0;               //-�ո�-
    }
    else
    {
        Code[0] = BaiValue + 0xA3B0;
    }

    Code[1] = ShiValue + 0xA3B0;
    Code[2] = 0xA3AE;                   //-С����-
    Code[3] = GeValue + 0xA3B0;

    DisplayBufIndex = 48;
    ShowBlock(&Code[0], 4, DisplayBufIndex);
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void AdjustInput4_20mA_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (pMenuPara->ColumnIndex == 3)    //-ȷ��-
    {
        if (PageFunctionIndex == Page_AdjustInput20mA_ID)
        {
            Valve.Adjust.Adjust1.Adjust1Bits.Input20mA = 1;
            Valve.Adjust.Adjust1.Adjust1Bits.Input4mA = 0;

            CurrentAdjustType = Frame_AdjustInput20mA;
        }
        else
        {
            Valve.Adjust.Adjust1.Adjust1Bits.Input4mA = 1;
            Valve.Adjust.Adjust1.Adjust1Bits.Input20mA = 0;

            CurrentAdjustType = Frame_AdjustInput4mA;
        }

        F_WaitAdjustFrame   = 1;

        SetTimer(WaitReplyTimer, WaitReply_Delay);
            
    }
    else                                //-����-
    {
        if (pMenu->ParentMenuID != InvalidMenuID)
        {
            IsNeedChangePage = 1;
            Show_Page(pMenu->ParentMenuID);
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
void AdjustInput4_20mA_UpKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    pMenuPara->ColumnIndex += 6;
    if (pMenuPara->ColumnIndex > 9)
    {
        pMenuPara->ColumnIndex = 3;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void AdjustInput4_20mA_DownKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    pMenuPara->ColumnIndex -= 6;
    if (pMenuPara->ColumnIndex < 3)
    {
        pMenuPara->ColumnIndex = 9;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void ShutCurrent_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex)
{
    int Index = 0;
    int DisplayBufIndex = 0;

    if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
    {
        ClearBuf(&g_DisplayBuf[0], sizeof(g_DisplayBuf));

        pMenuPara->RowIndex = 1;
        pMenuPara->ColumnIndex = 6;
        pMenuPara->ColumnReverseUnit = 5;
    }
    else 
    {
        pMenuPara->RowIndex = 2;
        pMenuPara->ColumnIndex = 4;
        pMenuPara->ColumnReverseUnit = 8;
    } 

    if (LineIndex == 1)
    {
        DisplayBufIndex = 48;
        if (Device.Para.CurrentDecimalBits == CurrentDecimalBits_One)
        {
            ShowNumbers(Device.Para.MaxShutCurrent, 3, 1, DisplayBufIndex, 1);
        }
        else
        {
            ShowNumbers(Device.Para.MaxShutCurrent, 3, 2, DisplayBufIndex, 1);
        }

        ClearBuf((unsigned char *)&LongCode[0], sizeof(LongCode));
        DisplayBufIndex += 32;
        LongCode[Index++] = 0x41;         //-A-
        ShowBlock(&LongCode[0], Index, DisplayBufIndex);
    }
    else
    {
        if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
        {
            ShowInc_Dec(LineIndex);
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
void OpenCurrent_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex)
{
    int Index = 0;
    int DisplayBufIndex = 0;

    if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
    {
        ClearBuf(&g_DisplayBuf[0], sizeof(g_DisplayBuf));

        pMenuPara->RowIndex = 1;
        pMenuPara->ColumnIndex = 6;
        pMenuPara->ColumnReverseUnit = 5;
    }
    else 
    {
        pMenuPara->RowIndex = 2;
        pMenuPara->ColumnIndex = 4;
        pMenuPara->ColumnReverseUnit = 8;
    } 

    if (LineIndex == 1)
    {
        DisplayBufIndex = 48;
        if (Device.Para.CurrentDecimalBits == CurrentDecimalBits_One)
        {
            ShowNumbers(Device.Para.MaxOpenCurrent, 3, 1, 48, 1);
        }
        else
        {
            ShowNumbers(Device.Para.MaxOpenCurrent, 3, 2, 48, 1);
        }
    
        ClearBuf((unsigned char *)&LongCode[0], sizeof(LongCode));
        DisplayBufIndex += 32;
        LongCode[Index++] = 0x41;         //-A-
        ShowBlock(&LongCode[0], Index, DisplayBufIndex);
    }
    else
    {
        if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
        {
            ShowInc_Dec(LineIndex);
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
void MaxActionTime_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex)
{
    unsigned int Code[4] = {0};

    int DisplayBufIndex = 0;

    if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
    {
        ClearBuf(&g_DisplayBuf[0], sizeof(g_DisplayBuf));

        pMenuPara->RowIndex = 1;
        pMenuPara->ColumnIndex = 6;
        pMenuPara->ColumnReverseUnit = 6;
    }
    else 
    {
        pMenuPara->RowIndex = 2;
        pMenuPara->ColumnIndex = 4;
        pMenuPara->ColumnReverseUnit = 8;
    } 

    if (LineIndex == 1)
    {
        if (Device.Para.MaxActionTime < 5)
        {
            Code[0] = 0xB2BB;                  //-��-
            Code[1] = 0xCFDE;                  //-��-
            Code[2] = 0xCAB1;                  //-ʱ-
    
            DisplayBufIndex = 48;
            ShowBlock(&Code[0], 4, DisplayBufIndex);
        } 
        else
        {
            DisplayBufIndex = 56;
            ShowNumbers(Device.Para.MaxActionTime, 3, 0, DisplayBufIndex, 0);
            
            //-S-
            DisplayBufIndex += 24;
            Code[0] = 0x53;
            ShowBlock(&Code[0], 1, DisplayBufIndex);
        }
    }
    else 
    {
        if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
        {
            ShowInc_Dec(LineIndex);
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
void MaxActionTime_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (pMenu->ParentMenuID != InvalidMenuID)
    {

        IsNeedChangePage = 1;
        Show_Page(pMenu->ParentMenuID);
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void MaxActionTime_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    //-��Χ:0��[5~250]-
    if (Device.Para.MaxActionTime == 0)
    {
        Device.Para.MaxActionTime = 5;  
    }
    else if(Device.Para.MaxActionTime == 250)
    {
        Device.Para.MaxActionTime = 0;
    }
    else
    {
        Device.Para.MaxActionTime++;
    }  
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void MaxActionTime_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    //-��Χ:0��[5~250]-
    if (Device.Para.MaxActionTime == 0)
    {
        Device.Para.MaxActionTime = 250;  
    }
    else if(Device.Para.MaxActionTime == 5)
    {
        Device.Para.MaxActionTime = 0;
    }
    else
    {
        Device.Para.MaxActionTime--;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void ErrorFeedback_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex)
{
    if ((LineIndex == 1) && (Device.Para.ErrorFeedBack == IO_NormallyOpen))
    {
        ShowStar(96);
    }
    else if ((LineIndex == 2) && (Device.Para.ErrorFeedBack == IO_NormallyShut))
    {
        ShowStar(96);
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void ErrorFeedback_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (pMenuPara->RowIndex == 1)
    {
        Device.Para.ErrorFeedBack = IO_NormallyOpen;
    }
    else if (pMenuPara->RowIndex == 2)
    {
        Device.Para.ErrorFeedBack = IO_NormallyShut;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void ESDSetting_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex)
{
    if ((LineIndex == 0) && (Device.Para.ESDMode == ESDMode_Disable))
    {
        ShowStar(56);
    }
    else if ((LineIndex == 1) && (Device.Para.ESDMode == ESDMode_NoAction))
    {
        ShowStar(88);
    }
    else if ((LineIndex == 2) && (Device.Para.ESDMode == ESDMode_Open))
    {
        ShowStar(88);
    }
    else if ((LineIndex == 3) && (Device.Para.ESDMode == ESDMode_Shut))
    {
        ShowStar(88);
    }
    else if ((LineIndex == 4) && (Device.Para.ESDMode == ESDMode_Middle))
    {
        ShowStar(88);
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void ESDSetting_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (pMenuPara->RowIndex == 0)
    {
        Device.Para.ESDMode = ESDMode_Disable;
    }
    else if (pMenuPara->RowIndex == 1)
    {
        Device.Para.ESDMode = ESDMode_NoAction;
    }
    else if (pMenuPara->RowIndex == 2)
    {
        Device.Para.ESDMode = ESDMode_Open;
    }
    else if (pMenuPara->RowIndex == 3)
    {
        Device.Para.ESDMode = ESDMode_Shut;
    }
    else if (pMenuPara->RowIndex == 4)
    {
        Device.Para.ESDMode = ESDMode_Middle;
    }
}


/*******************************************************************************
* ��������:    KeyProc
* ��������:    ������
* �������:    
* �������:    ��
* �� �� ֵ:    0,����Ҫ�ػ�ҳ��
*              1,��Ҫ�ػ�ҳ��
*******************************************************************************/
unsigned char KeyProc(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    int i = 0;
    unsigned char IsKeyPressed = 0;
    unsigned char KeyValid[VKey_Num] = {0};

    if ((Device.CommMode.CommModeBits.Local == 1) || (Device.CommMode.CommModeBits.Remote == 1))
    {
        return 0; 
    }
    if (IsInMenu() == 0)
    {
        if (PageFunctionIndex == Page_Normal_ID)
        {
            if ((KeyPressed[Key_Set] == KEY_PRESSED) && (PressedType[Key_Set] == LONG_KEY))
            {
                EnterMenu(1);
            }
            else if ((KeyPressed[Key_Shut] == KEY_PRESSED) && (PressedType[Key_Shut] == LONG_KEY))
            {
                EnterMenu(1);
            }
            else
            {
                return 0;
            }
        } 
        else
        {
            return  0;
        }
    }


    /*-Keys of IR Remoter-*/
    for (i = 0; i < VKey_Num; i++)
    {
        if (KeyPressed[Key_Inc + i] == KEY_PRESSED)
        {
            KeyValid[i] = 1;
        }
    }
	if (pMenuPara->IsMultiplex == Multiplex_Adjust)
	{
        //-������ֱ���иߵ͵�ƽ-
		if (KeyStateRead(Key_Inc) == KEY_PRESSED)
		{
			KeyValid[VKey_Inc] = 1;
		}
        //-������ֱ���иߵ͵�ƽ-
        if (KeyStateRead(Key_Dec) == KEY_PRESSED)
        {
			KeyValid[VKey_Dec] = 1;
        }
	}


    /*-Keys of the Board-*/
    if (pMenuPara->IsMultiplex == Multiplex_Digit)
    {
        if (KeyPressed[Key_Open] == KEY_PRESSED)
        { 
            KeyValid[VKey_Inc] = 1;
        }
        if (KeyPressed[Key_Shut] == KEY_PRESSED)
        { 
            KeyValid[VKey_Dec] = 1;
        }   
    }
    else if (pMenuPara->IsMultiplex == Multiplex_Adjust)
    {
        //-������ֱ���иߵ͵�ƽ-
        if(KeyStateRead(Key_Shut) == KEY_PRESSED)
        { 
            KeyValid[VKey_Dec] = 1;
        }
        //-������ֱ���иߵ͵�ƽ-
        if (KeyStateRead(Key_Open) == KEY_PRESSED)
        { 
            KeyValid[VKey_Inc] = 1;
        }
    }
    else
    {
        if (KeyPressed[Key_Shut] == KEY_PRESSED)
        {
            KeyValid[VKey_Set] = 1;
        }
        if (KeyPressed[Key_Open] == KEY_PRESSED)
        {
            KeyValid[VKey_Down] = 1;
        }
    }


    if (KeyValid[VKey_Set] == 1)
    {
        IsKeyPressed = 1;
        pMenu[pMenuPara->RowIndex].SetKeyDeal(&pMenu[pMenuPara->RowIndex], pMenuPara);
    }
    if (KeyValid[VKey_Back] == 1)
    {
        IsKeyPressed = 1;
        StandardMenu_Back2Parent(pMenu, pMenuPara);
    }
    if (KeyValid[VKey_Up] == 1)
    {
        IsKeyPressed = 1;
        pMenu[pMenuPara->RowIndex].UpKeyDeal(&pMenu[pMenuPara->RowIndex], pMenuPara);
    }
    if (KeyValid[VKey_Down] == 1)
    {
        IsKeyPressed = 1;
        pMenu[pMenuPara->RowIndex].DownKeyDeal(&pMenu[pMenuPara->RowIndex], pMenuPara);
    }
    if (KeyValid[VKey_Inc] == 1)
    {
        IsKeyPressed = 1;
        pMenu[pMenuPara->RowIndex].IncKeyDeal(&pMenu[pMenuPara->RowIndex], pMenuPara);
    }
    if (KeyValid[VKey_Dec] == 1)
    {
        IsKeyPressed = 1;
        pMenu[pMenuPara->RowIndex].DecKeyDeal(&pMenu[pMenuPara->RowIndex], pMenuPara);
    }

    if (IsKeyPressed == 1)
    {
        SetTimer(NoKeyTimer, NoKey_Delay);
    }
    if (IsNeedChangePage == 1)
    {
        IsNeedChangePage = 0;

        return 1;
    }

    return 0;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void Page_Draw(const MenuStructure *pMenu, MenuPara      *pMenuPara)
{
    int i = 0;
    int j = 0;
    int TempData = 0;
    unsigned char NeedChangePage = 0;

    int MenuNameIndex   = 0;
    int DisplayBufIndex = 0;

    int           StartRowIndex = 0;
    unsigned int  Code = 0;
    unsigned char CharacterType = 0;


    unsigned char FontBuf[32] = {0x00};
    unsigned char FontBufLen = 0;


    if (NeedResetMenuPara == 1)
    {
        NeedResetMenuPara = 0;
        (pMenuPara->Reset)(pMenuPara);
    }

    NeedChangePage = KeyProc(pMenu, pMenuPara);
    if (NeedChangePage == 1)
    {
        NeedChangePage = 0;
        return;
    }
 
    TempData = pMenuPara->RowIndex;   //-�Ƴ���ᵼ�����-
    TempData >>= Items_Per_Page_Shif_Count;
    StartRowIndex = TempData;         //-����ÿҳ�����˵���-
    StartRowIndex <<= Items_Per_Page_Shif_Count;
    for (i = 0; i < Items_Per_Page; i++)
    {
        ClearBuf(&g_DisplayBuf[0], Display_Buf_Size);

        DisplayBufIndex = 0;
        MenuNameIndex = 0;
        if (StartRowIndex + i < pMenuPara->NumOfRows)
        {
            for (j = 0; j < 16;     )    //-��j++�ŵ�ѭ������-
            {
                ClearBuf(&FontBuf[0], sizeof(FontBuf));

                Code = pMenu[StartRowIndex + i].MenuName[Device.Para.LanguageType][MenuNameIndex++];
                if (((Code >> 8) & 0xFF) >= 0xB0)    //-�����ַ�-
                {
                    CharacterType = CN_Character;
          
                    j += 2;             //!!!!j��ѭ�������ۼ�
                }
                else
                {
                    CharacterType = EN_Character;

                    j += 1;             //!!!!j��ѭ�������ۼ�
                }
                GetDotData(Code, CharacterType, &FontBuf[0], &FontBufLen);
    
                SwapFontBuf2DisplayBuf(FontBuf, FontBufLen, &g_DisplayBuf[0], DisplayBufIndex);

                if (CharacterType == CN_Character)    //-�����ַ�-
                {
                    DisplayBufIndex += CN_X_SIZE;
                }
                else
                {
                    DisplayBufIndex += EN_X_SIZE;
                }            
            }


            if (pMenu[StartRowIndex + i].SpecialDeal != Dummy_Special)
            {
                pMenu[StartRowIndex + i].SpecialDeal(&pMenu[StartRowIndex + i], pMenuPara, StartRowIndex + i);

                if (IsNeedChangePage == 1) 
                {
                    IsNeedChangePage = 0;
                
                    return;
                }
            }

            
            /*-����-*/
            if (pMenuPara->RowReverseSwitch == Enable)
            {
                if (StartRowIndex + i == pMenuPara->RowIndex)
                {
                    for (j = 0; j < Display_Buf_Size; j++)
                    {
                        g_DisplayBuf[j] ^= 0xFF;
                    }
                }
            }
            if (pMenuPara->ColumnReverseSwitch == Enable)
            {
                if (StartRowIndex + i == pMenuPara->RowIndex)
                {
                    TempData = pMenuPara->ColumnIndex << 3;
                    for (j = 0; j < (pMenuPara->ColumnReverseUnit << 3); j++)
                    {
                        g_DisplayBuf[TempData + j] ^= 0xFF;
                        g_DisplayBuf[TempData + j + 128] ^= 0xFF;
                    }
                }
            }
        }

        LcdRefresh(i);        
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
unsigned char IsInMenu(void)
{
    if (Device.Flag.FlagBits.IsInMenu == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void EnterMenu(unsigned char Flag)
{
    if (Flag == 0)
    {
        Device.Flag.FlagBits.IsInMenu = 0;
    }
    else
    {
        Device.Flag.FlagBits.IsInMenu = 1;
    }
    return;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void DisplayInit(void)
{

    PageFunctionIndex = Page_Normal_ID;

    InsertTimer(&NoKeyTimer);
    InsertTimer(&AdjustInfoTimer);
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void Task_Display(void)
{
    static unsigned char HasDone = 0;

    if (IsTimeOut(PowerOnTimer) != 1)
    {
        Show_Page(Page_PowerOn_ID);
    }
    else 
    {
        if (HasDone == 0)
        {
            HasDone = 1;

            DeleteTimer(&PowerOnTimer);
            Show_Page(Page_Normal_ID);
        }
    }

    if (IsInMenu() == 1)
    {
        if (IsTimeOut(NoKeyTimer) == 1)
        {
            Show_Page(Page_Normal_ID);
        }
    }
    else if (IsInMenu() == 0)
    {
        Show_Page(Page_Normal_ID);
    }

    if (IsTimeOut(AdjustInfoTimer) == 1)
    {
        StopTimer(AdjustInfoTimer);
        if ((CurrentAdjustType == Frame_AdjustZero) || (CurrentAdjustType == Frame_AdjustFull))
        {
            Show_Page(Page_MainMenu_ID);
        }
        else if ((CurrentAdjustType == Frame_AdjustInput4mA) || (CurrentAdjustType == Frame_AdjustInput20mA))
        {
            Show_Page(Page_InternalPara_ID);
        }
    }

    switch(PageFunctionIndex)
    {
    //--
    case Page_PowerOn_ID:
        Page_Draw(&Menu_PowerOn[0], &MenuPara_PowerOn);   
        break;
    //--
    case Page_Normal_ID: 
        EnterMenu(0);
        SetFrameType(Normal_Frame);
        Page_Draw(&Menu_Normal[0], &MenuPara_Normal); 
        break;
    //--
    case Page_MainMenu_ID:
        Page_Draw(&Menu_Main[0], &MenuPara_Main);
        break;
    //--
    case Page_AdjustZero_ID:
        SetFrameType(Adjust_Frame);
        Page_Draw(&Menu_AdjustZero[0], &MenuPara_AdjustZero);
        break;
    //--
    case Page_AdjustZeroInfo_ID:
        Page_Draw(&Menu_AdjustZeroInfo[0], &MenuPara_AdjustZeroInfo);
        break;
    //--
    case Page_AdjustFull_ID: 
        SetFrameType(Adjust_Frame);           
        Page_Draw(&Menu_AdjustFull[0], &MenuPara_AdjustFull);
        break;
    //--
    case Page_AdjustFullInfo_ID:
        Page_Draw(&Menu_AdjustFullInfo[0], &MenuPara_AdjustFullInfo);
        break;
    //--
    case Page_LocalMode_ID:;
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_LocalMode[0], &MenuPara_LocalMode);
        break;
    //--
    case Page_RemoteIOMode_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_RemoteIOMode[0], &MenuPara_RemoteIOMode);
        break;
    //--
    case Page_RemoteANMode_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_RemoteANMode[0], &MenuPara_RemoteANMode);
        break;
    //--
    case Page_DeadZone_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_DeadZone[0], &MenuPara_DeadZone);
        break;
    //--
    case Page_Password_ID:
        Page_Draw(&Menu_Password[0], &MenuPara_Password);
        break;
    //--
    case Page_InternalPara_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_InternalPara[0], &MenuPara_InternalPara);
        break;
    //--
    case Page_AdjustOutput4mA_ID:
        SetFrameType(Adjust_Frame);
        Page_Draw(&Menu_AdjustOutput4mA[0], &MenuPara_AdjustOutput4mA);
        break;
    //--
    case Page_AdjustOutput20mA_ID:
        SetFrameType(Adjust_Frame);
        Page_Draw(&Menu_AdjustOutput20mA[0], &MenuPara_AdjustOutput20mA);
        break;
    //--
    case Page_AdjustInput4mA_ID:
        SetFrameType(Adjust_Frame);
        Page_Draw(&Menu_AdjustInput4mA[0], &MenuPara_AdjustInput4mA);
        break;
    //--
    case Page_AdjustInput4mAInfo_ID:
        Page_Draw(&Menu_AdjustInput4mAInfo[0], &MenuPara_AdjustInput4mAInfo);
        break;
    //--
    case Page_AdjustInput20mA_ID:
        SetFrameType(Adjust_Frame);
        Page_Draw(&Menu_AdjustInput20mA[0], &MenuPara_AdjustInput20mA);
        break;
    //--
    case Page_AdjustInput20mAInfo_ID:
        Page_Draw(&Menu_AdjustInput20mAInfo[0], &MenuPara_AdjustInput20mAInfo);
        break;
    //--
    case Page_ShutCurrent_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_ShutCurrent[0], &MenuPara_ShutCurrent);
        break;
    //--
    case Page_OpenCurrent_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_OpenCurrent[0], &MenuPara_OpenCurrent);
        break;
    //--
    case Page_MaxActionTime_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_MaxActionTime[0], &MenuPara_MaxActionTime);
        break;
    //--
    case Page_ErrorFeedback_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_ErrorFeedback[0], &MenuPara_ErrorFeedback);
        break;
    //--
    case Page_ESDSetting_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_ESDSetting[0], &MenuPara_ESDSetting);
        break;
    //--
    default:    //-Ĭ�ϳ���-
        Page_Draw(&Menu_Normal[0], &MenuPara_Normal); 
        break;
    }
}

/*************************************END OF FILE*******************************/

