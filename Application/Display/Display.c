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
#include "Lcd.h"
#include "Communication.h"

/*******************************************************************************
*                               文件内部使用宏定义
********************************************************************************/
#define PasswordByte_0   0x01
#define PasswordByte_1   0x00
#define PasswordByte_2   0x00
#define PasswordByte_3   0x00

#define MaxLockTime      0xFFFF
#define MinLockTime      0x00

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

unsigned char PasswordErrorCount = 0;
unsigned char Password[4] = {0};

//-此数组先前定义在ShowNumber中,但是后来发现会导致栈空间不够,
// 只能将原来局部变量改成全局变量.-
unsigned int LongCode[16];

unsigned char IsNeedChangePage = 0;
unsigned char F_StartRecv = 1;

//-无按键延时-
signed long NoKeyTimer = 0;
//-标定成功信息提示延时-
signed long AdjustInfoTimer = 0;
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
    {{0xB9D8, 0xCFF2, 0xB9FD, 0xBED8}, {0x4F, 0x70, 0x65, 0x6E, 0x20, 0x54, 0x6F, 0x72, 0x71, 0x75, 0x65, 0x20, 0x45, 0x72, 0x72, 0x21}},
    //-开力矩故障(Shut Torque Err!)-
    {{0xBFAA, 0xCFF2, 0xB9FD, 0xBED8}, {0x53, 0x68, 0x75, 0x74, 0x20, 0x54, 0x6F, 0x72, 0x71, 0x75, 0x65, 0x20, 0x45, 0x72, 0x72, 0x21}}, 
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

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_Template = {2, 2, Disable, Enable, 8, 4, Multiplex_None, Dummy_Reset};
********************************************************************************/

/*-------------------上电页面(行索引固定的菜单)-------------------------------*/
const MenuStructure Menu_PowerOn[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-欢迎使用-
  {0,             InvalidMenuID,           InvalidMenuID,         {0xA3A0, 0xA3A0, 0xA3A0, 0xA3A0, 0xBBB6, 0xD3AD, 0xCAB9, 0xD3C3,  0xA3A0, 0xA3A0, 0xA3A0, 0xA3A0},
  Dummy_Special,  StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-电动阀门执行机构-
  {1,             InvalidMenuID,           InvalidMenuID,         {0xB5E7, 0xB6AF, 0xB7A7, 0xC3C5, 0xD6B4, 0xD0D0, 0xBBFA, 0xB9B9},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-版本V1.0-
  {2,            InvalidMenuID,           InvalidMenuID,          {0xA3A0, 0xA3A0, 0xB0E6, 0xB1BE, 0xA3BA, 0xA3A0, 0xA3D6, 0xA3B1, 0xA3AE, 0xA3B0, 0xA3A0, 0xA3A0, 0xA3A0, 0xA3A0},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {3,            InvalidMenuID,           InvalidMenuID,          {0},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_PowerOn = {0, Invalid, Disable, Disable, Invalid, 4, Multiplex_None, Dummy_Reset};    


/*------------------ 常显页面(行索引固定的菜单)-------------------------------*/
const MenuStructure Menu_Normal[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-占位,内容在NormalPage_Special中填充-
  {0,             InvalidMenuID,    Page_MainMenu_ID,             {0},
  NormalPage_Special0, StandardMenu_SetKey, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-占位,内容在NormalPage_Special中填充-
  {1,             InvalidMenuID,           InvalidMenuID,         {0},
  NormalPage_Special1, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-占位,内容在NormalPage_Special中填充-
  {2,             InvalidMenuID,           InvalidMenuID,         {0},
  NormalPage_Special2, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-占位,内容在NormalPage_Special中填充-
  {3,             InvalidMenuID,           InvalidMenuID,           {0},
  NormalPage_Special3, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_Normal = {0, Invalid, Disable, Disable, Invalid, 4, Multiplex_None, Dummy_Reset};    


/*--------------------主菜单--------------------------------------------------*/
const MenuStructure Menu_Main[] =         
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-标定全关-
  {0,             Page_Normal_ID,          Page_AdjustZero_ID,    {0xB1EA, 0xB6A8, 0xC8AB, 0xB9D8},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-标定全开-
  {1,             InvalidMenuID,           Page_AdjustFull_ID,    {0xB1EA, 0xB6A8, 0xC8AB, 0xBFAA},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
  
  //-现场控制方式-
  {2,             InvalidMenuID,           Page_LocalMode_ID,     {0xCFD6, 0xB3A1, 0xBFD8, 0xD6C6, 0xB7BD, 0xCABD},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
  
  //-远程开关型设置-
  {3,             InvalidMenuID,           Page_RemoteIOMode_ID,  {0xD4B6, 0xB3CC, 0xBFAA, 0xB9D8, 0xD0CD, 0xC9E8, 0xD6C3},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-远程调节型设置-
  {4,             InvalidMenuID,           Page_RemoteANMode_ID,  {0xD4B6, 0xB3CC, 0xB5F7, 0xBDDA, 0xD0CD, 0xC9E8, 0xD6C3},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
  
  //-内部参数-
  {5,             InvalidMenuID,           Page_Password_ID,      {0xC4DA, 0xB2BF, 0xB2CE, 0xCAFD},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
  
  //-退出设置-
  {6,             Page_Normal_ID,          InvalidMenuID,           {0xCDCB, 0xB3F6, 0xC9E8, 0xD6C3},
   Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction}
};   

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_Main = {0, Invalid, Enable, Disable, Invalid, 7, Multiplex_None, StandardMenu_Reset0};    


/*---------------------标定零点-----------------------------------------------*/
const MenuStructure Menu_AdjustZero[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-把当前位置#0-
  {0,             Page_MainMenu_ID,        InvalidMenuID,         {0xB0D1, 0xB5B1, 0xC7B0, 0xCEBB, 0xD6C3, 0xA3A3},
   AdjustZeroFull_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-标定为0%-
  {1,             InvalidMenuID,           InvalidMenuID,         {0xA3A0, 0xA3A0, 0xA3A0, 0xA3A0, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B0, 0xA3A5},
   AdjustZeroFull_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //--
  {2,             InvalidMenuID,           InvalidMenuID,         {0},
   AdjustZeroFull_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-确认  返回-
  {3,             Page_MainMenu_ID,        InvalidMenuID,         {0xA3A0, 0xA3A0, 0xA3A0, 0xC8B7, 0xC8CF, 0xA3A0, 0xA3A0, 0xB7B5, 0xBBD8},
   AdjustZeroFull_Special, AdjustZeroFull_SetKey, AdjustZeroFull_UpKey, AdjustZeroFull_DownKey, DummyFunction, DummyFunction},
};   
   
//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_AdjustZero = {3, 3, Disable, Enable, 4, 4, Multiplex_None, StandardMenu_Reset1};    


/*---------------------标定0%提示信息---------------------------------------*/
const MenuStructure Menu_AdjustZeroInfo[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //--
  {0,             Page_MainMenu_ID,        InvalidMenuID,         {0},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-已把当前位置-
  {1,             InvalidMenuID,           InvalidMenuID,         {0xA3A0, 0xA3A0, 0xD2D1, 0xB0D1, 0xB5B1, 0xC7B0, 0xCEBB, 0xD6C3},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-成功标定为0%-
  {2,             InvalidMenuID,           InvalidMenuID,         {0xA3A0, 0xA3A0, 0xB3C9, 0xB9A6, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B0, 0xA3A5},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //--
  {3,             Page_MainMenu_ID,        InvalidMenuID,         {0},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};   

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_AdjustZeroInfo = {0, Invalid, Disable, Disable, Invalid, 4, Multiplex_None, Dummy_Reset};


/*---------------------标定满点-----------------------------------------------*/
const MenuStructure Menu_AdjustFull[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-把当前位置#0-
  {0,             Page_MainMenu_ID,       InvalidMenuID,         {0xB0D1, 0xB5B1, 0xC7B0, 0xCEBB, 0xD6C3, 0xA3A3},
   AdjustZeroFull_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-标定为100%-
  {1,             InvalidMenuID,           InvalidMenuID,         {0xA3A0, 0xA3A0, 0xA3A0, 0xA3A0, 0xB1EA, 0xB6A8, 0xCEAA,0xA3B1, 0xA3B0, 0xA3B0,0xA3A5},
   AdjustZeroFull_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //--
  {2,             InvalidMenuID,           InvalidMenuID,         {0},
   AdjustZeroFull_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-确认  返回-
  {3,             Page_MainMenu_ID,        InvalidMenuID,         {0xA3A0, 0xA3A0, 0xA3A0, 0xC8B7, 0xC8CF, 0xA3A0, 0xA3A0, 0xB7B5, 0xBBD8},
   AdjustZeroFull_Special, AdjustZeroFull_SetKey, AdjustZeroFull_UpKey, AdjustZeroFull_DownKey, DummyFunction, DummyFunction},
};   

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_AdjustFull = {3, 3, Disable, Enable, 4, 4, Multiplex_None, StandardMenu_Reset1};  


/*---------------------标定100%提示信息---------------------------------------*/
const MenuStructure Menu_AdjustFullInfo[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //--
  {0,             Page_MainMenu_ID,        InvalidMenuID,         {0},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-已把当前位置-
  {1,             InvalidMenuID,           InvalidMenuID,         {0xA3A0, 0xA3A0, 0xD2D1, 0xB0D1, 0xB5B1, 0xC7B0, 0xCEBB, 0xD6C3},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-成功标定为100%-
  {2,             InvalidMenuID,           InvalidMenuID,         {0xA3A0, 0xA3A0, 0xB3C9, 0xB9A6, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B1, 0xA3B0, 0xA3B0, 0xA3A5},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //--
  {3,             InvalidMenuID,           InvalidMenuID,         {0},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};   

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_AdjustFullInfo = {0, Invalid, Disable, Disable, Invalid, 4, Multiplex_None, Dummy_Reset};   


/*---------------------现场设置-----------------------------------------------*/
const MenuStructure Menu_LocalMode[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-现场点动控制-
  {0,             Page_MainMenu_ID,        InvalidMenuID,           {0xCFD6, 0xB3A1, 0xB5E3, 0xB6AF, 0xBFD8, 0xD6C6},
  LocalMode_Special, LocalMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
  
  //-现场保持控制-
  {1,             InvalidMenuID,           InvalidMenuID,           {0xCFD6, 0xB3A1, 0xB1A3, 0xB3D6, 0xBFD8, 0xD6C6},
  LocalMode_Special, LocalMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
 
  //-返回上级-
  {2,             Page_MainMenu_ID,        InvalidMenuID,           {0xB7B5, 0xBBD8,0xC9CF, 0xBCB6},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction}
};   

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_LocalMode = {0, Invalid, Enable, Disable, Invalid, 3, Multiplex_None, StandardMenu_Reset0};    


/*---------------------远程开关型设置-----------------------------------------*/
const MenuStructure Menu_RemoteIOMode[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-远程点动控制-
  {0,             Page_MainMenu_ID,        InvalidMenuID,         {0xD4B6, 0xB3CC, 0xB5E3, 0xB6AF, 0xBFD8, 0xD6C6},
  RemoteIOMode_Special, RemoteIOMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-远程保持停常开-
  {1,             InvalidMenuID,           InvalidMenuID,         {0xD4B6, 0xB3CC, 0xB1A3, 0xB3D6, 0xCDA3, 0xB3A3, 0xBFAA},
  RemoteIOMode_Special, RemoteIOMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-远程保持停常闭-
  {2,             InvalidMenuID,           InvalidMenuID,         {0xD4B6, 0xB3CC, 0xB1A3, 0xB3D6, 0xCDA3, 0xB3A3, 0xB1D5},
  RemoteIOMode_Special, RemoteIOMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-有信开无信关-
  {3,             InvalidMenuID,           InvalidMenuID,         {0xD3D0, 0xD0C5, 0xBFAA, 0xCEDE, 0xD0C5, 0xB9D8},
  RemoteIOMode_Special, RemoteIOMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-有信关无信开-
  {4,             InvalidMenuID,           InvalidMenuID,         {0xD3D0, 0xD0C5, 0xB9D8, 0xCEDE, 0xD0C5, 0xBFAA},
  RemoteIOMode_Special, RemoteIOMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-返回上级-
  {5,             Page_MainMenu_ID,        InvalidMenuID,         {0xB7B5, 0xBBD8,0xC9CF, 0xBCB6},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction}
};  

//-第6个初始值代表菜单项个数,必须给正确的值- 
MenuPara      MenuPara_RemoteIOMode = {0, Invalid, Enable, Disable, Invalid, 6, Multiplex_None, StandardMenu_Reset0};  


/*---------------------远程调节型设置-----------------------------------------*/
const MenuStructure Menu_RemoteANMode[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-控制死区-
  {0,             Page_MainMenu_ID,        Page_DeadZone_ID,        {0xBFD8, 0xD6C6,0xCBC0, 0xC7F8},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-丢信保位-
  {1,             InvalidMenuID,           InvalidMenuID,           {0xB6AA, 0xD0C5,0xB1A3, 0xCEBB},
  RemoteANMode_Special, RemoteANMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-丢信关闭-
  {2,             InvalidMenuID,           InvalidMenuID,           {0xB6AA, 0xD0C5,0xB9D8, 0xB1D5},
  RemoteANMode_Special, RemoteANMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-丢信居中-
  {3,             InvalidMenuID,           InvalidMenuID,           {0xB6AA, 0xD0C5,0xBED3, 0xD6D0},
  RemoteANMode_Special, RemoteANMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-丢信打开-
  {4,             InvalidMenuID,           InvalidMenuID,           {0xB6AA, 0xD0C5,0xB4F2, 0xBFAA},
  RemoteANMode_Special, RemoteANMode_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-返回上级-
  {5,             Page_MainMenu_ID,        InvalidMenuID,           {0xB7B5, 0xBBD8,0xC9CF, 0xBCB6},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction}
};  

//-第6个初始值代表菜单项个数,必须给正确的值- 
MenuPara      MenuPara_RemoteANMode = {0, Invalid, Enable, Disable, Invalid, 6, Multiplex_None, StandardMenu_Reset0};    


/*---------------------死区---------------------------------------------------*/
//-注意:按照一般处理DeadZone_UpKey应该放到菜单项1而不是菜单项2(即返回上级).
//     但是死区的焦点一直固定在"返回上级",加减改变的菜单项1.故特殊处理.
const MenuStructure Menu_DeadZone[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-设置控制死区-
  {0,             Page_RemoteANMode_ID,    InvalidMenuID,           {0xC9E8, 0xD6C3, 0xBFD8, 0xD6C6, 0xCBC0, 0xC7F8},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-自适应/x.x%-
  {1,             InvalidMenuID,           InvalidMenuID,           {0},
  DeadZone_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-返回上级-
  {2,             Page_RemoteANMode_ID,    InvalidMenuID,           {0xA3A0, 0xA3A0, 0xA3A0, 0xA3A0, 0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6},
  DeadZone_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, DeadZone_IncKey, DeadZone_DecKey},

  //-切换至现场可调整-
  {3,             InvalidMenuID,           InvalidMenuID,           {0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB},
  DeadZone_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, AdjustOutput4mA_IncKey, AdjustOutput4mA_DecKey}
};  

//-第6个初始值代表菜单项个数,必须给正确的值- 
MenuPara      MenuPara_DeadZone = {2, 4, Disable, Enable, 8, 4, Multiplex_None, Dummy_Reset};  


/*---------------------密码输入-----------------------------------------------*/
const MenuStructure Menu_Password[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-需由专业人员设置-
  {0,             Page_MainMenu_ID,        InvalidMenuID,           {0xD0E8, 0xD3C9, 0xD7A8, 0xD2B5, 0xC8CB, 0xD4B1, 0xC9E8, 0xD6C3},  
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-请输入密码-
  {1,             InvalidMenuID,           InvalidMenuID,           {0xC7EB, 0xCAE4, 0xC8EB, 0xC3DC, 0xC2EB}, 
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //- X X X X-
  {2,             InvalidMenuID,           Page_InternalPara_ID,    {0xA3A0, 0xA3A0, 0xA3A0, 0xA3A0, 0xA3B0, 0xA3A0, 0xA3B0, 0xA3A0, 0xA3B0, 0xA3A0, 0xA3B0, 0xA3A0},
  Password_Special, Password_SetKey, Password_UpKey, Password_DownKey, Password_IncKey, Password_DecKey},

  //-密码错误重新输入-
  {3,             InvalidMenuID,           InvalidMenuID,           {0xC3DC, 0xC2EB, 0xB4ED, 0xCEF3, 0xD6D8, 0xD0C2, 0xCAE4, 0xC8EB},
  Password_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction}
};   

 //-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_Password = {2, 4, Disable,Enable, 1, 4, Multiplex_Digit, Dummy_Reset};   


/*---------------------内部参数-----------------------------------------------*/
const MenuStructure Menu_InternalPara[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-反馈4mA修正-
  {0,             Page_MainMenu_ID,        Page_AdjustOutput4mA_ID, {0xB7B4, 0xC0A1, 0xA3A0, 0xA3B4, 0xA3ED, 0xA3C1, 0xD0DE, 0xD5FD},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-反馈20mA修正-
  {1,             InvalidMenuID,           Page_AdjustOutput20mA_ID,{0xB7B4, 0xC0A1,0xA3B2, 0xA3B0, 0xA3ED, 0xA3C1, 0xD0DE, 0xD5FD},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-输入4mA校准-
  {2,             InvalidMenuID,           Page_AdjustInput4mA_ID,  {0xCAE4, 0xC8EB, 0xA3A0, 0xA3B4, 0xA3ED, 0xA3C1, 0xD0A3, 0xD7BC},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-输入20mA校准-
  {3,             InvalidMenuID,           Page_AdjustInput20mA_ID, {0xCAE4, 0xC8EB,0xA3B2, 0xA3B0, 0xA3ED, 0xA3C1, 0xD0A3, 0xD7BC},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-设置关动作电流-
  {4,             InvalidMenuID,           Page_ShutCurrent_ID,     {0xC9E8, 0xD6C3, 0xB9D8, 0xB6AF, 0xD7F7, 0xB5E7, 0xC1F7},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-设置开动作电流-
  {5,             InvalidMenuID,           Page_OpenCurrent_ID,     {0xC9E8, 0xD6C3, 0xBFAA, 0xB6AF, 0xD7F7, 0xB5E7, 0xC1F7},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-阀门最大开闭时间-
  {6,             InvalidMenuID,           Page_MaxActionTime_ID,   {0xB7A7, 0xC3C5,0xD7EE, 0xB4F3, 0xBFAA, 0xB1D5, 0xCAB1, 0xBCE4},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-故障反馈触点选择-
  {7,             InvalidMenuID,           Page_ErrorFeedback_ID,   {0xB9CA, 0xD5CF, 0xB7B4, 0xC0A1, 0xB4A5, 0xB5E3, 0xD1A1, 0xD4F1},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD设置-
  {8,             InvalidMenuID,           Page_ESDSetting_ID,      {0x45, 0x53, 0x44, 0xC9E8, 0xD6C3},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-返回上级-
  {9,             Page_MainMenu_ID,        InvalidMenuID,           {0xB7B5, 0xBBD8,0xC9CF, 0xBCB6},
  Dummy_Special, StandardMenu_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction}
};   

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_InternalPara = {0, Invalid, Enable, Disable, Invalid, 10, Multiplex_None, StandardMenu_Reset0};    


/*---------------------反馈电流4mA修正----------------------------------------*/
const MenuStructure Menu_AdjustOutput4mA[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-反馈电流4mA修正-
  {0,             Page_InternalPara_ID,    InvalidMenuID,           {0xB7B4, 0xC0A1, 0xB5E7, 0xC1F7, 0xA3B4, 0xA3ED, 0xA3C1, 0xD0DE, 0xD5FD},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {1,             InvalidMenuID,           InvalidMenuID,           {0},
  AdjustOutput4_20mA_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-返回上级-
  {2,             Page_InternalPara_ID,    InvalidMenuID,           {0xA3A0, 0xA3A0, 0xA3A0, 0xA3A0, 0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6},
  AdjustOutput4_20mA_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, AdjustOutput4mA_IncKey, AdjustOutput4mA_DecKey},

  //-切换至现场可调整-
  {3,             InvalidMenuID,           InvalidMenuID,           {0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB},
  AdjustOutput4_20mA_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, AdjustOutput4mA_IncKey, AdjustOutput4mA_DecKey}
};  

//-第6个初始值代表菜单项个数,必须给正确的值- 
MenuPara      MenuPara_AdjustOutput4mA = {2, 4, Disable, Enable, 8, 4, Multiplex_Adjust, Dummy_Reset}; 


/*---------------------反馈电流20mA修正----------------------------------------*/
const MenuStructure Menu_AdjustOutput20mA[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-反馈电流20mA修正-
  {0,             Page_InternalPara_ID,    InvalidMenuID,           {0xB7B4, 0xC0A1, 0xB5E7, 0xC1F7, 0xA3B2, 0xA3B0, 0xA3ED, 0xA3C1, 0xD0DE, 0xD5FD},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-空行-
  {1,             InvalidMenuID,           InvalidMenuID,           {0},
  AdjustOutput4_20mA_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-返回上级-
  {2,             Page_InternalPara_ID,    InvalidMenuID,           {0xA3A0, 0xA3A0, 0xA3A0, 0xA3A0, 0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6},
  AdjustOutput4_20mA_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, AdjustOutput20mA_IncKey, AdjustOutput20mA_DecKey}, 

  //-切换至现场可调整-
  {3,             InvalidMenuID,           InvalidMenuID,           {0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB},
  AdjustOutput4_20mA_Special, StandardMenu_SetKey, DummyFunction, DummyFunction, AdjustOutput4mA_IncKey, AdjustOutput4mA_DecKey}
};  

//-第6个初始值代表菜单项个数,必须给正确的值- 
MenuPara      MenuPara_AdjustOutput20mA = {2, 4, Disable, Enable, 8, 4, Multiplex_Adjust, Dummy_Reset}; 


/*---------------------标定输入4mA--------------------------------------------*/
const MenuStructure Menu_AdjustInput4mA[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-把当前输入电流-
  {0,             Page_InternalPara_ID,  InvalidMenuID,           {0xA3A0, 0xB0D1, 0xB5B1, 0xC7B0, 0xCAE4, 0xC8EB, 0xB5E7, 0xC1F7},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-约xx.xmA-
  {1,             InvalidMenuID,         InvalidMenuID,           {0, 0, 0, 0, 0xD4BC, 0, 0, 0, 0, 0xA3ED, 0xA3C1},
  AdjustInput4_20mA_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-标定为目标0%-
  {2,             InvalidMenuID,         InvalidMenuID,           {0xA3A0, 0xB1EA, 0xB6A8, 0xCEAA,0xC4BF, 0xB1EA, 0xA3B0,0xA3A5},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-确认  返回-
  {3,             Page_InternalPara_ID,   Page_AdjustInput4mAInfo_ID,{0xA3A0, 0xA3A0, 0xA3A0, 0xC8B7, 0xC8CF, 0xA3A0, 0xA3A0, 0xB7B5, 0xBBD8},
  Dummy_Special, AdjustInput4_20mA_SetKey, AdjustInput4_20mA_UpKey, AdjustInput4_20mA_DownKey, DummyFunction, DummyFunction},
};   

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_AdjustInput4mA = {3, 3, Disable, Enable, 4, 4, Multiplex_None, StandardMenu_Reset1}; 


/*---------------------输入电流标定4mA提示信息---------------------------------------*/
const MenuStructure Menu_AdjustInput4mAInfo[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //--
  {0,             Page_InternalPara_ID,    InvalidMenuID,           {0},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-已把当前电流-
  {1,             InvalidMenuID,           InvalidMenuID,           {0xA3A0, 0xA3A0, 0xD2D1, 0xB0D1, 0xB5B1, 0xC7B0, 0xB5E7, 0xC1F7},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-成功标定为0%-
  {2,             InvalidMenuID,           InvalidMenuID,           {0xA3A0, 0xA3A0, 0xB3C9, 0xB9A6, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B0, 0xA3A5},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //--
  {3,             InvalidMenuID,           InvalidMenuID,           {0},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};   

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_AdjustInput4mAInfo = {0, Invalid, Disable, Disable, Invalid, 4, Multiplex_None, Dummy_Reset};


/*---------------------标定输入20mA-------------------------------------------*/
const MenuStructure Menu_AdjustInput20mA[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-把当前输入电流-
  {0,             Page_InternalPara_ID,    InvalidMenuID,           {0xA3A0, 0xB0D1, 0xB5B1, 0xC7B0, 0xCAE4, 0xC8EB, 0xB5E7, 0xC1F7},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-约xx.xmA-
  {1,             InvalidMenuID,           InvalidMenuID,           {0, 0, 0, 0, 0xD4BC, 0, 0, 0, 0, 0xA3ED, 0xA3C1},
  AdjustInput4_20mA_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-标定为目标100%-
  {2,             InvalidMenuID,           InvalidMenuID,           {0xA3A0, 0xB1EA, 0xB6A8, 0xCEAA,0xC4BF, 0xB1EA, 0xA3B1, 0xA3B0, 0xA3B0,0xA3A5},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-确认  返回-
  {3,             Page_InternalPara_ID,     Page_AdjustInput20mAInfo_ID,{0xA3A0, 0xA3A0, 0xA3A0, 0xC8B7, 0xC8CF, 0xA3A0, 0xA3A0, 0xB7B5, 0xBBD8},
  Dummy_Special, AdjustInput4_20mA_SetKey, AdjustInput4_20mA_UpKey, AdjustInput4_20mA_DownKey, DummyFunction, DummyFunction},
};   

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_AdjustInput20mA = {3, 3, Disable, Enable, 4, 4, Multiplex_None, StandardMenu_Reset1}; 


/*---------------------输入20mA标定提示信息---------------------------------------*/
const MenuStructure Menu_AdjustInput20mAInfo[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //--
  {0,             Page_InternalPara_ID,    InvalidMenuID,           {0},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-已把当前电流-
  {1,             InvalidMenuID,           InvalidMenuID,           {0xA3A0, 0xA3A0, 0xD2D1, 0xB0D1, 0xB5B1, 0xC7B0, 0xB5E7, 0xC1F7},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //-成功标定为100%-
  {2,             InvalidMenuID,           InvalidMenuID,           {0xA3A0, 0xA3A0, 0xB3C9, 0xB9A6, 0xB1EA, 0xB6A8, 0xCEAA, 0xA3B1, 0xA3B0, 0xA3B0, 0xA3A5},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
  
  //--
  {3,             InvalidMenuID,           InvalidMenuID,           {0},
   Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};   

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_AdjustInput20mAInfo = {0, Invalid, Disable, Disable, Invalid, 4, Multiplex_None, Dummy_Reset};   


/*------------------ 关动作电流-------------------------------*/
const MenuStructure Menu_ShutCurrent[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-设置关向动作电流-
  {0,             Page_InternalPara_ID,  InvalidMenuID,           {0xC9E8, 0xD6C3, 0xB9D8, 0xCFF2, 0xB6AF, 0xD7F7, 0xB5E7, 0xC1F7},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {1,             InvalidMenuID,         InvalidMenuID,           {0},
  ShutCurrent_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-    返回上级-
  {2,             Page_InternalPara_ID,  InvalidMenuID,           {Space, Space, Space, Space, 0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6},
  ShutCurrent_Special, StandardMenu_Back2Parent, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-切换至现场可调整-
  {3,             InvalidMenuID,         InvalidMenuID,           {0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB},
  ShutCurrent_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_ShutCurrent = {2, 4, Disable, Enable, 8, 4, Multiplex_None, Dummy_Reset};


/*------------------ 开动作电流-------------------------------*/
const MenuStructure Menu_OpenCurrent[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-设置开向动作电流-
  {0,             Page_InternalPara_ID,  InvalidMenuID,           {0xC9E8, 0xD6C3, 0xBFAA, 0xCFF2, 0xB6AF, 0xD7F7, 0xB5E7, 0xC1F7},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //--
  {1,             InvalidMenuID,         InvalidMenuID,           {0},
  ShutCurrent_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-    返回上级-
  {2,             Page_InternalPara_ID,    InvalidMenuID,           {Space, Space, Space, Space, 0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6},
  ShutCurrent_Special, StandardMenu_Back2Parent, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-切换至现场可调整-
  {3,             InvalidMenuID,         InvalidMenuID,           {0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB},
  ShutCurrent_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_OpenCurrent = {2, 4, Disable, Enable, 8, 4, Multiplex_None, Dummy_Reset};


/*---------------------阀门最大开闭时间---------------------------------------*/
const MenuStructure Menu_MaxActionTime[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-阀门最大开闭时间-
  {0,             Page_InternalPara_ID,    InvalidMenuID,           {0xB7A7, 0xC3C5, 0xD7EE, 0xB4F3, 0xBFAA, 0xB1D5, 0xCAB1, 0xBCE4},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-不限时/xxxS-
  {1,             Page_InternalPara_ID,    InvalidMenuID,           {0},
  MaxActionTime_Special, MaxActionTime_SetKey, DummyFunction, DummyFunction, MaxActionTime_IncKey, MaxActionTime_DecKey},

  //-    返回上级-
  {2,             Page_InternalPara_ID,    InvalidMenuID,           {Space, Space, Space, Space, 0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6},
  MaxActionTime_Special, StandardMenu_Back2Parent, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-切换至现场可调整-
  {3,             InvalidMenuID,         InvalidMenuID,           {0xC7D0, 0xBBBB, 0xD6C1, 0xCFD6, 0xB3A1, 0xBFC9, 0xB5F7, 0xD5FB},
  MaxActionTime_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},
};  

//-第6个初始值代表菜单项个数,必须给正确的值- 
MenuPara      MenuPara_MaxActionTime = {1, 6, Disable, Enable, 6, 4, Multiplex_Digit, Dummy_Reset}; 


/*------------------ 故障反馈触点-------------------------------*/
const MenuStructure Menu_ErrorFeedback[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-故障反馈触点选择-
  {0,             Page_InternalPara_ID,  InvalidMenuID,           {0xB9CA, 0xD5CF, 0xB7B4, 0xC0A1, 0xB4A5, 0xB5E3, 0xD1A1, 0xD4F1},
  Dummy_Special, DummyFunction, DummyFunction, DummyFunction, DummyFunction, DummyFunction},

  //-有故障闭合NO-
  {1,             InvalidMenuID,         InvalidMenuID,           {0xD3D0, 0xB9CA, 0xD5CF, 0xB1D5, 0xBACF, 0x4E, 0x4F},
  ErrorFeedback_Special, ErrorFeedback_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-有故障闭合NC-
  {2,             InvalidMenuID,         InvalidMenuID,           {0xD3D0, 0xB9CA, 0xD5CF, 0xB1D5, 0xBACF, 0x4E, 0x43},
  ErrorFeedback_Special, ErrorFeedback_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-返回上级-
  {3,             Page_InternalPara_ID,  InvalidMenuID,           {0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6},
  Dummy_Special, StandardMenu_Back2Parent, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
};

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_ErrorFeedback = {1, Invalid, Enable, Disable, 8, 4, Multiplex_None, Dummy_Reset};


/*------------------ ESD设置-------------------------------*/
const MenuStructure Menu_ESDSetting[] = 
{
  /*-索引号       父菜单                 子菜单                   菜单项名称-*/
  //-ESD禁用-
  {0,             Page_InternalPara_ID,  InvalidMenuID,           {0x45, 0x53, 0x44, 0xBDFB, 0xD3C3},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD禁止动作-
  {1,             InvalidMenuID,         InvalidMenuID,           {0x45, 0x53, 0x44, 0xBDFB, 0xD6B9, 0xB6AF, 0xD7F7},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD打开阀门-
  {2,             InvalidMenuID,         InvalidMenuID,           {0x45, 0x53, 0x44, 0xB4F2, 0xBFAA, 0xB7A7, 0xC3C5},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD关闭阀门-
  {3,             InvalidMenuID,         InvalidMenuID,           {0x45, 0x53, 0x44, 0xB9D8, 0xB1D5, 0xB7A7, 0xC3C5},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-ESD到中间位-
  {4,             InvalidMenuID,         InvalidMenuID,           {0x45, 0x53, 0x44, 0xB5BD, 0xD6D0, 0xBCE4, 0xCEBB},
  ESDSetting_Special, ESDSetting_SetKey, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},

  //-返回上级-
  {5,             Page_InternalPara_ID,  InvalidMenuID,           {0xB7B5, 0xBBD8, 0xC9CF, 0xBCB6},
  Dummy_Special, StandardMenu_Back2Parent, StandardMenu_UpKey, StandardMenu_DownKey, DummyFunction, DummyFunction},
};

//-第6个初始值代表菜单项个数,必须给正确的值-
MenuPara      MenuPara_ESDSetting = {0, 0, Enable, Disable, Invalid, 6, Multiplex_None, Dummy_Reset};


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
void Dummy_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex)
{
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Dummy_Reset(MenuPara *pMenuPara)
{
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
* 函数功能:    
* 输入参数:    Value,要显示的值(去除小数点后的值)
*              BitCount,显示的位数(去除小数点后的位数,例如0.5的位数为2)
*              DecimalBitCount,小数点位数(例如0.5的位数为2)
*              X_Offset,显示的水平偏移0~128
*              FrontZeroDisplay, = 1,有效数字前面的零也显示; 否则不显示.
* 输出参数:    无
* 返 回 值:    无
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

    //-千-
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

    //-百-
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

    //-十-
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
            if (FrontZeroDisplay == 1)
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
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ShowInc_Dec(int LineIndex)
{
    int Index = 0;
    int DisplayBufIndex = 0;

    if (LineIndex == 2)
    {
        DisplayBufIndex= 0;
        //-打开一下:增大-
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
        //-关闭一下:减小-
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

    /*--------------------------------第一行------------------------------------*/
    LineIndex = 0;
    ClearBuf(&g_DisplayBuf[0], Display_Buf_Size);

    ClearBuf((unsigned char *)&Code[0], sizeof(Code));
    if (Valve.Status.StatusBits.Opening == 1)
    {
        IsMoving = 1; 

        Code[0] = 0xB4F2;    //-打-
        Code[1] = 0xBFAA;    //-开-
        Code[2] = 0xD6D0;    //-中-
    }
    else if(Valve.Status.StatusBits.Shutting == 1)
    {
        IsMoving = 1; 

        Code[0] = 0xB9D8;    //-关-
        Code[1] = 0xB1D5;    //-闭-
        Code[2] = 0xD6D0;    //-中-
    }
    else if (Valve.Status.StatusBits.OpenLimit == 1)
    {
        Code[0] = 0xBFAA;    //-开-
        Code[1] = 0xB5BD;    //-到-
        Code[2] = 0xCEBB;    //-位-     
    }
    else if (Valve.Status.StatusBits.ShutLimit == 1)
    {
        Code[0] = 0xB9D8;    //-关-
        Code[1] = 0xB5BD;    //-到-
        Code[2] = 0xCEBB;    //-位-
    }
    else 
    {
    }

    DisplayBufIndex = 0;
    ShowBlock(&Code[0], 3, DisplayBufIndex);

    /*-右上角部分显示优先级:严重错误、手轮手动、紧急状态、方向错误
      -*/
    ClearBuf((unsigned char *)&LongCode[0], sizeof(LongCode));
    if (F_Disconnect == 1)
    {
        LongCode[Index++] = 0xCDA8;    //-通-
        LongCode[Index++] = 0xD1B6;    //-讯-
        LongCode[Index++] = 0xB6CF;    //-断-
        LongCode[Index++] = 0xC1AC;    //-连-
    }
    else if (Device.Error.ErrorByte != 0)
    {
        HaveError = 1;
    }
    else if (Device.CommMode.CommModeBits.Manual == 1)
    {
        LongCode[Index++] = 0xCAD6;    //-手-
        LongCode[Index++] = 0xC2D6;    //-轮-
        LongCode[Index++] = 0xCAD6;    //-手-
        LongCode[Index++] = 0xC2D6;    //-动-
    }
    else if (Device.Status.ESDStatus == ESDStatus_Valid)
    {
        LongCode[Index++] = 0xBDF4;    //-紧-
        LongCode[Index++] = 0xBCB1;    //-急-
        LongCode[Index++] = 0xD7B4;    //-状-
        LongCode[Index++] = 0xCCAC;    //-态-
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

        return;    //-直接返回-
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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

    /*--------------------------------第二行------------------------------------*/
    LineIndex = 1;
    ClearBuf(&g_DisplayBuf[0], Display_Buf_Size); 

    //-----------当前开度特殊字符上半部分的处理-----------
    CurOpening = Valve.MiscInfo.CurOpening;
    GetSingleNumber(CurOpening, &GeValue, &ShiValue, &BaiValue, &DummyData, &DummyData);
 
    //-百位-
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
    //-十位-
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
    //-个位-
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

    //-----------丢信/目标-----------------
    ClearBuf((unsigned char *)&Code[0], sizeof(Code));
    if ((Device.CommMode.CommModeBits.Remote == 1) && (Device.Para.RemoteType == RemoteType_Regulate))
    {
        if (Valve.Status.StatusBits.NoSignal == 1)
        {
            Code[0] = 0xB6AA;    //-丢-  
            Code[1] = 0xD0C5;    //-信-  
        }
        else
        {
            Code[0] = 0xC4BF;    //-目-  
            Code[1] = 0xB1EA;    //-标-  
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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


    /*--------------------------------第三行------------------------------------*/
    LineIndex = 2; 
    ClearBuf(&g_DisplayBuf[0], Display_Buf_Size);

    //-----------当前开度特殊字符下半部分的处理-----------
    CurOpening = Valve.MiscInfo.CurOpening;
    GetSingleNumber(CurOpening, &GeValue, &ShiValue, &BaiValue, &DummyData, &DummyData);
    //-百位-
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
    //-十位-
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
    //-个位-
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

     //-----------空白/目标开度-----------------
    BaiValue = 0;
    ShiValue = 0;
    GeValue  = 0;
    DstOpening = (signed int)Valve.Operation.DstOpening;    //-考虑到开度不可能溢出-
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
                Code[0] = 0xA3A0;              //-空格-  
            } 
            else
            {
                Code[0] = 0xA3B0 + BaiValue;    //-百-  
            }
            if ((ShiValue == 0) && (BaiValue == 0))
            {
                Code[1] = 0xA3A0;              //-空格-  
            } 
            else
            {
                Code[1] = 0xA3B0 + ShiValue;    //-十-  
            } 
            Code[2] = 0xA3B0 + GeValue;         //-个-  
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void NormalPage_Special3(const MenuStructure *pMenu, MenuPara *pMenuPara, int Line)
{
    int LineIndex = 0;
    int DisplayBufIndex = 0;

    unsigned int  Code[4] = {0};

    /*--------------------------------第四行------------------------------------*/
    LineIndex = 3;  
    ClearBuf(&g_DisplayBuf[0], Display_Buf_Size);   

    ClearBuf((unsigned char *)&Code[0], sizeof(Code));
    switch(Device.WorkMode.CurWorkMode)
    {
    case 0:
        Code[0] = 0xD4B6;    //-远-  
        Code[1] = 0xB3CC;    //-程-  
        Code[2] = 0xCDA8;    //-通-  
        Code[3] = 0xD0C5;    //-信- 
        break;
    case 1:
        Code[0] = 0xD4B6;    //-远-  
        Code[1] = 0xB3CC;    //-程-  
        Code[2] = 0xC4A3;    //-模-  
        Code[3] = 0xC4E2;    //-拟- 
        break;
    case 2:
        Code[0] = 0xD4B6;    //-远-  
        Code[1] = 0xB3CC;    //-程-  
        Code[2] = 0xB5E3;    //-点-  
        Code[3] = 0xB6AF;    //-动- 
        break;
    case 3:
        Code[0] = 0xD4B6;    //-远-  
        Code[1] = 0xB3CC;    //-程-  
        Code[2] = 0xB1A3;    //-保-  
        Code[3] = 0xB3D6;    //-持- 
        break;
    case 4:
        Code[0] = 0xD4B6;    //-远-  
        Code[1] = 0xB3CC;    //-程-  
        Code[2] = 0xCBAB;    //-双-  
        Code[3] = 0xCEBB;    //-位- 
        break;
    case 5:
        Code[0] = 0xCFD6;    //-现-  
        Code[1] = 0xB3A1;    //-场-  
        Code[2] = 0xB5E3;    //-点-  
        Code[3] = 0xB6AF;    //-动- 
        break;
    case 6:
        Code[0] = 0xCFD6;    //-现-  
        Code[1] = 0xB3A1;    //-场-  
        Code[2] = 0xB1A3;    //-保-  
        Code[3] = 0xB3D6;    //-持- 
        break;
    case 7:
        Code[0] = 0xCDA3;    //-停-  
        Code[1] = 0;
        Code[2] = 0;
        Code[3] = 0xD6B9;    //-止-  
        break;
    default:
        break;
    }
   
    DisplayBufIndex = 32;
 
    ShowBlock(&Code[0], 4, DisplayBufIndex);

    LcdRefresh(LineIndex);
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
void StandardMenu_Reset0(MenuPara *pMenuPara)
{
    pMenuPara->RowIndex = 0;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void StandardMenu_Reset1(MenuPara *pMenuPara)
{
    pMenuPara->ColumnIndex = 3;
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
    //-标准菜单最后1行一般为"返回上级"-
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
            //-当前位置#xxxx-
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
            //-操作旋钮可无限位-
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
            //-调整阀门位置，返-
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
            //-回停止位完成标定-
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void AdjustZeroFull_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (pMenuPara->ColumnIndex == 3)    //-确定-
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void LocalMode_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (pMenuPara->RowIndex == 0)    //-点动-
    {
        Device.Para.LocalMode = LocalMode_Jog;
    }
    else if (pMenuPara->RowIndex == 1)    //-保持-
    {
        Device.Para.LocalMode = LocalMode_Hold;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
        //-范围:1~20-
        if (Device.Para.DeadZone <= 20)
        {
            //-DeadZone = Device.Para.DeadZone * 5-
            DeadZone = (Device.Para.DeadZone << 2) + Device.Para.DeadZone;
        }

        if (DeadZone < 5)
        {
            //-自适应-
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Password_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    MenuPara_Password.ColumnIndex += 2;

    //-范围[4,6,8,10]-
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

            //-3次错误直接退出设置菜单,返回至常显-
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Password_UpKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    MenuPara_Password.ColumnIndex -= 2;

    //-范围[4,6,8,10]-
    if (MenuPara_Password.ColumnIndex < 4)
    {
        MenuPara_Password.ColumnIndex = 10;
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
    MenuPara_Password.ColumnIndex += 2;

    //-范围[4,6,8,10]-
    if (MenuPara_Password.ColumnIndex > 10)
    {
        MenuPara_Password.ColumnIndex = 4;
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
            //-至停止位返回上级-
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void AdjustOutput4mA_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
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
void AdjustOutput4mA_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
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
void AdjustOutput20mA_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
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
void AdjustOutput20mA_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
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
        Code[0] = 0xA3A0;               //-空格-
    }
    else
    {
        Code[0] = BaiValue + 0xA3B0;
    }

    Code[1] = ShiValue + 0xA3B0;
    Code[2] = 0xA3AE;                   //-小数点-
    Code[3] = GeValue + 0xA3B0;

    DisplayBufIndex = 48;
    ShowBlock(&Code[0], 4, DisplayBufIndex);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void AdjustInput4_20mA_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    if (pMenuPara->ColumnIndex == 3)    //-确定-
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
    else                                //-返回-
    {
        if (pMenu->ParentMenuID != InvalidMenuID)
        {
            IsNeedChangePage = 1;
            Show_Page(pMenu->ParentMenuID);
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
void AdjustInput4_20mA_UpKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    pMenuPara->ColumnIndex += 6;
    if (pMenuPara->ColumnIndex > 9)
    {
        pMenuPara->ColumnIndex = 3;
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
            Code[0] = 0xB2BB;                  //-不-
            Code[1] = 0xCFDE;                  //-限-
            Code[2] = 0xCAB1;                  //-时-
    
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void MaxActionTime_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    //-范围:0、[5~250]-
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void MaxActionTime_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara)
{
    //-范围:0、[5~250]-
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
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
        //-这里是直接判高低电平-
		if (KeyStateRead(Key_Inc) == KEY_PRESSED)
		{
			KeyValid[VKey_Inc] = 1;
		}
        //-这里是直接判高低电平-
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
        //-这里是直接判高低电平-
        if(KeyStateRead(Key_Shut) == KEY_PRESSED)
        { 
            KeyValid[VKey_Dec] = 1;
        }
        //-这里是直接判高低电平-
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

                Code = pMenu[StartRowIndex + i].MenuName[Device.Para.LanguageType][MenuNameIndex++];
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
    
                SwapFontBuf2DisplayBuf(FontBuf, FontBufLen, &g_DisplayBuf[0], DisplayBufIndex);

                if (CharacterType == CN_Character)    //-中文字符-
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
        Device.Flag.FlagBits.IsInMenu = 0;
    }
    else
    {
        Device.Flag.FlagBits.IsInMenu = 1;
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

    PageFunctionIndex = Page_Normal_ID;

    InsertTimer(&NoKeyTimer);
    InsertTimer(&AdjustInfoTimer);
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
    default:    //-默认常显-
        Page_Draw(&Menu_Normal[0], &MenuPara_Normal); 
        break;
    }
}

/*************************************END OF FILE*******************************/

