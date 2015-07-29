/*******************************************************************************
* Copyright (c) 2105,�����Զ����Ƽ����޹�˾
 *            All rights reserved.
* �������ƣ�
* �� �� �ţ�1.0
* �� �� �ˣ�л�ֹ�
* ����ʱ�䣺2015-3-28
* �� �� ��: л�ֹ�
* �޸�ʱ��: 2015-5-11
* �޸�˵��: ��VPcon500D��ֲ��MDL_IO1410D.
* ��    �������Ų����ĳ���:
*           1��ValveOperation�ǶԷ��ŵĵ�һ���װ.����ʵ�ʵ���������.
*           2��ValveAction�ǶԷ��Ų����ĵڶ����װ.
*           3��ValveStatus.Opening/Shutting�����һ���װ.
*           ��������ĳ���:
*           1��DeviceInput�Ƕ�����ĵ�һ���װ.      
*           2��ValveStatus��ValveError�����һ���װ.
*           �豸�����ĳ���
*           1��DeviceOperation�Ƕ��豸�����ĵ�һ���װ.����ʵ�ʵ���������.
*           2��DeviceAction�Ƕ��豸�����ĵڶ����װ.
*           ע��:�豸�ͷ��ŵ�����.�豸������N������.      
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
    Operation_None,
}Operation;

//---
typedef enum
{
    Action_Open, 
    Action_Shut,
    Action_Reset,
    Action_Stop,
    Action_Run,
    Action_None
}Action;

//---
typedef enum
{  
    ControlMode_Bus,                  //-����-
    ControlMode_RemoteAN,             //-Զ��ģ��-
    ControlMode_RemoteJog,            //-Զ�̵㶯-
    ControlMode_RemoteHold,           //-Զ�̱���-
    ControlMode_RemoteDibit,          //-Զ��˫λ-
    ControlMode_LocalJog,             //-�ֳ��㶯-
    ControlMode_LocalHold,            //-�ֳ�����-
    ControlMode_Stop,                 //-ֹͣ- 
    ControlMode_None,
    ControlMode_Auto,                 //-�Զ�-
    ControlMode_Manual,               //-�ֶ�-
    ControlMode_AutoContinuous,       //-�Զ�����-
    ControlMode_AutoTimingRun,        //-�Զ���ʱ����-
    ControlMode_AutoTimingStop,       //-�Զ���ʱ�ȴ�-
    ControlMode_Local,                //-�ֳ�-
    ControlMode_Remote,               //-Զ��-
    ControlMode_LocalManual,          //-�ֳ��ֶ�-
    ControlMode_LocalTiming           //-�ֳ���ʱ-
}ControlMode;

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
    RemoteIOMode_HoldNormallyOpen,     //-����ͣ����-
    RemoteIOMode_HoldNormallyShut,     //-����ͣ����-
    RemoteIOMode_SignalOnNoOff,        //-���ſ����Ź�-
    RemoteIOMode_SignalOffNoOn,         //-���Ź����ſ�-
    RemoteIOMode_Hold,                 //-Զ�̱���-
    RemoteIOMode_BiPos                //-Զ��˫λ-
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
    ESD_Valid,
    ESD_Invalid,
    ESD_Disable,
    ESD_NoAction,
    ESD_Open,
    ESD_Shut,
    ESD_Middle
}ESD_TypeDef;

//--
typedef enum
{
    IO_NormallyOpen,
    IO_NormallyShut
}IO_TypeDef;

/*******************************************************************************
*                                  ȫ�ֱ�������
********************************************************************************/
/*==================���嵥��������صĽṹ��===================================*/
//-���Ų����ṹ��:���淧�ŵ���ز�����������ʽ.-
typedef struct
{
    unsigned char Operation;            //-���Ž�Ҫִ�е�����:������-
    unsigned int DstOpening;            //-Ŀ��(Destinantion)����-
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
            unsigned Reserved4:1;     //-Reserved-
            unsigned Reserved5:1;     //-Reserved-
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
        unsigned Reserved0:1;         //-Reserved-
        unsigned Reserved1:1;         //-Reserved-
        unsigned Reserved2:1;         //-Reserved-
        unsigned Reserved3:1;         //-Reserved-
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
        unsigned BusOk:1;               //-��������-
        unsigned Shutting:1;            //-���ڹ�-
        unsigned Opening:1;             //-���ڿ�-
        unsigned ShutLimit:1;           //-�ص�λ-
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
    signed   long  OpenedTimer;
    signed   long  ShuttedTimer;
    unsigned int   CurOpening;            //-��ǰ(Current)����- 
    unsigned int PositionADValue;         //-���ŵ�λ�����߱������Ĳ���ֵ- 
    unsigned int In4_20mA;                //-����4-20mAֵ(0~255����0~25.5mA)-         
}MiscInfo_T;


typedef struct
{
    ValveOperation_T      Operation;
    Adjust_T              Adjust;
    AdjustInfo_T          AdjustInfo;
    ValveAction_T         Action;
    ValveStatus_T         Status;
    ValveError_T          Error;
    MiscInfo_T            MiscInfo;
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


//-�����豸�Ĺ���ģʽ:���������豸�Ĺ���ģʽ-
typedef union 
{
    struct
    {
        unsigned Remote:1;              //-Զ��-
        unsigned Local:1;               //-�ֳ�-
        unsigned Bus:1;                 //-����-
        unsigned Analog:1;              //-ģ��-
        unsigned Reserved4:1;           //-Ԥ��-
        unsigned Reserved5:1;           //-Ԥ��-
        unsigned Reserved6:1;           //-Ԥ��-
        unsigned Reserved7:1;           //-Ԥ��-
    }ControlModeBits;
    unsigned char ControlMode;          //-�豸��ǰ����ģʽ-   
}DeviceWorkMode_T;

//-�����豸��״̬�ṹ��:���������豸������״̬-
typedef struct 
{
    unsigned char CurMode;              //-��ǰ����ģʽ-
    unsigned char Status;               //-�豸��ǰ״̬- 
    unsigned char ESDStatus;            //-ESD״̬-
}DeviceStatus_T;


//-�����豸�Ĵ���-
typedef union
{
    struct 
    {
        unsigned ValvePosError:1;       //-��λ�쳣-
        unsigned Reserved1:1;           //-Reserved-
        unsigned Reserved2:1;           //-Reserved-
        unsigned Reserved3:1;           //-Reserved-
        unsigned Reserved4:1;           //-Reserved-
        unsigned Reserved5:1;           //-Reserved-
        unsigned Reserved6:1;           //-Reserved-
        unsigned PhaseLack:1;           //-ȱ��-
    }ErrorBits;
    unsigned char ErrorByte;                                                                                   
}DeviceError_T;


/*-----------------------------���²��ָ���ʵ��Ӧ���޸�------------------------*/
//-��̬��Ϣ�ṹ��:���淧�ŵ�ʵʱ��̬��Ϣ-
typedef struct 
{
    unsigned short RunRemainder;
    unsigned short RestRemainder; 
    signed   long  WorkTimer;
    signed   long  RestTimer; 
    unsigned char  NeedReset;
    unsigned int   ErrorIndex;            //-���������-

#if Debug
    unsigned char DebugVar0;
#endif
}DeviceMiscInfo_T;


//-���ŵ�����ṹ��:���淧�ŵ�����-
typedef struct Input
{
    unsigned char Open;        //-����-
    unsigned char Shut;        //-�ط�-
    unsigned char Stop;        //-ֹͣ-
}DeviceInput_T;


//-�����ṹ��:���淧�ŵ���ز���,������Ϊ2����(��Ҫ�������н����ġ�����Ҫ���н�����)-
typedef struct 
{
    //-���²����������н���-
    unsigned char LocalMode;            //-�ֳ��㶯���Ǳ���-
    unsigned char RemoteIOMode;         //-Զ�̿���ģʽ:���ſ����Źء����Ź����޿�-
    unsigned char RemoteANMode;         //-Զ��ģ��ģʽ:���ű�λ������ȫ��������ȫ�ء����ž���-
    unsigned char DeadZone;             //-���ŵ�����(1~20,ÿ�������0.5%)-
    unsigned char MaxActionTime;        //-����������ʱ��(0 ������ʱ)-
    unsigned char MaxOpenCurrent;       //-�������������-
    unsigned char MaxShutCurrent;       //-��������������-
    unsigned char ESDMode;              //-ESDģʽ-
    unsigned char ErrorFeedBack;        //-���Ϸ�������-

    //-���²������������н���-
    unsigned char RemoteHold;           //-Զ�̱���-
    unsigned char RemoteType;           //-��������:�����ͻ��ǿ�����-
    unsigned int  FullADValue;          //-����ʱ���ŵĵ�λ�����������ֵ-
    unsigned int  ZeroADValue;          //-���ʱ���ŵĵ�λ�����������ֵ-
    unsigned int  Input4mAADValue;      //-����4mA��AD����ֵ-
    unsigned int  Input20mAADValue;     //-����20mA��AD����ֵ-       
    unsigned int  Output4mAPwmDuty;     //-���4mA��PWMռ�ձ�-
    unsigned int  Output20mAPwmDuty;    //-���20mA��PWMռ�ձ�-
    unsigned int  OpenInertia;          //-�����Գ���-
    unsigned int  ShutInertia;          //-�ع��Գ���-
    unsigned char LockEnable;           //-��������-
    unsigned int  LockTime;             //-����ʱ��-
    unsigned char LanguageType;         //-��������-
    unsigned char IsInMenu;             //-�Ƿ��������(�˵�)-

}DevicePara_T;


//-ע:�������Valve_TҲ������Device_T��,���ǿ��ǵ�����ʱ��̫����-
typedef struct
{
    DeviceOperation_T        Operation;
    DeviceAction_T           Action;
    DeviceWorkMode_T         Mode;
    DeviceStatus_T           Status;
    DeviceError_T            Error;
    DeviceMiscInfo_T         MiscInfo;       
    DevicePara_T             Para;              
    DeviceInput_T            Input; 
}Device_T;



extern Device_T Device;
extern Valve_T  Valve;
extern Valve_T *ValveArray[ValveNum];
/*******************************************************************************
*                                  ȫ�ֺ�������
********************************************************************************/
void ValveInit(void);

void DeviceSched(Device_T *pDevice);
void DeviceProcess(Device_T *pDevice);
void ValveProcess(Device_T *pDevice, Valve_T *pValve);

void EnableDeviceReset(Device_T *pDevice);
void EnableDeviceRun(Device_T *pDevice);
void EnableDeviceStop(Device_T *pDevice);

void EnableValveOpen(Valve_T *pValve);
void EnableValveShut(Valve_T *pValve);
void EnableValveStop(Valve_T *pValve);

#ifdef __cplusplus
}
#endif
 
#endif /* _VALVE_H_ */
 
 
/*************************************END OF FILE*******************************/

