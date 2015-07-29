/*******************************************************************************
* Copyright (c) 2015,威鹏自动化科技有限公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-6-4
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：此头文件的作用在于对类型进行了封装,屏蔽了不同编译器对类型的实现差异
********************************************************************************/
 
 
#ifndef _TYPE_H_
#define _TYPE_H_
 
#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
*DATA TYPES
*(Compiler Specific)
********************************************************************************/
typedef unsigned char       BOOLEAN;
typedef unsigned char       INT8U;
typedef signed   char       INT8S;
typedef unsigned short      INT16U;
typedef signed   short      INT16S;
typedef unsigned int        INT32U;
typedef signed   int        INT32S;
typedef float               FP32;
typedef double              FP64;
typedef signed long long    SLLONG;

typedef unsigned char       uint8;
typedef signed   char       sint8;
typedef unsigned short      uint16;
typedef signed   short      sint16;
typedef unsigned int        uint32;
typedef signed   int        sint32;
typedef unsigned long long  uint64;
typedef signed long long    sint64;
typedef float               fp32; 
typedef double              fp64;


#ifdef __cplusplus
}
#endif
 
#endif /* _TYPE_H_ */
 
 
/*************************************END OF FILE*******************************/


