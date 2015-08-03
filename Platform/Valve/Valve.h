/*******************************************************************************
* Copyright (c) 2015,xxx��˾
 *            All rights reserved.
* �������ƣ�
* �� �� �ţ�1.0
* �� �� �ˣ�л�ֹ�
* ����ʱ�䣺2015-3-28
* �� �� ��: л�ֹ�
* �޸�ʱ��: 2015-5-11
* �޸�˵��: ��VPcon500D��ֲ��MDL_IO1410D.
* ��    �������Ų����ĳ���:
*             1��ValveOperation�ǶԷ��ŵĵ�һ���װ.����ʵ�ʵ���������.
*             2��ValveAction�ǶԷ��Ų����ĵڶ����װ.
*             3��ValveStatus.Opening/Shutting�����һ���װ.
*           ��������ĳ���:
*             1��DeviceInput�Ƕ�����ĵ�һ���װ.      
*             2��ValveStatus��ValveError�����һ���װ.
*           �豸�����ĳ���:
*             1��DeviceOperation�Ƕ��豸�����ĵ�һ���װ.����ʵ�ʵ���������.
*             2��DeviceAction�Ƕ��豸�����ĵڶ����װ.
*             ע��:�豸�ͷ��ŵ�����.�豸������N������.
*           �豸�ͷ��ŵ�����:
*             1����Щ���������豸,��Щ�������ڷ���.����:�������˫�������.      
********************************************************************************/
 
 
#ifndef _VALVE_H_
#define _VALVE_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*******************************************************************************
*                                  ͷ  ��  ��
********************************************************************************/

 
/*******************************************************************************
*                                  ��  ��  ��
********************************************************************************/
//-!!!!���㿪��ʱ,�˷�����λʵ��,��λ����������:400 = 2^8 + 2^7 + 2^4-
#define Opening_LeftShift_Factor0    8
#define Opening_LeftShift_Factor1    7
#define Opening_LeftShift_Factor2    4

#define MaxOpening               400    //-����ȫ���Ŀ���-
#define MidOpening               200    //-���Ű뿪��յĿ���-
#define MinOpening               0      //-����ȫ�յĿ���(һ�����Ϊ0)- 
#define Opening2o5Percent        10     //-����2.5%�Ŀ���-
#define Opening15Percent         60     //-����15%�Ŀ���-
#define OpeningOffset            2      //-400����ʱ�ж�����ֹͣ�Ļ���-
#define OpeningDeviation         3      //-400����ʱ��������-
#define OpeningCompensation      3      //-���ŵĿ��Ȳ�������,�ڷ��Ž������������ʱ���õ�-

#define Mask_Status_Opening    0x08
#define Mask_Status_Shutting   0x04
#define Mask_Status_OpenLimit  0x10
#define Mask_Status_ShutLimit  0x20

#define ValveNum               1
/*----------------------ö�ٳ�����--------------------------------------*/
//---
typedef enum
{
    Operation_Open, 
    Operation_Shut,
    Operation_Stop,
    Operation_Run,
    Operation_NoLimitOpen,
    Operation_NoLimitShut,
    Operation_None
}Operation;

//---
typedef enum
{
    Action_Open, 
    Action_Shut,
    Action_Reset,
    Action_Stop,
    Action_Run,
    Action_DelayOpen,
    Action_DelayShut,
    Action_NoLimitOpen,
    Action_NoLimitShut,
    Action_NoLimitStop,
    Action_None
}Action;

//---
typedef enum
{  
    WorkMode_Bus,                  //-����-
    WorkMode_RemoteAN,             //-Զ��ģ��-
    WorkMode_RemoteJog,            //-Զ�̵㶯-
    WorkMode_RemoteHold,           //-Զ�̱���-
    WorkMode_RemoteDibit,          //-Զ��˫λ-
    WorkMode_LocalJog,             //-�ֳ��㶯-
    WorkMode_LocalHold,            //-�ֳ�����-
    WorkMode_Stop,                 //-ֹͣ- 
    WorkMode_None
}WorkMode;

//---
typedef enum
{
    RemoteType_Regulate,              //-������-
    RemoteType_Switch                 //-������-
}RemoteType;

//---
typedef enum
{
    LocalMode_Jog,                     //-�㶯-
    LocalMode_Hold                     //-����-
}LocalMode;

//---
typedef enum
{
    RemoteIOMode_Jog,                  //-�㶯-
    RemoteIOMode_Hold,                 //-Զ�̱���-
    RemoteIOMode_HoldNormallyOpen,     //-����ͣ����-
    RemoteIOMode_HoldNormallyShut,     //-����ͣ����-
    RemoteIOMode_SignalOnNoOff,        //-���ſ����Ź�-
    RemoteIOMode_SignalOffNoOn,        //-���Ź����ſ�-
    RemoteIOMode_BiPos
}RemoteIOMode;

//---
typedef enum
{
    RemoteANMode_Invalid,              //-����-
    RemoteANMode_NoSigKeep,            //-���ű�λ-
    RemoteANMode_NoSigShut,            //-����ȫ��-
    RemoteANMode_NoSigMid,             //-���ž���-
    RemoteANMode_NoSigOpen             //-����ȫ��-
}RemoteANMode;

//---
typedef enum
{
    AutoMode_Continuous,
    AutoMode_Timing,

    AutoMode_Invalid
}AutoMode;

//---
typedef enum
{
    Status_Reset,
    Status_TimingRun,
    Status_TimingRest,
    Status_Manual,
    Status_Auto,
    Status_Opening,                    
    Status_Shutting,
    Status_Run,
    Status_Stop,
    Status_OpenLimit,
    Status_ShutLimit
}Status;

//---
typedef enum
{
    Error_PowerPhaseLack,
    Error_VoltageHigh,
    Error_VoltageLow,
    Error_OverHeat,
    Error_SwitchException,
    Error_OpenTimeOut,
    Error_OpenTorques,
    Error_OpenDirection,
    Error_OpenOverCurrent,
    Error_ShutTimeOut,
    Error_ShutDirection,
    Error_ShutTorques,
    Error_ShutOverCurrent,
    Error_NoOffPositionSignal
}Error;

//---
typedef enum
{
    Comm_BusOk,
    Comm_RemoteNoSignal
}Communication;

//--
typedef enum
{
    Input_Invalid = 0xFF
}Input;


//--
typedef enum
{
    ESDDisplay_Enable,
    ESDDisplay_Disable
}ESDDisplay_TypdeDef;

//-ESDģʽ-
typedef enum
{
    ESDMode_Disable,
    ESDMode_NoAction,
    ESDMode_Open,
    ESDMode_Shut,
    ESDMode_Middle
}ESDMode_TypeDef;

//-ESD״̬-
typedef enum
{
    ESDStatus_Valid,
    ESDStatus_Invalid
}ESDStatus_TypeDef;

//-�����򳣱�-
typedef enum
{
    IO_NormallyOpen,
    IO_NormallyShut
}IO_TypeDef;

//-����������-
typedef enum
{
    EncoderDirection_Reverse,
    EncoderDirection_Forward
}EncoderDirection_TypeDef;

//-������ʾ����-
typedef enum
{
    CurrentDisplay_Enable,
    CurrentDisplay_Disable
}CurrentDisplay_TypeDef;

//-����С����λ��-
typedef enum
{
    CurrentDecimalBits_One,
    CurrentDecimalBits_Two
}CurrentDecimalBits_TypeDef;
/*******************************************************************************
*                                  ȫ�ֱ�������
********************************************************************************/
/*==================���嵥��������صĽṹ��===================================*/
//-���ż�����-
typedef enum{Counter_PVHigherInStop, Counter_PVHigherInOpen, Counter_PVHigherInShut, 
             Counter_SVHigherInStop, Counter_SVHigherInOpen, Counter_SVHigherInShut,
             Counter_CheckRealStop,  Counter_CheckRealRun, 
             Counter_Num
            }Counter_TypeDef;



//-���Ų����ṹ��:���淧�ŵ���ز�����������ʽ.-
typedef struct
{
    unsigned char Operation;            //-���Ž�Ҫִ�е�����:������-
    unsigned int  DstOpening;           //-Ŀ��(Destinantion)����-
}ValveOperation_T;


//-���Ŷ����ṹ��:���淧�ż���Ҫִ�еĶ���(�Ͳ����ṹ�������:������������Ĳ�������).-
typedef struct
{
    unsigned char PreAction;            //-������ȥִ�еĶ���:������-
    unsigned char Action;               //-���Ž�Ҫִ�еĶ���:������-
}ValveAction_T;


//-���ŵı궨�������ṹ��:���淧�ŵı궨����-
typedef struct Adjust
{
    union
    {
        struct                      
        {
            unsigned Zero:1;          //-��������-
            unsigned Full:1;          //-��������-
            unsigned Reserved0:1;     //-Reserved-
            unsigned Reserved1:1;     //-Reserved-
            unsigned Reserved2:1;     //-Reserved-
            unsigned Reserved3:1;     //-Reserved-
            unsigned NoLimitShut:1;   //-����λ��-
            unsigned NoLimitOpen:1;   //-����λ��-
        }Adjust0Bits;
        unsigned char Adjust0Byte;  
    }Adjust0;

    union
    {
        struct                      
        {
            unsigned Input4mA:1;      //-�궨����4mA-
            unsigned Input20mA:1;     //-�궨����20mA-
            unsigned Reserved0:1;     //-Reserved-
            unsigned Reserved1:1;     //-Reserved-
            unsigned DecOutput20mA:1; //-��С���20mA-
            unsigned IncOutput20mA:1; //-�������20mA-
            unsigned DecOutput4mA:1;  //-��С���4mA-
            unsigned IncOutput4mA:1;  //-�������4mA-
        }Adjust1Bits;
        unsigned char Adjust1Byte;  
    }Adjust1;
}Adjust_T;


//-���ŵı궨�ɹ�������:���淧�ŵı궨�ɹ���־-
typedef union
{
    struct                      
    {
        unsigned Zero:1;              //-����-
        unsigned Full:1;              //-����-
        unsigned Reserved2:1;         //-Reserved-
        unsigned Reserved3:1;         //-Reserved-
        unsigned Reserved4:1;         //-Reserved-
        unsigned Reserved5:1;         //-Reserved-
        unsigned Input4mA:1;          //-�궨����4mA-
        unsigned Input20mA:1;         //-�궨����20mA-
    }AdjustInfoBits;
    unsigned char AdjustInfoByte;  
}AdjustInfo_T;


//-����״̬������:���淧�ŵ�״̬-
typedef union 
{
    struct                      
    {
        unsigned NoSignal:1;            //-����-
        unsigned BusValid:1;            //-��������-
        unsigned Shutting:1;            //-���ڹ�-
        unsigned Opening:1;             //-���ڿ�-
        unsigned ShutLimit:1;           //-����λ-
        unsigned OpenLimit:1;           //-����λ-
        unsigned Reserved6:1;           //-Ԥ��-
        unsigned Reserved7:1;           //-Ԥ��-
    }StatusBits;
    unsigned char StatusByte;        
}ValveStatus_T;


//-���Ŵ���������-
typedef union
{
    struct
    {
        unsigned ShutTorques:1;      //-�����ع���-
        unsigned OpenTorques:1;      //-�����ع���-
        unsigned ShutTimeout:1;      //-����ʱ-
        unsigned OpenTimeout:1;      //-����ʱ-
        unsigned ShutOverCurrent:1;  //-�������-
        unsigned OpenOverCurrent:1;  //-�������-
        unsigned ShutDirection:1;    //-�������-
        unsigned OpenDirection:1;    //-�������-
    }ErrorBits;
    unsigned char ErrorByte;
}ValveError_T;


//-��̬��Ϣ�ṹ��:���淧�ŵ�ʵʱ��̬��Ϣ-
typedef struct
{           
    signed   long  ActionTimer; 
    signed   long  BigLoadTimer;
    signed   long  JustStopTimer;
    signed   long  MotorTimer;

    unsigned int   PositionValue;         //-���ŵ�λ�����߱������Ĳ���ֵ-
    unsigned int   In4_20mAADValue;       //-����4-20mA�Ĳ���ֵ-
    unsigned int   Current;               //-����ֵ-
    unsigned int   PwmDuty_H;             //-���4-20mA��ռ�ձȸ��ֽ�-
    unsigned int   PwmDuty_L;             //-���4-20mA��ռ�ձȵ��ֽ�-
    unsigned int   CurOpening;            //-��ǰ(Current)����-
    unsigned int   StopOutputOpening;     //-����ֹͣ��������ź�ʱ�Ŀ���(����߳����õ�)-
    unsigned int   StopTmpOpening;        //-����ֹͣʱ���м俪��(�ж��Ƿ�����ֹͣ�õ�)-
    unsigned int   RunTmpOpening;         //-��������ʱ���м俪��(�ж��Ƿ����������õ�)-
    unsigned int   DeadZone;              //-����(ע���Para�еĲ�һ��һ��)-
    unsigned int   OpenInertia;           //-�����Գ���(ע���Para�еĲ�һ��һ��)-
    unsigned int   ShutInertia;           //-�ع��Գ���(ע���Para�еĲ�һ��һ��)-
    
}MiscInfo_T;


//-���ű�־-
typedef union
{
    struct
    {
        unsigned F_JustRun:1;           //-�ſ�����Źط�-
        unsigned F_JustStop:1;          //-��ֹͣ(��JustRun��ͬ:3s��ǿ�����)-
        unsigned F_JustOpen:1;          //-��ǰ�п�������(ѧϰ�߳���ʱ�õ�)-
        unsigned F_BigLoad:1;           //-����-
        unsigned F_StartInInertia:1;    //-�Ƿ�������������������ı�־:=1,��; ����,����-
        unsigned Reserved5:1;           //-Ԥ��-
        unsigned Reserved6:1;           //-Ԥ��-
        unsigned Reserved7:1;           //-Ԥ��-
    }FlagBits;
    unsigned char FlagByte;
}Flag_T;


//-���ż�����-
typedef struct
{           
    unsigned char CounterArray[Counter_Num];   
}Counter_T;


typedef struct
{
    ValveOperation_T      Operation;
    ValveAction_T         Action;
    Adjust_T              Adjust;
    AdjustInfo_T          AdjustInfo;
    ValveStatus_T         Status;
    ValveError_T          Error;
    MiscInfo_T            MiscInfo;
    Flag_T                Flag;
    Counter_T             Counter;
}Valve_T;


/*==================���������豸��صĽṹ��===================================*/
//-�����ṹ��:�����豸����ز�����������ʽ-
typedef struct
{
    unsigned char PreOperation;
    unsigned char Operation;            //-��������-
}DeviceOperation_T;


//-�����ṹ��:�����豸���ȹض���-
typedef struct
{
    unsigned char PreAction;            //-��������ֹͣ(�Ͳ����ṹ�������:������������Ĳ�������)-
    unsigned char Action;
}DeviceAction_T;


//-�����豸��ͨѶģʽ:���������豸��ͨѶģʽ(�˽ṹ���WorkMode���ظ�,����Ϊ����д���㶨��)-
typedef union 
{
    struct
    {
        unsigned Local:1;               //-�ֳ�״̬-
        unsigned Remote:1;              //-Զ��״̬-
        unsigned Bus:1;                 //-����-
        unsigned Manual:1;              //-�����ֶ�-
        unsigned Reserved4:1;           //-Ԥ��-
        unsigned Reserved5:1;           //-Ԥ��-
        unsigned Reserved6:1;           //-Ԥ��-
        unsigned Reserved7:1;           //-Ԥ��-
    }CommModeBits;
    unsigned char CommModeByte;         //-�豸ͨѶģʽ-   
}DeviceCommMode_T;


//-�����豸�Ĺ���ģʽ:���������豸�Ĺ���ģʽ-
typedef struct 
{
    unsigned char CurWorkMode;         //-�豸����ģʽ-   
}DeviceWorkMode_T;


//-�����豸��״̬�ṹ��:���������豸������״̬-
typedef struct 
{
    unsigned char Status;               //-�豸��ǰ״̬-
    unsigned char ESDStatus;            //-ESD��orû��-
}DeviceStatus_T;


//-�����豸�Ĵ���-
typedef union
{
    struct 
    {
        unsigned ValveException:1;      //-��λ�쳣-
        unsigned EncoderNoChange:1;     //-�����Ŷ���ʱ,���������仯-
        unsigned Reserved2:1;           //-Reserved-
        unsigned Reserved3:1;           //-Reserved-
        unsigned Reserved4:1;           //-Reserved-
        unsigned Reserved5:1;           //-Reserved-
        unsigned Reserved6:1;           //-Reserved-
        unsigned LackPhase:1;           //-Reserved-
    }ErrorBits;
    unsigned char ErrorByte;                                                                                   
}DeviceError_T;

//-�����豸�ı�־-
typedef union
{
    struct
    {
        unsigned IsInMenu:1;            //-�Ƿ����˵�����-
        unsigned IsInLocalAdjust:1;     //-�Ƿ������ֳ�ģʽ�������ֵ���-
        unsigned Reserved2:1;           //-Ԥ��-
        unsigned Reserved3:1;           //-Ԥ��-
        unsigned Reserved4:1;           //-Ԥ��-
        unsigned Reserved5:1;           //-Ԥ��-
        unsigned Reserved6:1;           //-Ԥ��-
        unsigned Reserved7:1;           //-Ԥ��-
    }FlagBits;
    unsigned char FlagByte;
}DeviceFlag_T;


/*-----------------------------���²��ָ���ʵ��Ӧ���޸�------------------------*/
//-��̬��Ϣ�ṹ��:���淧�ŵ�ʵʱ��̬��Ϣ-
typedef struct 
{
    unsigned char Reserved;            //-Ԥ��-
    unsigned char NeedReset;           //--
}DeviceMiscInfo_T;


//-���ŵ�����ṹ��:���淧�ŵ�����-
typedef struct Input
{
    //-�ֳ�/Զ��-
    unsigned char Remote;
    unsigned char Local;

    //-�ֳ���/��/ͣ-
    unsigned char LocalOpen;
    unsigned char LocalShut;
    unsigned char LocalStop;
}DeviceInput_T;


//-�����ṹ��:���淧�ŵ���ز���,������Ϊ2����(��Ҫ�������н����ġ�����Ҫ���н�����)-
typedef struct 
{
    //-���²����������н���-
    unsigned char LocalMode;            //-�ֳ��㶯���Ǳ���-
    unsigned char RemoteIOMode;         //-Զ�̿���ģʽ:���ſ����Źء����Ź����޿�-
    unsigned char RemoteANMode;         //-Զ��ģ��ģʽ:���ű�λ������ȫ��������ȫ�ء����ž���-
    unsigned char DeadZone;             //-���ŵ�����(1~20,ÿ�������0.5%)-
    unsigned char MaxActionTime;        //-����������ʱ��-
    unsigned char MaxOpenCurrent;       //-�������������-
    unsigned char MaxShutCurrent;       //-��������������-
    unsigned char ESDMode;              //-ESDģʽ-
    unsigned char ErrorFeedBack;        //-���Ϸ�������-
 

    //-���²������������н���-
    unsigned char RemoteType;           //-��������:�����ͻ��ǿ�����-
    unsigned char ESDDisplayEnable;     //-ESD(����״̬)�˵�����ʾ���-
    unsigned char CurrentDisplayEnable; //-����ʱ,�Ƿ���ʾ����-
    unsigned char CurrentDecimalBits;   //-��ʾ������λ��-
    unsigned int  FullValue;            //-����ʱ���ŵĵ�λ�����������ֵ-
    unsigned int  ZeroValue;            //-���ʱ���ŵĵ�λ�����������ֵ-
    unsigned int  Input4mAADValue;      //-����4mA��AD����ֵ-
    unsigned int  Input20mAADValue;     //-����20mA��AD����ֵ-       
    unsigned int  Output4mAPwmDuty;     //-���4mA��PWMռ�ձ�-
    unsigned int  Output20mAPwmDuty;    //-���20mA��PWMռ�ձ�-
    unsigned int  OpenInertia;          //-�����Գ���-
    unsigned int  ShutInertia;          //-�ع��Գ���-
    unsigned char EncoderDirection;     //-����������-
    unsigned char LanguageType;         //-��������-

}DevicePara_T;


//-ע:�������Valve_TҲ������Device_T��,���ǿ��ǵ�����ʱ��̫����-
typedef struct
{
    DeviceOperation_T        Operation;
    DeviceAction_T           Action;
    DeviceCommMode_T         CommMode;
    DeviceWorkMode_T         WorkMode;
    DeviceStatus_T           Status;
    DeviceError_T            Error;
    DeviceFlag_T             Flag;
    DeviceMiscInfo_T         MiscInfo;       
    DevicePara_T             Para;              
    DeviceInput_T            Input; 
}Device_T;


extern Device_T              Device;
extern Valve_T               Valve;
extern Valve_T *ValveArray[ValveNum];
extern const unsigned char Counter_Max[Counter_Num];

extern unsigned int ShutInertiaBuf[6];
extern unsigned int OpenInertiaBuf[6];
/*******************************************************************************
*                                  ȫ�ֺ�������
********************************************************************************/
void ValveInit(void);

void DeviceSched(Device_T *pDevice);
void DeviceProcess(Device_T *pDevice);
void ValveTimer(Device_T *pDevice, Valve_T *pValve);
void ValveProcess(Device_T *pDevice, Valve_T *pValve);

void EnableDeviceReset(Device_T *pDevice);
void EnableDeviceRun(Device_T *pDevice);
void EnableDeviceStop(Device_T *pDevice);

void EnableValveNoLimitOpen(Valve_T *pValve);
void EnableValveNoLimitShut(Valve_T *pValve);
void EnableValveNoLimitStop(Valve_T *pValve);

void EnableValveOpen(Valve_T *pValve);
void EnableValveShut(Valve_T *pValve);
void EnableValveStop(Valve_T *pValve);
unsigned char GetValveStatus(Valve_T *pValve);

#ifdef __cplusplus
}
#endif
 
#endif /* _VALVE_H_ */
 
 
/*************************************END OF FILE*******************************/

