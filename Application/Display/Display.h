/*******************************************************************************
* Copyright (c) 2105,威鹏自动化科技有限公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-3-12
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：
********************************************************************************/
 
 
#ifndef _DISPLAY_H_
#define _DISPLAY_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*******************************************************************************
*                                  头  文  件
********************************************************************************/
#define Debug            0
 
/*******************************************************************************
*                                  宏  定  义
********************************************************************************/
//-无按键延时,单位:s-
#define NoKeyTime        50
#define NoKey_Delay      (Multiplication(NoKeyTime, Delay_Second_Factor))

//-标定成功延时,单位:ms-
#define AdjustInfoTime   2000
#define AdjustInfo_Delay (Division(AdjustInfoTime, Delay_MiliSecond_Factor))

#define Show_Page(ID)    (PageFunctionIndex = ID)

#define Items_Per_Page               4
#define Items_Per_Page_Shif_Count    2
#define Items_Per_Page_Mask          0x03

#define Items_Per_Page1              2
#define Items_Per_Page_Shif_Count1   1
#define Items_Per_Page_Mask1         0x01

#define InvalidMenuID                0xFF


#define Adjust_Zero                  0
#define Adjust_Full                  1

#define Default                      0
#define Invalid                      0

#define Enable                       1
#define Disable                      0

#define Multiplex_Digit              0     //-数字复用-      
#define Multiplex_Adjust             1     //-标定时开/关-
#define Multiplex_None               2     //-正常开和关-
#define Multiplex_Password           3     //-密码输入-        


/*------------页面编号------------*/
typedef enum
{
    Page_PowerOn_ID,
    Page_Normal_ID,
    Page_MainMenu_ID,
    Page_AdjustZero_ID,
    Page_AdjustZeroAction_ID,
    Page_AdjustZeroInfo_ID,
    Page_AdjustFull_ID,
    Page_AdjustFullAction_ID,
    Page_AdjustFullInfo_ID,
    Page_LocalMode_ID,
    Page_RemoteIOMode_ID,
    Page_RemoteANMode_ID,
    Page_InternalPara_ID,                     
    Page_Password_ID,
    Page_DeadZone_ID,
    Page_AdjustOutput4mA_ID,
    Page_AdjustOutput20mA_ID,
    Page_AdjustInput4mA_ID,
    Page_AdjustInput4mAInfo_ID,
    Page_AdjustInput20mA_ID,
    Page_AdjustInput20mAInfo_ID,
    Page_ShutCurrent_ID,
    Page_OpenCurrent_ID,
    Page_MaxActionTime_ID,
    Page_ErrorFeedback_ID,
    Page_ESDSetting_ID,
    Page_CommTest_ID
}PagesID;
/*******************************************************************************
*                                  全局变量声明
********************************************************************************/
//-菜单参数(注意!必须和菜单结构体相匹配)-
typedef struct MenuPara
{
    signed         char RowIndex;                //-菜单行焦点索引号-
    signed         char ColumnIndex;             //-菜单列焦点索引号(以英文字符大小为单位)-
    unsigned       char RowReverseSwitch;        //-Enable/Disable-
    unsigned       char ColumnReverseSwitch;     //-Enable/Disable-
    unsigned       char ColumnReverseUnit;       //-列焦点的单位(以英文字符大小为单位)-
    const unsigned char NumOfRows;               //-菜单项的行数-
    const unsigned char IsMultiplex;             //--
    void (*Reset)(struct MenuPara *pMenuPara);
}MenuPara;


//-菜单结构体(注意!这里的菜单含义比较广,标定零点页面也可能看作是菜单)-
typedef struct MenuStructure
{
    unsigned char         ItemIndex;            //-菜单项索引号-
    unsigned int          ParentMenuID;         //-父菜单的编号(编号请参照枚举定义PagesID)-
    unsigned int          ChildMenuID;          //-子菜单的编号-
    unsigned int          MenuName[2][16];      //-菜单项的内容(GB2312编码)-

    void (*SpecialDeal)(const struct MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
    void (*SetKeyDeal) (const struct MenuStructure *pMenu,  MenuPara *pMenuPara);
    void (*UpKeyDeal) (const struct MenuStructure *pMenu,  MenuPara *pMenuPara);
    void (*DownKeyDeal) (const struct MenuStructure *pMenu,  MenuPara *pMenuPara); 
    void (*IncKeyDeal) (const struct MenuStructure *pMenu,  MenuPara *pMenuPara);
    void (*DecKeyDeal) (const struct MenuStructure *pMenu,  MenuPara *pMenuPara); 

}MenuStructure; 


extern unsigned char PageFunctionIndex;
extern struct MenuPara      MenuPara_Main;
extern struct MenuPara      MenuPara_InternalPara;

extern unsigned char NeedResetMenuPara;
extern signed long AdjustInfoTimer;
/*******************************************************************************
*                                  全局函数声明
********************************************************************************/
void Dummy_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void DummyFunction(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Dummy_Reset(MenuPara *pMenuPara);
void ClearBuf(unsigned char *Buf, int BufLen);

void NormalPage_Special0(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void NormalPage_Special1(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void NormalPage_Special2(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void NormalPage_Special3(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void NormalPage_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);

void StandardMenu_Reset0(MenuPara *pMenuPara);
void StandardMenu_Reset1(MenuPara *pMenuPara);
void StandardMenu_Back2Parent(const MenuStructure *pMenu, MenuPara *pMenuPara);
void StandardMenu_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void StandardMenu_DownKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void StandardMenu_UpKey(const MenuStructure *pMenu, MenuPara *pMenuPara);

void AdjustZeroFull_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void AdjustZeroFull_SetKey (const MenuStructure *pMenu, MenuPara *pMenuPara);
void AdjustZeroFull_DownKey (const MenuStructure *pMenu, MenuPara *pMenuPara);
void AdjustZeroFull_UpKey (const MenuStructure *pMenu, MenuPara *pMenuPara);
void AdjustZeroFull_IncKey (const MenuStructure *pMenu, MenuPara *pMenuPara);
void AdjustZeroFull_DecKey (const MenuStructure *pMenu, MenuPara *pMenuPara);

void LocalMode_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void LocalMode_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);


void RemoteIOMode_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void RemoteIOMode_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);

void RemoteANMode_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void RemoteANMode_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);

void DeadZone_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void DeadZone_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void DeadZone_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara);

void Password_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void Password_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Password_UpKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Password_DownKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Password_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Password_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara);


void InternalPara_ESD_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void InternalPara_ESD_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);


void AdjustOutput4_20mA_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void AdjustOutput4mA_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void AdjustOutput4mA_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara);

void AdjustOutput20mA_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void AdjustOutput20mA_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara);

void AdjustInput4_20mA_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void AdjustInput4_20mA_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void AdjustInput4_20mA_DownKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void AdjustInput4_20mA_UpKey(const MenuStructure *pMenu, MenuPara *pMenuPara);


void ShutCurrent_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void ShutCurrent_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void ShutCurrent_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara);


void OpenCurrent_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void OpenCurrent_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void OpenCurrent_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara);

void MaxActionTime_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void MaxActionTime_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void MaxActionTime_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void MaxActionTime_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara);


void ErrorFeedback_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void ErrorFeedback_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);


void ESDSetting_Special(const MenuStructure *pMenu, MenuPara *pMenuPara, int LineIndex);
void ESDSetting_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);


void Task_Display(void);
void Page_PowerOn(void);
void Page_Normal(void);
void Page_MinMenu(void);
void DisplayInit(void);
unsigned char IsInMenu(void);
void EnterMenu(unsigned char Flag);
void GetSingleNumber(unsigned int Data, unsigned char *pGe, unsigned char *pShi, unsigned char *pBai, unsigned char *pQian, unsigned char *pWan);
#ifdef __cplusplus
}
#endif
 
#endif /* _DISPLAY_H_ */
 
 
/*************************************END OF FILE*******************************/

