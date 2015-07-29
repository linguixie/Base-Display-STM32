/*******************************************************************************
* Copyright (c) 2105,威鹏自动化科技有限公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-3-28
* 修 改 者: 谢林贵
* 修改时间: 2015-5-11
* 修改说明: 从VPcon500D移植到MDL_IO1410D.
* 其    他：阀门操作的抽象:
*           1、ValveOperation是对阀门的第一层封装.关联实际的物理输入.
*           2、ValveAction是对阀门操作的第二层封装.
*           3、ValveStatus.Opening/Shutting是最后一层封装.
*           阀门输入的抽象:
*           1、DeviceInput是对输入的第一层封装.      
*           2、ValveStatus和ValveError是最后一层封装.
*           设备操作的抽象
*           1、DeviceOperation是对设备操作的第一层封装.关联实际的物理输入.
*           2、DeviceAction是对设备操作的第二层封装.
*           注意:设备和阀门的区别.设备包含了N个阀门.      
********************************************************************************/
 
 
#ifndef _VALVE_H_
#define _VALVE_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*******************************************************************************
*                                  头  文  件
********************************************************************************/

 
/*******************************************************************************
*                                  宏  定  义
********************************************************************************/
//-!!!!计算开度时,乘法由移位实现,移位的因子如下:400 = 2^8 + 2^7 + 2^4-
#define Opening_LeftShift_Factor0    8
#define Opening_LeftShift_Factor1    7
#define Opening_LeftShift_Factor2    4

#define MaxOpening               400    //-阀门全开的开度-
#define MidOpening               200    //-阀门半开半闭的开度-
#define MinOpening               0      //-阀门全闭的开度(一般情况为0)- 
#define Opening2o5Percent        10     //-阀门2.5%的开度-
#define Opening15Percent         60     //-阀门15%的开度-
#define OpeningOffset            2      //-400个点时判断真正停止的滑差-
#define OpeningDeviation         3      //-400个点时允许的误差-
#define OpeningCompensation      3      //-阀门的开度补偿常数,在阀门进行四舍五入的时候用到-

#define Mask_Status_Opening    0x08
#define Mask_Status_Shutting   0x04
#define Mask_Status_OpenLimit  0x10
#define Mask_Status_ShutLimit  0x20

#define ValveNum               1
/*----------------------枚举常数宏--------------------------------------*/
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
    ControlMode_Bus,                  //-总线-
    ControlMode_RemoteAN,             //-远程模拟-
    ControlMode_RemoteJog,            //-远程点动-
    ControlMode_RemoteHold,           //-远程保持-
    ControlMode_RemoteDibit,          //-远程双位-
    ControlMode_LocalJog,             //-现场点动-
    ControlMode_LocalHold,            //-现场保持-
    ControlMode_Stop,                 //-停止- 
    ControlMode_None,
    ControlMode_Auto,                 //-自动-
    ControlMode_Manual,               //-手动-
    ControlMode_AutoContinuous,       //-自动连续-
    ControlMode_AutoTimingRun,        //-自动定时工作-
    ControlMode_AutoTimingStop,       //-自动定时等待-
    ControlMode_Local,                //-现场-
    ControlMode_Remote,               //-远程-
    ControlMode_LocalManual,          //-现场手动-
    ControlMode_LocalTiming           //-现场定时-
}ControlMode;

//---
typedef enum
{
    RemoteType_Regulate,              //-调节型-
    RemoteType_Switch                 //-开关型-
}RemoteType;

//---
typedef enum
{
    LocalMode_Jog,                     //-点动-
    LocalMode_Hold                     //-保持-
}LocalMode;

//---
typedef enum
{
    RemoteIOMode_Jog,                  //-点动-
    RemoteIOMode_HoldNormallyOpen,     //-保持停常开-
    RemoteIOMode_HoldNormallyShut,     //-保持停常关-
    RemoteIOMode_SignalOnNoOff,        //-有信开无信关-
    RemoteIOMode_SignalOffNoOn,         //-有信关无信开-
    RemoteIOMode_Hold,                 //-远程保持-
    RemoteIOMode_BiPos                //-远程双位-
}RemoteIOMode;

//---
typedef enum
{
    RemoteANMode_Invalid,              //-屏蔽-
    RemoteANMode_NoSigKeep,            //-丢信保位-
    RemoteANMode_NoSigShut,            //-丢信全关-
    RemoteANMode_NoSigMid,             //-丢信居中-
    RemoteANMode_NoSigOpen             //-丢信全开-
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
*                                  全局变量声明
********************************************************************************/
/*==================定义单个阀门相关的结构体===================================*/
//-阀门操作结构体:保存阀门的相关操作及操作方式.-
typedef struct
{
    unsigned char Operation;            //-阀门将要执行的命令:开、关-
    unsigned int DstOpening;            //-目标(Destinantion)开度-
}ValveOperation_T;


//-阀门动作结构体:保存阀门即将要执行的动作(和操作结构体的区别:操作代表输入的操作命令).-
typedef struct
{
    unsigned char PreAction;            //-阀门先去执行的动作:开、关-
    unsigned char Action;               //-阀门将要执行的动作:开、关-
}ValveAction_T;


//-阀门的标定相关请求结构体:保存阀门的标定请求-
typedef struct Adjust
{
    union
    {
        struct                      
        {
            unsigned Zero:1;          //-调零请求-
            unsigned Full:1;          //-调满请求-
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
            unsigned Input4mA:1;      //-标定输入4mA-
            unsigned Input20mA:1;     //-标定输入20mA-
            unsigned Reserved0:1;     //-Reserved-
            unsigned Reserved1:1;     //-Reserved-
            unsigned DecOutput20mA:1; //-减小输出20mA-
            unsigned IncOutput20mA:1; //-增大输出20mA-
            unsigned DecOutput4mA:1;  //-减小输出4mA-
            unsigned IncOutput4mA:1;  //-增大输出4mA-
        }Adjust1Bits;
        unsigned char Adjust1Byte;  
    }Adjust1;
}Adjust_T;


//-阀门的标定成功联合体:保存阀门的标定成功标志-
typedef union
{
    struct                      
    {
        unsigned Zero:1;              //-调零-
        unsigned Full:1;              //-调满-
        unsigned Reserved0:1;         //-Reserved-
        unsigned Reserved1:1;         //-Reserved-
        unsigned Reserved2:1;         //-Reserved-
        unsigned Reserved3:1;         //-Reserved-
        unsigned Input4mA:1;          //-标定输入4mA-
        unsigned Input20mA:1;         //-标定输入20mA-
    }AdjustInfoBits;
    unsigned char AdjustInfoByte;  
}AdjustInfo_T;


//-阀门状态联合体:保存阀门的状态-
typedef union 
{
    struct                      
    {
        unsigned NoSignal:1;            //-丢信-
        unsigned BusOk:1;               //-总线正常-
        unsigned Shutting:1;            //-正在关-
        unsigned Opening:1;             //-正在开-
        unsigned ShutLimit:1;           //-关到位-
        unsigned OpenLimit:1;           //-开到位-
        unsigned Reserved6:1;           //-预留-
        unsigned Reserved7:1;           //-预留-
    }StatusBits;
    unsigned char StatusByte;        
}ValveStatus_T;


//-阀门错误联合体-
typedef union
{
    struct
    {
            unsigned ShutTorques:1;      //-关力矩故障-
            unsigned OpenTorques:1;      //-开力矩故障-
            unsigned ShutTimeout:1;      //-关向超时-
            unsigned OpenTimeout:1;      //-开向超时-
            unsigned ShutOverCurrent:1;  //-关向过流-
            unsigned OpenOverCurrent:1;  //-开向过流-
            unsigned ShutDirection:1;    //-关向错误-
            unsigned OpenDirection:1;    //-开向错误-
    }ErrorBits;
    unsigned char ErrorByte;
}ValveError_T;


//-动态信息结构体:保存阀门的实时动态信息-
typedef struct
{           
    signed   long  ActionTimer; 
    signed   long  OpenedTimer;
    signed   long  ShuttedTimer;
    unsigned int   CurOpening;            //-当前(Current)开度- 
    unsigned int PositionADValue;         //-阀门电位器或者编码器的采样值- 
    unsigned int In4_20mA;                //-输入4-20mA值(0~255代表0~25.5mA)-         
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


/*==================定义整个设备相关的结构体===================================*/
//-操作结构体:保存设备的相关操作及操作方式-
typedef struct
{
    unsigned char PreOperation;
    unsigned char Operation;            //-操作命令-
}DeviceOperation_T;


//-动作结构体:保存设备的先关动作-
typedef struct
{
    unsigned char PreAction;            //-启动还是停止(和操作结构体的区别:操作代表输入的操作命令)-
    unsigned char Action;
}DeviceAction_T;


//-整个设备的工作模式:保存整个设备的工作模式-
typedef union 
{
    struct
    {
        unsigned Remote:1;              //-远程-
        unsigned Local:1;               //-现场-
        unsigned Bus:1;                 //-总线-
        unsigned Analog:1;              //-模拟-
        unsigned Reserved4:1;           //-预留-
        unsigned Reserved5:1;           //-预留-
        unsigned Reserved6:1;           //-预留-
        unsigned Reserved7:1;           //-预留-
    }ControlModeBits;
    unsigned char ControlMode;          //-设备当前工作模式-   
}DeviceWorkMode_T;

//-整个设备的状态结构体:保存整个设备的运行状态-
typedef struct 
{
    unsigned char CurMode;              //-当前工作模式-
    unsigned char Status;               //-设备当前状态- 
    unsigned char ESDStatus;            //-ESD状态-
}DeviceStatus_T;


//-整个设备的错误-
typedef union
{
    struct 
    {
        unsigned ValvePosError:1;       //-阀位异常-
        unsigned Reserved1:1;           //-Reserved-
        unsigned Reserved2:1;           //-Reserved-
        unsigned Reserved3:1;           //-Reserved-
        unsigned Reserved4:1;           //-Reserved-
        unsigned Reserved5:1;           //-Reserved-
        unsigned Reserved6:1;           //-Reserved-
        unsigned PhaseLack:1;           //-缺相-
    }ErrorBits;
    unsigned char ErrorByte;                                                                                   
}DeviceError_T;


/*-----------------------------以下部分根据实际应用修改------------------------*/
//-动态信息结构体:保存阀门的实时动态信息-
typedef struct 
{
    unsigned short RunRemainder;
    unsigned short RestRemainder; 
    signed   long  WorkTimer;
    signed   long  RestTimer; 
    unsigned char  NeedReset;
    unsigned int   ErrorIndex;            //-错误计数器-

#if Debug
    unsigned char DebugVar0;
#endif
}DeviceMiscInfo_T;


//-阀门的输入结构体:保存阀门的输入-
typedef struct Input
{
    unsigned char Open;        //-开阀-
    unsigned char Shut;        //-关阀-
    unsigned char Stop;        //-停止-
}DeviceInput_T;


//-参数结构体:保存阀门的相关参数,参数分为2部分(需要和面板进行交互的、不需要进行交互的)-
typedef struct 
{
    //-以下参数和面板进行交互-
    unsigned char LocalMode;            //-现场点动还是保持-
    unsigned char RemoteIOMode;         //-远程开关模式:有信开无信关、有信关无限开-
    unsigned char RemoteANMode;         //-远程模拟模式:丢信保位、丢信全开、丢信全关、丢信居中-
    unsigned char DeadZone;             //-阀门的死区(1~20,每个点代表0.5%)-
    unsigned char MaxActionTime;        //-允许的最大动作时间(0 代表不限时)-
    unsigned char MaxOpenCurrent;       //-允许的最大开向电流-
    unsigned char MaxShutCurrent;       //-允许的最大关向电流-
    unsigned char ESDMode;              //-ESD模式-
    unsigned char ErrorFeedBack;        //-故障反馈触点-

    //-以下参数不合面板进行交互-
    unsigned char RemoteHold;           //-远程保持-
    unsigned char RemoteType;           //-阀门类型:调节型还是开关型-
    unsigned int  FullADValue;          //-满点时阀门的电位器或编码器的值-
    unsigned int  ZeroADValue;          //-零点时阀门的电位器或编码器的值-
    unsigned int  Input4mAADValue;      //-输入4mA的AD采样值-
    unsigned int  Input20mAADValue;     //-输入20mA的AD采样值-       
    unsigned int  Output4mAPwmDuty;     //-输出4mA的PWM占空比-
    unsigned int  Output20mAPwmDuty;    //-输出20mA的PWM占空比-
    unsigned int  OpenInertia;          //-开惯性冲量-
    unsigned int  ShutInertia;          //-关惯性冲量-
    unsigned char LockEnable;           //-锁定允许-
    unsigned int  LockTime;             //-锁定时间-
    unsigned char LanguageType;         //-语言类型-
    unsigned char IsInMenu;             //-是否进入设置(菜单)-

}DevicePara_T;


//-注:本来想把Valve_T也定义在Device_T中,但是考虑到引用时不太方便-
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
*                                  全局函数声明
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

