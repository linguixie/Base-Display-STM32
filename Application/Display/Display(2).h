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
 
 
#ifndef _DISPLAY_H_
#define _DISPLAY_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*******************************************************************************
*                                  ͷ  ��  ��
********************************************************************************/
#define Debug  0

/*******************************************************************************
*                                  ��  ��  ��
********************************************************************************/
//-�ް�����ʱ,��λ:s-
#define NoKeyTime        50
#define NoKey_Delay      (Multiplication(NoKeyTime, Delay_Second_Factor))

//-�궨�ɹ���ʱ,��λ:s-
#define AdjustInfoTime   5
#define AdjustInfo_Delay (Multiplication(AdjustInfoTime, Delay_Second_Factor))

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

#define Multiplex_Digit              0     //-���ָ���-
#define Multiplex_Adjust             1     //-�궨ʱ��/��-
#define Multiplex_None               2     //-�������͹�-


/*------------ҳ����------------*/
typedef enum
{
    Page_PowerOn_ID,
    Page_Normal_ID,
    Page_Error_ID,
    Page_Language_ID,
    Page_Configuration_ID,
    Page_Password_ID,

    Page_ParaSetting_ID,
    Page_ESDSetting_ID,
    Page_Emergency_ID,
    Page_Emergency1_ID,
    Page_DeadZone_ID,
    Page_OpenCurrent_ID,
    Page_ShutCurrent_ID,
    Page_MaxActionTime_ID,

    Page_Terminal_ID,
    Page_OpenTerminal_ID,
    Page_AdjustFullInfo_ID,
    Page_ShutTerminal_ID,
    Page_AdjustZeroInfo_ID,


    Page_ErrorFeedBack_ID,
    Page_RemoteHold_ID,

    Page_Control_ID,
    Page_Local_ID,
    Page_RemoteIO_ID,
    Page_RemoteAN_ID,

    Page_ANSignal_ID,
    Page_In4mA_ID,
    Page_In4mAInfo_ID,
    Page_In20mA_ID,
    Page_In20mAInfo_ID,
    Page_Out4mA_ID,
    Page_Out20mA_ID,

    Page_ChangePassword_ID,

    Page_ParaQuery_ID,
    Page_QueryESDSetting_ID,
    Page_QueryEmergency_ID,
    Page_QueryEmergency1_ID,
    Page_QueryDeadZone_ID,
    Page_QueryOpenCurrent_ID,
    Page_QueryShutCurrent_ID,
    Page_QueryMaxActionTime_ID,

    Page_QueryErrorFeedBack_ID,
    Page_QueryRemoteHold_ID,

    Page_QueryControl_ID,
    Page_QueryLocal_ID,
    Page_QueryRemoteIO_ID,
    Page_QueryRemoteAN_ID,

    Page_QueryPassword_ID,

    Page_QueryError_ID,

    Page_CommTest_ID,

    Page_Num
}PagesID;
/*******************************************************************************
*                                  ȫ�ֱ�������
********************************************************************************/
//-�˵�����(ע��!����Ͳ˵��ṹ����ƥ��)-
typedef struct MenuPara
{
    signed         char RowIndex;                //-�˵��н���������-
    signed         char ColumnIndex;             //-�˵��н���������(��Ӣ���ַ���СΪ��λ)-
    unsigned       char RowReverseSwitch;        //-Enable/Disable-
    unsigned       char ColumnReverseSwitch;     //-Enable/Disable-
    unsigned       char ColumnReverseUnit;       //-�н���ĵ�λ(��Ӣ���ַ���СΪ��λ)-
    const unsigned char NumOfRows;               //-�˵��������-
    void (*Reset)(struct MenuPara *pMenuPara);
}MenuPara;


//-�˵��ṹ��(ע��!����Ĳ˵�����ȽϹ�,�궨���ҳ��Ҳ���ܿ����ǲ˵�)-
typedef struct MenuStructure
{
    unsigned char         RowIndex;             //-�˵���������-
    unsigned int          ParentMenuID;         //-���˵��ı��(��������ö�ٶ���PagesID)-
    unsigned int          ChildMenuID;          //-�Ӳ˵��ı��-
    unsigned int          MenuName[2][16];      //-�˵��������(GB2312����)-

    void (*SpecialDeal)(const struct MenuStructure *pMenu, MenuPara *pMenuPara);
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
*                                  ȫ�ֺ�������
********************************************************************************/
void DummyFunction(const MenuStructure *pMenu, MenuPara *pMenuPara);
void DummyFunction1(MenuPara *pMenuPara);
void ClearBuf(unsigned char *Buf, int BufLen);


void Opening_Special(const MenuStructure *pMenu, MenuPara *pMenuPara);
void ValvePos_Special(const MenuStructure *pMenu, MenuPara *pMenuPara);


void Normal_Special0(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Normal_Special1(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Normal_Special2(const MenuStructure *pMenu, MenuPara *pMenuPara);


void Error_Special0(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Error_Special1(const MenuStructure *pMenu, MenuPara *pMenuPara);


void StandardMenu_Reset0(MenuPara *pMenuPara);
void StandardMenu_Reset1(MenuPara *pMenuPara);
void StandardMenu_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void StandardMenu_UpKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void StandardMenu_DownKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void StandardMenu_Back2Parent(const MenuStructure *pMenu, MenuPara *pMenuPara);
void StandardMenu_Set2Child(const MenuStructure *pMenu, MenuPara *pMenuPara);


void Language_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);


void Password_Reset(MenuPara *pMenuPara);
void Password_Special(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Password_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Password_UpKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Password_DownKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Password_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Password_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara);


void ESDSetting_Reset(MenuPara *pMenuPara);
void ESDSetting_Special(const MenuStructure *pMenu, MenuPara *pMenuPara);
void ESDSetting_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara); 


void DeadZone_Special(const MenuStructure *pMenu, MenuPara *pMenuPara);
void DeadZone_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void DeadZone_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara);


void OpenCurrent_Special(const MenuStructure *pMenu, MenuPara *pMenuPara);
void OpenCurrent_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void OpenCurrent_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara);


void ShutCurrent_Special(const MenuStructure *pMenu, MenuPara *pMenuPara);
void ShutCurrent_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void ShutCurrent_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara);


void MaxActionTime_Special(const MenuStructure *pMenu, MenuPara *pMenuPara);
void MaxActionTime_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void MaxActionTime_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara);


void OpenTerminal_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void OpenTerminal_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void OpenTerminal_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void ShutTerminal_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void ShutTerminal_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void ShutTerminal_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);


void ErrorFeedBack_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void ErrorFeedBack_Reset(MenuPara *pMenuPara);
void RemoteHold_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void RemoteHold_Reset(MenuPara *pMenuPara);


void Local_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Local_Reset(MenuPara *pMenuPara);
void RemoteIO_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void RemoteIO_Reset(MenuPara *pMenuPara);
void RemoteAN_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void RemoteAN_Reset(MenuPara *pMenuPara);


void In4mA_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void In20mA_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void In4_20mA_Special(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Out4mA_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Out4mA_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Out4mA_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Out20mA_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Out20mA_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void Out20mA_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);


void ChangePassword_Special(const MenuStructure *pMenu, MenuPara *pMenuPara);
void ChangePassword_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void ChangePassword_UpKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void ChangePassword_DownKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void ChangePassword_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void ChangePassword_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara);


void QueryError_Special(const MenuStructure *pMenu, MenuPara *pMenuPara);
void QueryError_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);


#if Debug
void CommTest_Special(const MenuStructure *pMenu, MenuPara *pMenuPara);
void CommTest_SetKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void CommTest_IncKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
void CommTest_DecKey(const MenuStructure *pMenu, MenuPara *pMenuPara);
#endif

void ShowStar(int X_Offset);
void Task_Display(void);
void Page_PowerOn(void);
void Page_Normal(void);
void Page_MainMenu(void);
void DisplayInit(void);
unsigned char IsInMenu(void);
void EnterMenu(unsigned char Flag);
void GetSingleNumber(unsigned int Data, unsigned char *pGe, unsigned char *pShi, unsigned char *pBai, unsigned char *pQian, unsigned char *pWan);
#ifdef __cplusplus
}
#endif
 
#endif /* _DISPLAY_H_ */
 
 
/*************************************END OF FILE*******************************/

