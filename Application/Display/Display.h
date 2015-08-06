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
#define Debug            0
 
/*******************************************************************************
*                                  ��  ��  ��
********************************************************************************/
//-�ް�����ʱ,��λ:s-
#define NoKeyTime        50
#define NoKey_Delay      (Multiplication(NoKeyTime, Delay_Second_Factor))

//-�궨�ɹ���ʱ,��λ:ms-
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

#define Multiplex_Digit              0     //-���ָ���-      
#define Multiplex_Adjust             1     //-�궨ʱ��/��-
#define Multiplex_None               2     //-�������͹�-
#define Multiplex_Password           3     //-��������-        


/*------------ҳ����------------*/
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
    const unsigned char IsMultiplex;             //--
    void (*Reset)(struct MenuPara *pMenuPara);
}MenuPara;


//-�˵��ṹ��(ע��!����Ĳ˵�����ȽϹ�,�궨���ҳ��Ҳ���ܿ����ǲ˵�)-
typedef struct MenuStructure
{
    unsigned char         ItemIndex;            //-�˵���������-
    unsigned int          ParentMenuID;         //-���˵��ı��(��������ö�ٶ���PagesID)-
    unsigned int          ChildMenuID;          //-�Ӳ˵��ı��-
    unsigned int          MenuName[2][16];      //-�˵��������(GB2312����)-

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
*                                  ȫ�ֺ�������
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

