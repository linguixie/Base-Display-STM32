/*******************************************************************************
* Copyright (c) 2105,威鹏自动化科技有限公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-6-13
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：
********************************************************************************/
/*******************************************************************************
*                                    头  文  件
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

/*******************************************************************************
*                               文件内部使用宏定义
********************************************************************************/
//--
#define MaxLockTime      0xFFFF
#define MinLockTime      0x00

//--
#define PasswordNum      3
#define InitPassword     0xFF

//-为了书写方便-
#define Space            GBK_Space


//-虚拟按键,对按键的封装,V是Virtual-
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
*                                 全局函数(变量)声明
********************************************************************************/
unsigned char PageFunctionIndex = Page_PowerOn_ID;
  
unsigned char NeedResetMenuPara = 0;

//-密码相关-
unsigned char Password[PasswordNum] = {0, 0, 0};
unsigned char PasswordInput[PasswordNum] = {InitPassword, InitPassword, InitPassword};
unsigned char DigitInputCount = 0;


//-此数组先前定义在ShowNumber中,但是后来发现会导致栈空间不够,
// 只能将原来局部变量改成全局变量.-
#if 1
unsigned int LongCode[16];
#endif

unsigned char IsNeedChangePage = 0;
unsigned char F_StartRecv = 1;


//-无按键延时-
signed long NoKeyTimer = 0;
//-标定成功信息提示延时-
signed long AdjustInfoTimer = 0;

#if Debug
unsigned char PrePageFunctionIndex = Page_CommTest_ID;
unsigned char NeedShowSYNC = 0;
#endif
/*******************************************************************************
*                                 静态函数(变量)声明
********************************************************************************/
const unsigned short int SeriousErrorArray[8][2][16] = 
{
    //-阀位异常(Valve Pos Err!)-
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
    //-电源缺相(Phase Lack Err!)-
    {{0xB5E7, 0xD4B4, 0xC8B1, 0xCFE0}, {0x50, 0x68, 0x61, 0x73, 0x65, 0x20, 0x4C, 0x61, 0x63, 0x6B, 0x20, 0x45, 0x72, 0x72, 0x21}}
    
};
const unsigned short int SlightErrorArray[8][2][16] = 
{
    //-关力矩故障(Open Torque Err!)-
    {{0xB9D8, 0xC1A6, 0xBED8, 0xB9CA, 0xD5CF}, {0x4F, 0x70, 0x65, 0x6E, 0x20, 0x54, 0x6F, 0x72, 0x71, 0x75, 0x65, 0x20, 0x45, 0x72, 0x72, 0x21}},
    //-开力矩故障(Shut Torque Err!)-
    {{0xBFAA, 0xC1A6, 0xBED8, 0xB9CA, 0xD5CF}, {0x53, 0x68, 0x75, 0x74, 0x20, 0x54, 0x6F, 0x72, 0x71, 0x75, 0x65, 0x20, 0x45, 0x72, 0x72, 0x21}}, 
    //-关向超时(Shut Timeout!)-
    {{0xB9D8, 0xCFF2, 0xB3AC, 0xCAB1}, {0x53, 0x68, 0x75, 0x74, 0x20, 0x54, 0x69, 0x6D, 0x65, 0x6F, 0x75, 0x74, 0x21}},
    //-开向超时(Open Timeout!)-
    {{0xBFAA, 0xCFF2, 0xB3AC, 0xCAB1}, {0x4F, 0x70, 0x65, 0x6E, 0x20, 0x54, 0x69, 0x6D, 0x65, 0x6F, 0x75, 0x74, 0x21}},
    //-关向过流(Shut OverCurrent)-
    {{0xB9D8, 0xCFF2, 0xB9FD, 0xC1F7}, {0x53, 0x68, 0x75, 0x74, 0x20, 0x4F, 0x76, 0x65, 0x72, 0x43, 0x75, 0x72, 0x72, 0x65, 0x6E, 0x74}},
    //-开向过流(Open OverCurrent)-
    {{0xBFAA, 0xCFF2, 0xB9FD, 0xC1F7}, {0x4F, 0x70, 0x65, 0x6E, 0x20, 0x4F, 0x76, 0x65, 0x72, 0x43, 0x75, 0x72, 0x72, 0x65, 0x6E, 0x74}},
    //-关向错误(Shut DIR Error!)-
    {{0xB9D8, 0xCFF2, 0xB4ED, 0xCEF3}, {0x53, 0x68, 0x75, 0x74, 0x20, 0x44, 0x49, 0x52, 0x20, 0x45, 0x72, 0x72, 0x6F, 0x72, 0x21}},
    //-开向错误(Open DIR Error!)-
    {{0xBFAA, 0xCFF2, 0xB4ED, 0xCEF3}, {0x4F, 0x70, 0x65, 0x6E, 0x20, 0x44, 0x49, 0x52, 0x20, 0x45, 0x72, 0x72, 0x6F, 0x72, 0x21}},
};



/*******************************************************************************
*                                 菜单(页面)的说明
*1、所有画面均看成菜单,包含普通菜单和特殊菜单.
*2、普通菜单示例:主菜单.
*3、特殊菜单示例:上电页面.看做是行索引固定的菜单.
*4、菜单项名称为显示的菜单项内容,用GB2312码字表示.
*5、注意特殊菜单的内容:可能来源于菜单项名称,也可能在实际的页面中赋值。
*6、菜单项有3个模板:主菜单:    新增普通菜单时参考.
*                   调零菜单:  新增含"确认"和"返回"的菜单时参考。
*                   上电页面:  新增无焦点、只有1个菜单项的菜单时参考
*7、菜单项内容只允许有效字符后面(按行,从左到右)的空格不特别定义.
*8、空格的处理,在有效字符前面的空格必须处理,后面的无需处理.例如" 威 鹏 "中的
    中共有3个空格,其中第1个空格和第2个空格必须填充0xA3A0
*9、{0}表示空白行或者占位
*10、父菜单的概念是相对于整个菜单来说,子菜单则相对于单个菜单项来说.
*11、菜单模板
//------------------ 模板-------------------------------
const MenuStructure Menu_Template[] = 
{
  //-索引号       父菜单                 子菜单                   菜单项名称-
  //--
  {0,             InvalidMenuID,         InvalidMenuID,           {0},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {1,             InvalidMenuID,         InvalidMenuID,           {0},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {2,             InvalidMenuID,         InvalidMenuID,           {0},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {3,             InvalidMenuID,         InvalidMenuID,           {0},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {3,             InvalidMenuID,         InvalidMenuID,           {0},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_Template = {0, 0, 4, 5, Multiplex_None, StandardMenu_Reset0};
********************************************************************************/


/*-------------------上电页面(行索引固定的菜单)-------------------------------*/
const MenuStructure Menu_PowerOn[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //--
  {0,             InvalidMenuID,           InvalidMenuID,          {0},
  DummyFunction,  StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-  电动执行机构-
  {1,             InvalidMenuID,           InvalidMenuID,          {{Space, Space, 0xB5E7, 0xB6AF, 0xD6B4, 0xD0D0, 0xBBFA, 0xB9B9}, {Space, Space, 0xB5E7, 0xB6AF, 0xD6B4, 0xD0D0, 0xBBFA, 0xB9B9}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {2,            InvalidMenuID,           InvalidMenuID,           {0},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-扬修    YEPEF-
  {3,            InvalidMenuID,           InvalidMenuID,           {{0xD1EF, 0xD0DE, Space, Space, Space, Space, Space, Space, 0xA3D9, 0xA3C5, 0xA3D0, 0xA3C5, 0xA3C6}, {0xD1EF, 0xD0DE, Space, Space, Space, Space, Space, Space, 0xA3D9, 0xA3C5, 0xA3D0, 0xA3C5, 0xA3C6}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_PowerOn = {0, 0, 4, 4, Multiplex_None, DummyFunction1};    


/*-------------------常显界面-------------------------------*/
const MenuStructure Menu_Normal[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //--
  {0,             InvalidMenuID,           Page_Language_ID,       {0},
  Normal_Special0,  StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {1,             InvalidMenuID,           InvalidMenuID,          {0},
  Normal_Special1, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {2,            InvalidMenuID,           InvalidMenuID,           {0},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {3,            InvalidMenuID,           InvalidMenuID,           {0},
  Normal_Special2, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_Normal = {0, 0, 4, 4, Multiplex_None, DummyFunction1}; 


/*-------------------错误界面-------------------------------*/
const MenuStructure Menu_Error[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //--
  {0,             InvalidMenuID,           Page_Language_ID,       {0},
  Opening_Special,  StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {1,             InvalidMenuID,           InvalidMenuID,          {0},
  Error_Special0, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {2,            InvalidMenuID,           InvalidMenuID,           {0},
  Error_Special1, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {3,            InvalidMenuID,           InvalidMenuID,           {0},
  Normal_Special2, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_Error = {0, 0, 4, 4, Multiplex_None, DummyFunction1};


/*------------------ 语言选择页面-------------------------------*/
const MenuStructure Menu_Language[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-语言选择(语言选择)-
  {0,             Page_Normal_ID,        InvalidMenuID,           {{0xD3EF, 0xD1D4, 0xD1A1, 0xD4F1}, {0xD3EF, 0xD1D4, 0xD1A1, 0xD4F1}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  //-Language(Language)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xA3CC, 0xA3E1, 0xA3EE, 0xA3E7, 0xA3F5, 0xA3E1, 0xA3E7, 0xA3E5}, {0xA3CC, 0xA3E1, 0xA3EE, 0xA3E7, 0xA3F5, 0xA3E1, 0xA3E7, 0xA3E5}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-    简体中文(简体中文)-
  {2,             InvalidMenuID,         Page_Configuration_ID,   {{Space, Space, Space, Space, 0xBCF2, 0xCCE5, 0xD6D0, 0xCEC4}, {Space, Space, Space, Space, 0xBCF2, 0xCCE5, 0xD6D0, 0xCEC4}},
  DummyFunction, Language_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-    ENGLISH(ENGLISH)
  {3,             InvalidMenuID,         Page_Configuration_ID,   {{Space, Space, Space, Space, 0xA3C5, 0xA3CE, 0xA3C7, 0xA3CC, 0xA3C9, 0xA3D3, 0xA3C8}, {Space, Space, Space, Space, 0xA3C5, 0xA3CE, 0xA3C7, 0xA3CC, 0xA3C9, 0xA3D3, 0xA3C8}},
  DummyFunction, Language_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_Language = {2, 2, 4, 4, Multiplex_None, StandardMenu_Reset0}; 


/*------------------ 组态页面-------------------------------*/
const MenuStructure Menu_Configuration[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-组态(Config)-
  {0,             Page_Language_ID,      InvalidMenuID,           {{0xD7E9, 0xCCAC}, {0x43, 0x6F, 0x6E, 0x66, 0x69, 0x67}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  //-参数(Para)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xB2CE, 0xCAFD}, {0x50, 0x61, 0x72, 0x61}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-  设置(Setting)-
  {2,             InvalidMenuID,         Page_Password_ID,        {{Space, Space, 0xC9E8, 0xD6C3}, {Space, Space, 0x53, 0x65, 0x74, 0x74, 0x69, 0x6E, 0x67}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-  浏览(Browse)-
  {3,             InvalidMenuID,         Page_ParaQuery_ID,       {{Space, Space, 0xE4AF, 0xC0C0}, {Space, Space, 0x42, 0x72, 0x6F, 0x77, 0x73, 0x65}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_Configuration = {2, 2, 4, 4, Multiplex_None, StandardMenu_Reset0};

 
/*------------------ 输入密码页面-------------------------------*/
const MenuStructure Menu_Password[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_Language_ID,      InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  //-输入密码(Enter Pwd)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xCAE4, 0xC8EB, 0xC3DC, 0xC2EB}, {0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x50, 0x77, 0x64}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //- * * *-
  {2,             Page_Configuration_ID, Page_ParaSetting_ID,     {0},
  Password_Special, Password_SetKey, Password_UpKey, Password_DownKey, Password_IncKey, Password_DecKey},

  //--
  {3,             InvalidMenuID,         InvalidMenuID,           {0},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_Password = {2, 2, 4, 4, Multiplex_Digit, Password_Reset};


/*------------------ 参数设置(Para Set)-------------------------------*/
const MenuStructure Menu_ParaSetting[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_Configuration_ID, InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  //-  基本(Basic)-
  {1,             InvalidMenuID,         Page_ESDSetting_ID,      {{Space, Space, 0xBBF9, 0xB1BE}, {Space, Space, 0x42, 0x61, 0x73, 0x69, 0x63}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-  终端(Tmn)-
  {2,             InvalidMenuID,         Page_Terminal_ID,        {{Space, Space, 0xD6D5, 0xB6CB}, {Space, Space, 0x54, 0x6D, 0x6E}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-  I/O(I/O)-
  {3,             InvalidMenuID,         Page_ErrorFeedBack_ID,   {{Space, Space, 0xA3C9, 0xA3AF, 0xA3CF}, {Space, Space, 0xA3C9, 0xA3AF, 0xA3CF}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-      控制(Contrl)-
  {1,             InvalidMenuID,         Page_Control_ID,         {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xBFD8, 0xD6C6}, {Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0x43, 0x6F, 0x6E, 0x74, 0x72, 0x6C}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-      信号(Signal)-
  {2,             InvalidMenuID,         Page_ANSignal_ID,        {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xD0C5, 0xBAC5}, {Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0x53, 0x69, 0x67, 0x6E, 0x61, 0x6C}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey,StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-      其他(Other)-
  {3,             InvalidMenuID,         Page_ChangePassword_ID,  {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xC6E4, 0xCBFB}, {Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0x4F, 0x74, 0x68, 0x65, 0x72}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_ParaSetting = {1, 1, 4, 7, Multiplex_None, StandardMenu_Reset0};


/*------------------ ESD设置-------------------------------*/
MenuStructure Menu_ESDSetting[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_ParaSetting_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-基本参数(Basic Para)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xBBF9, 0xB1BE, 0xB2CE, 0xCAFD}, {0x42, 0x61, 0x73, 0x69, 0x63, 0x20, 0x50, 0x61, 0x72, 0x61}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-ESD设置(ESD Setting)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xA3C5, 0xA3D3, 0xA3C4, 0xC9E8, 0xD6C3}, {0x45, 0x53, 0x44, 0x20, 0x53, 0x65, 0x74, 0x74, 0x69, 0x6E, 0x67}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-ESD禁用(ESD Disable)-
  {3,             InvalidMenuID,         Page_DeadZone_ID,        {{Space, Space, 0xA3C5, 0xA3D3, 0xA3C4, 0xBDFB, 0xD3C3}, {Space, Space, 0x45, 0x53, 0x44, 0x20, 0x44, 0x69, 0x73, 0x61, 0x62, 0x6C, 0x65}},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD禁止动作(ESD NoAction)-
  {3,             InvalidMenuID,         Page_DeadZone_ID,        {{Space, Space, 0xA3C5, 0xA3D3, 0xA3C4, 0xBDFB, 0xD6B9, 0xB6AF, 0xD7F7}, {Space, Space, 0x45, 0x53, 0x44, 0x20, 0x4E, 0x6F, 0x41, 0x63, 0x74, 0x69, 0x6F, 0x6E}},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD打开阀门(ESD Open)-
  {3,             InvalidMenuID,         Page_DeadZone_ID,        {{Space, Space, 0xA3C5, 0xA3D3, 0xA3C4, 0xB4F2, 0xBFAA, 0xB7A7, 0xC3C5}, {Space, Space, 0x45, 0x53, 0x44, 0x20, 0x4F, 0x70, 0x65, 0x6E}},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD关闭阀门(ESD Shut)-
  {3,             InvalidMenuID,         Page_DeadZone_ID,        {{Space, Space, 0xA3C5, 0xA3D3, 0xA3C4, 0xB9D8, 0xB1D5, 0xB7A7, 0xC3C5}, {Space, Space, 0x45, 0x53, 0x44, 0x20, 0x53, 0x68, 0x75, 0x74}},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD到中间位(ESD Middle)-
  {3,             InvalidMenuID,         Page_DeadZone_ID,        {{Space, Space, 0xA3C5, 0xA3D3, 0xA3C4, 0xB5BD, 0xD6D0, 0xBCE4, 0xCEBB}, {Space, Space, 0x45, 0x53, 0x44, 0x20, 0x4D, 0x69, 0x64, 0x64, 0x6C, 0x65}},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_ESDSetting = {3, 3, 4, 8, Multiplex_None, ESDSetting_Reset};


/*------------------死区-------------------------------*/
const MenuStructure Menu_DeadZone[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_ParaSetting_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-基本参数(Basic Para)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xBBF9, 0xB1BE, 0xB2CE, 0xCAFD}, {0x42, 0x61, 0x73, 0x69, 0x63, 0x20, 0x50, 0x61, 0x72, 0x61}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-控制死区(DeadZone Set)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xBFD8, 0xD6C6, 0xCBC0, 0xC7F8}, {0x44, 0x65, 0x61, 0x64, 0x5A, 0x6F, 0x6E, 0x65, 0x20, 0x53, 0x65, 0x74}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {3,             InvalidMenuID,         Page_OpenCurrent_ID,     {0},
  DeadZone_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, DeadZone_IncKey, DeadZone_DecKey},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_DeadZone = {3, 3, 4, 4, Multiplex_Digit, StandardMenu_Reset0};


/*------------------开向电流-------------------------------*/
const MenuStructure Menu_OpenCurrent[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_ParaSetting_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-基本参数(Basic Para)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xBBF9, 0xB1BE, 0xB2CE, 0xCAFD}, {0x42, 0x61, 0x73, 0x69, 0x63, 0x20, 0x50, 0x61, 0x72, 0x61}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-最大开向电流(Max Open Current)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xD7EE, 0xB4F3, 0xBFAA, 0xCFF2, 0xB5E7, 0xC1F7}, {0x4D, 0x61, 0x78, 0x20, 0x4F, 0x70, 0x65, 0x6E, 0x20, 0x43, 0x75, 0x72, 0x72, 0x65, 0x6E, 0x74}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {3,             InvalidMenuID,         Page_ShutCurrent_ID,     {0},
  OpenCurrent_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, OpenCurrent_IncKey, OpenCurrent_DecKey},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_OpenCurrent = {3, 3, 4, 4, Multiplex_Digit, StandardMenu_Reset0};


/*------------------关向电流-------------------------------*/
const MenuStructure Menu_ShutCurrent[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_ParaSetting_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-基本参数(Basic Para)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xBBF9, 0xB1BE, 0xB2CE, 0xCAFD}, {0x42, 0x61, 0x73, 0x69, 0x63, 0x20, 0x50, 0x61, 0x72, 0x61}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-最大关向电流(Max Shut Current)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xD7EE, 0xB4F3, 0xB9D8, 0xCFF2, 0xB5E7, 0xC1F7}, {0x4D, 0x61, 0x78, 0x20, 0x53, 0x68, 0x75, 0x74, 0x20, 0x43, 0x75, 0x72, 0x72, 0x65, 0x6E, 0x74}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {3,             InvalidMenuID,         Page_MaxActionTime_ID,   {0},
  ShutCurrent_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, ShutCurrent_IncKey, ShutCurrent_DecKey},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_ShutCurrent = {3, 3, 4, 4, Multiplex_Digit, StandardMenu_Reset0};


/*------------------最大动作时间-------------------------------*/
const MenuStructure Menu_MaxActionTime[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_ParaSetting_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-基本参数(Basic Para)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xBBF9, 0xB1BE, 0xB2CE, 0xCAFD}, {0x42, 0x61, 0x73, 0x69, 0x63, 0x20, 0x50, 0x61, 0x72, 0x61}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-最大开闭时间(Max Action Time)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xD7EE, 0xB4F3, 0xBFAA, 0xB1D5, 0xCAB1, 0xBCE4}, {0x4D, 0x61, 0x78, 0x20, 0x41, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x54, 0x69, 0x6D, 0x65}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {3,             InvalidMenuID,         Page_ESDSetting_ID,      {0},
  MaxActionTime_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, MaxActionTime_IncKey, MaxActionTime_DecKey},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_MaxActionTime = {3, 3, 4, 4, Multiplex_Digit, StandardMenu_Reset0};


/*------------------ 终端-------------------------------*/
const MenuStructure Menu_Terminal[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_ParaSetting_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-ESC退出!(ESC Exit!)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xA3C5, 0xA3D3, 0xA3C3, 0xCDCB, 0xB3F6, 0xA3A1}, {0x45, 0x53, 0x43, 0x20, 0x45, 0x78, 0x69, 0x74, 0x21}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-开向终端(Full Open Pos)-
  {2,             InvalidMenuID,         Page_OpenTerminal_ID,    {{Space, Space, Space, Space, 0xBFAA, 0xCFF2, 0xD6D5, 0xB6CB}, {Space, Space, 0x46, 0x75, 0x6C, 0x6C, 0x20, 0x4F, 0x70, 0x65, 0x6E, 0x20, 0x50, 0x6F, 0x73}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-关向终端(Full Shut Pos)-
  {3,             InvalidMenuID,         Page_ShutTerminal_ID,    {{Space, Space, Space, Space, 0xB9D8, 0xCFF2, 0xD6D5, 0xB6CB}, {Space, Space, 0x46, 0x75, 0x6C, 0x6C, 0x20, 0x53, 0x68, 0x75, 0x74, 0x20, 0x50, 0x6F, 0x73}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_Terminal = {2, 2, 4, 4, Multiplex_None, StandardMenu_Reset0};


/*------------------关向终端-------------------------------*/
const MenuStructure Menu_ShutTerminal[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_Terminal_ID,     InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  ValvePos_Special, ShutTerminal_SetKey, DummyFunction, DummyFunction, ShutTerminal_IncKey, ShutTerminal_DecKey},

  //-终端位置(Tmn Pos)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xD6D5, 0xB6CB, 0xCEBB, 0xD6C3}, {0x54, 0x6D, 0x6E, 0x20, 0x50, 0x6F, 0x73}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-请开到0%(Open To 0%)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xC7EB, 0xBFAA, 0xB5BD, Space, 0xA3B0, 0xA3A5}, {0x4F, 0x70, 0x65, 0x6E, 0x20, 0x54, 0x6F, 0x20, 0x30, 0x25}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-按OK确定(Confirm By OK)-
  {3,             InvalidMenuID,         InvalidMenuID,           {{Space, Space, Space, Space, 0xB0B4, 0xA3CF, 0xA3CB, 0xC8B7, 0xB6A8}, {0x43, 0x6F, 0x6E, 0x66, 0x69, 0x72, 0x6D, 0x20, 0x42, 0x79, 0x20, 0x4F, 0x4B}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_ShutTerminal = {0, 0, 4, 4, Multiplex_Adjust, StandardMenu_Reset0};


/*---------------------标定0%提示信息---------------------------------------*/
const MenuStructure Menu_AdjustZeroInfo[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //--
  {0,             Page_Terminal_ID,        InvalidMenuID,           {0},
   DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-已把当前位置-
  {1,             InvalidMenuID,           InvalidMenuID,           {0xA3A0, 0xA3A0, 0xD2D1, 0xB0D1, 0xB5B1, 0xC7B0, 0xCEBB, 0xD6C3},
   DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-成功标定为0%-
  {2,             InvalidMenuID,           InvalidMenuID,           {0xA3A0, 0xA3A0, 0xB3C9, 0xB9A6, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B0, 0xA3A5},
   DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //--
  {3,             Page_Terminal_ID,        InvalidMenuID,           {0},
   DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};   

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_AdjustZeroInfo = {0, 0, 4, 4, Multiplex_None, StandardMenu_Reset0};


/*------------------开向终端()-------------------------------*/
const MenuStructure Menu_OpenTerminal[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_Terminal_ID,      InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  ValvePos_Special, OpenTerminal_SetKey, DummyFunction, DummyFunction, OpenTerminal_IncKey, OpenTerminal_DecKey},

  //-终端位置(Tmn Pos)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xD6D5, 0xB6CB, 0xCEBB, 0xD6C3}, {0x54, 0x6D, 0x6E, 0x20, 0x50, 0x6F, 0x73}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-请开到100%(Open To 100%)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xC7EB, 0xBFAA, 0xB5BD, Space, 0xA3B1, 0xA3B0, 0xA3B0, 0xA3A5}, {0x4F, 0x70, 0x65, 0x6E, 0x20, 0x54, 0x6F, 0x20, 0x31, 0x30, 0x30, 0x25}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-按OK确定(Confirm By OK)-
  {3,             InvalidMenuID,         InvalidMenuID,           {{Space, Space, Space, Space, 0xB0B4, 0xA3CF, 0xA3CB, 0xC8B7, 0xB6A8}, {Space, 0x43, 0x6F, 0x6E, 0x66, 0x69, 0x72, 0x6D, 0x20, 0x42, 0x79, 0x20, 0x4F, 0x4B}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_OpenTerminal = {0, 0, 4, 4, Multiplex_Adjust, StandardMenu_Reset0};


/*---------------------标定100%提示信息---------------------------------------*/
const MenuStructure Menu_AdjustFullInfo[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //--
  {0,             InvalidMenuID,           InvalidMenuID,           {0},
   DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-已把当前位置-
  {1,             InvalidMenuID,           InvalidMenuID,           {0xA3A0, 0xA3A0, 0xD2D1, 0xB0D1, 0xB5B1, 0xC7B0, 0xCEBB, 0xD6C3},
   DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-成功标定为100%-
  {2,             InvalidMenuID,           InvalidMenuID,           {0xA3A0, 0xA3A0, 0xB3C9, 0xB9A6, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B1, 0xA3B0, 0xA3B0, 0xA3A5},
   DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //--
  {3,             InvalidMenuID,           InvalidMenuID,           {0},
   DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};   

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_AdjustFullInfo = {0, 0, 4, 4, Multiplex_None, StandardMenu_Reset0};   


/*------------------故障反馈触点-------------------------------*/
const MenuStructure Menu_ErrorFeedBack[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_ParaSetting_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-I/O参数(I/O Para)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xA3C9, 0xA3AF, 0xA3CF, 0xB2CE, 0xCAFD}, {0x49, 0x2F, 0x4F, 0x20, 0x50, 0x61, 0x72, 0x61}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-故障反馈触点(Fault FeedBack)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xB9CA, 0xD5CF, 0xB7B4, 0xC0A1, 0xB4A5, 0xB5E3}, {0x46, 0x61, 0x75, 0x6C, 0x74, 0x20, 0x46, 0x65, 0x65, 0x64, 0x42, 0x61, 0x63, 0x6B}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-  常开(NO)-
  {3,             InvalidMenuID,         Page_RemoteHold_ID,      {{Space, Space, 0xB3A3, 0xBFAA}, {Space, Space, 0x4E, 0x4F}},
  DummyFunction, ErrorFeedBack_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-          常闭(NC)-
  {3,             InvalidMenuID,         Page_RemoteHold_ID,      {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xB3A3, 0xB1D5}, {Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0x4E, 0x43}},
  DummyFunction, ErrorFeedBack_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_ErrorFeedBack = {3, 3, 4, 5, Multiplex_None, ErrorFeedBack_Reset};


/*------------------远程保持-------------------------------*/
const MenuStructure Menu_RemoteHold[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_ParaSetting_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-I/O参数(I/O Para)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xA3C9, 0xA3AF, 0xA3CF, 0xB2CE, 0xCAFD}, {0x49, 0x2F, 0x4F, 0x20, 0x50, 0x61, 0x72, 0x61}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-远程保持(Remote Hold)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xD4B6, 0xB3CC, 0xB1A3, 0xB3D6}, {0x52, 0x65, 0x6D, 0x6F, 0x74, 0x65, 0x20, 0x48, 0x6F, 0x6C, 0x64}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-  常开(NO)-
  {3,             Page_ParaSetting_ID,   InvalidMenuID,           {{Space, Space, 0xB3A3, 0xBFAA}, {Space, Space, 0x4E, 0x4F}},
  DummyFunction, RemoteHold_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-          常闭(NC)-
  {3,             Page_ParaSetting_ID,   InvalidMenuID,           {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xB3A3, 0xB1D5}, {Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0x4E, 0x43}},
  DummyFunction, RemoteHold_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_RemoteHold = {3, 3, 4, 5, Multiplex_None, RemoteHold_Reset};


/*------------------ 控制-------------------------------*/
const MenuStructure Menu_Control[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_ParaSetting_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-    现场控制(Local)-
  {1,             InvalidMenuID,         Page_Local_ID,           {{Space, Space, Space, 0xCFD6, 0xB3A1, 0xBFD8, 0xD6C6}, {Space, Space, Space, Space, 0x4C, 0x6F, 0x63, 0x61, 0x6C}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-    远程开关控制(Remote IO)-
  {2,             InvalidMenuID,         Page_RemoteIO_ID,        {{Space, Space, Space, 0xD4B6, 0xB3CC, 0xBFAA, 0xB9D8, 0xBFD8, 0xD6C6}, {Space, Space, Space, Space, 0x52, 0x65, 0x6D, 0x6F, 0x74, 0x65, 0x20, 0x49, 0x4F}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
  
  //-    远程模拟控制(Remote AN)-
  {3,             InvalidMenuID,         Page_RemoteAN_ID,        {{Space, Space, Space, 0xD4B6, 0xB3CC, 0xC4A3, 0xC4E2, 0xBFD8, 0xD6C6}, {Space, Space, Space, Space, 0x52, 0x65, 0x6D, 0x6F, 0x74, 0x65, 0x20, 0x41, 0x4E}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_Control = {1, 1, 4, 4, Multiplex_None, StandardMenu_Reset0};


/*------------------现场控制-------------------------------*/
const MenuStructure Menu_Local[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_Control_ID,       InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-控制参数(Control Para)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xBFD8, 0xD6C6, 0xB2CE, 0xCAFD}, {0x43, 0x6F, 0x6E, 0x74, 0x72, 0x6F, 0x6C, 0x20, 0x50, 0x61, 0x72, 0x61}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-现场控制(Local Mode)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xCFD6, 0xB3A1, 0xBFD8, 0xD6C6}, {0x4C, 0x6F, 0x63, 0x61, 0x6C, 0x20, 0x4D, 0x6F, 0x64, 0x65}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-  点动(Jog)-
  {3,             Page_Control_ID,       InvalidMenuID,           {{Space, Space, 0xB5E3, 0xB6AF}, {Space, Space, 0x4A, 0x6F, 0x67}},
  DummyFunction, Local_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-          保持(Hold)-
  {3,             Page_Control_ID,       InvalidMenuID,           {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xB1A3, 0xB3D6}, {Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0x48, 0x6F, 0x6C, 0x64}},
  DummyFunction, Local_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_Local = {3, 3, 4, 5, Multiplex_None, Local_Reset};


/*------------------远程开关控制-------------------------------*/
const MenuStructure Menu_RemoteIO[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_Control_ID,       InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-远程开关控制(Remote IO)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xD4B6, 0xB3CC, 0xBFAA, 0xB9D8, 0xBFD8, 0xD6C6}, {0x52, 0x65, 0x6D, 0x6F, 0x74, 0x65, 0x20, 0x49, 0x4F}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-  点动(Jog)-
  {2,             Page_Control_ID,         InvalidMenuID,         {{Space, Space, 0xB5E3, 0xB6AF}, {Space, Space, 0x4A, 0x6F, 0x67}},
  DummyFunction, RemoteIO_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-  保持(Hold)-
  {3,             Page_Control_ID,         InvalidMenuID,         {{Space, Space, 0xB1A3, 0xB3D6}, {Space, Space, 0x48, 0x6F, 0x6C, 0x64}},
  DummyFunction, RemoteIO_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-          有信开(Open-Sig)-
  {2,             Page_Control_ID,         InvalidMenuID,         {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space,0xD3D0, 0xD0C5, 0xBFAA}, {Space, Space, Space, Space, Space, Space, Space, Space, 0x4F, 0x70, 0x65, 0x6E, 0x2D, 0x53, 0x69, 0x67}},
  DummyFunction, RemoteIO_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-          有信关(Shut-Sig)-
  {3,             Page_Control_ID,         InvalidMenuID,         {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xD3D0, 0xD0C5, 0xB9D8}, {Space, Space, Space, Space, Space, Space, Space, Space, 0x53, 0x68, 0x75, 0x74, 0x2D, 0x53, 0x69, 0x67}},
  DummyFunction, RemoteIO_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_RemoteIO = {2, 2, 4, 6, Multiplex_None, RemoteIO_Reset};


/*------------------远程模拟控制-------------------------------*/
const MenuStructure Menu_RemoteAN[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_Control_ID,       InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-远程模拟控制(Remote AN)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xD4B6, 0xB3CC, 0xC4A3, 0xC4E2, 0xBFD8, 0xD6C6}, {0x52, 0x65, 0x6D, 0x6F, 0x74, 0x65, 0x20, 0x41, 0x4E}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-  保持(Keep)-
  {2,             Page_Control_ID,         InvalidMenuID,         {{Space, Space, 0xB1A3, 0xB3D6}, {Space, Space, 0x4B, 0x65, 0x65, 0x70}},
  DummyFunction, RemoteAN_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-  全关(Shut)-
  {3,             Page_Control_ID,         InvalidMenuID,         {{Space, Space, 0xC8AB, 0xB9D8}, {Space, Space, 0x53, 0x68, 0x75, 0x74}},
  DummyFunction, RemoteAN_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-          居中(Middle)-
  {2,             Page_Control_ID,         InvalidMenuID,         {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xBED3, 0xD6D0}, {Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0x4D, 0x69, 0x64, 0x64, 0x6C, 0x65}},
  DummyFunction, RemoteAN_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-          全开(Open)-
  {3,             Page_Control_ID,         InvalidMenuID,         {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xC8AB, 0xBFAA}, {Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0x4F, 0x70, 0x65, 0x6E}},
  DummyFunction, RemoteAN_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_RemoteAN = {2, 2, 4, 6, Multiplex_None, RemoteAN_Reset};


/*------------------信号标定-------------------------------*/
const MenuStructure Menu_ANSignal[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_ParaSetting_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-信号标定(Signal Adjust)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xD0C5, 0xBAC5, 0xB1EA, 0xB6A8}, {0x53, 0x69, 0x67, 0x6E, 0x61, 0x6C, 0x20, 0x41, 0x64, 0x6A, 0x75, 0x73, 0x74}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-  I4mA(I4mA)-
  {2,             InvalidMenuID,         Page_In4mA_ID,           {{Space, Space, 0xA3C9, 0xA3B4, 0xA3ED, 0xA3C1}, {Space, Space, 0xA3C9, 0xA3B4, 0xA3ED, 0xA3C1}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-  I20mA(I20mA)-
  {3,             InvalidMenuID,         Page_In20mA_ID,          {{Space, Space, 0xA3C9, 0xA3B2, 0xA3B0, 0xA3ED, 0xA3C1}, {Space, Space, 0xA3C9, 0xA3B2, 0xA3B0, 0xA3ED, 0xA3C1}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-          O4mA(O4mA)-
  {2,             InvalidMenuID,         Page_Out4mA_ID,          {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xA3CF, 0xA3B4, 0xA3ED, 0xA3C1}, {Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xA3CF, 0xA3B4, 0xA3ED, 0xA3C1}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-          O20mA(O20mA)-
  {3,             InvalidMenuID,         Page_Out20mA_ID,         {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xA3CF,  0xA3B2, 0xA3B0, 0xA3ED, 0xA3C1}, {Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xA3CF,  0xA3B2, 0xA3B0, 0xA3ED, 0xA3C1}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},


};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_ANSignal = {2, 2, 4, 6, Multiplex_None, StandardMenu_Reset0};


/*------------------输入4mA-------------------------------*/
const MenuStructure Menu_In4mA[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_ANSignal_ID,      InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, In4mA_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-In4mA(In4mA)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xA3C9, 0xA3EE, 0xA3B4, 0xA3ED, 0xA3C1}, {0xA3C9, 0xA3EE, 0xA3B4, 0xA3ED, 0xA3C1}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-把当前输入xx.xmA(Adjust In XX.XmA)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xB0D1, 0xB5B1, 0xC7B0, 0xCAE4, 0xC8EB}, {0x41, 0x64, 0x6A, 0x75, 0x73, 0x74, 0x20, 0x49, 0x6E, 0x20}},
  In4_20mA_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-  标定为0%(To 0%)-
  {3,             InvalidMenuID,         InvalidMenuID,           {{Space, Space, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B0, 0xA3A5}, {Space, Space, 0x54, 0x6F, 0x20, 0x30, 0x25}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_In4mA = {0, 0, 4, 4, Multiplex_None, StandardMenu_Reset0};


/*---------------------输入电流标定4mA提示信息---------------------------------------*/
const MenuStructure Menu_In4mAInfo[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //--
  {0,             Page_ANSignal_ID,        InvalidMenuID,           {0},
   DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-已把当前电流-
  {1,             InvalidMenuID,           InvalidMenuID,           {0xA3A0, 0xA3A0, 0xD2D1, 0xB0D1, 0xB5B1, 0xC7B0, 0xB5E7, 0xC1F7},
   DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-成功标定为0%-
  {2,             InvalidMenuID,           InvalidMenuID,           {0xA3A0, 0xA3A0, 0xB3C9, 0xB9A6, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B0, 0xA3A5},
   DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //--
  {3,             InvalidMenuID,           InvalidMenuID,           {0},
   DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};   

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_In4mAInfo = {0, 0, 4, 4, Multiplex_None, StandardMenu_Reset0};


/*------------------输入20mA-------------------------------*/
const MenuStructure Menu_In20mA[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_ANSignal_ID,      InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, In20mA_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-In20mA(In20mA)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xA3C9, 0xA3EE, 0xA3B2, 0xA3B0, 0xA3ED, 0xA3C1}, {0xA3C9, 0xA3EE, 0xA3B2, 0xA3B0, 0xA3ED, 0xA3C1}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-把当前输入xx.xmA(Adjust In XX.XmA)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xB0D1, 0xB5B1, 0xC7B0, 0xCAE4, 0xC8EB}, {0x41, 0x64, 0x6A, 0x75, 0x73, 0x74, 0x20, 0x49, 0x6E, 0x20}},
  In4_20mA_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-标定为100%(To 100%)-
  {3,             InvalidMenuID,         InvalidMenuID,           {{Space, Space, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B1, 0xA3B0, 0xA3B0, 0xA3A5}, {Space, Space, 0x54, 0x6F, 0x20, 0x31, 0x30, 0x30, 0x25}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_In20mA = {0, 0, 4, 4, Multiplex_None, StandardMenu_Reset0};


/*---------------------输入20mA标定提示信息---------------------------------------*/
const MenuStructure Menu_In20mAInfo[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //--
  {0,             Page_ANSignal_ID,        InvalidMenuID,           {0},
   DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-已把当前电流-
  {1,             InvalidMenuID,           InvalidMenuID,           {0xA3A0, 0xA3A0, 0xD2D1, 0xB0D1, 0xB5B1, 0xC7B0, 0xB5E7, 0xC1F7},
   DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-成功标定为100%-
  {2,             InvalidMenuID,           InvalidMenuID,           {0xA3A0, 0xA3A0, 0xB3C9, 0xB9A6, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B1, 0xA3B0, 0xA3B0, 0xA3A5},
   DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //--
  {3,             InvalidMenuID,           InvalidMenuID,           {0},
   DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};   

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_In20mAInfo = {0, 0, 4, 4, Multiplex_None, StandardMenu_Reset0};  


/*------------------输出4mA-------------------------------*/
const MenuStructure Menu_Out4mA[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_ANSignal_ID,      InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, In20mA_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-Out4mA(Out4mA)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xA3CF, 0xA3F5, 0xA3F4, 0xA3B4, 0xA3ED, 0xA3A3C1}, {0xA3CF, 0xA3F5, 0xA3F4, 0xA3B4, 0xA3ED, 0xA3A3C1}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-增大: 按打开键(Increas:Open Key)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xD4F6, 0xB4F3, 0xA3BA, 0xB0B4, 0xB4F2, 0xBFAA, 0xBCFC}, {0x49, 0x6E, 0x63, 0x72, 0x65, 0x61, 0x73, 0x3A, 0x4F, 0x70, 0x65, 0x6E, 0x20, 0x4B, 0x65, 0x79}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-减小: 按关闭键(Decreas:Shut Key)-
  {3,             Page_ANSignal_ID,      InvalidMenuID,           {{0xBCF5, 0xD0A1, 0xA3BA, 0xB0B4, 0xB9D8, 0xB1D5, 0xBCFC}, {0x44, 0x65, 0x63, 0x72, 0x65, 0x61, 0x73, 0x3A, 0x53, 0x68, 0x75, 0x74, 0x20, 0x4B, 0x65, 0x79}},
  DummyFunction, Out4mA_SetKey, DummyFunction, DummyFunction, Out4mA_IncKey, Out4mA_DecKey},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_Out4mA = {3, 3, 4, 4, Multiplex_Adjust, StandardMenu_Reset0};


/*------------------输出20mA-------------------------------*/
const MenuStructure Menu_Out20mA[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数设置(Para Set)-
  {0,             Page_ANSignal_ID,      InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, In20mA_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-Out20mA(Out20mA)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xA3CF, 0xA3F5, 0xA3F4, 0xA3B2, 0xA3B0, 0xA3ED, 0xA3A3C1}, {0xA3CF, 0xA3F5, 0xA3F4, 0xA3B2, 0xA3B0, 0xA3ED, 0xA3A3C1}}, 
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-增大: 按打开键(Increas:Open Key)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xD4F6, 0xB4F3, 0xA3BA, 0xB0B4, 0xB4F2, 0xBFAA, 0xBCFC}, {0x49, 0x6E, 0x63, 0x72, 0x65, 0x61, 0x73, 0x3A, 0x4F, 0x70, 0x65, 0x6E, 0x20, 0x4B, 0x65, 0x79}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-减小: 按关闭键(Decreas:Shut Key)-
  {3,             Page_ANSignal_ID,      InvalidMenuID,           {{0xBCF5, 0xD0A1, 0xA3BA, 0xB0B4, 0xB9D8, 0xB1D5, 0xBCFC}, {0x44, 0x65, 0x63, 0x72, 0x65, 0x61, 0x73, 0x3A, 0x53, 0x68, 0x75, 0x74, 0x20, 0x4B, 0x65, 0x79}},
  DummyFunction, Out20mA_SetKey, DummyFunction, DummyFunction, Out20mA_IncKey, Out20mA_DecKey},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_Out20mA = {3, 3, 4, 4, Multiplex_Adjust, StandardMenu_Reset0};


//------------------更改密码-------------------------------
const MenuStructure Menu_ChangePassword[] = 
{
  //-索引号       父菜单                 子菜单                   菜单项名称-
  //-参数设置(Para Set)-
  {0,             Page_ParaSetting_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xC9E8, 0xD6C3}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x53, 0x65, 0x74}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-其他参数(Other Para)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xC6E4, 0xCBFB, 0xB2CE, 0xCAFD}, {0x4F, 0x74, 0x68, 0x65, 0x72, 0x20, 0x50, 0x61, 0x72, 0x61}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-密码修改(Change Pwd)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xC3DC, 0xC2EB, 0xD0DE, 0xB8C4}, {0x43, 0x68, 0x61, 0x6E, 0x67, 0x65, 0x20, 0x50, 0x77, 0x64}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-* * *-
  {3,             Page_ParaSetting_ID,    InvalidMenuID,           {0},
  ChangePassword_Special, ChangePassword_SetKey, ChangePassword_UpKey, ChangePassword_DownKey, ChangePassword_IncKey, ChangePassword_DecKey},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_ChangePassword = {3, 3, 4, 4, Multiplex_Digit, StandardMenu_Reset0};


/*------------------ 参数浏览-------------------------------*/
const MenuStructure Menu_QueryPara[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数浏览(Para Query)-
  {0,             Page_Language_ID,      InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xE4AF, 0xC0C0}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  //-  基本(Basic)-
  {1,             InvalidMenuID,         Page_QueryESDSetting_ID, {{Space, Space, 0xBBF9, 0xB1BE}, {Space, Space, 0x42, 0x61, 0x73, 0x69, 0x63}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-  I/O(I/O)-
  {2,             InvalidMenuID,         Page_QueryErrorFeedBack_ID, {{Space, Space, 0xA3C9, 0xA3AF, 0xA3CF}, {Space, Space, 0xA3C9, 0xA3AF, 0xA3CF}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-      控制(Contrl)-
  {3,             InvalidMenuID,         Page_QueryControl_ID,    {{Space, Space, 0xBFD8, 0xD6C6}, {Space, Space, 0x43, 0x6F, 0x6E, 0x74, 0x72, 0x6C}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-      故障(Error)-
  {1,             InvalidMenuID,         Page_QueryError_ID,       {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xB9CA, 0xD5CF}, {Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0x45, 0x72, 0x72, 0x6F, 0x72}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-      其他(Other)-
  {2,             InvalidMenuID,         Page_QueryPassword_ID,   {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xC6E4, 0xCBFB}, {Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0x4F, 0x74, 0x68, 0x65, 0x72}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_QueryPara = {1, 1, 4, 6, Multiplex_None, StandardMenu_Reset0}; 


/*------------------ ESD设置-------------------------------*/
MenuStructure Menu_QueryESDSetting[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数浏览(Para Query)-
  {0,             Page_ParaQuery_ID,   InvalidMenuID,             {{0xB2CE, 0xCAFD, 0xE4AF, 0xC0C0}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-基本参数(Basic Para)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xBBF9, 0xB1BE, 0xB2CE, 0xCAFD}, {0x42, 0x61, 0x73, 0x69, 0x63, 0x20, 0x50, 0x61, 0x72, 0x61}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-ESD设置(ESD Setting)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xA3C5, 0xA3D3, 0xA3C4, 0xC9E8, 0xD6C3}, {0x45, 0x53, 0x44, 0x20, 0x53, 0x65, 0x74, 0x74, 0x69, 0x6E, 0x67}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-ESD禁用(ESD Disable)-
  {3,             InvalidMenuID,         Page_QueryDeadZone_ID,   {{Space, Space, 0xA3C5, 0xA3D3, 0xA3C4, 0xBDFB, 0xD3C3}, {Space, Space, 0x45, 0x53, 0x44, 0x20, 0x44, 0x69, 0x73, 0x61, 0x62, 0x6C, 0x65}},
  ESDSetting_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-ESD禁止动作(ESD NoAction)-
  {3,             InvalidMenuID,         Page_QueryDeadZone_ID,    {{Space, Space, 0xA3C5, 0xA3D3, 0xA3C4, 0xBDFB, 0xD6B9, 0xB6AF, 0xD7F7}, {Space, Space, 0x45, 0x53, 0x44, 0x20, 0x4E, 0x6F, 0x41, 0x63, 0x74, 0x69, 0x6F, 0x6E}},
  ESDSetting_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-ESD打开阀门(ESD Open)-
  {3,             InvalidMenuID,         Page_QueryDeadZone_ID,    {{Space, Space, 0xA3C5, 0xA3D3, 0xA3C4, 0xB4F2, 0xBFAA, 0xB7A7, 0xC3C5}, {Space, Space, 0x45, 0x53, 0x44, 0x20, 0x4F, 0x70, 0x65, 0x6E}},
  ESDSetting_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-ESD关闭阀门(ESD Shut)-
  {3,             InvalidMenuID,         Page_QueryDeadZone_ID,    {{Space, Space, 0xA3C5, 0xA3D3, 0xA3C4, 0xB9D8, 0xB1D5, 0xB7A7, 0xC3C5}, {Space, Space, 0x45, 0x53, 0x44, 0x20, 0x53, 0x68, 0x75, 0x74}},
  ESDSetting_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-ESD到中间位(ESD Middle)-
  {3,             InvalidMenuID,         Page_QueryDeadZone_ID,    {{Space, Space, 0xA3C5, 0xA3D3, 0xA3C4, 0xB5BD, 0xD6D0, 0xBCE4, 0xCEBB}, {Space, Space, 0x45, 0x53, 0x44, 0x20, 0x4D, 0x69, 0x64, 0x64, 0x6C, 0x65}},
  ESDSetting_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_QueryESDSetting = {3, 3, 4, 7, Multiplex_None, ESDSetting_Reset};


/*------------------死区-------------------------------*/
const MenuStructure Menu_QueryDeadZone[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数浏览(Para Query)-
  {0,             Page_ParaQuery_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xE4AF, 0xC0C0}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-基本参数(Basic Para)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xBBF9, 0xB1BE, 0xB2CE, 0xCAFD}, {0x42, 0x61, 0x73, 0x69, 0x63, 0x20, 0x50, 0x61, 0x72, 0x61}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-控制死区(DeadZone Set)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xBFD8, 0xD6C6, 0xCBC0, 0xC7F8}, {0x44, 0x65, 0x61, 0x64, 0x5A, 0x6F, 0x6E, 0x65, 0x20, 0x53, 0x65, 0x74}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {3,             InvalidMenuID,         Page_QueryOpenCurrent_ID,     {0},
  DeadZone_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_QueryDeadZone = {3, 3, 4, 4, Multiplex_Digit, StandardMenu_Reset0};


/*------------------开向电流-------------------------------*/
const MenuStructure Menu_QueryOpenCurrent[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数浏览(Para Query)-
  {0,             Page_ParaQuery_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xE4AF, 0xC0C0}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-基本参数(Basic Para)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xBBF9, 0xB1BE, 0xB2CE, 0xCAFD}, {0x42, 0x61, 0x73, 0x69, 0x63, 0x20, 0x50, 0x61, 0x72, 0x61}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-最大开向电流(Max Open Current)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xD7EE, 0xB4F3, 0xBFAA, 0xCFF2, 0xB5E7, 0xC1F7}, {0x4D, 0x61, 0x78, 0x20, 0x4F, 0x70, 0x65, 0x6E, 0x20, 0x43, 0x75, 0x72, 0x72, 0x65, 0x6E, 0x74}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {3,             InvalidMenuID,         Page_QueryShutCurrent_ID,     {0},
  OpenCurrent_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_QueryOpenCurrent = {3, 3, 4, 4, Multiplex_Digit, StandardMenu_Reset0};


/*------------------关向电流-------------------------------*/
const MenuStructure Menu_QueryShutCurrent[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数浏览(Para Query)-
  {0,             Page_ParaQuery_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xE4AF, 0xC0C0}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-基本参数(Basic Para)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xBBF9, 0xB1BE, 0xB2CE, 0xCAFD}, {0x42, 0x61, 0x73, 0x69, 0x63, 0x20, 0x50, 0x61, 0x72, 0x61}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-最大关向电流(Max Shut Current)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xD7EE, 0xB4F3, 0xB9D8, 0xCFF2, 0xB5E7, 0xC1F7}, {0x4D, 0x61, 0x78, 0x20, 0x53, 0x68, 0x75, 0x74, 0x20, 0x43, 0x75, 0x72, 0x72, 0x65, 0x6E, 0x74}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {3,             InvalidMenuID,         Page_QueryMaxActionTime_ID,   {0},
  ShutCurrent_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_QueryShutCurrent = {3, 3, 4, 4, Multiplex_Digit, StandardMenu_Reset0};


/*------------------最大动作时间-------------------------------*/
const MenuStructure Menu_QueryMaxActionTime[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数浏览(Para Query)-
  {0,             Page_ParaQuery_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xE4AF, 0xC0C0}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-基本参数(Basic Para)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xBBF9, 0xB1BE, 0xB2CE, 0xCAFD}, {0x42, 0x61, 0x73, 0x69, 0x63, 0x20, 0x50, 0x61, 0x72, 0x61}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-最大开闭时间(Max Action Time)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xD7EE, 0xB4F3, 0xBFAA, 0xB1D5, 0xCAB1, 0xBCE4}, {0x4D, 0x61, 0x78, 0x20, 0x41, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x54, 0x69, 0x6D, 0x65}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {3,             InvalidMenuID,         Page_QueryESDSetting_ID,      {0},
  MaxActionTime_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_QueryMaxActionTime = {3, 3, 4, 4, Multiplex_Digit, StandardMenu_Reset0};


/*------------------故障反馈触点-------------------------------*/
const MenuStructure Menu_QueryErrorFeedBack[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数浏览(Para Query)-
  {0,             Page_ParaQuery_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xE4AF, 0xC0C0}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-I/O参数(I/O Para)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xA3C9, 0xA3AF, 0xA3CF, 0xB2CE, 0xCAFD}, {0x49, 0x2F, 0x4F, 0x20, 0x50, 0x61, 0x72, 0x61}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-故障反馈触点(Fault FeedBack)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xB9CA, 0xD5CF, 0xB7B4, 0xC0A1, 0xB4A5, 0xB5E3}, {0x46, 0x61, 0x75, 0x6C, 0x74, 0x20, 0x46, 0x65, 0x65, 0x64, 0x42, 0x61, 0x63, 0x6B}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-  常开(NO)-
  {3,             InvalidMenuID,         Page_QueryRemoteHold_ID,      {{Space, Space, 0xB3A3, 0xBFAA}, {Space, Space, 0x4E, 0x4F}},
  DummyFunction, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-          常闭(NC)-
  {3,             InvalidMenuID,         Page_QueryRemoteHold_ID,      {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xB3A3, 0xB1D5}, {Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0x4E, 0x43}},
  DummyFunction, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_QueryErrorFeedBack = {3, 3, 4, 5, Multiplex_None, ErrorFeedBack_Reset};


/*------------------远程保持-------------------------------*/
const MenuStructure Menu_QueryRemoteHold[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数浏览(Para Query)-
  {0,             Page_ParaQuery_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xE4AF, 0xC0C0}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-I/O参数(I/O Para)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xA3C9, 0xA3AF, 0xA3CF, 0xB2CE, 0xCAFD}, {0x49, 0x2F, 0x4F, 0x20, 0x50, 0x61, 0x72, 0x61}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-远程保持(Remote Hold)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xD4B6, 0xB3CC, 0xB1A3, 0xB3D6}, {0x52, 0x65, 0x6D, 0x6F, 0x74, 0x65, 0x20, 0x48, 0x6F, 0x6C, 0x64}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-  常开(NO)-
  {3,             Page_ParaSetting_ID,   InvalidMenuID,           {{Space, Space, 0xB3A3, 0xBFAA}, {Space, Space, 0x4E, 0x4F}},
  DummyFunction, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-          常闭(NC)-
  {3,             Page_ParaSetting_ID,   InvalidMenuID,           {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xB3A3, 0xB1D5}, {Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0x4E, 0x43}},
  DummyFunction, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_QueryRemoteHold = {3, 3, 4, 5, Multiplex_None, RemoteHold_Reset};


/*------------------ 控制-------------------------------*/
const MenuStructure Menu_QueryControl[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数浏览(Para Query)-
  {0,             Page_ParaQuery_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xE4AF, 0xC0C0}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-    现场控制(Local)-
  {1,             InvalidMenuID,         Page_QueryLocal_ID,    {{Space, Space, Space, 0xCFD6, 0xB3A1, 0xBFD8, 0xD6C6}, {Space, Space, Space, Space, 0x4C, 0x6F, 0x63, 0x61, 0x6C}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-    远程开关控制(Remote IO)-
  {2,             InvalidMenuID,         Page_QueryRemoteIO_ID, {{Space, Space, Space, 0xD4B6, 0xB3CC, 0xBFAA, 0xB9D8, 0xBFD8, 0xD6C6}, {Space, Space, Space, Space, 0x52, 0x65, 0x6D, 0x6F, 0x74, 0x65, 0x20, 0x49, 0x4F}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
  
  //-    远程模拟控制(Remote AN)-
  {3,             InvalidMenuID,         Page_QueryRemoteAN_ID, {{Space, Space, Space, 0xD4B6, 0xB3CC, 0xC4A3, 0xC4E2, 0xBFD8, 0xD6C6}, {Space, Space, Space, Space, 0x52, 0x65, 0x6D, 0x6F, 0x74, 0x65, 0x20, 0x41, 0x4E}},
  DummyFunction, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_QueryControl = {1, 1, 4, 4, Multiplex_None, StandardMenu_Reset0};


/*------------------现场控制-------------------------------*/
const MenuStructure Menu_QueryLocal[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数浏览(Para Query)-
  {0,             Page_QueryControl_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xE4AF, 0xC0C0}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-控制参数(Control Para)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xBFD8, 0xD6C6, 0xB2CE, 0xCAFD}, {0x43, 0x6F, 0x6E, 0x74, 0x72, 0x6F, 0x6C, 0x20, 0x50, 0x61, 0x72, 0x61}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-现场控制(Local Mode)-
  {2,             InvalidMenuID,         InvalidMenuID,           {{0xCFD6, 0xB3A1, 0xBFD8, 0xD6C6}, {0x4C, 0x6F, 0x63, 0x61, 0x6C, 0x20, 0x4D, 0x6F, 0x64, 0x65}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-  点动(Jog)-
  {3,             Page_QueryControl_ID,       InvalidMenuID,           {{Space, Space, 0xB5E3, 0xB6AF}, {Space, Space, 0x4A, 0x6F, 0x67}},
  DummyFunction, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-          保持(Hold)-
  {3,             Page_QueryControl_ID,       InvalidMenuID,           {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xB1A3, 0xB3D6}, {Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0x48, 0x6F, 0x6C, 0x64}},
  DummyFunction, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_QueryLocal = {3, 3, 4, 5, Multiplex_None, Local_Reset};


/*------------------远程开关控制-------------------------------*/
const MenuStructure Menu_QueryRemoteIO[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数浏览(Para Query)-
  {0,             Page_QueryControl_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xE4AF, 0xC0C0}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-远程开关控制(Remote IO)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xD4B6, 0xB3CC, 0xBFAA, 0xB9D8, 0xBFD8, 0xD6C6}, {0x52, 0x65, 0x6D, 0x6F, 0x74, 0x65, 0x20, 0x49, 0x4F}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-  点动(Jog)-
  {2,             Page_QueryControl_ID,         InvalidMenuID,         {{Space, Space, 0xB5E3, 0xB6AF}, {Space, Space, 0x4A, 0x6F, 0x67}},
  DummyFunction, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-  保持(Hold)-
  {3,             Page_QueryControl_ID,         InvalidMenuID,         {{Space, Space, 0xB1A3, 0xB3D6}, {Space, Space, 0x48, 0x6F, 0x6C, 0x64}},
  DummyFunction, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-          有信开(Open-Sig)-
  {2,             Page_QueryControl_ID,         InvalidMenuID,         {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space,0xD3D0, 0xD0C5, 0xBFAA}, {Space, Space, Space, Space, Space, Space, Space, Space, 0x4F, 0x70, 0x65, 0x6E, 0x2D, 0x53, 0x69, 0x67}},
  DummyFunction, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-          有信关(Shut-Sig)-
  {3,             Page_QueryControl_ID,         InvalidMenuID,         {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xD3D0, 0xD0C5, 0xB9D8}, {Space, Space, Space, Space, Space, Space, Space, Space, 0x53, 0x68, 0x75, 0x74, 0x2D, 0x53, 0x69, 0x67}},
  DummyFunction, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_QueryRemoteIO = {2, 2, 4, 6, Multiplex_None, RemoteIO_Reset};


/*------------------远程模拟控制-------------------------------*/
const MenuStructure Menu_QueryRemoteAN[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-参数浏览(Para Query)-
  {0,             Page_QueryControl_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xE4AF, 0xC0C0}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-远程模拟控制(Remote AN)-
  {1,             InvalidMenuID,         InvalidMenuID,           {{0xD4B6, 0xB3CC, 0xC4A3, 0xC4E2, 0xBFD8, 0xD6C6}, {0x52, 0x65, 0x6D, 0x6F, 0x74, 0x65, 0x20, 0x41, 0x4E}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-  保持(Keep)-
  {2,             Page_QueryControl_ID,         InvalidMenuID,         {{Space, Space, 0xB1A3, 0xB3D6}, {Space, Space, 0x4B, 0x65, 0x65, 0x70}},
  DummyFunction, RemoteAN_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-  全关(Shut)-
  {3,             Page_QueryControl_ID,         InvalidMenuID,         {{Space, Space, 0xC8AB, 0xB9D8}, {Space, Space, 0x53, 0x68, 0x75, 0x74}},
  DummyFunction, RemoteAN_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-          居中(Middle)-
  {2,             Page_QueryControl_ID,         InvalidMenuID,         {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xBED3, 0xD6D0}, {Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0x4D, 0x69, 0x64, 0x64, 0x6C, 0x65}},
  DummyFunction, RemoteAN_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-          全开(Open)-
  {3,             Page_QueryControl_ID,         InvalidMenuID,         {{Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0xC8AB, 0xBFAA}, {Space, Space, Space, Space, Space, Space, Space, Space, Space, Space, 0x4F, 0x70, 0x65, 0x6E}},
  DummyFunction, RemoteAN_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_QueryRemoteAN = {2, 2, 4, 6, Multiplex_None, RemoteAN_Reset};


//------------------更改密码-------------------------------
const MenuStructure Menu_QueryPassword[] = 
{
  //-索引号       父菜单                 子菜单                   菜单项名称-
  //-参数浏览(Para Query)-
  {0,             Page_ParaQuery_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xE4AF, 0xC0C0}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-其他查询(Other Query)-
  {1,             InvalidMenuID,         InvalidMenuID,         {{0xC6E4, 0xCBFB, 0xB2E9, 0xD1AF}, {0x4F, 0x74, 0x68, 0x65, 0x72, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-密码查询(Pwd Query)-
  {2,             InvalidMenuID,         InvalidMenuID,          {{0xC3DC, 0xC2EB, 0xB2E9, 0xD1AF}, {0x50, 0x77, 0x64, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-* * *-
  {3,             Page_ParaQuery_ID,    InvalidMenuID,           {0},
  ChangePassword_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_QueryPassword = {3, 3, 4, 4, Multiplex_None, StandardMenu_Reset0};


//------------------查询错误-------------------------------
const MenuStructure Menu_QueryError[] = 
{
  //-索引号       父菜单                 子菜单                   菜单项名称-
  //-参数浏览(Para Query)-
  {0,             Page_ParaQuery_ID,   InvalidMenuID,           {{0xB2CE, 0xCAFD, 0xE4AF, 0xC0C0}, {0x50, 0x61, 0x72, 0x61, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79}},
  Opening_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-故障(Error)-
  {1,             InvalidMenuID,         InvalidMenuID,         {{0xB9CA, 0xD5CF}, {0x45, 0x72, 0x72, 0x6F, 0x72}},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {2,             InvalidMenuID,         InvalidMenuID,         {0},
  QueryError_Special, QueryError_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-0-
  {3,             InvalidMenuID,         InvalidMenuID,         {0},
  DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第4个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_QueryError = {2, 2, 4, 4, Multiplex_None, StandardMenu_Reset0};


#if Debug
/*---------------------通讯测试-----------------------------------------------*/
const MenuStructure Menu_CommTest[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-发-
  {0,             InvalidMenuID,           InvalidMenuID,           {{0xB7A2}, 0},
  CommTest_Special, CommTest_SetKey, CommTest_IncKey, CommTest_DecKey, DummyFunction, DummyFunction},

  //--
  {1,             InvalidMenuID,           InvalidMenuID,           {0},
  CommTest_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-收-
  {2,             InvalidMenuID,           InvalidMenuID,           {{0xCAD5}, {0}},
  CommTest_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {3,             InvalidMenuID,           InvalidMenuID,           {0},
  CommTest_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction}
};  

//-第6个初始值代表菜单项个数,必须给正确的值- 
MenuPara      MenuPara_CommTest = {0, 0, 4, 4, Multiplex_None, StandardMenu_Reset0}; 
#endif


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void DummyFunction(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void DummyFunction1(MenuPara *pMenuPara)
{
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ShowArrovw(int X_Offset)
{
    unsigned char FontBuf[32] = {0};
    unsigned char FontBufLen = 0;
  
    unsigned int Code = 0;

    //-补上"->"- 
    Code = 0xA1FA;
    GetDotData(Code, CN_Character, &FontBuf[0], &FontBufLen);
    SwapFontBuf2DisplayBuf(&FontBuf[0], FontBufLen, &g_DisplayBuf[0], X_Offset);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ShowFocus(const struct MenuStructure *pMenu, MenuPara *pMenuPara)
{
    int i = 0;
    int X_Offset = 0;

    while((pMenu->MenuName[Device.Para.LanguageType][i++] == Space));

    //-没有预留焦点符号的空间-
    if (--i < 2)
    {
        return;
    }

    //-X_Offset = (i - 2) * EN_X_SIZE-
    X_Offset = (i- 2) << EN_X_SIZE_Shift_Count;

    ShowArrovw(X_Offset);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ShowStar(int X_Offset)
{
    unsigned char FontBuf[32] = {0};
    unsigned char FontBufLen = 0;
  
    unsigned int Code = 0;

    //-补上"*"- 
    Code = 0xA3AA;
    GetDotData(Code, EN_Character, &FontBuf[0], &FontBufLen);
    SwapFontBuf2DisplayBuf(&FontBuf[0], FontBufLen, &g_DisplayBuf[0], X_Offset);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void GetSingleNumber(unsigned int Data, unsigned char *pGe, unsigned char *pShi, unsigned char *pBai, unsigned char *pQian, unsigned char *pWan)
{
    signed long TempData = Data;    //-必须进行转换,否则溢出-
    
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
        if (pCode[i] == 0x00)    //-无效:如果是空格则是0xA3A0-
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
* 函数名称:    ShowBlock
* 函数功能:    根据字符码字填充显示缓冲
* 输入参数:    *pCode,码字缓冲
*              CodeLen,码字缓冲长度
*              X_Offset,水平偏移0~128
* 输出参数:    无
* 返 回 值:    无
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
        if (pCode[i] == 0x00)    //-无效:如果是空格则是0xA3A0-
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
* 函数名称:    ShowNumbers
* 函数功能:    显示数字
* 输入参数:    Value,要显示的值(去除小数点后的值)
*              BitCount,显示的位数(去除小数点后的位数,例如0.5的位数为2)
*              DecimalBitCount,小数点位数
*              X_Offset,显示的水平偏移0~128
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ShowNumbers(unsigned int Value, int BitCount, int DecimalBitCount, int X_Offset)
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

    if (BitCount > 5)    //-至多显示十进制的5位数-
    {
        return;
    }
    if (DecimalBitCount >= BitCount)
    {
        return;
    }

    //-万-
    if (WanValue == 0)
    {
        LongCode[0] = Space;
    }
    else
    {

        LongCode[0] = WanValue + 0xA3B0;
    }

    //-千-
    if ((QianValue == 0) && (WanValue == 0))
    {
        LongCode[1] = Space;
    }
    else
    {
        LongCode[1] = QianValue + 0xA3B0;
    }

    //-百-
    if ((BaiValue == 0) && (QianValue == 0) && (WanValue == 0))
    {
        LongCode[2] = Space;
    }
    else 
    {
        LongCode[2] = BaiValue + 0xA3B0;
    }

    //-十-
    if ((ShiValue == 0) && (BaiValue == 0) && (QianValue == 0) && (WanValue == 0))
    {
        LongCode[3] = Space;
    }
    else
    {
        LongCode[3] = ShiValue + 0xA3B0;
    } 

    //-个-
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
            //-要判断显示的位数-
            ShowBlock(&LongCode[5 - BitCount], BitCount, X_Offset);
        }
        else
        {
            LongCode[DecimalIndex] = 0xA3AE;    //-小数点-
    
            //-要判断显示的位数-
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
            //-==0时,显示0,所以要去除小数点所占的位置-
            ShowBlock(&LongCode[5 - BitCount], BitCount, X_Offset + 8);
        }
    }
}


/*******************************************************************************
* 函数名称:    ShowOpening
* 函数功能:    显示开度XX.X%
* 输入参数:    Opening,开度值
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ShowOpening(unsigned int Opening)
{
    ShowNumbers(Opening, 3, 0, 96);

    ClearBuf((unsigned char *)&LongCode, sizeof(LongCode));
    LongCode[0] = 0xA3A5;        //-%-
    ShowBlock(&LongCode[0], 1, 120);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
unsigned int GetNumOfErrors(void)
{
    int i = 0;
    unsigned char SeriousError = 0;
    unsigned char SlightError = 0;
    unsigned int ErrorCount = 0;

    SeriousError = Device.Error.ErrorByte;
    for (i = 0; i < 8; i++)
    {
        if ((SeriousError >> i & 0x01) == 1)
        {
            ErrorCount++;
        }
    }

    SlightError = Valve.Error.ErrorByte;
    for (i = 0; i < 8; i++)
    {
        if ((SlightError >> i & 0x01) == 1)
        {
            ErrorCount++;
        }
    }

    return ErrorCount;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Opening_Special(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    ShowOpening(Valve.MiscInfo.CurOpening);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ValvePos_Special(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    unsigned int DisplayBufIndex = 0;

    DisplayBufIndex = 80;
    ShowNumbers(Valve.MiscInfo.PositionADValue, 5, 0, DisplayBufIndex);

    ClearBuf((unsigned char *)&LongCode, sizeof(LongCode));
    LongCode[0] = 0xA3A3;        //-#-
    ShowBlock(&LongCode[0], 1, 120);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Normal_Special0(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    int i = 0;
    int DisplayBufIndex = 0;

    signed int CurOpening = 0;
    
    unsigned char DummyData = 0;
    unsigned char BaiValue = 0;
    unsigned char ShiValue = 0;
    unsigned char GeValue =  0;

    if ((Device.Error.ErrorByte != 0) || (Valve.Error.ErrorByte != 0))
    {
        Show_Page(Page_Error_ID);
        NeedResetMenuPara = 1;
        IsNeedChangePage = 1;

        return;
    }


    /*--------------------------------第一行------------------------------------*/
    ClearBuf(&g_DisplayBuf[0], Display_Buf_Size); 

    //-----------当前开度特殊字符上半部分的处理-----------
    CurOpening = Valve.MiscInfo.CurOpening;
    GetSingleNumber(CurOpening, &GeValue, &ShiValue, &BaiValue, &DummyData, &DummyData);
 
    //-百位-
    DisplayBufIndex = 16;
    if (BaiValue == 0)
    {     
        DisplayBufIndex += 16;
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
    //-十位-
    if ((ShiValue == 0) && (BaiValue == 0))
    {  
        DisplayBufIndex += 16;         
    }
    else
    {
        for (i = 0; i < 16; i++)
        {
            g_DisplayBuf[DisplayBufIndex]       = SpecicalCharacter[ShiValue][0][i];
            g_DisplayBuf[DisplayBufIndex + 128] = SpecicalCharacter[ShiValue][1][i];

            DisplayBufIndex++;
        }
    }
    //-个位-
    for (i = 0; i < 16; i++)
    {
        g_DisplayBuf[DisplayBufIndex]       = SpecicalCharacter[GeValue][0][i];
        g_DisplayBuf[DisplayBufIndex + 128] = SpecicalCharacter[GeValue][1][i];
        
        DisplayBufIndex++;
    }
    //-%-
    DisplayBufIndex += 16;
    for (i = 0; i < 16; i++)
    {
        g_DisplayBuf[DisplayBufIndex]       = SpecicalCharacter[PercentIndex][0][i];
        g_DisplayBuf[DisplayBufIndex + 128] = SpecicalCharacter[PercentIndex][1][i];

        DisplayBufIndex++;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Normal_Special1(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    int i = 0;
    int DisplayBufIndex = 0;

    signed int CurOpening = 0;
    
    unsigned char DummyData = 0;
    unsigned char BaiValue = 0;
    unsigned char ShiValue = 0;
    unsigned char GeValue =  0;


    /*--------------------------------第二行------------------------------------*/
    ClearBuf(&g_DisplayBuf[0], Display_Buf_Size);

    //-----------当前开度特殊字符下半部分的处理-----------
    CurOpening = Valve.MiscInfo.CurOpening;
    GetSingleNumber(CurOpening, &GeValue, &ShiValue, &BaiValue, &DummyData, &DummyData);
    //-百位-
    DisplayBufIndex = 16;
    if (BaiValue == 0)
    { 
        DisplayBufIndex += 16;  
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
    //-十位-
    if ((ShiValue == 0) && (BaiValue == 0))
    {   
        DisplayBufIndex += 16;          
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
    //-个位-
    for (i = 0; i < 16; i++)
    {
        g_DisplayBuf[DisplayBufIndex]         = SpecicalCharacter[GeValue][2][i];
        g_DisplayBuf[DisplayBufIndex + 128] = SpecicalCharacter[GeValue][3][i];
        
        DisplayBufIndex++;
    }
    //-%-
    DisplayBufIndex += 16;
    for (i = 0; i < 16; i++)
    {
        g_DisplayBuf[DisplayBufIndex]         = SpecicalCharacter[PercentIndex][2][i];
        g_DisplayBuf[DisplayBufIndex + 128] = SpecicalCharacter[PercentIndex][3][i];

        DisplayBufIndex++;
    }
} 


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Normal_Special2(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    int Index = 0;
    unsigned int DisplayBufIndex0 = 0;
    unsigned int DisplayBufIndex1 = 0;

    ClearBuf((unsigned char *)&LongCode, sizeof(LongCode));

    //-模拟-
    if (Device.Status.CurMode == ControlMode_RemoteAN)
    {
        if ((Valve.Status.StatusBits.NoSignal == 1) && (Device.Para.RemoteANMode == RemoteANMode_NoSigKeep))
        {
            //-左部分-
            DisplayBufIndex0 = 0;
    
            if (Device.Para.LanguageType == Language_EN)
            {
                //-Remote-
                LongCode[Index++] = 0x52;    
                LongCode[Index++] = 0x65;    
                LongCode[Index++] = 0x6D;    
                LongCode[Index++] = 0x6F;   
                LongCode[Index++] = 0x74;    
                LongCode[Index++] = 0x65;   
            }
            else
            {
                LongCode[Index++] = 0xD4B6;    //-远-
                LongCode[Index++] = 0xB3CC;    //-程-
            }
            
            ShowBlock(&LongCode[0], Index, DisplayBufIndex0);
    
            //-右部分-
            Index = 0;
            DisplayBufIndex1 = 80;
            ClearBuf((unsigned char *)&LongCode, sizeof(LongCode));
    
            if (Device.Para.RemoteIOMode == RemoteIOMode_Jog)
            {
                if (Device.Para.LanguageType == Language_EN)
                {
                    //-Jog-
                    LongCode[Index++] = 0x4A;    
                    LongCode[Index++] = 0x6F;    
                    LongCode[Index++] = 0x67;      
                }
                else
                {	
                    LongCode[Index++] = 0xB5E3;    //-点-
                    LongCode[Index++] = 0xB6AF;    //-动-
                }
            }
            else if (Device.Para.RemoteIOMode == RemoteIOMode_Hold)
            {
                if (Device.Para.LanguageType == Language_EN)
                {
                    //-Hold-
                    LongCode[Index++] = 0x48;    
                    LongCode[Index++] = 0x6F;    
                    LongCode[Index++] = 0x6C;   
                    LongCode[Index++] = 0x64; 
                }
                else
                {
                    LongCode[Index++] = 0xB1A3;    //-保-
                    LongCode[Index++] = 0xB3D6;    //-持-
                }
            } 
            else
            {
                if (Device.Para.LanguageType == Language_EN)
                {
                    //-Bi-Pos-
                    LongCode[Index++] = 0x48;    
                    LongCode[Index++] = 0x6F;    
                    LongCode[Index++] = 0x6C;   
                    LongCode[Index++] = 0x64; 
                }
                else
                {
                    LongCode[Index++] = 0xCBAB;    //-双-
                    LongCode[Index++] = 0xCEBB;    //-位-
                }
            }
    
            ShowBlock(&LongCode[0], Index, DisplayBufIndex1);
        }
        else
        {
            if (Device.Para.LanguageType == Language_EN)
            {
                //-Remote Analog-
                LongCode[Index++] = 0x52;    
                LongCode[Index++] = 0x65;    
                LongCode[Index++] = 0x6D;    
                LongCode[Index++] = 0x6F;   
                LongCode[Index++] = 0x74;    
                LongCode[Index++] = 0x65;  
                LongCode[Index++] = 0x20;    
                LongCode[Index++] = 0x41;    
                LongCode[Index++] = 0x6E;    
                LongCode[Index++] = 0x61;   
                LongCode[Index++] = 0x6C;    
                LongCode[Index++] = 0x6F; 
                LongCode[Index++] = 0x67;  
     
                DisplayBufIndex0 = 16;
            }
            else
            {
                //-远程模拟-
                LongCode[Index++] = 0xD4B6;    
                LongCode[Index++] = 0xB3CC;    
                LongCode[Index++] = 0xC4A3;    
                LongCode[Index++] = 0xC4E2;
    
                DisplayBufIndex0 = 40;   
            }
    
            ShowBlock(&LongCode[0], Index, DisplayBufIndex0);
        }
    }

    //-远程-
    else if ((Device.Status.CurMode == ControlMode_RemoteJog) || (Device.Status.CurMode == ControlMode_RemoteHold) || (Device.Status.CurMode == ControlMode_RemoteDibit))
    {

        //-左部分-
        DisplayBufIndex0 = 0;

        if (Device.Para.LanguageType == Language_EN)
        {
            //-Remote-
            LongCode[Index++] = 0x52;    
            LongCode[Index++] = 0x65;    
            LongCode[Index++] = 0x6D;    
            LongCode[Index++] = 0x6F;   
            LongCode[Index++] = 0x74;    
            LongCode[Index++] = 0x65;   
        }
        else
        {
            LongCode[Index++] = 0xD4B6;    //-远-
            LongCode[Index++] = 0xB3CC;    //-程-
        }
        
        ShowBlock(&LongCode[0], Index, DisplayBufIndex0);

        //-右部分-
        Index = 0;
        DisplayBufIndex1 = 80;
        ClearBuf((unsigned char *)&LongCode, sizeof(LongCode));

        if (Device.Status.CurMode == ControlMode_RemoteJog)
        {
            if (Device.Para.LanguageType == Language_EN)
            {
                //-Jog-
                LongCode[Index++] = 0x4A;    
                LongCode[Index++] = 0x6F;    
                LongCode[Index++] = 0x67;      
            }
            else
            {	
                LongCode[Index++] = 0xB5E3;    //-点-
                LongCode[Index++] = 0xB6AF;    //-动-
            }
        }
        else if (Device.Status.CurMode == ControlMode_RemoteHold)
        {
            if (Device.Para.LanguageType == Language_EN)
            {
                //-Hold-
                LongCode[Index++] = 0x48;    
                LongCode[Index++] = 0x6F;    
                LongCode[Index++] = 0x6C;   
                LongCode[Index++] = 0x64; 
            }
            else
            {
                LongCode[Index++] = 0xB1A3;    //-保-
                LongCode[Index++] = 0xB3D6;    //-持-
            }
        } 
        else
        {
            if (Device.Para.LanguageType == Language_EN)
            {
                //-Bi-Pos-
                LongCode[Index++] = 0x48;    
                LongCode[Index++] = 0x6F;    
                LongCode[Index++] = 0x6C;   
                LongCode[Index++] = 0x64; 
            }
            else
            {
                LongCode[Index++] = 0xCBAB;    //-双-
                LongCode[Index++] = 0xCEBB;    //-位-
            }
        }

        ShowBlock(&LongCode[0], Index, DisplayBufIndex1);
    }

    //-现场-
    else if ((Device.Status.CurMode == ControlMode_LocalJog) || (Device.Status.CurMode == ControlMode_LocalHold))
    {
        //-左部分-
        DisplayBufIndex0 = 0;

        if (Device.Para.LanguageType == Language_EN)
        {
            //-Local-
            LongCode[Index++] = 0x4C;    
            LongCode[Index++] = 0x6F;    
            LongCode[Index++] = 0x63;    
            LongCode[Index++] = 0x61;   
            LongCode[Index++] = 0x6C;    
        }
        else
        {
            LongCode[Index++] = 0xCFD6;    //-现-
            LongCode[Index++] = 0xB3A1;    //-场-
        }

        ShowBlock(&LongCode[0], Index, DisplayBufIndex0);


        //-右部分-
        Index = 0;
        DisplayBufIndex1 = 80; 
        ClearBuf((unsigned char *)&LongCode, sizeof(LongCode));
       
        if (Device.Status.CurMode == ControlMode_LocalHold)
        {
            if (Device.Para.LanguageType == Language_EN)
            {
                //-Hold-
                LongCode[Index++] = 0x48;    
                LongCode[Index++] = 0x6F;    
                LongCode[Index++] = 0x6C;   
                LongCode[Index++] = 0x64; 
            }
            else
            {
                LongCode[Index++] = 0xB1A3;    //-保-
                LongCode[Index++] = 0xB3D6;    //-持-
            }

        }
        else
        {
            if (Device.Para.LanguageType == Language_EN)
            {
                //-Jog-
                LongCode[Index++] = 0x4A;    
                LongCode[Index++] = 0x6F;    
                LongCode[Index++] = 0x67;      
            }
            else
            {
                LongCode[Index++] = 0xB5E3;    //-点-
                LongCode[Index++] = 0xB6AF;    //-动-
            }
        } 

        ShowBlock(&LongCode[0], Index, DisplayBufIndex1);
    }
    else
    {
        DisplayBufIndex0 = 40;

        if (Device.Para.LanguageType == Language_EN)
        {
            //-Stop-
            LongCode[Index++] = 0x53;    
            LongCode[Index++] = 0x74;    
            LongCode[Index++] = 0x6F;    
            LongCode[Index++] = 0x70;      
        }
        else
        {
            LongCode[Index++] = 0xCDA3;    //-停-
            LongCode[Index++] = Space;
            LongCode[Index++] = 0xD6B9;    //-止-
        }

        ShowBlock(&LongCode[0], Index, DisplayBufIndex0);
    }
} 


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Error_Special0(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    int Index = 0;
    int DisplayBufIndex =0;
    unsigned int ErrorCount = 0;

    ErrorCount = GetNumOfErrors();

    if (ErrorCount == 0)
    {
        Show_Page(Page_Normal_ID);
        NeedResetMenuPara = 1;
        IsNeedChangePage = 1;

        return;
    }

    DisplayBufIndex = 32;
    ShowNumbers(ErrorCount, 2, 0, DisplayBufIndex);
    DisplayBufIndex += 16;
    
    ClearBuf((unsigned char *)&LongCode, sizeof(LongCode));
    if (Device.Para.LanguageType == Language_EN)
    {
        //- Errors-
        LongCode[Index++] = 0x20;    
        LongCode[Index++] = 0x45;    
        LongCode[Index++] = 0x72;    
        LongCode[Index++] = 0x72;    
        LongCode[Index++] = 0x6F;    
        LongCode[Index++] = 0x72;    
        LongCode[Index++] = 0x73;    
    }
    else
    {
        LongCode[Index++] = 0xD6D6;    //-种-
        LongCode[Index++] = 0xB9CA;    //-故-
        LongCode[Index++] = 0xD5CF;    //-障-
    }
    ShowBlock(&LongCode[0], Index, DisplayBufIndex);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Error_Special1(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    int Index = 0;
    int i = 0;
    int j = 0;
    int DisplayBufIndex = 0;

    unsigned char SeriousError = 0;
    unsigned char SlightError  = 0;
    unsigned char Error        = 0;
    const unsigned short (*pError)[2][16];


    SeriousError = Device.Error.ErrorByte;
    SlightError = Valve.Error.ErrorByte;

    ClearBuf((unsigned char *)&LongCode, sizeof(LongCode));


    if (SeriousError != 0)
    {
        Error = SeriousError;
        pError = SeriousErrorArray;
    }
    else
    {
        Error = SlightError;
        pError = SlightErrorArray;
    }


    for (i = 7; i >= 0; i--)
    {
        if (Error & (0x01 << i))
        {
            for (j = 0; j < 16; j++)
            {
                LongCode[Index++] = pError[i][Device.Para.LanguageType][j];
            }

            break;
        }
    }

    if (Device.Para.LanguageType == Language_EN)
    {
        DisplayBufIndex = 0;
    }
    else
    {
        DisplayBufIndex = 32;
    }

    ShowBlock(&LongCode[0], Index, DisplayBufIndex);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void StandardMenu_Reset0(MenuPara *pMenuPara)
{
    if (pMenuPara->IsMultiplex == Multiplex_Digit)
    {
        DigitInputCount = 0;
    }
    pMenuPara->FocusMenuIndex = pMenuPara->FocusStartMenuIndex;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void StandardMenu_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (pMenu->ChildMenuID != InvalidMenuID)
    {
        NeedResetMenuPara = 1;
        IsNeedChangePage = 1;
        Show_Page(pMenu->ChildMenuID);
    }
    else if (pMenu->ParentMenuID != InvalidMenuID)
    {
        IsNeedChangePage = 1;
        Show_Page(pMenu->ParentMenuID);
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void StandardMenu_UpKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (pMenuPara->FocusMenuIndex == pMenuPara->FocusStartMenuIndex)
    {
        pMenuPara->FocusMenuIndex = pMenuPara->NumOfItems - 1;
    }
    else
    {
        pMenuPara->FocusMenuIndex--;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void StandardMenu_DownKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    pMenuPara->FocusMenuIndex++;
    if (pMenuPara->FocusMenuIndex >= pMenuPara->NumOfItems)
    {
        pMenuPara->FocusMenuIndex = pMenuPara->FocusStartMenuIndex;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Language_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    unsigned char PreLanguage = 0;
    
    PreLanguage = Device.Para.LanguageType;

    if (pMenuPara->FocusMenuIndex == 3)
    {
        Device.Para.LanguageType = Language_EN;
    }
    else
    {
        Device.Para.LanguageType = Language_CN;
    }

    StandardMenu_SetKey(pMenu, pMenuPara);

    if (PreLanguage != Device.Para.LanguageType)
    {
        //-写入EEPROM-
        STM_EEPROM_Write((unsigned char *)&Device.Para.LanguageType, EEPROM_ADDR_LANGUAGETYPE, sizeof(Device.Para.LanguageType));
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Password_Reset(MenuPara *pMenuPara)
{
    int i = 0;

    for (i = 0; i < PasswordNum; i++)
    {
        PasswordInput[i] = InitPassword;
    }
   
    DigitInputCount = 0;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Password_Special(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    int i = 0;
    int DisplayBufIndex = 0;
    unsigned int Code = 0;

    DisplayBufIndex = 32;
    for (i = 0; i < PasswordNum; i++)
    {
        if (DigitInputCount == i)
        {
            Code = GBK_Triangle;
        }
        else
        {
            Code = Space;
        }

        ShowBlock(&Code, 1, DisplayBufIndex);
        DisplayBufIndex += EN_X_SIZE;


        if (PasswordInput[i] == InitPassword)
        {
            Code = 0xA3AA;
        }
        else
        {
            Code = PasswordInput[i] + 0xA3B0;
        }
        
        ShowBlock(&Code, 1, DisplayBufIndex);
        DisplayBufIndex += EN_X_SIZE;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Password_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    int i = 0;

    if (DigitInputCount < PasswordNum - 1)
    {
        if (PasswordInput[DigitInputCount] != InitPassword)
        {
            DigitInputCount++;
        }
    }
    else if (PasswordInput[DigitInputCount] != InitPassword)
    {
        for(i = 0; i < PasswordNum; i++)
        {
            if (PasswordInput[i] != Password[i])
            {
                //-返回上一级-
                StandardMenu_Back2Parent(pMenu, pMenuPara);
                return;
            }
        }

        if (i == PasswordNum)
        {
            //-返回下一级-
            StandardMenu_Set2Child(pMenu, pMenuPara);
            
            return;
        }
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Password_UpKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (DigitInputCount !=  0)
    {
        DigitInputCount--;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Password_DownKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (DigitInputCount < PasswordNum - 1)
    {
        if (PasswordInput[DigitInputCount] != InitPassword)
        {
            DigitInputCount++;
        }
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Password_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (PasswordInput[DigitInputCount] == InitPassword)
    {
        PasswordInput[DigitInputCount] = 0;
    }
    else if (PasswordInput[DigitInputCount] == 0x09)
    {
        PasswordInput[DigitInputCount] = 0;
    }
    else 
    {
        PasswordInput[DigitInputCount]++;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Password_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (PasswordInput[DigitInputCount] == InitPassword)
    {
        PasswordInput[DigitInputCount] = 0x09;
    }
    else if (PasswordInput[DigitInputCount] == 0)
    {
        PasswordInput[DigitInputCount] = 0x09;
    }
    else 
    {
        PasswordInput[DigitInputCount]--;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ESDSetting_Reset(MenuPara *pMenuPara)
{
    StandardMenu_Reset0(pMenuPara);

    switch(Device.Para.ESDMode)
    {
    case ESD_Disable:
        pMenuPara->FocusMenuIndex = 3;
        break;
    case ESD_NoAction:
        pMenuPara->FocusMenuIndex = 4;
        break;
    case ESD_Open:
        pMenuPara->FocusMenuIndex = 5;
        break;
    case ESD_Shut:
        pMenuPara->FocusMenuIndex = 6;
        break;
    case ESD_Middle:
        pMenuPara->FocusMenuIndex = 7;
        break;
    default:
        break;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ESDSetting_Special(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    int j = 0;
    int Index = 0;

    int MenuNameIndex   = 0;
    int DisplayBufIndex = 0;
         
    unsigned int  Code = 0;

    ClearBuf(&g_DisplayBuf[0], Display_Buf_Size);
    ClearBuf((unsigned char *)&LongCode, sizeof(LongCode));
    for (j = 0; j < 16;     )    //-把j++放到循环体中-
    {
        Code = Menu_ESDSetting[pMenuPara->FocusMenuIndex].MenuName[Device.Para.LanguageType][MenuNameIndex++];
        LongCode[Index++] = Code;
        if (((Code >> 8) & 0xFF) >= 0xB0)    //-中文字符-
        {
  
            j += 2;             //!!!!j在循环体中累加
        }
        else
        {

            j += 1;             //!!!!j在循环体中累加
        }        
    }

    ShowBlock(&LongCode[0], Index++, DisplayBufIndex);

    ShowFocus(&Menu_ESDSetting[pMenuPara->FocusMenuIndex], pMenuPara);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ESDSetting_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    switch(pMenuPara->FocusMenuIndex)
    {
    case 3:
        Device.Para.ESDMode = ESD_Disable;
        break;
    case 4:
        Device.Para.ESDMode = ESD_NoAction;
        break;
    case 5:
        Device.Para.ESDMode = ESD_Open;
        break;
    case 6:
        Device.Para.ESDMode = ESD_Shut;
        break;
    case 7:
        Device.Para.ESDMode = ESD_Middle;
        break;
    default:
        break;
    }
     
    StandardMenu_SetKey(pMenu, pMenuPara);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void DeadZone_Special(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    int Index = 0;

    unsigned int  DeadZone = 0;
    unsigned char DummyData = 0;
    unsigned char GeValue = 0;
    unsigned char ShiValue = 0;
    unsigned char BaiValue = 0;
 
    int DisplayBufIndex = 0;

    ClearBuf((unsigned char *)&LongCode, sizeof(LongCode));
    
    //-范围:1~20-
    if (Device.Para.DeadZone <= 20)
    {
        DeadZone = (Device.Para.DeadZone << 2) + Device.Para.DeadZone;
    }

    
    if (DeadZone < 5)
    {
        if (Device.Para.LanguageType == Language_EN)
        {
            //-Self Adpat-
            LongCode[Index++] = 0x53;
            LongCode[Index++] = 0x65;
            LongCode[Index++] = 0x6C;
            LongCode[Index++] = 0x66;
            LongCode[Index++] = 0x20;
            LongCode[Index++] = 0x41;
            LongCode[Index++] = 0x64;
            LongCode[Index++] = 0x61;
            LongCode[Index++] = 0x70;
            LongCode[Index++] = 0x74;
        }
        else
        {
            //-自适应-
            LongCode[Index++] = 0xD7D4;
            LongCode[Index++] = 0xCACA;
            LongCode[Index++] = 0xD3A6;
        }

        DisplayBufIndex = 48;
    }
    else
    {
        GetSingleNumber(DeadZone, &GeValue, &ShiValue, &BaiValue, &DummyData, &DummyData);
        if (BaiValue == 0)
        {
            LongCode[Index++] = 0xA3A0;               //-空格-
        }
        else
        {
            LongCode[Index++] = BaiValue + 0xA3B0;
        }

        LongCode[Index++] = ShiValue + 0xA3B0;
        LongCode[Index++] = 0xA3AE;                   //-小数点-

        LongCode[Index++] = GeValue + 0xA3B0;
        LongCode[Index++] = 0xA3A5;                   //-%-
        DisplayBufIndex = 48;
    }

    ShowBlock(&LongCode[0], Index, DisplayBufIndex);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void OpenCurrent_Special(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    unsigned int  Current = 0;
 
    ClearBuf((unsigned char *)&LongCode, sizeof(LongCode));
    
    //-范围:5-250 代表0.5-25 A
    Current = Device.Para.MaxOpenCurrent;
    if (Current < 5)
    {
        Current = 5;
    }
    else if (Current > 250)
    {
        Current = 250;
    }

    ShowNumbers(Current, 3, 1, 48);

    ClearBuf((unsigned char *)&LongCode, sizeof(LongCode));
    LongCode[0] = 0xA3C1;        //-A-
    ShowBlock(&LongCode[0], 1, 80);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void OpenCurrent_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (Device.Para.MaxOpenCurrent <= 5)
    {
        Device.Para.MaxOpenCurrent = 250;  
    }
    else
    {
        Device.Para.MaxOpenCurrent--;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ShutCurrent_Special(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    unsigned int  Current = 0;
 
    ClearBuf((unsigned char *)&LongCode, sizeof(LongCode));
    
    //-范围:5-250 代表0.5-25 A
    Current = Device.Para.MaxShutCurrent;
    if (Current < 5)
    {
        Current = 5;
    }
    else if (Current > 250)
    {
        Current = 250;
    }

    ShowNumbers(Current, 3, 1, 48);

    ClearBuf((unsigned char *)&LongCode, sizeof(LongCode));
    LongCode[0] = 0xA3C1;        //-A-
    ShowBlock(&LongCode[0], 1, 80);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ShutCurrent_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (Device.Para.MaxShutCurrent <= 5)
    {
        Device.Para.MaxShutCurrent = 250;  
    }
    else
    {
        Device.Para.MaxShutCurrent--;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void MaxActionTime_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    //-范围:0、[5~250]-
    if (Device.Para.MaxActionTime <= 0)
    {
        Device.Para.MaxActionTime = 5;  
    }
    else if(Device.Para.MaxActionTime >= 250)
    {
        Device.Para.MaxActionTime = 0;
    }
    else
    {
        Device.Para.MaxActionTime++;
    } 
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void MaxActionTime_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    //-范围:0、[5~250]-
    if (Device.Para.MaxActionTime <= 0)
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void MaxActionTime_Special(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    int Index = 0;
    unsigned int  ActionTime = 0;
 
    ClearBuf((unsigned char *)&LongCode, sizeof(LongCode));
    
    //-范围:5-250 代表5-250 S
    ActionTime = Device.Para.MaxActionTime;
    if (ActionTime > 250)
    {
        ActionTime = 250;
    }

    if (ActionTime < 5)
    {
        if (Device.Para.LanguageType == Language_EN)
        {
            LongCode[Index++] = 0x55;                  //-U-
            LongCode[Index++] = 0x6E;                  //-n-
            LongCode[Index++] = 0x6C;                  //-l-
            LongCode[Index++] = 0x69;                  //-i-
            LongCode[Index++] = 0x6D;                  //-m-
            LongCode[Index++] = 0x69;                  //-i-
            LongCode[Index++] = 0x74;                  //-t-
            LongCode[Index++] = 0x65;                  //-e-
            LongCode[Index++] = 0x64;                  //-d-

        }
        else
        {    
            LongCode[Index++] = 0xB2BB;                  //-不-
            LongCode[Index++] = 0xCFDE;                  //-限-
            LongCode[Index++] = 0xCAB1;                  //-时-
        }

        ShowBlock(&LongCode[0], Index, 48);
    }
    else
    {
        ShowNumbers(ActionTime, 3, 0, 56);

        ClearBuf((unsigned char *)&LongCode, sizeof(LongCode));
        LongCode[0] = 0xA3D3;        //-S-
        ShowBlock(&LongCode[0], 1, 80);
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ShutTerminal_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    Valve.Adjust.Adjust1.Adjust1Byte = 0;
    Valve.Adjust.Adjust0.Adjust0Bits.Zero = 1;
    Valve.Adjust.Adjust0.Adjust0Bits.Full = 0;

	F_WaitAdjustFrame	= 1;
    SetTimer(WaitReplyTimer, WaitReply_Delay);
    CurrentAdjustType = Frame_AdjustZero;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ShutTerminal_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    OpenTerminal_IncKey(pMenu, pMenuPara);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ShutTerminal_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    OpenTerminal_DecKey(pMenu, pMenuPara);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void OpenTerminal_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    Valve.Adjust.Adjust1.Adjust1Byte = 0;
    Valve.Adjust.Adjust0.Adjust0Bits.Zero = 0;
    Valve.Adjust.Adjust0.Adjust0Bits.Full = 1;

	F_WaitAdjustFrame	= 1;
    SetTimer(WaitReplyTimer, WaitReply_Delay);
    CurrentAdjustType = Frame_AdjustFull;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void OpenTerminal_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
  
    Valve.Operation.Operation = Operation_NoLimitOpen;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void OpenTerminal_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    Valve.Operation.Operation = Operation_NoLimitShut;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ErrorFeedBack_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    switch(pMenuPara->FocusMenuIndex)
    {
    case 3:
        Device.Para.ErrorFeedBack = IO_NormallyOpen;
        break;
    case 4:
        Device.Para.ErrorFeedBack = IO_NormallyShut;
        break;
    default:
        break;
    }
    StandardMenu_SetKey(pMenu, pMenuPara);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ErrorFeedBack_Reset(MenuPara *pMenuPara)
{
    StandardMenu_Reset0(pMenuPara);

    switch(Device.Para.ErrorFeedBack)
    {
    case IO_NormallyOpen:
        pMenuPara->FocusMenuIndex = 3;
        break;
    case IO_NormallyShut:
        pMenuPara->FocusMenuIndex = 4;
        break;
    default:
        break;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void RemoteHold_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    switch(pMenuPara->FocusMenuIndex)
    {
    case 3:
        Device.Para.RemoteHold = IO_NormallyOpen;
        break;
    case 4:
        Device.Para.RemoteHold = IO_NormallyShut;
        break;
    default:
        break;
    }
    StandardMenu_Back2Parent(pMenu, pMenuPara);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void RemoteHold_Reset(MenuPara *pMenuPara)
{
    StandardMenu_Reset0(pMenuPara);

    switch(Device.Para.RemoteHold)
    {
    case IO_NormallyOpen:
        pMenuPara->FocusMenuIndex = 3;
        break;
    case IO_NormallyShut:
        pMenuPara->FocusMenuIndex = 4;
        break;
    default:
        break;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Local_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    switch(pMenuPara->FocusMenuIndex)
    {
    case 3:
        Device.Para.LocalMode = LocalMode_Jog;
        break;
    case 4:
        Device.Para.LocalMode = LocalMode_Hold;
        break;
    default:
        break;
    }
    StandardMenu_Back2Parent(pMenu, pMenuPara);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Local_Reset(MenuPara *pMenuPara)
{
    StandardMenu_Reset0(pMenuPara);

    switch(Device.Para.LocalMode)
    {
    case LocalMode_Jog:
        pMenuPara->FocusMenuIndex = 3;
        break;
    case LocalMode_Hold:
        pMenuPara->FocusMenuIndex = 4;
        break;
    default:
        break;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void RemoteIO_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    switch(pMenuPara->FocusMenuIndex)
    {
    case 2:
        Device.Para.RemoteIOMode = RemoteIOMode_Jog;
        break;
    case 3:
        Device.Para.RemoteIOMode = RemoteIOMode_Hold;
        break;
    case 4:
        Device.Para.RemoteIOMode = RemoteIOMode_SignalOnNoOff;
        break;
    case 5:
        Device.Para.RemoteIOMode = RemoteIOMode_SignalOffNoOn;
        break;
    default:
        break;
    }
    StandardMenu_Back2Parent(pMenu, pMenuPara);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void RemoteIO_Reset(MenuPara *pMenuPara)
{
    StandardMenu_Reset0(pMenuPara);

    switch(Device.Para.RemoteIOMode)
    {
    case RemoteIOMode_Jog:
        pMenuPara->FocusMenuIndex = 2;
        break;
    case RemoteIOMode_Hold:
        pMenuPara->FocusMenuIndex = 3;
        break;
    case RemoteIOMode_SignalOnNoOff:
        pMenuPara->FocusMenuIndex = 4;
        break;
    case RemoteIOMode_SignalOffNoOn:
        pMenuPara->FocusMenuIndex = 5;
        break;
    default:
        break;
    }    
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void RemoteAN_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    switch(pMenuPara->FocusMenuIndex)
    {
    case 2:
        Device.Para.RemoteANMode = RemoteANMode_NoSigKeep;
        break;
    case 3:
        Device.Para.RemoteANMode = RemoteANMode_NoSigShut;
        break;
    case 4:
        Device.Para.RemoteANMode = RemoteANMode_NoSigMid;
        break;
    case 5:
        Device.Para.RemoteANMode = RemoteANMode_NoSigOpen;
        break;
    default:
        break;
    }
    StandardMenu_Back2Parent(pMenu, pMenuPara);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void RemoteAN_Reset(MenuPara *pMenuPara)
{
    StandardMenu_Reset0(pMenuPara);

    switch(Device.Para.RemoteANMode)
    {
    case RemoteANMode_NoSigKeep:
        pMenuPara->FocusMenuIndex = 2;
        break;
    case RemoteANMode_NoSigShut:
        pMenuPara->FocusMenuIndex = 3;
        break;
    case RemoteANMode_NoSigMid:
        pMenuPara->FocusMenuIndex = 4;
        break;
    case RemoteANMode_NoSigOpen:
        pMenuPara->FocusMenuIndex = 5;
        break;
    default:
        break;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void In4_20mA_Special(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    int Index = 0;
    int DisplayBufIndex = 0;

    DisplayBufIndex = 80;
    ShowNumbers(Valve.MiscInfo.In4_20mA, 3, 1, DisplayBufIndex);
    
    ClearBuf((unsigned char *)&LongCode[0], sizeof(LongCode));
    LongCode[Index++] = 0xA3ED;
    LongCode[Index++] = 0xA3C1;
    ShowBlock(&LongCode[0], Index, DisplayBufIndex + 32);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void In4mA_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    Valve.Adjust.Adjust0.Adjust0Byte = 0;
    Valve.Adjust.Adjust1.Adjust1Bits.Input4mA  = 1;
    Valve.Adjust.Adjust1.Adjust1Bits.Input20mA = 0;

	F_WaitAdjustFrame	= 1;
    SetTimer(WaitReplyTimer, WaitReply_Delay);
    CurrentAdjustType = Frame_AdjustInput4mA;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void In20mA_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    Valve.Adjust.Adjust0.Adjust0Byte = 0;
    Valve.Adjust.Adjust1.Adjust1Bits.Input4mA  = 0;
    Valve.Adjust.Adjust1.Adjust1Bits.Input20mA = 1;

	F_WaitAdjustFrame	= 1;
    SetTimer(WaitReplyTimer, WaitReply_Delay);
    CurrentAdjustType = Frame_AdjustInput20mA;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Out4mA_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    Valve.Adjust.Adjust1.Adjust1Bits.IncOutput4mA = 1;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Out4mA_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    Valve.Adjust.Adjust1.Adjust1Bits.DecOutput4mA = 1;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Out4mA_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    StandardMenu_Back2Parent(pMenu, pMenuPara);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Out20mA_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    Valve.Adjust.Adjust1.Adjust1Bits.IncOutput20mA = 1;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Out20mA_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    Valve.Adjust.Adjust1.Adjust1Bits.DecOutput20mA = 1;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Out20mA_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    StandardMenu_Back2Parent(pMenu, pMenuPara);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ChangePassword_Special(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    int i = 0;
    int DisplayBufIndex = 0;
    unsigned int Code = 0;

    DisplayBufIndex = 32;
    for (i = 0; i < PasswordNum; i++)
    {
        if (DigitInputCount == i)
        {
            Code = GBK_Triangle;
        }
        else
        {
            Code = Space;
        }

        ShowBlock(&Code, 1, DisplayBufIndex);
        DisplayBufIndex += EN_X_SIZE;


        if (Password[i] == InitPassword)
        {
            Code = 0xA3AA;
        }
        else
        {
            Code = Password[i] + 0xA3B0;
        }
        
        ShowBlock(&Code, 1, DisplayBufIndex);
        DisplayBufIndex += EN_X_SIZE;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ChangePassword_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (DigitInputCount < PasswordNum - 1)
    {
        if (Password[DigitInputCount] != InitPassword)
        {
            DigitInputCount++;
        }
    }
    else
    {
        StandardMenu_Back2Parent(pMenu, pMenuPara);
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ChangePassword_UpKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (DigitInputCount == 0)
    { 
        DigitInputCount = PasswordNum - 1;
    }
    else
    {
        DigitInputCount--;
    }
 
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ChangePassword_DownKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (DigitInputCount == PasswordNum - 1)
    { 
        DigitInputCount = 0;
    }
    else
    {
        DigitInputCount++;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ChangePassword_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (Password[DigitInputCount] == InitPassword)
    {
        Password[DigitInputCount] = 0;
    }
    else if (Password[DigitInputCount] == 0x09)
    {
        Password[DigitInputCount] = 0;
    }
    else 
    {
        Password[DigitInputCount]++;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ChangePassword_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (Password[DigitInputCount] == InitPassword)
    {
        Password[DigitInputCount] = 0x09;
    }
    else if (Password[DigitInputCount] == 0)
    {
        Password[DigitInputCount] = 0x09;
    }
    else 
    {
        Password[DigitInputCount]--;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void QueryError_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    unsigned int ErrorNum = 0;

    ErrorNum = GetNumOfErrors();

    if (Device.MiscInfo.ErrorIndex == ErrorNum - 1)
    {
        Device.MiscInfo.ErrorIndex = 0;
    }
    else
    {
        Device.MiscInfo.ErrorIndex++;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void QueryError_Special(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    int i = 0;
    int j = 0;
    int Index = 0;
    int DisplayBufIndex = 0;
    uint16 Error = 0;
    uint32 ErrorCount = 0;
    const unsigned short (*pError)[2][16];

    Error = Device.Error.ErrorByte;
    Error <<= 8;
    Error += Valve.Error.ErrorByte;

    ClearBuf((unsigned char *)&LongCode[0], sizeof(LongCode));

    DisplayBufIndex = 0;
    for (i = 15; i >= 0; i--)
    {
        if (i >= 8)
        {
            pError = &SeriousErrorArray[i - 8];
        }
        else
        {
            pError = &SlightErrorArray[i];
        }
        if (Error & (0x01 << i))
        {
            ErrorCount++;
        }
        if (ErrorCount == Device.MiscInfo.ErrorIndex + 1)
        {
            for (j = 0; j < 16; j++)
            {
                LongCode[Index++] = pError[0][Device.Para.LanguageType][j];
            }

            break;
        }
    }
    
    if (Device.Para.LanguageType == Language_EN)
    {
        DisplayBufIndex = 0;
    }
    else
    {
        DisplayBufIndex = 32;
    }

    ShowBlock(&LongCode[0], Index, DisplayBufIndex);
}


#if Debug
/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void CommTest_Special(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
#if 0
    if (F_Sending == 1) 
    {
        return;
    }

#if 0
    SendBuf[0] = 0x12;
    SendBuf[1] = 0x34;
    SendBuf[2] = 0x56;
    SendBuf[3] = 0x78;
    SendBuf[4] = 0x90;
    SendBuf[5] = 0x00;
    SendBuf[6] = 0x00;
    SendBuf[7] = 0x00;
    SendBuf[8] = 0x00;
    SendBuf[9] = 0x00;
#endif
#endif

#if Debug
    int i = 0;
    int j = 0;

    unsigned int Code[2] = {0};

    int DisplayBufIndex = 0;

    unsigned char *pStartBuf;

    if (F_StartRecv == 0)
    {
        return;
    }
    //if (F_Sending == 1)
    {
        //return;
    }
   
    if(pMenu->RowIndex == 0)
    {
        pStartBuf = &SendBuf[0];
    }
    else if (pMenu->RowIndex == 1)
    {
        pStartBuf = &SendBuf[5];
    }
    else if (pMenu->RowIndex == 2)
    {
        pStartBuf = &RecvBuf[0];
    }
    else
    {
        pStartBuf = &RecvBuf[5];
    }

    DisplayBufIndex = 16;
    for (i = 0; i < 5; i++)
    {
        for (j = 1; j >= 0; j--)
        {
            Code[j] = ((pStartBuf[i] >> (4 * j)) & 0x0F);
            if ((Code[j] >= 10) && (Code[j] <= 0x0F))
            {
                Code[j] -= 10;
                Code[j] += 0xA3C1;
            }
            else if (Code[j] <= 9)
            {
                Code[j] += 0xA3B0;         
            }
            else
            {
                continue;
            }
            ShowBlock(&Code[j], 1, DisplayBufIndex);
            DisplayBufIndex += EN_X_SIZE;
        }

        DisplayBufIndex += EN_X_SIZE;    //-空一个空格-
    }
#endif
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void CommTest_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
#if 0
    if (F_StartRecv == 0)
    {
        F_StartRecv = 1;
    }
    else
    {
        F_StartRecv = 0;
    }
#endif
    //NeedShowSYNC = !NeedShowSYNC;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void CommTest_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
#if 0
    if (ShowDebugInfo >= Max_ShowDebugInfo)
    {
        ShowDebugInfo = 0;
    }
    else
    {
        ShowDebugInfo++;
    }
#endif
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void CommTest_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
#if 0
    if (ShowDebugInfo <= 0)
    {
        ShowDebugInfo = Max_ShowDebugInfo;
    }
    else
    {
        ShowDebugInfo--;
    }
#endif
}
#endif


/*******************************************************************************
* 函数名称:    KeyProc
* 函数功能:    处理按键
* 输入参数:    
* 输出参数:    无
* 返 回 值:    0,不需要重画页面
*              1,需要重画页面
*******************************************************************************/
unsigned char KeyProc(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    int i = 0;
    unsigned char IsKeyPressed = 0;
    unsigned char KeyValid[VKey_Num] = {0};

    if (IsInMenu() == 0)
    {
        return 0; 
    }

    /*-Keys of IR Remoter-*/
    for (i = 0; i < VKey_Num; i++)
    {
        if (KeyPressed[Key_Open + i] == KEY_PRESSED)
        {
            KeyValid[i] = 1;
        }
    }
	if (pMenuPara->IsMultiplex == Multiplex_Adjust)
	{
        //-这里是直接判高低电平-
		if (KeyStateRead(Key_Open) == KEY_PRESSED)
		{
			KeyValid[VKey_Inc] = 1;
		}
        //-这里是直接判高低电平-
        if (KeyStateRead(Key_Shut) == KEY_PRESSED)
        {
			KeyValid[VKey_Dec] = 1;
        }
	}



    /*-Keys of the Board-*/
    if (pMenuPara->IsMultiplex == Multiplex_Digit)
    {
        if (KeyPressed[Key_OpenOnBoard] == KEY_PRESSED)
        { 
            KeyValid[VKey_Inc] = 1;
        }
        if (KeyPressed[Key_ShutOnBoard] == KEY_PRESSED)
        { 
            KeyValid[VKey_Dec] = 1;
        }   
    }
    else if (pMenuPara->IsMultiplex == Multiplex_Adjust)
    {
        //-这里是直接判高低电平-
        if(KeyStateRead(Key_ShutOnBoard) == KEY_PRESSED)
        { 
            KeyValid[VKey_Dec] = 1;
        }
        //-这里是直接判高低电平-
        if (KeyStateRead(Key_OpenOnBoard) == KEY_PRESSED)
        { 
            KeyValid[VKey_Inc] = 1;
        }
    }
    else
    {
        if (KeyPressed[Key_ShutOnBoard] == KEY_PRESSED)
        {
            KeyValid[VKey_Down] = 1;
        }
        if (KeyPressed[Key_OpenOnBoard] == KEY_PRESSED)
        {
            KeyValid[VKey_Back] = 1;
        }
    }


    if (KeyValid[VKey_Set] == 1)
    {
        IsKeyPressed = 1;
        pMenu[pMenuPara->FocusMenuIndex].SetKeyDeal(&pMenu[pMenuPara->FocusMenuIndex], pMenuPara);
    }
    if (KeyValid[VKey_Back] == 1)
    {
        IsKeyPressed = 1;
        StandardMenu_Back2Parent(pMenu, pMenuPara);
    }
    if (KeyValid[VKey_Up] == 1)
    {
        IsKeyPressed = 1;
        pMenu[pMenuPara->FocusMenuIndex].UpKeyDeal(&pMenu[pMenuPara->FocusMenuIndex], pMenuPara);
    }
    if (KeyValid[VKey_Down] == 1)
    {
        IsKeyPressed = 1;
        pMenu[pMenuPara->FocusMenuIndex].DownKeyDeal(&pMenu[pMenuPara->FocusMenuIndex], pMenuPara);
    }
    if (KeyValid[VKey_Inc] == 1)
    {
        IsKeyPressed = 1;
        pMenu[pMenuPara->FocusMenuIndex].IncKeyDeal(&pMenu[pMenuPara->FocusMenuIndex], pMenuPara);
    }
    if (KeyValid[VKey_Dec] == 1)
    {
        IsKeyPressed = 1;
        pMenu[pMenuPara->FocusMenuIndex].DecKeyDeal(&pMenu[pMenuPara->FocusMenuIndex], pMenuPara);
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
 
    TempData = pMenuPara->RowIndex;   //-移出后会导致溢出-
    TempData >>= Items_Per_Page_Shif_Count;
    StartRowIndex = TempData;         //-除以每页的最大菜单数-
    StartRowIndex <<= Items_Per_Page_Shif_Count;
    for (i = 0; i < Items_Per_Page; i++)
    {
        ClearBuf(&g_DisplayBuf[0], Display_Buf_Size);

        DisplayBufIndex = 0;
        MenuNameIndex = 0;
        if (StartRowIndex + i < pMenuPara->NumOfRows)
        {
            for (j = 0; j < 16;     )    //-把j++放到循环体中-
            {
                ClearBuf(&FontBuf[0], sizeof(FontBuf));

                Code = pMenu[StartRowIndex + i].MenuName[MenuNameIndex++];
                if (((Code >> 8) & 0xFF) >= 0xB0)    //-中文字符-
                {
                    CharacterType = CN_Character;
          
                    j += 2;             //!!!!j在循环体中累加
                }
                else
                {
                    CharacterType = EN_Character;

                    j += 1;             //!!!!j在循环体中累加
                }
                GetDotData(Code, CharacterType, &FontBuf[0], &FontBufLen);
    
                SwapFontBuf2DisplayBuf(FontBuf, FontBufLen, &g_LcdDisplayBuf[0], DisplayBufIndex);

                if (CharacterType == CN_Character)    //-中文字符-
                {
                    DisplayBufIndex += CN_X_SIZE;
                }
                else
                {
                    DisplayBufIndex += EN_X_SIZE;
                }            
            }


            if (pMenu[StartRowIndex + i].SpecialDeal != DummyFunction)
            {
                pMenu[StartRowIndex + i].SpecialDeal(&pMenu[StartRowIndex + i], pMenuPara);

                if (IsNeedChangePage == 1) 
                {
                    IsNeedChangePage = 0;
                
                    return;
                }
            }

            
            /*-反显-*/
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
unsigned char IsInMenu(void)
{
    if (Device.Para.IsInMenu == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void EnterMenu(unsigned char Flag)
{
    if (Flag == 0)
    {
        Device.Para.IsInMenu = 0;
    }
    else
    {
        Device.Para.IsInMenu = 1;
    }
    return;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void DisplayInit(void)
{
    int i = 0;

    InsertTimer(&NoKeyTimer);
    InsertTimer(&AdjustInfoTimer);

    for (i = 0; i < Page_Num; i++)
    {
    
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Task_Display(void)
{
    static unsigned char HasDone = 0;
#if Debug
    unsigned char HasDone1 = 0;
#endif

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
            if ((Device.Error.ErrorByte != 0) || (Valve.Error.ErrorByte != 0))
            {
                Show_Page(Page_Error_ID);
            }
            else
            {
                Show_Page(Page_Normal_ID);
            }
        }
    }
    else if (IsInMenu() == 0)
    {
        if ((Device.Error.ErrorByte != 0) || (Valve.Error.ErrorByte != 0))
        {
            Show_Page(Page_Error_ID);
        }
        else
        {
            Show_Page(Page_Normal_ID);
        }
    }

    if (IsTimeOut(AdjustInfoTimer) == 1)
    {
        StopTimer(AdjustInfoTimer);
        if ((CurrentAdjustType == Frame_AdjustZero) || (CurrentAdjustType == Frame_AdjustFull))
        {
            Show_Page(Page_Terminal_ID);
        }
        else if ((CurrentAdjustType == Frame_AdjustInput4mA) || (CurrentAdjustType == Frame_AdjustInput20mA))
        {
            Show_Page(Page_ANSignal_ID);
        }
    }

#if Debug
    if (KeyPressed[Key_Up] == KEY_PRESSED && KeyPressed[Key_Down] == KEY_PRESSED)
    {
        if (HasDone1 == 1)
        {
            return;
        }
        HasDone1 = 1;

        if (PageFunctionIndex != Page_CommTest_ID)
        {
            PrePageFunctionIndex = PageFunctionIndex;
            Show_Page(Page_CommTest_ID);
        }
        else
        {
            Show_Page(PrePageFunctionIndex);
        }

        return;
    }
    else
    {
        HasDone1 = 0;
    }
#endif


    switch(PageFunctionIndex)
    {
    //--
    case Page_PowerOn_ID:
        Page_Draw(&Menu_PowerOn[0], &MenuPara_PowerOn);   
        break;
    //--
    case Page_Normal_ID:
        SetFrameType(Normal_Frame);
        Page_Draw(&Menu_Normal[0], &MenuPara_Normal);   
        break;
    //--
    case Page_Error_ID:
        SetFrameType(Normal_Frame);
        Page_Draw(&Menu_Error[0], &MenuPara_Error);   
        break;
    //--
    case Page_Language_ID:
        Page_Draw(&Menu_Language[0], &MenuPara_Language);   
        break;
    //--
    case Page_Configuration_ID:
        Page_Draw(&Menu_Configuration[0], &MenuPara_Configuration);   
        break;
    //--
    case Page_Password_ID:
        Page_Draw(&Menu_Password[0], &MenuPara_Password);   
        break;
    //--
    case Page_ParaSetting_ID:
        Page_Draw(&Menu_ParaSetting[0], &MenuPara_ParaSetting);
        break;
    //--
    case Page_ESDSetting_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_ESDSetting[0], &MenuPara_ESDSetting);
        break;
    //--
    case Page_DeadZone_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_DeadZone[0], &MenuPara_DeadZone);
        break;
    //--
    case Page_OpenCurrent_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_OpenCurrent[0], &MenuPara_OpenCurrent);
        break;
    //--
    case Page_ShutCurrent_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_ShutCurrent[0], &MenuPara_ShutCurrent);
        break;
    //--
    case Page_MaxActionTime_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_MaxActionTime[0], &MenuPara_MaxActionTime);
        break;
    //--
    case Page_Terminal_ID:
        SetFrameType(Adjust_Frame);
        Page_Draw(&Menu_Terminal[0], &MenuPara_Terminal);
        break;
    //--
    case Page_ShutTerminal_ID:
        SetFrameType(Adjust_Frame);
        Page_Draw(&Menu_ShutTerminal[0], &MenuPara_ShutTerminal);
        break;
    //--
    case Page_AdjustZeroInfo_ID:
        SetFrameType(Adjust_Frame);
        Page_Draw(&Menu_AdjustZeroInfo[0], &MenuPara_AdjustZeroInfo);
        break;
    //--
    case Page_OpenTerminal_ID: 
        SetFrameType(Adjust_Frame);
        Page_Draw(&Menu_OpenTerminal[0], &MenuPara_OpenTerminal); 
        break;
    //--
    case Page_AdjustFullInfo_ID:
        SetFrameType(Adjust_Frame);
        Page_Draw(&Menu_AdjustFullInfo[0], &MenuPara_AdjustFullInfo);
        break;
    //--
    case Page_ErrorFeedBack_ID:
        SetFrameType(Setting_Frame); 
        Page_Draw(&Menu_ErrorFeedBack[0], &MenuPara_ErrorFeedBack); 
        break;
    //--
    case Page_RemoteHold_ID: 
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_RemoteHold[0], &MenuPara_RemoteHold); 
        break;
    //--
    case Page_Control_ID: 
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_Control[0], &MenuPara_Control); 
        break;
    //--
    case Page_Local_ID: 
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_Local[0], &MenuPara_Local); 
        break;
    //--
    case Page_RemoteIO_ID: 
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_RemoteIO[0], &MenuPara_RemoteIO); 
        break;
    //--
    case Page_RemoteAN_ID: 
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_RemoteAN[0], &MenuPara_RemoteAN); 
        break;

    //--
    case Page_ANSignal_ID: 
        SetFrameType(Adjust_Frame);
        Page_Draw(&Menu_ANSignal[0], &MenuPara_ANSignal); 
        break;
    //--
    case Page_In4mA_ID: 
        SetFrameType(Adjust_Frame);
        Page_Draw(&Menu_In4mA[0], &MenuPara_In4mA); 
        break;
    //--
    case Page_In4mAInfo_ID: 
        SetFrameType(Adjust_Frame);
        Page_Draw(&Menu_In4mAInfo[0], &MenuPara_In4mAInfo); 
        break;
    //--
    case Page_In20mA_ID: 
        SetFrameType(Adjust_Frame);
        Page_Draw(&Menu_In20mA[0], &MenuPara_In20mA); 
        break;
    //--
    case Page_In20mAInfo_ID: 
        SetFrameType(Adjust_Frame);
        Page_Draw(&Menu_In20mAInfo[0], &MenuPara_In20mAInfo); 
        break;
    //--
    case Page_Out4mA_ID: 
        SetFrameType(Adjust_Frame);
        Page_Draw(&Menu_Out4mA[0], &MenuPara_Out4mA); 
        break;
    //--
    case Page_Out20mA_ID:
        SetFrameType(Adjust_Frame); 
        Page_Draw(&Menu_Out20mA[0], &MenuPara_Out20mA); 
        break;

    //--
    case Page_ChangePassword_ID: 
        Page_Draw(&Menu_ChangePassword[0], &MenuPara_ChangePassword); 
        break;

    //--
    case Page_ParaQuery_ID: 
        Page_Draw(&Menu_QueryPara[0], &MenuPara_QueryPara); 
        break;

    //--
    case Page_QueryESDSetting_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_QueryESDSetting[0], &MenuPara_QueryESDSetting);
        break; 
    //--
    case Page_QueryDeadZone_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_QueryDeadZone[0], &MenuPara_QueryDeadZone);
        break;
    //--
    case Page_QueryOpenCurrent_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_QueryOpenCurrent[0], &MenuPara_QueryOpenCurrent);
        break;
    //--
    case Page_QueryShutCurrent_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_QueryShutCurrent[0], &MenuPara_QueryShutCurrent);
        break;
    //--
    case Page_QueryMaxActionTime_ID:
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_QueryMaxActionTime[0], &MenuPara_QueryMaxActionTime);
        break;

    //--
    case Page_QueryErrorFeedBack_ID: 
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_QueryErrorFeedBack[0], &MenuPara_QueryErrorFeedBack); 
        break;
    //--
    case Page_QueryRemoteHold_ID: 
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_QueryRemoteHold[0], &MenuPara_QueryRemoteHold); 
        break;
    //--
    case Page_QueryControl_ID: 
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_QueryControl[0], &MenuPara_QueryControl); 
        break;
    //--
    case Page_QueryLocal_ID: 
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_QueryLocal[0], &MenuPara_QueryLocal); 
        break;
    //--
    case Page_QueryRemoteIO_ID: 
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_QueryRemoteIO[0], &MenuPara_QueryRemoteIO); 
        break;
    //--
    case Page_QueryRemoteAN_ID: 
        SetFrameType(Setting_Frame);
        Page_Draw(&Menu_QueryRemoteAN[0], &MenuPara_QueryRemoteAN); 
        break;

    //--
    case Page_QueryPassword_ID: 
        Page_Draw(&Menu_QueryPassword[0], &MenuPara_QueryPassword); 
        break;

    //--
    case Page_QueryError_ID: 
        SetFrameType(Normal_Frame);
        Page_Draw(&Menu_QueryError[0], &MenuPara_QueryError); 
        break;
#if Debug
    //--
    case Page_CommTest_ID:
        Page_Draw(&Menu_CommTest[0], &MenuPara_CommTest);
        break;
    //--
#endif

    //--
    default:    //-默认常显-
        break;
    }
}

/*************************************END OF FILE*******************************/

