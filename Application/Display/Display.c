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
//-��������,��xx������-
#define PasswordGroupNum    3 
//-ÿ������ĸ���-  
#define PasswordNum         4

#define MaxLockTime         0xFFFF
#define MinLockTime         0x00

//-Ϊ����д����-
#define ClearLongCode()     (ClearBuf((unsigned char *)&LongCode[0], sizeof(LongCode)))
#define Space               GBK_Space

//-���ⰴ��,�԰����ķ�װ,V��Virtual-
typedef enum
{
   VKey_Inc,
   VKey_Dec,
   VKey_Set,
   VKey_Back,
   VKey_Up, 
   VKey_Down,
   VKey_Num,
   VKey_None  
}VKey_TypeDef;

/*******************************************************************************
*                                 ȫ�ֺ���(����)����
********************************************************************************/
unsigned char PageFunctionIndex = Page_PowerOn_ID;
  
unsigned char NeedResetMenuPara = 0;

unsigned char PasswordErrorCount = 0;
unsigned char InputPassword[PasswordNum] = {0};
const unsigned char ConstPassword[PasswordGroupNum][PasswordNum] = {{1, 0, 0, 0}, {1, 2, 3, 4}, {8, 8, 8, 8}};
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
    //-��λ�쳣(ERR-POS)-
    {{0xB7A7, 0xCEBB, 0xD2EC, 0xB3A3},  {0x45, 0x52, 0x52, 0x2D, 0x50, 0x4F, 0x53}},
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
    //-��Դȱ��(ERR-PWR)-
    {{0xB5E7, 0xD4B4, 0xC8B1, 0xCFE0}, {0x45, 0x52, 0x52, 0x2D, 0x50, 0x57, 0x52}}
    
};
const unsigned short int SlightErrorArray[8][2][16] = 
{
    //-�������(ERR-TCP)-
    {{0xB9D8, 0xCFF2, 0xB9FD, 0xBED8}, {0x45, 0x52, 0x52, 0x2D, 0x54, 0x43, 0x50}},
    //-�������(ERR-TOP)-
    {{0xBFAA, 0xCFF2, 0xB9FD, 0xBED8}, {0x45, 0x52, 0x52, 0x2D, 0x54, 0x4F, 0x50}}, 
    //-����ʱ(ERR-CLT)-
    {{0xB9D8, 0xCFF2, 0xB3AC, 0xCAB1}, {0x45, 0x52, 0x52, 0x2D, 0x43, 0x4C, 0x54}},
    //-����ʱ(ERR-OLT)-
    {{0xBFAA, 0xCFF2, 0xB3AC, 0xCAB1}, {0x45, 0x52, 0x52, 0x2D, 0x4F, 0x4C, 0x54}},
    //-�������(ERR-COC)-
    {{0xB9D8, 0xCFF2, 0xB9FD, 0xC1F7}, {0x45, 0x52, 0x52, 0x2D, 0x43, 0x4F, 0x43}},
    //-�������(ERR-OOC)-
    {{0xBFAA, 0xCFF2, 0xB9FD, 0xC1F7}, {0x45, 0x52, 0x52, 0x2D, 0x4F, 0x4F, 0x43}},
    //-�������(ERR-CLD)-
    {{0xB9D8, 0xCFF2, 0xB4ED, 0xCEF3}, {0x45, 0x52, 0x52, 0x2D, 0x43, 0x4C, 0x44}},
    //-�������(ERR-OPD)-
    {{0xBFAA, 0xCFF2, 0xB4ED, 0xCEF3}, {0x45, 0x52, 0x52, 0x2D, 0x4F, 0x50, 0x44}},
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
*11������ֻ��Ե�ǰ��������Ч,�ʰ���������Ӧ��ȷ��ֵ���˵���.
*12���˵�ģ��
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
  //-��ӭʹ��(Welcome to use)-
  {0,             InvalidMenuID,           InvalidMenuID,         {{Space, Space, Space, Space, 0xBBB6, 0xD3AD, 0xCAB9, 0xD3C3,  Space, Space, Space, Space}, {Space, 0x57, 0x65, 0x6C, 0x63, 0x6F, 0x6D, 0x65, 0x20, 0x74, 0x6F, 0x20, 0x75, 0x73, 0x65}},
  Dummy_Special,  StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-�綯����ִ�л���(valve actuator)-
  {1,             InvalidMenuID,           InvalidMenuID,         {{0xB5E7, 0xB6AF, 0xB7A7, 0xC3C5, 0xD6B4, 0xD0D0, 0xBBFA, 0xB9B9}, {Space, 0x76, 0x61, 0x6C, 0x76, 0x65, 0x20, 0x61, 0x63, 0x74, 0x75, 0x61, 0x74, 0x6F, 0x72}},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {2,            InvalidMenuID,           InvalidMenuID,          {0},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-�汾V1.2(Version1.2)-
  {3,            InvalidMenuID,           InvalidMenuID,          {{Space, Space, 0xB0E6, 0xB1BE, 0xA3BA, Space, 0xA3D6, 0xA3B1, 0xA3AE, 0xA3B2, Space, Space, Space, Space}, {Space, Space, Space, 0x56, 0x65, 0x72, 0x73, 0x69, 0x6F, 0x6E, 0x31, 0x2E, 0x32}},
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
  //-�궨ȫ��(Zero Set)-
  {0,             Page_Normal_ID,          Page_AdjustZero_ID,    {{0xB1EA, 0xB6A8, 0xC8AB, 0xB9D8}, {0x5A, 0x65, 0x72, 0x6F, 0x20, 0x53, 0x65, 0x74}},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-�궨ȫ��(Full Set)-
  {1,             InvalidMenuID,           Page_AdjustFull_ID,    {{0xB1EA, 0xB6A8, 0xC8AB, 0xBFAA}, {0x46, 0x75, 0x6C, 0x6C, 0x20, 0x53, 0x65, 0x74}},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
  
  //-�ֳ����Ʒ�ʽ(Local Mode)-
  {2,             InvalidMenuID,           Page_LocalMode_ID,     {{0xCFD6, 0xB3A1, 0xBFD8, 0xD6C6, 0xB7BD, 0xCABD}, {0x4C, 0x6F, 0x63, 0x61, 0x6C, 0x20, 0x4D, 0x6F, 0x64, 0x65}},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
  
  //-Զ�̿���������(Remote(I/O) Mode)-
  {3,             InvalidMenuID,           Page_RemoteIOMode_ID,  {{0xD4B6, 0xB3CC, 0xBFAA, 0xB9D8, 0xD0CD, 0xC9E8, 0xD6C3}, {0x52, 0x65, 0x6D, 0x6F, 0x74, 0x65, 0x28, 0x49, 0x2F, 0x4F, 0x29, 0x20, 0x4D, 0x6F, 0x64, 0x65}},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-Զ�̵���������(Remote(AN) Mode)-
  {4,             InvalidMenuID,           Page_RemoteANMode_ID,  {{0xD4B6, 0xB3CC, 0xB5F7, 0xBDDA, 0xD0CD, 0xC9E8, 0xD6C3}, {0x52, 0x65, 0x6D, 0x6F, 0x74, 0x65, 0x28, 0x41, 0x4E, 0x29, 0x20, 0x4D, 0x6F, 0x64, 0x65}},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
  
  //-�ڲ�����(Internal Para)-
  {5,             InvalidMenuID,           Page_Password_ID,      {{0xC4DA, 0xB2BF, 0xB2CE, 0xCAFD}, {0x49, 0x6E, 0x74, 0x65, 0x72, 0x6E, 0x61, 0x6C, 0x20, 0x50, 0x61, 0x72, 0x61}},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
  
  //-�˳�����(Exit)-
  {6,             Page_Normal_ID,          InvalidMenuID,          {{0xCDCB, 0xB3F6, 0xC9E8, 0xD6C3}, {0x45, 0x78, 0x69, 0x74}},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction}
};   

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_Main = {0, Invalid, Enable, Disable, Invalid, 7, Multiplex_None, StandardMenu_Reset0};    


/*---------------------�궨���-----------------------------------------------*/
const MenuStructure Menu_AdjustZero[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-�ѵ�ǰλ��#0(POS value #)-
  {0,             Page_MainMenu_ID,        InvalidMenuID,         {{0xB0D1, 0xB5B1, 0xC7B0, 0xCEBB, 0xD6C3, 0xA3A3}, {0x50, 0x4F, 0x53, 0x20, 0x76, 0x61, 0x6C, 0x75, 0x65, Space, 0x23}},
   AdjustZeroFull_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�궨Ϊ0%(set to 0%)-
  {1,             InvalidMenuID,           InvalidMenuID,         {{Space, Space, Space, Space, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B0, 0xA3A5}, {Space, Space, Space, Space, 0x73, 0x65, 0x74, 0x20, 0x74, 0x6F, 0x20, 0x30, 0x25}},
   AdjustZeroFull_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-ȷ��  ����(OK  Return)-
  {2,             Page_MainMenu_ID,        InvalidMenuID,         {{Space, Space, Space, 0xC8B7, 0xC8CF, Space, Space, 0xB7B5, 0xBBD8}, {Space, Space, Space, 0x4F, 0x4B, Space, Space, 0x52, 0x65, 0x74, 0x75, 0x72, 0x6E}},
   AdjustZeroFull_Special, AdjustZeroFull_SetKey, AdjustZeroFull_UpKey, AdjustZeroFull_DownKey, AdjustZeroFull_IncKey, AdjustZeroFull_DecKey},  

  //-�л����ֳ��ɵ���(Adjust in LOCAL)-
  {3,             InvalidMenuID,           InvalidMenuID,         {{0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB}, {0x41, 0x64, 0x6A, 0x75, 0x73, 0x74, 0x20, 0x69, 0x6E, 0x20, 0x4C, 0x4F, 0x43, 0x41, 0x4C}},
  AdjustZeroFull_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction}
};   
   
//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_AdjustZero = {2, 3, Disable, Enable, 4, 4, Multiplex_Adjust, StandardMenu_Reset1};    


/*---------------------�궨0%��ʾ��Ϣ---------------------------------------*/
const MenuStructure Menu_AdjustZeroInfo[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //--
  {0,             Page_MainMenu_ID,        InvalidMenuID,         {0},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�Ѱѵ�ǰλ��(Adjust)-
  {1,             InvalidMenuID,           InvalidMenuID,         {{Space, Space, 0xD2D1, 0xB0D1, 0xB5B1, 0xC7B0, 0xCEBB, 0xD6C3}, {Space, Space, Space, Space, Space, 0x41, 0x64, 0x6A, 0x75, 0x73, 0x74}},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�ɹ��궨Ϊ0%(Successfully!)-
  {2,             InvalidMenuID,           InvalidMenuID,         {{Space, Space, 0xB3C9, 0xB9A6, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B0, 0xA3A5}, {Space, Space, 0x53, 0x75, 0x63, 0x63, 0x65, 0x73, 0x73, 0x66, 0x75, 0x6C, 0x6C, 0x79, 0x21}},
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
  //-�ѵ�ǰλ��#0(POS value #)-
  {0,             Page_MainMenu_ID,       InvalidMenuID,         {{0xB0D1, 0xB5B1, 0xC7B0, 0xCEBB, 0xD6C3, 0xA3A3}, {0x50, 0x4F, 0x53, 0x20, 0x76, 0x61, 0x6C, 0x75, 0x65, Space, 0x23}},
   AdjustZeroFull_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�궨Ϊ100%(set to 100%)-
  {1,             InvalidMenuID,           InvalidMenuID,         {{Space, Space, Space, Space, 0xB1EA, 0xB6A8, 0xCEAA,0xA3B1, 0xA3B0, 0xA3B0,0xA3A5}, {Space, Space, Space, Space, 0x73, 0x65, 0x74, 0x20, 0x74, 0x6F, 0x20, 0x31, 0x30, 0x30, 0x25}},
   AdjustZeroFull_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-ȷ��  ����(OK  Return)-
  {2,             Page_MainMenu_ID,        InvalidMenuID,         {{Space, Space, Space, 0xC8B7, 0xC8CF, Space, Space, 0xB7B5, 0xBBD8}, {Space, Space, Space, 0x4F, 0x4B, Space, Space, 0x52, 0x65, 0x74, 0x75, 0x72, 0x6E}},
   AdjustZeroFull_Special, AdjustZeroFull_SetKey, AdjustZeroFull_UpKey, AdjustZeroFull_DownKey, AdjustZeroFull_IncKey, AdjustZeroFull_DecKey},

  //-�л����ֳ��ɵ���(Adjust in LOCAL)-
  {3,             InvalidMenuID,           InvalidMenuID,         {{0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB}, {0x41, 0x64, 0x6A, 0x75, 0x73, 0x74, 0x20, 0x69, 0x6E, 0x20, 0x4C, 0x4F, 0x43, 0x41, 0x4C}},
   AdjustZeroFull_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};   

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_AdjustFull = {2, 3, Disable, Enable, 4, 4, Multiplex_Adjust, StandardMenu_Reset1};  


/*---------------------�궨100%��ʾ��Ϣ---------------------------------------*/
const MenuStructure Menu_AdjustFullInfo[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //--
  {0,             Page_MainMenu_ID,        InvalidMenuID,         {0},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�Ѱѵ�ǰλ��(Adjust)-
  {1,             InvalidMenuID,           InvalidMenuID,         {{Space, Space, 0xD2D1, 0xB0D1, 0xB5B1, 0xC7B0, 0xCEBB, 0xD6C3}, {Space, Space, Space, Space, Space, 0x41, 0x64, 0x6A, 0x75, 0x73, 0x74}},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�ɹ��궨Ϊ100%(Successfully!)-
  {2,             InvalidMenuID,           InvalidMenuID,         {{Space, Space, 0xB3C9, 0xB9A6, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B1, 0xA3B0, 0xA3B0, 0xA3A5}, {Space, Space, 0x53, 0x75, 0x63, 0x63, 0x65, 0x73, 0x73, 0x66, 0x75, 0x6C, 0x6C, 0x79, 0x21}},
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
  //-�ֳ��㶯����(Inching)-
  {0,             Page_MainMenu_ID,        InvalidMenuID,           {{0xCFD6, 0xB3A1, 0xB5E3, 0xB6AF, 0xBFD8, 0xD6C6}, {0x49, 0x6E, 0x63, 0x68, 0x69, 0x6E, 0x67}},
  LocalMode_Special, LocalMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
  
  //-�ֳ����ֿ���(Holding)-
  {1,             InvalidMenuID,           InvalidMenuID,           {{0xCFD6, 0xB3A1, 0xB1A3, 0xB3D6, 0xBFD8, 0xD6C6}, {0x48, 0x6F, 0x6C, 0x64, 0x69, 0x6E, 0x67}},
  LocalMode_Special, LocalMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
 
  //-�����ϼ�(Return)-
  {2,             Page_MainMenu_ID,        InvalidMenuID,           {{0xB7B5, 0xBBD8,0xC9CF, 0xBCB6}, {0x52, 0x65, 0x74, 0x75, 0x72, 0x6E}},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction}
};   

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_LocalMode = {0, Invalid, Enable, Disable, Invalid, 3, Multiplex_None, StandardMenu_Reset0};    


/*---------------------Զ�̿���������-----------------------------------------*/
const MenuStructure Menu_RemoteIOMode[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-Զ�̵㶯����(Inching)-
  {0,             Page_MainMenu_ID,        InvalidMenuID,         {{0xD4B6, 0xB3CC, 0xB5E3, 0xB6AF, 0xBFD8, 0xD6C6}, {0x49, 0x6E, 0x63, 0x68, 0x69, 0x6E, 0x67}},
  RemoteIOMode_Special, RemoteIOMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-Զ�̱���ͣ����(Holding&Stp(NO))-
  {1,             InvalidMenuID,           InvalidMenuID,         {{0xD4B6, 0xB3CC, 0xB1A3, 0xB3D6, 0xCDA3, 0xB3A3, 0xBFAA}, {0x48, 0x6F, 0x6C, 0x64, 0x69, 0x6E, 0x67, 0x26, 0x53, 0x74, 0x70, 0x28, 0x4E, 0x4F, 0x29}},
  RemoteIOMode_Special, RemoteIOMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-Զ�̱���ͣ����(Holding&Stp(NC))-
  {2,             InvalidMenuID,           InvalidMenuID,         {{0xD4B6, 0xB3CC, 0xB1A3, 0xB3D6, 0xCDA3, 0xB3A3, 0xB1D5}, {0x48, 0x6F, 0x6C, 0x64, 0x69, 0x6E, 0x67, 0x26, 0x53, 0x74, 0x70, 0x28, 0x4E, 0x43, 0x29}},
  RemoteIOMode_Special, RemoteIOMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-���ſ����Ź�(SIG-ON-No-OFF)-
  {3,             InvalidMenuID,           InvalidMenuID,         {{0xD3D0, 0xD0C5, 0xBFAA, 0xCEDE, 0xD0C5, 0xB9D8}, {0x53, 0x49, 0x47, 0x2D, 0x4F, 0x4E, 0x2D, 0x4E, 0x6F, 0x2D, 0x4F, 0x46, 0x46}},
  RemoteIOMode_Special, RemoteIOMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-���Ź����ſ�(SIG-OFF-No-ON)-
  {4,             InvalidMenuID,           InvalidMenuID,         {{0xD3D0, 0xD0C5, 0xB9D8, 0xCEDE, 0xD0C5, 0xBFAA}, {0x53, 0x49, 0x47, 0x2D, 0x4F, 0x46, 0x46, 0x2D, 0x4E, 0x6F, 0x2D, 0x4F, 0x4E}},
  RemoteIOMode_Special, RemoteIOMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-�����ϼ�(Return)-
  {5,             Page_MainMenu_ID,        InvalidMenuID,         {{0xB7B5, 0xBBD8,0xC9CF, 0xBCB6}, {0x52, 0x65, 0x74, 0x75, 0x72, 0x6E}},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction}
};  

//-��6����ʼֵ����˵������,�������ȷ��ֵ- 
MenuPara      MenuPara_RemoteIOMode = {0, Invalid, Enable, Disable, Invalid, 6, Multiplex_None, StandardMenu_Reset0};  


/*---------------------Զ�̵���������-----------------------------------------*/
const MenuStructure Menu_RemoteANMode[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-��������(DeadZone)-
  {0,             Page_MainMenu_ID,        Page_DeadZone_ID,        {{0xBFD8, 0xD6C6,0xCBC0, 0xC7F8}, {0x44, 0x65, 0x61, 0x64, 0x20, 0x5A, 0x6F, 0x6E, 0x65}},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-���ű�λ(No-SV Hold)-
  {1,             InvalidMenuID,           InvalidMenuID,           {{0xB6AA, 0xD0C5,0xB1A3, 0xCEBB}, {0x4E, 0x6F, 0x2D, 0x53, 0x56, 0x20, 0x48, 0x6F, 0x6C, 0x64}},
  RemoteANMode_Special, RemoteANMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-���Źر�(No-SV Close)-
  {2,             InvalidMenuID,           InvalidMenuID,           {{0xB6AA, 0xD0C5,0xB9D8, 0xB1D5}, {0x4E, 0x6F, 0x2D, 0x53, 0x56, 0x20, 0x43, 0x6C, 0x6F, 0x73, 0x65}},
  RemoteANMode_Special, RemoteANMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-���ž���(No-SV Mediate)-
  {3,             InvalidMenuID,           InvalidMenuID,           {{0xB6AA, 0xD0C5,0xBED3, 0xD6D0}, {0x4E, 0x6F, 0x2D, 0x53, 0x56, 0x20, 0x4D, 0x65, 0x64, 0x69, 0x61, 0x74, 0x65}},
  RemoteANMode_Special, RemoteANMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-���Ŵ�(No-SV Open)-
  {4,             InvalidMenuID,           InvalidMenuID,           {{0xB6AA, 0xD0C5,0xB4F2, 0xBFAA}, {0x4E, 0x6F, 0x2D, 0x53, 0x56, 0x20, 0x4F, 0x70, 0x65, 0x6E}},
  RemoteANMode_Special, RemoteANMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-�����ϼ�(Return)-
  {5,             Page_MainMenu_ID,        InvalidMenuID,           {{0xB7B5, 0xBBD8,0xC9CF, 0xBCB6}, {0x52, 0x65, 0x74, 0x75, 0x72, 0x6E}},
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
  //-���ÿ�������(Set Dead Zone)-
  {0,             Page_RemoteANMode_ID,    InvalidMenuID,           {{0xC9E8, 0xD6C3, 0xBFD8, 0xD6C6, 0xCBC0, 0xC7F8}, {0x53, 0x65, 0x74, 0x20, 0x44, 0x65, 0x61, 0x64, 0x20, 0x5A, 0x6F, 0x6E, 0x65}},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-����Ӧ/x.x%-
  {1,             InvalidMenuID,           InvalidMenuID,           {0},
  DeadZone_Special, DummyFunction, DummyFunction, DummyFunction, DeadZone_IncKey, DeadZone_DecKey},

  //-�����ϼ�(Return)-
  {2,             Page_RemoteANMode_ID,    InvalidMenuID,           {{Space, Space, Space, Space, 0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6}, {Space, Space, Space, Space, 0x52, 0x65, 0x74, 0x75, 0x72, 0x6E}},
  DeadZone_Special, StandardMenu_Back2Parent, DummyFunction, DummyFunction, DeadZone_IncKey, DeadZone_DecKey},

  //-�л����ֳ��ɵ���((Adjust in LOCAL))-
  {3,             InvalidMenuID,           InvalidMenuID,           {{0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB}, {0x41, 0x64, 0x6A, 0x75, 0x73, 0x74, 0x20, 0x69, 0x6E, 0x20, 0x4C, 0x4F, 0x43, 0x41, 0x4C}},
  DeadZone_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction}
};  

//-��6����ʼֵ����˵������,�������ȷ��ֵ- 
MenuPara      MenuPara_DeadZone = {2, 4, Disable, Enable, 8, 4, Multiplex_Digit, Dummy_Reset};  


/*---------------------��������-----------------------------------------------*/
const MenuStructure Menu_Password[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-����רҵ��Ա����(Set by PRO only)-
  {0,             Page_MainMenu_ID,        InvalidMenuID,           {{0xD0E8, 0xD3C9, 0xD7A8, 0xD2B5, 0xC8CB, 0xD4B1, 0xC9E8, 0xD6C3}, {0x53, 0x65, 0x74, 0x20, 0x62, 0x79, 0x20, 0x50, 0x52, 0x4F, 0x20, 0x6F, 0x6E, 0x6C, 0x79}},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-����������(Input Password)-
  {1,             InvalidMenuID,           InvalidMenuID,           {{0xC7EB, 0xCAE4, 0xC8EB, 0xC3DC, 0xC2EB}, {0x49, 0x6E, 0x70, 0x75, 0x74, 0x20, 0x50, 0x61, 0x73, 0x73, 0x77, 0x6F, 0x72, 0x64}}, 
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //- X X X X-
  {2,             InvalidMenuID,           Page_InternalPara_ID,    {Space, Space, Space, Space, 0xA3B0, Space, 0xA3B0, Space, 0xA3B0, Space, 0xA3B0, Space},
  Password_Special, Password_SetKey, Password_UpKey, Password_DownKey, Password_IncKey, Password_DecKey},

  //-���������������(Err! Input again)-
  {3,             InvalidMenuID,           InvalidMenuID,           {{0xC3DC, 0xC2EB, 0xB4ED, 0xCEF3, 0xD6D8, 0xD0C2, 0xCAE4, 0xC8EB}, {0x45, 0x72, 0x72, 0x21, 0x20, 0x49, 0x6E, 0x70, 0x75, 0x74, 0x20, 0x61, 0x67, 0x61, 0x69, 0x6E}},
  Password_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction}
};   

 //-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_Password = {2, 4, Disable,Enable, 1, 4, Multiplex_Password, Password_Reset};   


/*---------------------�ڲ�����-----------------------------------------------*/
const MenuStructure Menu_InternalPara[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-����4mA����(Adjust Out-4mA)-
  {0,             Page_MainMenu_ID,        Page_AdjustOutput4mA_ID, {{0xB7B4, 0xC0A1, Space, 0xA3B4, 0xA3ED, 0xA3C1, 0xD0DE, 0xD5FD}, {0x41, 0x64, 0x6A, 0x75, 0x73, 0x74, 0x20, 0x4F, 0x75, 0x74, 0x2D, 0x34, 0x6D, 0x41}},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-����20mA����(Adjust Out-20mA)-
  {1,             InvalidMenuID,           Page_AdjustOutput20mA_ID,{{0xB7B4, 0xC0A1,0xA3B2, 0xA3B0, 0xA3ED, 0xA3C1, 0xD0DE, 0xD5FD}, {0x41, 0x64, 0x6A, 0x75, 0x73, 0x74, 0x20, 0x4F, 0x75, 0x74, 0x2D, 0x32, 0x30, 0x6D, 0x41}},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-����4mAУ׼(Adjust In-4mA)-
  {2,             InvalidMenuID,           Page_AdjustInput4mA_ID,  {{0xCAE4, 0xC8EB, Space, 0xA3B4, 0xA3ED, 0xA3C1, 0xD0A3, 0xD7BC}, {0x41, 0x64, 0x6A, 0x75, 0x73, 0x74, 0x20, 0x49, 0x6E, 0x2D, 0x34, 0x6D, 0x41}},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-����20mAУ׼(Adjust In-20mA)-
  {3,             InvalidMenuID,           Page_AdjustInput20mA_ID, {{0xCAE4, 0xC8EB,0xA3B2, 0xA3B0, 0xA3ED, 0xA3C1, 0xD0A3, 0xD7BC}, {0x41, 0x64, 0x6A, 0x75, 0x73, 0x74, 0x20, 0x49, 0x6E, 0x2D, 0x32, 0x30, 0x6D, 0x41}},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-���ùض�������(Max Clse Current)-
  {4,             InvalidMenuID,           Page_ShutCurrent_ID,     {{0xC9E8, 0xD6C3, 0xB9D8, 0xB6AF, 0xD7F7, 0xB5E7, 0xC1F7}, {0x4D, 0x61, 0x78, 0x20, 0x43, 0x6C, 0x73, 0x65, 0x20, 0x43, 0x75, 0x72, 0x72, 0x65, 0x6E, 0x74}},
  InternalPara_Current_Special, InternalPara_Current_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-���ÿ���������(Max Open Current)-
  {5,             InvalidMenuID,           Page_OpenCurrent_ID,     {{0xC9E8, 0xD6C3, 0xBFAA, 0xB6AF, 0xD7F7, 0xB5E7, 0xC1F7}, {0x4D, 0x61, 0x78, 0x20, 0x4F, 0x70, 0x65, 0x6E, 0x20, 0x43, 0x75, 0x72, 0x72, 0x65, 0x6E, 0x74}},
  InternalPara_Current_Special, InternalPara_Current_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-������󿪱�ʱ��(Max Action Time)-
  {6,             InvalidMenuID,           Page_MaxActionTime_ID,   {{0xB7A7, 0xC3C5,0xD7EE, 0xB4F3, 0xBFAA, 0xB1D5, 0xCAB1, 0xBCE4}, {0x4D, 0x61, 0x78, 0x20, 0x41, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x54, 0x69, 0x6D, 0x65}},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-���Ϸ�������ѡ��(Set Err contact)-
  {7,             InvalidMenuID,           Page_ErrorFeedback_ID,   {{0xB9CA, 0xD5CF, 0xB7B4, 0xC0A1, 0xB4A5, 0xB5E3, 0xD1A1, 0xD4F1}, {0x53, 0x65, 0x74, 0x20, 0x45, 0x72, 0x72, 0x20, 0x63, 0x6F, 0x6E, 0x74, 0x61, 0x63, 0x74}},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD����(Set ESD Mode)-
  {8,             InvalidMenuID,           Page_ESDSetting_ID,      {{0x45, 0x53, 0x44, 0xC9E8, 0xD6C3}, {0x53, 0x65, 0x74, 0x20, 0x45, 0x53, 0x44, 0x20, 0x4D, 0x6F, 0x64, 0x65}},
  InternalPara_ESD_Special, InternalPara_ESD_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-�����ϼ�(Return)-
  {9,             Page_MainMenu_ID,        InvalidMenuID,           {{0xB7B5, 0xBBD8,0xC9CF, 0xBCB6}, {0x52, 0x65, 0x74, 0x75, 0x72, 0x6E}},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction}
};   

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_InternalPara = {0, Invalid, Enable, Disable, Invalid, 10, Multiplex_None, StandardMenu_Reset0};    


/*---------------------��������4mA����----------------------------------------*/
const MenuStructure Menu_AdjustOutput4mA[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-��������4mA����(Adjust Out-4mA)-
  {0,             Page_InternalPara_ID,    InvalidMenuID,           {{0xB7B4, 0xC0A1, 0xB5E7, 0xC1F7, 0xA3B4, 0xA3ED, 0xA3C1, 0xD0DE, 0xD5FD}, {0x41, 0x64, 0x6A, 0x75, 0x73, 0x74, 0x20, 0x4F, 0x75, 0x74, 0x2D, 0x34, 0x6D, 0x41}},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {1,             InvalidMenuID,           InvalidMenuID,           {0},
  AdjustOutput4_20mA_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-�����ϼ�(Return)-
  {2,             Page_InternalPara_ID,    InvalidMenuID,           {{Space, Space, Space, Space, 0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6}, {Space, Space, Space, Space, 0x52, 0x65, 0x74, 0x75, 0x72, 0x6E}},
  AdjustOutput4_20mA_Special, StandardMenu_Back2Parent, DummyFunction, DummyFunction, AdjustOutput4mA_IncKey, AdjustOutput4mA_DecKey},

  //-�л����ֳ��ɵ���(Adjust in LOCAL)-
  {3,             InvalidMenuID,           InvalidMenuID,           {{0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB}, {0x41, 0x64, 0x6A, 0x75, 0x73, 0x74, 0x20, 0x69, 0x6E, 0x20, 0x4C, 0x4F, 0x43, 0x41, 0x4C}},
  AdjustOutput4_20mA_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction}
};  

//-��6����ʼֵ����˵������,�������ȷ��ֵ- 
MenuPara      MenuPara_AdjustOutput4mA = {2, 4, Disable, Enable, 8, 4, Multiplex_Adjust, Dummy_Reset}; 


/*---------------------��������20mA����----------------------------------------*/
const MenuStructure Menu_AdjustOutput20mA[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-��������20mA����(Adjust Out-20mA)-
  {0,             Page_InternalPara_ID,    InvalidMenuID,           {{0xB7B4, 0xC0A1, 0xB5E7, 0xC1F7, 0xA3B2, 0xA3B0, 0xA3ED, 0xA3C1, 0xD0DE, 0xD5FD}, {0x41, 0x64, 0x6A, 0x75, 0x73, 0x74, 0x20, 0x4F, 0x75, 0x74, 0x2D, 0x32, 0x30, 0x6D, 0x41}},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-����-
  {1,             InvalidMenuID,           InvalidMenuID,           {0},
  AdjustOutput4_20mA_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-�����ϼ�(Return)-
  {2,             Page_InternalPara_ID,    InvalidMenuID,           {{Space, Space, Space, Space, 0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6}, {Space, Space, Space, Space, 0x52, 0x65, 0x74, 0x75, 0x72, 0x6E}},
  AdjustOutput4_20mA_Special, StandardMenu_Back2Parent, DummyFunction, DummyFunction, AdjustOutput20mA_IncKey, AdjustOutput20mA_DecKey}, 

  //-�л����ֳ��ɵ���(Adjust in LOCAL)-
  {3,             InvalidMenuID,           InvalidMenuID,           {{0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB}, {0x41, 0x64, 0x6A, 0x75, 0x73, 0x74, 0x20, 0x69, 0x6E, 0x20, 0x4C, 0x4F, 0x43, 0x41, 0x4C}},
  AdjustOutput4_20mA_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction}
};  

//-��6����ʼֵ����˵������,�������ȷ��ֵ- 
MenuPara      MenuPara_AdjustOutput20mA = {2, 4, Disable, Enable, 8, 4, Multiplex_Adjust, Dummy_Reset}; 


/*---------------------�궨����4mA--------------------------------------------*/
const MenuStructure Menu_AdjustInput4mA[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-�ѵ�ǰ�������(Set input Currnt)-
  {0,             Page_InternalPara_ID,  InvalidMenuID,           {{Space, 0xB0D1, 0xB5B1, 0xC7B0, 0xCAE4, 0xC8EB, 0xB5E7, 0xC1F7}, {0x53, 0x65, 0x74, 0x20, 0x69, 0x6E, 0x70, 0x75, 0x74, 0x20, 0x43, 0x75, 0x72, 0x72, 0x6E, 0x74}},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-Լxx.xmA(xx.xmA)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0, 0, 0, 0, 0xD4BC, 0, 0, 0, 0, 0xA3ED, 0xA3C1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0xA3ED, 0xA3C1}},
  AdjustInput4_20mA_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�궨ΪĿ��0%(to SV-0%)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{Space, 0xB1EA, 0xB6A8, 0xCEAA,0xC4BF, 0xB1EA, 0xA3B0,0xA3A5}, {Space, Space, Space, Space, 0x74, 0x6F, 0x20, 0x53, 0x56, 0x2D, 0x30, 0x25}},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-ȷ��  ����(OK Return)-
  {3,             Page_InternalPara_ID,   Page_AdjustInput4mAInfo_ID,{{Space, Space, Space, 0xC8B7, 0xC8CF, Space, Space, 0xB7B5, 0xBBD8}, {Space, Space, Space, 0x4F, 0x4B, Space, Space, 0x52, 0x65, 0x74, 0x75, 0x72, 0x6E}},
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
  
  //-�Ѱѵ�ǰ����(Adjust)-
  {1,             InvalidMenuID,           InvalidMenuID,           {{Space, Space, 0xD2D1, 0xB0D1, 0xB5B1, 0xC7B0, 0xB5E7, 0xC1F7}, {Space, Space, Space, Space, Space, 0x41, 0x64, 0x6A, 0x75, 0x73, 0x74}},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�ɹ��궨Ϊ0%(Successfully!)-
  {2,             InvalidMenuID,           InvalidMenuID,           {{Space, Space, 0xB3C9, 0xB9A6, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B0, 0xA3A5}, {Space, Space, 0x53, 0x75, 0x63, 0x63, 0x65, 0x73, 0x73, 0x66, 0x75, 0x6C, 0x6C, 0x79, 0x21}},
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
  //-�ѵ�ǰ�������(Set input Currnt)-
  {0,             Page_InternalPara_ID,    InvalidMenuID,           {{Space, 0xB0D1, 0xB5B1, 0xC7B0, 0xCAE4, 0xC8EB, 0xB5E7, 0xC1F7}, {0x53, 0x65, 0x74, 0x20, 0x69, 0x6E, 0x70, 0x75, 0x74, 0x20, 0x43, 0x75, 0x72, 0x72, 0x6E, 0x74}},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-Լxx.xmA(xx.xmA)-
  {1,             InvalidMenuID,           InvalidMenuID,           {{0, 0, 0, 0, 0xD4BC, 0, 0, 0, 0, 0xA3ED, 0xA3C1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0xA3ED, 0xA3C1}},
  AdjustInput4_20mA_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�궨ΪĿ��100%(to SV-100%)-
  {2,             InvalidMenuID,           InvalidMenuID,           {{Space, 0xB1EA, 0xB6A8, 0xCEAA,0xC4BF, 0xB1EA, 0xA3B1, 0xA3B0, 0xA3B0,0xA3A5}, {Space, Space, Space, Space, 0x74, 0x6F, 0x20, 0x53, 0x56, 0x2D, 0x31, 0x30, 0x30, 0x25}},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-ȷ��  ����(OK Return)-
  {3,             Page_InternalPara_ID,     Page_AdjustInput20mAInfo_ID,{{Space, Space, Space, 0xC8B7, 0xC8CF, Space, Space, 0xB7B5, 0xBBD8}, {Space, Space, Space, 0x4F, 0x4B, Space, Space, 0x52, 0x65, 0x74, 0x75, 0x72, 0x6E}},
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
  
  //-�Ѱѵ�ǰ����(Adjust)-
  {1,             InvalidMenuID,           InvalidMenuID,           {{Space, Space, 0xD2D1, 0xB0D1, 0xB5B1, 0xC7B0, 0xB5E7, 0xC1F7}, {Space, Space, Space, Space, Space, 0x41, 0x64, 0x6A, 0x75, 0x73, 0x74}},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-�ɹ��궨Ϊ100%(Successfully!)-
  {2,             InvalidMenuID,           InvalidMenuID,           {{Space, Space, 0xB3C9, 0xB9A6, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B1, 0xA3B0, 0xA3B0, 0xA3A5}, {Space, Space, 0x53, 0x75, 0x63, 0x63, 0x65, 0x73, 0x73, 0x66, 0x75, 0x6C, 0x6C, 0x79, 0x21}},
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
  //-���ù���������(Max Clse Current)-
  {0,             Page_InternalPara_ID,  InvalidMenuID,           {{0xC9E8, 0xD6C3, 0xB9D8, 0xCFF2, 0xB6AF, 0xD7F7, 0xB5E7, 0xC1F7}, {0x4D, 0x61, 0x78, 0x20, 0x43, 0x6C, 0x73, 0x65, 0x20, 0x43, 0x75, 0x72, 0x72, 0x65, 0x6E, 0x74}},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {1,             InvalidMenuID,         InvalidMenuID,           {0},
  ShutCurrent_Special, DummyFunction, DummyFunction, DummyFunction, ShutCurrent_IncKey, ShutCurrent_DecKey},

  //-    �����ϼ�(Return)-
  {2,             Page_InternalPara_ID,  InvalidMenuID,           {{Space, Space, Space, Space, 0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6}, {Space, Space, Space, Space, 0x52, 0x65, 0x74, 0x75, 0x72, 0x6E}},
  ShutCurrent_Special, StandardMenu_Back2Parent, DummyFunction, DummyFunction, ShutCurrent_IncKey, ShutCurrent_DecKey},

  //-�л����ֳ��ɵ���(Adjust in LOCAL)-
  {3,             InvalidMenuID,         InvalidMenuID,           {{0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB}, {0x41, 0x64, 0x6A, 0x75, 0x73, 0x74, 0x20, 0x69, 0x6E, 0x20, 0x4C, 0x4F, 0x43, 0x41, 0x4C}},
  ShutCurrent_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_ShutCurrent = {2, 4, Disable, Enable, 8, 4, Multiplex_Digit, Dummy_Reset};


/*------------------ ����������-------------------------------*/
const MenuStructure Menu_OpenCurrent[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-���ÿ���������(Max Open Current)-
  {0,             Page_InternalPara_ID,  InvalidMenuID,           {{0xC9E8, 0xD6C3, 0xBFAA, 0xCFF2, 0xB6AF, 0xD7F7, 0xB5E7, 0xC1F7}, {0x4D, 0x61, 0x78, 0x20, 0x4F, 0x70, 0x65, 0x6E, 0x20, 0x43, 0x75, 0x72, 0x72, 0x65, 0x6E, 0x74}},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {1,             InvalidMenuID,         InvalidMenuID,           {0},
  OpenCurrent_Special, DummyFunction, DummyFunction, DummyFunction, OpenCurrent_IncKey, OpenCurrent_DecKey},

  //-    �����ϼ�(Return)-
  {2,             Page_InternalPara_ID,    InvalidMenuID,         {{Space, Space, Space, Space, 0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6}, {Space, Space, Space, Space, 0x52, 0x65, 0x74, 0x75, 0x72, 0x6E}},
  OpenCurrent_Special, StandardMenu_Back2Parent, DummyFunction, DummyFunction, OpenCurrent_IncKey, OpenCurrent_DecKey},

  //-�л����ֳ��ɵ���(Adjust in LOCAL)-
  {3,             InvalidMenuID,         InvalidMenuID,           {{0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB}, {0x41, 0x64, 0x6A, 0x75, 0x73, 0x74, 0x20, 0x69, 0x6E, 0x20, 0x4C, 0x4F, 0x43, 0x41, 0x4C}},
  OpenCurrent_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_OpenCurrent = {2, 4, Disable, Enable, 8, 4, Multiplex_Digit, Dummy_Reset};


/*---------------------������󿪱�ʱ��---------------------------------------*/
const MenuStructure Menu_MaxActionTime[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-������󿪱�ʱ��(Max Action Time)-
  {0,             Page_InternalPara_ID,    InvalidMenuID,           {{0xB7A7, 0xC3C5, 0xD7EE, 0xB4F3, 0xBFAA, 0xB1D5, 0xCAB1, 0xBCE4}, {0x4D, 0x61, 0x78, 0x20, 0x41, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x54, 0x69, 0x6D, 0x65}},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-����ʱ/xxxS-
  {1,             Page_InternalPara_ID,    InvalidMenuID,           {0},
  MaxActionTime_Special, MaxActionTime_SetKey, DummyFunction, DummyFunction, MaxActionTime_IncKey, MaxActionTime_DecKey},

  //-    �����ϼ�(Return)-
  {2,             Page_InternalPara_ID,    InvalidMenuID,          {{Space, Space, Space, Space, 0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6}, {Space, Space, Space, Space, 0x52, 0x65, 0x74, 0x75, 0x72, 0x6E}},
  MaxActionTime_Special, StandardMenu_Back2Parent, DummyFunction, DummyFunction, MaxActionTime_IncKey, MaxActionTime_DecKey},

  //-�л����ֳ��ɵ���(Adjust in LOCAL)-
  {3,             InvalidMenuID,         InvalidMenuID,           {{0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB}, {0x41, 0x64, 0x6A, 0x75, 0x73, 0x74, 0x20, 0x69, 0x6E, 0x20, 0x4C, 0x4F, 0x43, 0x41, 0x4C}},
  MaxActionTime_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};  

//-��6����ʼֵ����˵������,�������ȷ��ֵ- 
MenuPara      MenuPara_MaxActionTime = {1, 6, Disable, Enable, 6, 4, Multiplex_Digit, Dummy_Reset}; 


/*------------------ ���Ϸ�������-------------------------------*/
const MenuStructure Menu_ErrorFeedback[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-���Ϸ�������ѡ��(Set Err contact)-
  {0,             Page_InternalPara_ID,  InvalidMenuID,           {{0xB9CA, 0xD5CF, 0xB7B4, 0xC0A1, 0xB4A5, 0xB5E3, 0xD1A1, 0xD4F1}, {0x53, 0x65, 0x74, 0x20, 0x45, 0x72, 0x72, 0x20, 0x63, 0x6F, 0x6E, 0x74, 0x61, 0x63, 0x74}},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-�й��ϱպ�NO(Error NO)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xD3D0, 0xB9CA, 0xD5CF, 0xB1D5, 0xBACF, 0x4E, 0x4F}, {0x45, 0x72, 0x72, 0x6F, 0x72, 0x20, 0x4E, 0x4F}},
  ErrorFeedback_Special, ErrorFeedback_SetKey, ErrorFeedback_UpKey, ErrorFeedback_DownKey, DummyFunction, DummyFunction},

  //-�й��϶Ͽ�NC(Error NC)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xD3D0, 0xB9CA, 0xD5CF, 0xB6CF, 0xBFAA, 0x4E, 0x43}, {0x45, 0x72, 0x72, 0x6F, 0x72, 0x20, 0x4E, 0x43}},
  ErrorFeedback_Special, ErrorFeedback_SetKey, ErrorFeedback_UpKey, ErrorFeedback_DownKey, DummyFunction, DummyFunction},

  //-�����ϼ�(Return)-
  {3,             Page_InternalPara_ID,  InvalidMenuID,           {{0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6}, {0x52, 0x65, 0x74, 0x75, 0x72, 0x6E}},
  Dummy_Special, StandardMenu_Back2Parent, ErrorFeedback_UpKey, ErrorFeedback_DownKey, DummyFunction, DummyFunction},
};

//-��6����ʼֵ����˵������,�������ȷ��ֵ-
MenuPara      MenuPara_ErrorFeedback = {1, Invalid, Enable, Disable, 8, 4, Multiplex_None, Dummy_Reset};


/*------------------ ESD����-------------------------------*/
const MenuStructure Menu_ESDSetting[] = 
{
  /*-������       ���˵�                 �Ӳ˵�                   �˵�������-*/
  //-ESD����(ESD Disable)-
  {0,             Page_InternalPara_ID,  InvalidMenuID,           {{0x45, 0x53, 0x44, 0xBDFB, 0xD3C3}, {0x45, 0x53, 0x44, 0x20, 0x44, 0x69, 0x73, 0x61, 0x62, 0x6C, 0x65}},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD��ֹ����(ESD Hold)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0x45, 0x53, 0x44, 0xBDFB, 0xD6B9, 0xB6AF, 0xD7F7}, {0x45, 0x53, 0x44, 0x20, 0x48, 0x6F, 0x6C, 0x64}},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD�򿪷���(ESD Open)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0x45, 0x53, 0x44, 0xB4F2, 0xBFAA, 0xB7A7, 0xC3C5}, {0x45, 0x53, 0x44, 0x20, 0x4F, 0x70, 0x65, 0x6E}},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD�رշ���(ESD Close)-
  {3,             InvalidMenuID,         InvalidMenuID,           {{0x45, 0x53, 0x44, 0xB9D8, 0xB1D5, 0xB7A7, 0xC3C5}, {0x45, 0x53, 0x44, 0x20, 0x43, 0x6C, 0x6F, 0x73, 0x65}},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD���м�λ(ESD Mediate)-
  {4,             InvalidMenuID,         InvalidMenuID,           {{0x45, 0x53, 0x44, 0xB5BD, 0xD6D0, 0xBCE4, 0xCEBB}, {0x45, 0x53, 0x44, 0x20, 0x4D, 0x65, 0x64, 0x69, 0x61, 0x74, 0x65}},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-�����ϼ�(Return)-
  {5,             Page_InternalPara_ID,  InvalidMenuID,           {{0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6}, {0x52, 0x65, 0x74, 0x75, 0x72, 0x6E}},
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
*              FrontZeroDisplay, = 1,��Ч����ǰ�����Ҳ��ʾ; =0����ʾ(��С����ʱ,С����ǰ���0������ʾ.����0.0)
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

    ClearLongCode();
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

    if (DecimalBitCount != 0)
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

        LongCode[DecimalIndex] = 0xA3AE;    //-С����-
        ShowBlock(&LongCode[5 - BitCount], BitCount + 1, X_Offset);
    }
    else
    {
        ShowBlock(&LongCode[5 - BitCount], BitCount, X_Offset);
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

        if (Device.Para.LanguageType == Language_CN)
        {
            //-��һ��:����-
            LongCode[Index++] = 0xB4F2;
            LongCode[Index++] = 0xBFAA;
            LongCode[Index++] = 0xD2BB;
            LongCode[Index++] = 0xCFC2;
            LongCode[Index++] = 0x3A;
            LongCode[Index++] = 0xD4F6;
            LongCode[Index++] = 0xB4F3;
        }
        else
        {
            //-Increase:Open-
            LongCode[Index++] = 0x49;
            LongCode[Index++] = 0x6E;
            LongCode[Index++] = 0x63;
            LongCode[Index++] = 0x72;
            LongCode[Index++] = 0x65;
            LongCode[Index++] = 0x61;
            LongCode[Index++] = 0x73;
            LongCode[Index++] = 0x65;
            LongCode[Index++] = 0x3A;
            LongCode[Index++] = 0x4F;
            LongCode[Index++] = 0x70;
            LongCode[Index++] = 0x65;
            LongCode[Index++] = 0x6E;

        }
        ShowBlock(&LongCode[0], Index, DisplayBufIndex);
    }
    else if (LineIndex == 3)
    {
        DisplayBufIndex= 0;
        
        if (Device.Para.LanguageType == Language_CN)
        {
            //-�ر�һ��:��С-
            LongCode[Index++] = 0xB9D8;
            LongCode[Index++] = 0xB1D5;
            LongCode[Index++] = 0xD2BB;
            LongCode[Index++] = 0xCFC2;
            LongCode[Index++] = 0x3A;
            LongCode[Index++] = 0xBCF5;
            LongCode[Index++] = 0xD0A1;
        }
        else
        {
            //-Decrease:Close-
            LongCode[Index++] = 0x44;
            LongCode[Index++] = 0x65;
            LongCode[Index++] = 0x63;
            LongCode[Index++] = 0x72;
            LongCode[Index++] = 0x65;
            LongCode[Index++] = 0x61;
            LongCode[Index++] = 0x73;
            LongCode[Index++] = 0x65;
            LongCode[Index++] = 0x3A;
            LongCode[Index++] = 0x43;
            LongCode[Index++] = 0x6C;
            LongCode[Index++] = 0x6F;
            LongCode[Index++] = 0x73;
            LongCode[Index++] = 0x65;
        }
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
void ShowReservedItem(int X_Offset)
{
    int Index = 0;
    
    ClearLongCode();
    if (Device.Para.LanguageType == Language_CN)
    {
        //-������Ŀ-
        LongCode[Index++] = 0xB1A3;
        LongCode[Index++] = 0xC1F4;
        LongCode[Index++] = 0xCFEE;
        LongCode[Index++] = 0xC4BF;
    }
    else
    {
        //-Reserved Item-
        LongCode[Index++] = 0x52;
        LongCode[Index++] = 0x65;
        LongCode[Index++] = 0x73;
        LongCode[Index++] = 0x65;
        LongCode[Index++] = 0x72;
        LongCode[Index++] = 0x76;
        LongCode[Index++] = 0x65;
        LongCode[Index++] = 0x64;
        LongCode[Index++] = 0x20;
        LongCode[Index++] = 0x49;
        LongCode[Index++] = 0x74;
        LongCode[Index++] = 0x65;
        LongCode[Index++] = 0x6D;
    }

    ShowBlock(&LongCode[0], Index, X_Offset);
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
    ClearLongCode();
    if (Valve.Status.StatusBits.Opening == 1)
    {
        IsMoving = 1; 

        if (Device.Para.LanguageType == Language_CN)
        {
            LongCode[Index++] = 0xB4F2;    //-��-
            LongCode[Index++] = 0xBFAA;    //-��-
            LongCode[Index++] = 0xD6D0;    //-��-
        }
        else
        {
            //-Opening-
            LongCode[Index++] = 0x4F;    
            LongCode[Index++] = 0x70;    
            LongCode[Index++] = 0x65;   
            LongCode[Index++] = 0x6E;    
            LongCode[Index++] = 0x69;    
            LongCode[Index++] = 0x6E; 
            LongCode[Index++] = 0x67;    
        }
    }
    else if(Valve.Status.StatusBits.Shutting == 1)
    {
        IsMoving = 1; 

        if (Device.Para.LanguageType == Language_CN)
        {
            LongCode[Index++] = 0xB9D8;    //-��-
            LongCode[Index++] = 0xB1D5;    //-��-
            LongCode[Index++] = 0xD6D0;    //-��-
        }
        else
        {
            //-Closing-
            LongCode[Index++] = 0x43;    
            LongCode[Index++] = 0x6C;    
            LongCode[Index++] = 0x6F;   
            LongCode[Index++] = 0x73;    
            LongCode[Index++] = 0x69;    
            LongCode[Index++] = 0x6E; 
            LongCode[Index++] = 0x67; 
        }
    }
    else if (Valve.Status.StatusBits.OpenLimit == 1)
    {
        if (Device.Para.LanguageType == Language_CN)
        {
            LongCode[Index++] = 0xBFAA;    //-��-
            LongCode[Index++] = 0xB5BD;    //-��-
            LongCode[Index++] = 0xCEBB;    //-λ-    
        } 
        else
        {
            //-Open-OK-
            LongCode[Index++] = 0x4F;    
            LongCode[Index++] = 0x70;    
            LongCode[Index++] = 0x65;   
            LongCode[Index++] = 0x6E;    
            LongCode[Index++] = 0x2D;    
            LongCode[Index++] = 0x4F; 
            LongCode[Index++] = 0x4B; 
        }
    }
    else if (Valve.Status.StatusBits.ShutLimit == 1)
    {
        if (Device.Para.LanguageType == Language_CN)
        {
            LongCode[Index++] = 0xB9D8;    //-��-
            LongCode[Index++] = 0xB5BD;    //-��-
            LongCode[Index++] = 0xCEBB;    //-λ-
        }
        else
        {
            //-Close-OK-
            LongCode[Index++] = 0x43;    
            LongCode[Index++] = 0x6C;    
            LongCode[Index++] = 0x6F;   
            LongCode[Index++] = 0x73;    
            LongCode[Index++] = 0x65;    
            LongCode[Index++] = 0x2D; 
            LongCode[Index++] = 0x4F; 
            LongCode[Index++] = 0x4B; 
        }
    }
    else 
    {
    }

    DisplayBufIndex = 0;
    ShowBlock(&LongCode[0], Index, DisplayBufIndex);

    /*-���Ͻǲ�����ʾ���ȼ�:���ش��������ֶ�������״̬���������
      -*/
    Index = 0;
    ClearLongCode();
    if (F_Disconnect == 1)
    {
        if (Device.Para.LanguageType == Language_CN)
        {
            LongCode[Index++] = 0xCDA8;    //-ͨ-
            LongCode[Index++] = 0xD1B6;    //-Ѷ-
            LongCode[Index++] = 0xB6CF;    //-��-
            LongCode[Index++] = 0xC1AC;    //-��-
        }
        else
        {
            //-No Comm-
            LongCode[Index++] = 0x4E;    
            LongCode[Index++] = 0x6F;    
            LongCode[Index++] = 0x20;   
            LongCode[Index++] = 0x43;    
            LongCode[Index++] = 0x6F;    
            LongCode[Index++] = 0x6D; 
            LongCode[Index++] = 0x6D; 
        }
    }
    else if (Device.Error.ErrorByte != 0)
    {
        HaveError = 1;
    }
    else if (Device.CurCommMode.CommModeBits.Manual == 1)
    {
        if (Device.Para.LanguageType == Language_CN)
        {
            LongCode[Index++] = 0xCAD6;    //-��-
            LongCode[Index++] = 0xC2D6;    //-��-
            LongCode[Index++] = 0xCAD6;    //-��-
            LongCode[Index++] = 0xB6AF;    //-��-
        }
        else
        {
            //-Manual-
            LongCode[Index++] = 0x4D;    
            LongCode[Index++] = 0x61;    
            LongCode[Index++] = 0x6E;   
            LongCode[Index++] = 0x75;    
            LongCode[Index++] = 0x61;    
            LongCode[Index++] = 0x6C; 
        }
    }
    else if (Device.Status.ESDStatus == ESDStatus_Valid)
    {
        if (Device.Para.LanguageType == Language_CN)
        {
            LongCode[Index++] = 0xBDF4;    //-��-
            LongCode[Index++] = 0xBCB1;    //-��-
            LongCode[Index++] = 0xD7B4;    //-״-
            LongCode[Index++] = 0xCCAC;    //-̬-
        }
        else
        {
            //-Exigency-
            LongCode[Index++] = 0x45;    
            LongCode[Index++] = 0x78;    
            LongCode[Index++] = 0x69;   
            LongCode[Index++] = 0x67;    
            LongCode[Index++] = 0x65;    
            LongCode[Index++] = 0x6E; 
            LongCode[Index++] = 0x63;    
            LongCode[Index++] = 0x79; 
        }
    }
    else if (Valve.Error.ErrorByte != 0)
    {
        HaveError = 1;
    }
    else if (IsMoving == 1)
    {
        if (Device.Para.CurrentDisplayEnable == CurrentDisplay_Disable)
        {
            return;
        }

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
            ShowNumbers(Valve.MiscInfo.Current, 3, 2, DisplayBufIndex, 0);
        }
        else
        {
            ShowNumbers(Valve.MiscInfo.Current, 3, 1, DisplayBufIndex, 0);
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
    int Index = 0;

    int LineIndex = 0;
    int DisplayBufIndex = 0;
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
    ClearLongCode();
    if ((Device.CurCommMode.CommModeBits.Remote == 1) && (Device.Para.RemoteType == RemoteType_Regulate))
    {
        if (Valve.Status.StatusBits.NoSignal == 1)
        {
            if (Device.Para.LanguageType == Language_CN)
            {
                DisplayBufIndex = 96;
                LongCode[Index++] = 0xB6AA;    //-��-  
                LongCode[Index++] = 0xD0C5;    //-��-  
            }
            else
            {
                DisplayBufIndex = 80;
                //-No SV-
                LongCode[Index++] = 0x4E;     
                LongCode[Index++] = 0x6F; 
                LongCode[Index++] = 0x20;     
                LongCode[Index++] = 0x53;
                LongCode[Index++] = 0x56;       
            }
        }
        else
        {
            if (Device.Para.LanguageType == Language_CN)
            {
                DisplayBufIndex = 96;
                LongCode[Index++] = 0xC4BF;    //-Ŀ-  
                LongCode[Index++] = 0xB1EA;    //-��-  
            }
            else
            {
                DisplayBufIndex = 80;
                //-Target-
                LongCode[Index++] = 0x54;     
                LongCode[Index++] = 0x61; 
                LongCode[Index++] = 0x72;     
                LongCode[Index++] = 0x67;
                LongCode[Index++] = 0x65;     
                LongCode[Index++] = 0x74; 
            }
        }
    }
    else
    {
    }

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
    if ((Device.CurCommMode.CommModeBits.Remote == 1) && (Device.Para.RemoteType == RemoteType_Regulate))
    {
        if (Valve.Status.StatusBits.NoSignal == 1)
        { 
        }
        else
        {
            if (BaiValue == 0)
            {
                Code[0] = Space;              //-�ո�-  
            } 
            else
            {
                Code[0] = 0xA3B0 + BaiValue;    //-��-  
            }
            if ((ShiValue == 0) && (BaiValue == 0))
            {
                Code[1] = Space;              //-�ո�-  
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
    int Index = 0;
    int LineIndex = 0;
    int DisplayBufIndex = 0;
    unsigned char RemoteMode = 0xFF;

    /*--------------------------------������------------------------------------*/
    LineIndex = 3;  
    ClearBuf(&g_DisplayBuf[0], Display_Buf_Size);   

    if (Device.Para.LanguageType == Language_CN)
    {
        DisplayBufIndex = 32;
    }
    else
    {
        //Ӣ��, ������ʾ�����ݵ�������
    }

    ClearLongCode();
    switch(Device.WorkMode.CurWorkMode)
    {
    case WorkMode_Bus:
        if (Device.Para.LanguageType == Language_CN)
        {
            LongCode[Index++] = 0xD4B6;    //-Զ-  
            LongCode[Index++] = 0xB3CC;    //-��-  
            LongCode[Index++] = 0xCDA8;    //-ͨ-  
            LongCode[Index++] = 0xD0C5;    //-��- 
        }
        else
        {
            //-Remote-Bus-
            DisplayBufIndex = 24;
            LongCode[Index++] = 0x52;
            LongCode[Index++] = 0x65;
            LongCode[Index++] = 0x6D;
            LongCode[Index++] = 0x6F;
            LongCode[Index++] = 0x74;
            LongCode[Index++] = 0x65;
            LongCode[Index++] = 0x2D;
            LongCode[Index++] = 0x42;
            LongCode[Index++] = 0x75;
            LongCode[Index++] = 0x73;
        }
        break;
    case WorkMode_RemoteAN:
        if ((Valve.Status.StatusBits.NoSignal == 1) && (Device.Para.RemoteANMode == RemoteANMode_NoSigKeep))
        {
            RemoteMode = Device.Para.RemoteIOMode;
        }
        else
        {
            if (Device.Para.LanguageType == Language_CN)
            {
                LongCode[Index++] = 0xD4B6;    //-Զ-  
                LongCode[Index++] = 0xB3CC;    //-��-  
                LongCode[Index++] = 0xC4A3;    //-ģ-  
                LongCode[Index++] = 0xC4E2;    //-��- 
            }
            else
            {
                //-Remote-Auto-
                DisplayBufIndex = 24;
                LongCode[Index++] = 0x52;
                LongCode[Index++] = 0x65;
                LongCode[Index++] = 0x6D;
                LongCode[Index++] = 0x6F;
                LongCode[Index++] = 0x74;
                LongCode[Index++] = 0x65;
                LongCode[Index++] = 0x2D;
                LongCode[Index++] = 0x41;
                LongCode[Index++] = 0x75;
                LongCode[Index++] = 0x74;
                LongCode[Index++] = 0x6F;
            }
        }
        break;
    case WorkMode_RemoteJog:
        RemoteMode = RemoteIOMode_Jog;
        break;
    case WorkMode_RemoteHold:
        RemoteMode = RemoteIOMode_Hold;
        break;
    case WorkMode_RemoteDibit:
        RemoteMode = RemoteIOMode_BiPos;
        break;
    case WorkMode_LocalJog:
        if (Device.Para.LanguageType == Language_CN)
        {
            LongCode[Index++] = 0xCFD6;    //-��-  
            LongCode[Index++] = 0xB3A1;    //-��-  
            LongCode[Index++] = 0xB5E3;    //-��-  
            LongCode[Index++] = 0xB6AF;    //-��- 
        }
        else
        {
            //-Local-Inching-
            DisplayBufIndex = 16;
            LongCode[Index++] = 0x4C;
            LongCode[Index++] = 0x6F;
            LongCode[Index++] = 0x63;
            LongCode[Index++] = 0x61;
            LongCode[Index++] = 0x6C;
            LongCode[Index++] = 0x2D;
            LongCode[Index++] = 0x49;
            LongCode[Index++] = 0x6E;
            LongCode[Index++] = 0x63;
            LongCode[Index++] = 0x68;
            LongCode[Index++] = 0x69;
            LongCode[Index++] = 0x6E;
            LongCode[Index++] = 0x67;
        }
        break;
    case WorkMode_LocalHold:
        if (Device.Para.LanguageType == Language_CN)
        {
            LongCode[Index++] = 0xCFD6;    //-��-  
            LongCode[Index++] = 0xB3A1;    //-��-  
            LongCode[Index++] = 0xB1A3;    //-��-  
            LongCode[Index++] = 0xB3D6;    //-��- 
        }
        else
        {
            //-Local-Holding-
            DisplayBufIndex = 16;
            LongCode[Index++] = 0x4C;
            LongCode[Index++] = 0x6F;
            LongCode[Index++] = 0x63;
            LongCode[Index++] = 0x61;
            LongCode[Index++] = 0x6C;
            LongCode[Index++] = 0x2D;
            LongCode[Index++] = 0x48;
            LongCode[Index++] = 0x6F;
            LongCode[Index++] = 0x6C;
            LongCode[Index++] = 0x64;
            LongCode[Index++] = 0x69;
            LongCode[Index++] = 0x6E;
            LongCode[Index++] = 0x67;
        }
        break;
    case WorkMode_Stop:
        if (Device.Para.LanguageType == Language_CN)
        {
            LongCode[Index++] = 0xCDA3;    //-ͣ-  
            LongCode[Index++] = Space;
            LongCode[Index++] = Space;
            LongCode[Index++] = 0xD6B9;    //-ֹ-  
        }
        else
        {
            DisplayBufIndex = 48;

            //-Stop-
            LongCode[Index++] = 0x73;
            LongCode[Index++] = 0x74;
            LongCode[Index++] = 0x6F;
            LongCode[Index++] = 0x70;
        }
        break;
    default:
        break;
    }

    switch(RemoteMode)
    {
    case RemoteIOMode_Jog:
        if (Device.Para.LanguageType == Language_CN)
        {
            LongCode[Index++] = 0xD4B6;    //-Զ-  
            LongCode[Index++] = 0xB3CC;    //-��-  
            LongCode[Index++] = 0xB5E3;    //-��-  
            LongCode[Index++] = 0xB6AF;    //-��- 
        }
        else
        {
            //-Remote-Inching-
            DisplayBufIndex = 8;
            LongCode[Index++] = 0x52;
            LongCode[Index++] = 0x65;
            LongCode[Index++] = 0x6D;
            LongCode[Index++] = 0x6F;
            LongCode[Index++] = 0x74;
            LongCode[Index++] = 0x65;
            LongCode[Index++] = 0x2D;
            LongCode[Index++] = 0x49;
            LongCode[Index++] = 0x6E;
            LongCode[Index++] = 0x63;
            LongCode[Index++] = 0x68;
            LongCode[Index++] = 0x69;
            LongCode[Index++] = 0x6E;
            LongCode[Index++] = 0x67;
        }
        break;

    case RemoteIOMode_Hold:
    case RemoteIOMode_HoldNormallyOpen:
    case RemoteIOMode_HoldNormallyShut:
        if (Device.Para.LanguageType == Language_CN)
        {
            LongCode[Index++] = 0xD4B6;    //-Զ-  
            LongCode[Index++] = 0xB3CC;    //-��-  
            LongCode[Index++] = 0xB1A3;    //-��-  
            LongCode[Index++] = 0xB3D6;    //-��- 
        }
        else
        {
            //-Remote-Holding-
            DisplayBufIndex = 8;
            LongCode[Index++] = 0x52;
            LongCode[Index++] = 0x65;
            LongCode[Index++] = 0x6D;
            LongCode[Index++] = 0x6F;
            LongCode[Index++] = 0x74;
            LongCode[Index++] = 0x65;
            LongCode[Index++] = 0x2D;
            LongCode[Index++] = 0x48;
            LongCode[Index++] = 0x6F;
            LongCode[Index++] = 0x6C;
            LongCode[Index++] = 0x64;
            LongCode[Index++] = 0x69;
            LongCode[Index++] = 0x6E;
            LongCode[Index++] = 0x67;
        }
        break;

    case RemoteIOMode_BiPos:
    case RemoteIOMode_SignalOffNoOn:
    case RemoteIOMode_SignalOnNoOff:
        if (Device.Para.LanguageType == Language_CN)
        {
            LongCode[Index++] = 0xD4B6;    //-Զ-  
            LongCode[Index++] = 0xB3CC;    //-��-  
            LongCode[Index++] = 0xCBAB;    //-˫-  
            LongCode[Index++] = 0xCEBB;    //-λ- 
        }
        else
        {
            //-Remote-Dibit-
            DisplayBufIndex = 16;
            LongCode[Index++] = 0x52;
            LongCode[Index++] = 0x65;
            LongCode[Index++] = 0x6D;
            LongCode[Index++] = 0x6F;
            LongCode[Index++] = 0x74;
            LongCode[Index++] = 0x65;
            LongCode[Index++] = 0x2D;
            LongCode[Index++] = 0x44;
            LongCode[Index++] = 0x69;
            LongCode[Index++] = 0x62;
            LongCode[Index++] = 0x69;
            LongCode[Index++] = 0x74;
        }
        break;

    default:
        break;
    }
 
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
    if (Device.Para.LanguageType == Language_CN)
    {
        pMenuPara->ColumnIndex = 3;
        pMenuPara->ColumnReverseUnit = 4;
    }
    else
    {
        pMenuPara->ColumnIndex = 3;
        pMenuPara->ColumnReverseUnit = 2;
    }
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

    ClearLongCode();
    if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
    {
        pMenuPara->ColumnReverseSwitch = Disable;
        ClearBuf(&g_DisplayBuf[0], Display_Buf_Size);

        if (LineIndex == 0)
        {
            if (Device.Para.LanguageType == Language_CN)
            {
                DisplayBufIndex= 0;
                //-��ǰλ��#xxxx-
                LongCode[Index++] = Space;
                LongCode[Index++] = Space;
                LongCode[Index++] = 0xB5B1;
                LongCode[Index++] = 0xC7B0;
                LongCode[Index++] = 0xCEBB;
                LongCode[Index++] = 0xD6C3;
                LongCode[Index++] = 0x23;
            }
            else
            { 
                DisplayBufIndex= 0;
                //-POS value #-
                LongCode[Index++] = 0x50;
                LongCode[Index++] = 0x4F;
                LongCode[Index++] = 0x53;
                LongCode[Index++] = 0x20;
                LongCode[Index++] = 0x76;
                LongCode[Index++] = 0x61;
                LongCode[Index++] = 0x6C;
                LongCode[Index++] = 0x75;
                LongCode[Index++] = 0x65;
                LongCode[Index++] = 0x20;
                LongCode[Index++] = 0x23;

            }
            ShowBlock(&LongCode[0], Index, DisplayBufIndex);

            DisplayBufIndex += 88;
            ShowNumbers(Valve.MiscInfo.PositionValue, 5, 0, DisplayBufIndex, 1);
        }
        else if (LineIndex == 1)
        {
            DisplayBufIndex= 0;

            if (Device.Para.LanguageType == Language_CN)
            {
                //-������ť������λ-
                LongCode[Index++] = 0xB2D9;
                LongCode[Index++] = 0xD7F7;
                LongCode[Index++] = 0xD0FD;
                LongCode[Index++] = 0xC5A5;
                LongCode[Index++] = 0xBFC9;
                LongCode[Index++] = 0xCEDE;
                LongCode[Index++] = 0xCFDE;
                LongCode[Index++] = 0xCEBB;
            }
            else
            {
                //-Adjust by OPERA--
                LongCode[Index++] = 0x41;
                LongCode[Index++] = 0x64;
                LongCode[Index++] = 0x6A;
                LongCode[Index++] = 0x75;
                LongCode[Index++] = 0x73;
                LongCode[Index++] = 0x74;
                LongCode[Index++] = 0x20;
                LongCode[Index++] = 0x62;
                LongCode[Index++] = 0x79;
                LongCode[Index++] = 0x20;
                LongCode[Index++] = 0x4F;
                LongCode[Index++] = 0x50;
                LongCode[Index++] = 0x45;
                LongCode[Index++] = 0x52;
                LongCode[Index++] = 0x41;
                LongCode[Index++] = 0x54;
            }

            ShowBlock(&LongCode[0], Index, DisplayBufIndex);
        }
        else if (LineIndex == 2)
        {
            DisplayBufIndex= 0;

            if (Device.Para.LanguageType == Language_CN)
            {
                //-��������λ�ã���-
                LongCode[Index++] = 0xB5F7;
                LongCode[Index++] = 0xD5FB;
                LongCode[Index++] = 0xB7A7;
                LongCode[Index++] = 0xC3C5;
                LongCode[Index++] = 0xCEBB;
                LongCode[Index++] = 0xD6C3;
                LongCode[Index++] = 0xA3AC;
                LongCode[Index++] = 0xB7B5;
            }
            else
            {
                //-TE KNOB, Turn to-
                LongCode[Index++] = 0x45;
                LongCode[Index++] = 0x20;
                LongCode[Index++] = 0x4B;
                LongCode[Index++] = 0x4E;
                LongCode[Index++] = 0x4F;
                LongCode[Index++] = 0x42;
                LongCode[Index++] = 0x2C;
                LongCode[Index++] = 0x20;
                LongCode[Index++] = 0x54;
                LongCode[Index++] = 0x75;
                LongCode[Index++] = 0x72;
                LongCode[Index++] = 0x6E;
                LongCode[Index++] = 0x20;
                LongCode[Index++] = 0x74;
                LongCode[Index++] = 0x6F;
                LongCode[Index++] = 0x20;
            }
            ShowBlock(&LongCode[0], Index, DisplayBufIndex);
        }
        else if (LineIndex == 3)
        {
            DisplayBufIndex= 0;

            if (Device.Para.LanguageType == Language_CN)
            {
                //-��ֹͣλ��ɱ궨-
                LongCode[Index++] = 0xBBD8;
                LongCode[Index++] = 0xCDA3;
                LongCode[Index++] = 0xD6B9;
                LongCode[Index++] = 0xCEBB;
                LongCode[Index++] = 0xCDEA;
                LongCode[Index++] = 0xB3C9;
                LongCode[Index++] = 0xB1EA;
                LongCode[Index++] = 0xB6A8;
            }
            else
            {
                //- STOP to confirm.-
                LongCode[Index++] = 0x53;
                LongCode[Index++] = 0x54;
                LongCode[Index++] = 0x4F;
                LongCode[Index++] = 0x50;
                LongCode[Index++] = 0x20;
                LongCode[Index++] = 0x74;
                LongCode[Index++] = 0x6F;
                LongCode[Index++] = 0x20;
                LongCode[Index++] = 0x63;
                LongCode[Index++] = 0x6F;
                LongCode[Index++] = 0x6E;
                LongCode[Index++] = 0x66;
                LongCode[Index++] = 0x69;
                LongCode[Index++] = 0x72;
                LongCode[Index++] = 0x6D;
                LongCode[Index++] = 0x2E;
            }
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
    if (Device.Para.LanguageType == Language_CN)
    {
        pMenuPara->ColumnIndex -= 6;
        if (pMenuPara->ColumnIndex < 3)
        {
            pMenuPara->ColumnIndex = 9;
        }
    }
    else
    {
        pMenuPara->ColumnIndex -= 4;
        pMenuPara->ColumnReverseUnit = 2;

        if (pMenuPara->ColumnIndex < 3)
        {
            pMenuPara->ColumnIndex = 7;
            pMenuPara->ColumnReverseUnit = 6;
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
void AdjustZeroFull_DownKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (Device.Para.LanguageType == Language_CN)
    {
        pMenuPara->ColumnIndex += 6;
        if (pMenuPara->ColumnIndex > 9)
        {
            pMenuPara->ColumnIndex = 3;
        }
    }
    else
    {
        pMenuPara->ColumnIndex += 4;
        pMenuPara->ColumnReverseUnit = 6;

        if (pMenuPara->ColumnIndex > 9)
        {
            pMenuPara->ColumnIndex = 3;
            pMenuPara->ColumnReverseUnit = 2;
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
void AdjustZeroFull_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
    {
        //-�������LCDcon-ZFQ-V2,�����з�������-
        Valve.Operation.Operation = Operation_NoLimitOpen;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void AdjustZeroFull_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
    {
        //-�������LCDcon-ZFQ-V2,�����з�������-
        Valve.Operation.Operation = Operation_NoLimitShut;
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
        if (Device.Para.LanguageType == Language_CN)
        {
            DisplayBufIndex = 96;
        }
        else
        {
            DisplayBufIndex = 64;
        }
        ShowStar(DisplayBufIndex);
    }
    else if ((LineIndex == 1) && (Device.Para.LocalMode == LocalMode_Hold))
    {
        if (Device.Para.LanguageType == Language_CN)
        {
            DisplayBufIndex = 96;
        }
        else
        {
            DisplayBufIndex = 64;
        }
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
            if (Device.Para.LanguageType == Language_CN)
            {
                DisplayBufIndex = 96;
            }
            else
            {
                DisplayBufIndex = 64;
            }
            ShowStar(DisplayBufIndex);
        }
    }
    else if ((LineIndex == 1))
    {
        if (Device.Para.RemoteIOMode == RemoteIOMode_HoldNormallyOpen)
        {
            if (Device.Para.LanguageType == Language_CN)
            {
                DisplayBufIndex = 112;
            }
            else
            {
                DisplayBufIndex = 120;
            }
            ShowStar(DisplayBufIndex);
        }
    }
    else if ((LineIndex == 2) )
    {
        if (Device.Para.RemoteIOMode == RemoteIOMode_HoldNormallyShut)
        {
            if (Device.Para.LanguageType == Language_CN)
            {
                DisplayBufIndex = 112;
            }
            else
            {
                DisplayBufIndex = 120;
            }
            ShowStar(DisplayBufIndex);
        }
    }
    else if ((LineIndex == 3) )
    {
        if (Device.Para.RemoteIOMode == RemoteIOMode_SignalOnNoOff)
        {
            if (Device.Para.LanguageType == Language_CN)
            {
                DisplayBufIndex = 96;
            }
            else
            {
                DisplayBufIndex = 112;
            }
            ShowStar(DisplayBufIndex);
        }
    }
    else if ((LineIndex == 4) )
    {
        if (Device.Para.RemoteIOMode == RemoteIOMode_SignalOffNoOn)
        {
            if (Device.Para.LanguageType == Language_CN)
            {
                DisplayBufIndex = 96;
            }
            else
            {
                DisplayBufIndex = 112;
            }
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
        if (Device.Para.LanguageType == Language_CN)
        {
            DisplayBufIndex = 64;
        }
        else
        {
            DisplayBufIndex = 88;
        }
        ShowStar(DisplayBufIndex);
    }
    else if ((LineIndex == 2) && (Device.Para.RemoteANMode == RemoteANMode_NoSigShut))
    {
        if (Device.Para.LanguageType == Language_CN)
        {
            DisplayBufIndex = 64;
        }
        else
        {
            DisplayBufIndex = 96;
        }
        ShowStar(DisplayBufIndex);
    }
    else if ((LineIndex == 3) && (Device.Para.RemoteANMode == RemoteANMode_NoSigMid))
    {
        if (Device.Para.LanguageType == Language_CN)
        {
            DisplayBufIndex = 64;
        }
        else
        {
            DisplayBufIndex = 112;
        }
        ShowStar(DisplayBufIndex);
    }
    else if ((LineIndex == 4) && (Device.Para.RemoteANMode == RemoteANMode_NoSigOpen))
    {
        if (Device.Para.LanguageType == Language_CN)
        {
            DisplayBufIndex = 64;
        }
        else
        {
            DisplayBufIndex = 88;
        }
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
    int Index = 0;
    unsigned int  DeadZone = 0;
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
        //-��Χ:1~20-
        if (Device.Para.DeadZone <= 20)
        {
            //-DeadZone = Device.Para.DeadZone * 5-
            DeadZone = (Device.Para.DeadZone << 2) + Device.Para.DeadZone;
        }

        if (DeadZone < 5)
        {
            //-����Ӧ-
            ClearLongCode();
            if (Device.Para.LanguageType == Language_CN)
            {
                DisplayBufIndex = 48;
                LongCode[Index++] = 0xD7D4;
                LongCode[Index++] = 0xCACA;
                LongCode[Index++] = 0xD3A6;
            }
            else
            {
                DisplayBufIndex = 48;

                if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
                {
                    pMenuPara->ColumnReverseUnit = 10;
                }
                else
                {
                    pMenuPara->ColumnReverseUnit = 8;
                }

                //-Self-adpat-
                LongCode[Index++] = 0x53;
                LongCode[Index++] = 0x65;
                LongCode[Index++] = 0x6C;
                LongCode[Index++] = 0x66;
                LongCode[Index++] = 0x2D;
                LongCode[Index++] = 0x61;
                LongCode[Index++] = 0x64;
                LongCode[Index++] = 0x61;
                LongCode[Index++] = 0x70;
                LongCode[Index++] = 0x74;
            }
    
            ShowBlock(&LongCode[0], Index, DisplayBufIndex);
        }
        else
        {
            DisplayBufIndex = 48;
            ShowNumbers(DeadZone, 3, 1, DisplayBufIndex, 0);
            
            //-%-
            DisplayBufIndex += 32;
            LongCode[0] = 0x25;
            ShowBlock(&LongCode[0], 1, DisplayBufIndex);
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


void Password_Reset(MenuPara *pMenuPara)
{
    ClearBuf(&InputPassword[0], sizeof(InputPassword));
    MenuPara_Password.ColumnIndex = 4;
    PasswordErrorCount = 0;
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
            Code = InputPassword[i] + 0xA3B0;
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
    int i = 0;
    int j = 0;
    unsigned char IsPasswordError = 0;

    MenuPara_Password.ColumnIndex += 2;

    //-��Χ[4,6,8,10]-
    if (MenuPara_Password.ColumnIndex > 10)
    {
        MenuPara_Password.ColumnIndex = 4;  

        for (i = 0; i < PasswordGroupNum; i++)
        {
            IsPasswordError = 0;
            for (j = 0; j < PasswordNum; j++)
            {
                if (InputPassword[j] != ConstPassword[i][j])
                {
                    IsPasswordError = 1;
                    break;
                }
            }

            //-��һ������ͨ��-
            if (j == PasswordNum)
            {
                break;
            }
        }
           
        if (IsPasswordError == 0)    
        {
            if (pMenu->ChildMenuID != 0)
            {
                IsNeedChangePage = 1;
                NeedResetMenuPara = 1;
                Show_Page(pMenu->ChildMenuID);
 
                PasswordErrorCount = 0;
                ClearBuf(&InputPassword[0], 4);
                 
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
                ClearBuf(&InputPassword[0], 4);

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

    InputPassword[Index]++;
    if (InputPassword[Index] >= 10)
    {
        InputPassword[Index] = 0;
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

    if (InputPassword[Index] <= 0)
    {
        InputPassword[Index] = 10;
    }

    InputPassword[Index]--;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void InternalPara_Current_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex)
{
    if ((LineIndex != 4) && (LineIndex != 5))
    {
        return;
    }

    if (Device.Para.CurrentDisplayEnable == CurrentDisplay_Disable)
    {
        ClearBuf(&g_DisplayBuf[0], sizeof(g_DisplayBuf));
        ShowReservedItem(0);
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void InternalPara_Current_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (Device.Para.CurrentDisplayEnable == CurrentDisplay_Disable)
    {
    }
    else
    {
        StandardMenu_SetKey(pMenu, pMenuPara);
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void InternalPara_ESD_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex)
{
    if (LineIndex != 8)
    {
        return;
    }

    if (Device.Para.ESDDisplayEnable == ESDDisplay_Disable)
    {
        ClearBuf(&g_DisplayBuf[0], sizeof(g_DisplayBuf));
        ShowReservedItem(0);
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void InternalPara_ESD_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (Device.Para.ESDDisplayEnable == ESDDisplay_Disable)
    {
    }
    else
    {
        StandardMenu_SetKey(pMenu, pMenuPara);
    }
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
            if (Device.Para.LanguageType == Language_CN)
            {
                //-��ֹͣλ�����ϼ�-
                LongCode[Index++] = 0xD6C1;
                LongCode[Index++] = 0xCDA3;
                LongCode[Index++] = 0xD6B9;
                LongCode[Index++] = 0xCEBB;
                LongCode[Index++] = 0xB7B5;
                LongCode[Index++] = 0xBBD8;
                LongCode[Index++] = 0xC9CF;
                LongCode[Index++] = 0xBCB6;
            }
            else
            {
                //--Back:Turn 2 STOP
                LongCode[Index++] = 0x42;
                LongCode[Index++] = 0x61;
                LongCode[Index++] = 0x63;
                LongCode[Index++] = 0x6B;
                LongCode[Index++] = 0x3A;
                LongCode[Index++] = 0x54;
                LongCode[Index++] = 0x75;
                LongCode[Index++] = 0x72;
                LongCode[Index++] = 0x6E;
                LongCode[Index++] = 0x20;
                LongCode[Index++] = 0x32;
                LongCode[Index++] = 0x20;
                LongCode[Index++] = 0x53;
                LongCode[Index++] = 0x54;
                LongCode[Index++] = 0x4F;
                LongCode[Index++] = 0x50;
            }
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
    if (Device.Flag.FlagBits.IsInLocalAdjust == 0)
    {
        return;
    }

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
    if (Device.Flag.FlagBits.IsInLocalAdjust == 0)
    {
        return;
    }

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
    if (Device.Flag.FlagBits.IsInLocalAdjust == 0)
    {
        return;
    }

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
    if (Device.Flag.FlagBits.IsInLocalAdjust == 0)
    {
        return;
    }

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
        Code[0] = Space;               //-�ո�-
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
    AdjustZeroFull_UpKey(pMenu, pMenuPara);
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
    AdjustZeroFull_DownKey(pMenu, pMenuPara);
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void ShutCurrent_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    Device.Para.MaxShutCurrent++;
    if (Device.Para.MaxShutCurrent > 250)
    {
        Device.Para.MaxShutCurrent = 5;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void ShutCurrent_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    Device.Para.MaxShutCurrent--;
    if (Device.Para.MaxShutCurrent < 5)
    {
        Device.Para.MaxShutCurrent = 250;
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
            ShowNumbers(Device.Para.MaxShutCurrent, 3, 1, DisplayBufIndex, 0);
        }
        else
        {
            ShowNumbers(Device.Para.MaxShutCurrent, 3, 2, DisplayBufIndex, 0);
        }

        ClearLongCode();
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
void OpenCurrent_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    Device.Para.MaxOpenCurrent++;
    if (Device.Para.MaxOpenCurrent > 250)
    {
        Device.Para.MaxOpenCurrent = 5;
    }
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void OpenCurrent_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    Device.Para.MaxOpenCurrent--;
    if (Device.Para.MaxOpenCurrent < 5)
    {
        Device.Para.MaxOpenCurrent = 250;
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
            ShowNumbers(Device.Para.MaxOpenCurrent, 3, 1, 48, 0);
        }
        else
        {
            ShowNumbers(Device.Para.MaxOpenCurrent, 3, 2, 48, 0);
        }
    
        ClearLongCode();
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
    int Index = 0;

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
        if (Device.Para.MaxActionTime > 250)
        {
            if (Device.Para.LanguageType == Language_CN)
            {
                LongCode[Index++] = 0xB2BB;                  //-��-
                LongCode[Index++] = 0xCFDE;                  //-��-
                LongCode[Index++] = 0xCAB1;                  //-ʱ-
            }
            else
            {
                //-Untimed-
                LongCode[Index++] = 0x55;
                LongCode[Index++] = 0x6E;
                LongCode[Index++] = 0x74;
                LongCode[Index++] = 0x69;
                LongCode[Index++] = 0x6D;
                LongCode[Index++] = 0x65;
                LongCode[Index++] = 0x64;
            }

            if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
            {
                pMenuPara->ColumnReverseUnit = 7;
            }
            else
            {
                pMenuPara->ColumnReverseUnit = 8;
            }
    
            DisplayBufIndex = 48;
            ShowBlock(&LongCode[0], Index, DisplayBufIndex);
        } 
        else
        {
            DisplayBufIndex = 56;
            ShowNumbers(Device.Para.MaxActionTime, 3, 0, DisplayBufIndex, 0);
            
            //-S-
            DisplayBufIndex += 24;
            LongCode[Index++] = 0x53;
            ShowBlock(&LongCode[0], Index, DisplayBufIndex);
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
    //-��Χ:251��[10~250]-
    if (Device.Para.MaxActionTime >= 251)
    {
        Device.Para.MaxActionTime = 10;  
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
    //-��Χ:251��[10~250]-
    if (Device.Para.MaxActionTime <= 10)
    {
        Device.Para.MaxActionTime = 251;  
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
    int StarIndex = 0;

    if (Device.Para.LanguageType == Language_CN)
    {
        StarIndex = 96;
    }
    else
    {
        StarIndex = 72;
    }

    if ((LineIndex == 1) && (Device.Para.ErrorFeedBack == IO_NormallyOpen))
    {
        ShowStar(StarIndex);
    }
    else if ((LineIndex == 2) && (Device.Para.ErrorFeedBack == IO_NormallyShut))
    {
        ShowStar(StarIndex);
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
void ErrorFeedback_UpKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    pMenuPara->RowIndex--;
    if (pMenuPara->RowIndex < 1)
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
void ErrorFeedback_DownKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    pMenuPara->RowIndex++;
    if (pMenuPara->RowIndex >= pMenuPara->NumOfRows)
    {
        pMenuPara->RowIndex = 1;
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
    int StarIndex = 0;

    if ((LineIndex == 0) && (Device.Para.ESDMode == ESDMode_Disable))
    {
        if (Device.Para.LanguageType == Language_CN)
        {
            StarIndex = 56;
        }
        else
        {
            StarIndex = 96;
        }
        ShowStar(StarIndex);
    }
    else if ((LineIndex == 1) && (Device.Para.ESDMode == ESDMode_NoAction))
    {
        if (Device.Para.LanguageType == Language_CN)
        {
            StarIndex = 88;
        }
        else
        {
            StarIndex = 72;
        }
        ShowStar(StarIndex);
    }
    else if ((LineIndex == 2) && (Device.Para.ESDMode == ESDMode_Open))
    {
        if (Device.Para.LanguageType == Language_CN)
        {
            StarIndex = 88;
        }
        else
        {
            StarIndex = 72;
        }
        ShowStar(StarIndex);
    }
    else if ((LineIndex == 3) && (Device.Para.ESDMode == ESDMode_Shut))
    {
        if (Device.Para.LanguageType == Language_CN)
        {
            StarIndex = 88;
        }
        else
        {
            StarIndex= 80;
        }
        ShowStar(StarIndex);
    }
    else if ((LineIndex == 4) && (Device.Para.ESDMode == ESDMode_Middle))
    {
        if (Device.Para.LanguageType == Language_CN)
        {
            StarIndex = 88;
        }
        else
        {
            StarIndex = 96;
        }
        ShowStar(StarIndex);
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
    static unsigned char PreIncKeyValid = 0;
    static unsigned char PreDecKeyValid = 0;

    if ((Device.CurCommMode.CommModeBits.Local == 1) || (Device.CurCommMode.CommModeBits.Remote == 1))
    {
        return 0; 
    }
    if (IsInMenu() == 0)
    {
        if (PageFunctionIndex == Page_Normal_ID)
        {
            if ((KeyPressed[Key_Set] == KEY_PRESSED) && (PressedType[Key_Set] == LONG_KEY))
            {
                UI_Buzz_Short();
                EnterMenu(1);
            }
            else if ((KeyPressed[Key_Shut] == KEY_PRESSED) && (PressedType[Key_Shut] == LONG_KEY))
            {
                UI_Buzz_Short();
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
        if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
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
    }
    else if (pMenuPara->IsMultiplex == Multiplex_Adjust)
    {
        if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
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
    }
    else if(pMenuPara->IsMultiplex == Multiplex_Password)
    {
        if (KeyPressed[Key_Shut] == KEY_PRESSED)
        {
            KeyValid[VKey_Set] = 1;
        }
        if (KeyPressed[Key_Open] == KEY_PRESSED)
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
        if (pMenu[pMenuPara->RowIndex].SetKeyDeal != DummyFunction)
        {
            UI_Buzz_Short();
            pMenu[pMenuPara->RowIndex].SetKeyDeal(&pMenu[pMenuPara->RowIndex], pMenuPara);
        }
    }
    if (KeyValid[VKey_Back] == 1)
    {
        IsKeyPressed = 1;
        if (pMenu->ParentMenuID != InvalidMenuID)
        {
            UI_Buzz_Short();
            StandardMenu_Back2Parent(pMenu, pMenuPara);
        }
    }
    if (KeyValid[VKey_Up] == 1)
    {
        IsKeyPressed = 1;
        if (pMenu[pMenuPara->RowIndex].UpKeyDeal != DummyFunction)
        {
            UI_Buzz_Short();
            pMenu[pMenuPara->RowIndex].UpKeyDeal(&pMenu[pMenuPara->RowIndex], pMenuPara);
        }
    }
    if (KeyValid[VKey_Down] == 1)
    {
        IsKeyPressed = 1;
        if (pMenu[pMenuPara->RowIndex].DownKeyDeal != DummyFunction)
        {
            UI_Buzz_Short();
            pMenu[pMenuPara->RowIndex].DownKeyDeal(&pMenu[pMenuPara->RowIndex], pMenuPara);
        }
    }
    if (KeyValid[VKey_Inc] == 1)
    {
        IsKeyPressed = 1;
        if (pMenu[pMenuPara->RowIndex].IncKeyDeal != DummyFunction)
        {
            if (pMenuPara->IsMultiplex != Multiplex_Digit)
            {
                if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
                {
                    if (PreIncKeyValid != 1)
                    {
                        PreIncKeyValid = 1;
                        UI_Buzz_Short();
                    }
                }
                else
                {
                    UI_Buzz_Short();
                }
            }
            pMenu[pMenuPara->RowIndex].IncKeyDeal(&pMenu[pMenuPara->RowIndex], pMenuPara);
        }
    }
    else
    {
        PreIncKeyValid = 0;
    }
    if (KeyValid[VKey_Dec] == 1)
    {
        IsKeyPressed = 1;
        if (pMenu[pMenuPara->RowIndex].DecKeyDeal != DummyFunction)
        {
            if (pMenuPara->IsMultiplex != Multiplex_Digit)
            {
                if (Device.Flag.FlagBits.IsInLocalAdjust == 1)
                {
                    if (PreDecKeyValid != 1)
                    {
                        PreDecKeyValid = 1;
                        UI_Buzz_Short();
                    }
                }
                else
                {
                    UI_Buzz_Short();
                }
            }
            pMenu[pMenuPara->RowIndex].DecKeyDeal(&pMenu[pMenuPara->RowIndex], pMenuPara);
        }
    }
    else
    {
        PreDecKeyValid = 0;
    }

    if ((IsKeyPressed == 1) || (Device.Flag.FlagBits.IsInLocalAdjust == 1))
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

