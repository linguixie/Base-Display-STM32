/*******************************************************************************
* Copyright (c) 2015,xxx公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-3-28
* 修 改 者: 谢林贵
* 修改时间: 2015-5-11
* 修改说明: 从VPcon500D移植到MDL_IO1410D.
* 其    他：阀门操作的抽象:
*             1、ValveOperation是对阀门的第一层封装.关联实际的物理输入.
*             2、ValveAction是对阀门操作的第二层封装.
*             3、ValveStatus.Opening/Shutting是最后一层封装.
*           阀门输入的抽象:
*             1、DeviceInput是对输入的第一层封装.      
*             2、ValveStatus和ValveError是最后一层封装.
*           设备操作的抽象:
*             1、DeviceOperation是对设备操作的第一层封装.关联实际的物理输入.
*             2、DeviceAction是对设备操作的第二层封装.
*             注意:设备和阀门的区别.设备包含了N个阀门.
*           设备和阀门的区别:
*             1、有些变量属于设备,有些变量属于阀门.区分:设想存在双阀门情况.      
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
    WorkMode_Bus,                  //-总线-
    WorkMode_RemoteAN,             //-远程模拟-
    WorkMode_RemoteJog,            //-远程点动-
    WorkMode_RemoteHold,           //-远程保持-
    WorkMode_RemoteDibit,          //-远程双位-
    WorkMode_LocalJog,             //-现场点动-
    WorkMode_LocalHold,            //-现场保持-
    WorkMode_Stop,                 //-停止- 
    WorkMode_None
}WorkMode;

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
    RemoteIOMode_Hold,                 //-远程保持-
    RemoteIOMode_HoldNormallyOpen,     //-保持停常开-
    RemoteIOMode_HoldNormallyShut,     //-保持停常关-
    RemoteIOMode_SignalOnNoOff,        //-有信开无信关-
    RemoteIOMode_SignalOffNoOn,        //-有信关无信开-
    RemoteIOMode_BiPos
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
    ESDDisplay_Enable,
    ESDDisplay_Disable
}ESDDisplay_TypdeDef;

//-ESD模式-
typedef enum
{
    ESDMode_Disable,
    ESDMode_NoAction,
    ESDMode_Open,
    ESDMode_Shut,
    ESDMode_Middle
}ESDMode_TypeDef;

//-ESD状态-
typedef enum
{
    ESDStatus_Valid,
    ESDStatus_Invalid
}ESDStatus_TypeDef;

//-常开或常闭-
typedef enum
{
    IO_NormallyOpen,
    IO_NormallyShut
}IO_TypeDef;

//-编码器方向-
typedef enum
{
    EncoderDirection_Reverse,
    EncoderDirection_Forward
}EncoderDirection_TypeDef;

//-电流显示开关-
typedef enum
{
    CurrentDisplay_Enable,
    CurrentDisplay_Disable
}CurrentDisplay_TypeDef;

//-电流小数点位数-
typedef enum
{
    CurrentDecimalBits_One,
    CurrentDecimalBits_Two
}CurrentDecimalBits_TypeDef;
/*******************************************************************************
*                                  全局变量声明
********************************************************************************/
/*==================定义单个阀门相关的结构体===================================*/
//-阀门计数器-
typedef enum{Counter_PVHigherInStop, Counter_PVHigherInOpen, Counter_PVHigherInShut, 
             Counter_SVHigherInStop, Counter_SVHigherInOpen, Counter_SVHigherInShut,
             Counter_CheckRealStop,  Counter_CheckRealRun, 
             Counter_Num
            }Counter_TypeDef;



//-阀门操作结构体:保存阀门的相关操作及操作方式.-
typedef struct
{
    unsigned char Operation;            //-阀门将要执行的命令:开、关-
    unsigned int  DstOpening;           //-目标(Destinantion)开度-
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
            unsigned NoLimitShut:1;   //-无限位关-
            unsigned NoLimitOpen:1;   //-无限位开-
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
        unsigned Reserved2:1;         //-Reserved-
        unsigned Reserved3:1;         //-Reserved-
        unsigned Reserved4:1;         //-Reserved-
        unsigned Reserved5:1;         //-Reserved-
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
        unsigned BusValid:1;            //-总线正常-
        unsigned Shutting:1;            //-正在关-
        unsigned Opening:1;             //-正在开-
        unsigned ShutLimit:1;           //-关限位-
        unsigned OpenLimit:1;           //-开限位-
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
    signed   long  BigLoadTimer;
    signed   long  JustStopTimer;
    signed   long  MotorTimer;

    unsigned int   PositionValue;         //-阀门电位器或者编码器的采样值-
    unsigned int   In4_20mAADValue;       //-输入4-20mA的采样值-
    unsigned int   Current;               //-电流值-
    unsigned int   PwmDuty_H;             //-输出4-20mA的占空比高字节-
    unsigned int   PwmDuty_L;             //-输出4-20mA的占空比低字节-
    unsigned int   CurOpening;            //-当前(Current)开度-
    unsigned int   StopOutputOpening;     //-阀门停止输出物理信号时的开度(计算惯冲量用到)-
    unsigned int   StopTmpOpening;        //-阀门停止时的中间开度(判断是否真正停止用到)-
    unsigned int   RunTmpOpening;         //-阀门运行时的中间开度(判断是否真正运行用到)-
    unsigned int   DeadZone;              //-死区(注意和Para中的不一定一致)-
    unsigned int   OpenInertia;           //-开惯性冲量(注意和Para中的不一定一致)-
    unsigned int   ShutInertia;           //-关惯性冲量(注意和Para中的不一定一致)-
    
}MiscInfo_T;


//-阀门标志-
typedef union
{
    struct
    {
        unsigned F_JustRun:1;           //-才开阀或才关阀-
        unsigned F_JustStop:1;          //-才停止(和JustRun不同:3s后强制清除)-
        unsigned F_JustOpen:1;          //-先前有开阀动作(学习惯冲量时用到)-
        unsigned F_BigLoad:1;           //-大负载-
        unsigned F_StartInInertia:1;    //-是否在死区外惯性内启动的标志:=1,是; 其他,不是-
        unsigned Reserved5:1;           //-预留-
        unsigned Reserved6:1;           //-预留-
        unsigned Reserved7:1;           //-预留-
    }FlagBits;
    unsigned char FlagByte;
}Flag_T;


//-阀门计数器-
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


//-整个设备的通讯模式:保存整个设备的通讯模式(此结构体跟WorkMode相重复,但是为了书写方便定义)-
typedef union 
{
    struct
    {
        unsigned Local:1;               //-现场状态-
        unsigned Remote:1;              //-远程状态-
        unsigned Bus:1;                 //-总线-
        unsigned Manual:1;              //-手轮手动-
        unsigned Reserved4:1;           //-预留-
        unsigned Reserved5:1;           //-预留-
        unsigned Reserved6:1;           //-预留-
        unsigned Reserved7:1;           //-预留-
    }CommModeBits;
    unsigned char CommModeByte;         //-设备通讯模式-   
}DeviceCommMode_T;


//-整个设备的工作模式:保存整个设备的工作模式-
typedef struct 
{
    unsigned char CurWorkMode;         //-设备工作模式-   
}DeviceWorkMode_T;


//-整个设备的状态结构体:保存整个设备的运行状态-
typedef struct 
{
    unsigned char Status;               //-设备当前状态-
    unsigned char ESDStatus;            //-ESD有or没有-
}DeviceStatus_T;


//-整个设备的错误-
typedef union
{
    struct 
    {
        unsigned ValveException:1;      //-阀位异常-
        unsigned EncoderNoChange:1;     //-当阀门动作时,编码器不变化-
        unsigned Reserved2:1;           //-Reserved-
        unsigned Reserved3:1;           //-Reserved-
        unsigned Reserved4:1;           //-Reserved-
        unsigned Reserved5:1;           //-Reserved-
        unsigned Reserved6:1;           //-Reserved-
        unsigned LackPhase:1;           //-Reserved-
    }ErrorBits;
    unsigned char ErrorByte;                                                                                   
}DeviceError_T;

//-整个设备的标志-
typedef union
{
    struct
    {
        unsigned IsInMenu:1;            //-是否进入菜单设置-
        unsigned IsInLocalAdjust:1;     //-是否旋至现场模式进行数字调整-
        unsigned Reserved2:1;           //-预留-
        unsigned Reserved3:1;           //-预留-
        unsigned Reserved4:1;           //-预留-
        unsigned Reserved5:1;           //-预留-
        unsigned Reserved6:1;           //-预留-
        unsigned Reserved7:1;           //-预留-
    }FlagBits;
    unsigned char FlagByte;
}DeviceFlag_T;


/*-----------------------------以下部分根据实际应用修改------------------------*/
//-动态信息结构体:保存阀门的实时动态信息-
typedef struct 
{
    unsigned char Reserved;            //-预留-
    unsigned char NeedReset;           //--
}DeviceMiscInfo_T;


//-阀门的输入结构体:保存阀门的输入-
typedef struct Input
{
    //-现场/远程-
    unsigned char Remote;
    unsigned char Local;

    //-现场开/关/停-
    unsigned char LocalOpen;
    unsigned char LocalShut;
    unsigned char LocalStop;
}DeviceInput_T;


//-参数结构体:保存阀门的相关参数,参数分为2部分(需要和面板进行交互的、不需要进行交互的)-
typedef struct 
{
    //-以下参数和面板进行交互-
    unsigned char LocalMode;            //-现场点动还是保持-
    unsigned char RemoteIOMode;         //-远程开关模式:有信开无信关、有信关无限开-
    unsigned char RemoteANMode;         //-远程模拟模式:丢信保位、丢信全开、丢信全关、丢信居中-
    unsigned char DeadZone;             //-阀门的死区(1~20,每个点代表0.5%)-
    unsigned char MaxActionTime;        //-允许的最大动作时间-
    unsigned char MaxOpenCurrent;       //-允许的最大开向电流-
    unsigned char MaxShutCurrent;       //-允许的最大关向电流-
    unsigned char ESDMode;              //-ESD模式-
    unsigned char ErrorFeedBack;        //-故障反馈触点-
 

    //-以下参数不合面板进行交互-
    unsigned char RemoteType;           //-阀门类型:调节型还是开关型-
    unsigned char ESDDisplayEnable;     //-ESD(紧急状态)菜单中显示与否-
    unsigned char CurrentDisplayEnable; //-动作时,是否显示电流-
    unsigned char CurrentDecimalBits;   //-显示电流的位数-
    unsigned int  FullValue;            //-满点时阀门的电位器或编码器的值-
    unsigned int  ZeroValue;            //-零点时阀门的电位器或编码器的值-
    unsigned int  Input4mAADValue;      //-输入4mA的AD采样值-
    unsigned int  Input20mAADValue;     //-输入20mA的AD采样值-       
    unsigned int  Output4mAPwmDuty;     //-输出4mA的PWM占空比-
    unsigned int  Output20mAPwmDuty;    //-输出20mA的PWM占空比-
    unsigned int  OpenInertia;          //-开惯性冲量-
    unsigned int  ShutInertia;          //-关惯性冲量-
    unsigned char EncoderDirection;     //-编码器方向-
    unsigned char LanguageType;         //-语言类型-

}DevicePara_T;


//-注:本来想把Valve_T也定义在Device_T中,但是考虑到引用时不太方便-
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
*                                  全局函数声明
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

