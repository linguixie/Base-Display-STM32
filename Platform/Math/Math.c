/*******************************************************************************
* Copyright (c) 2014,xxx公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-5-15
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：存放一些通用的函数实现,例如乘除的自实现。
********************************************************************************/
/*******************************************************************************
*                                    头  文  件
********************************************************************************/


/*******************************************************************************
*                               文件内部使用宏定义
********************************************************************************/
 

/*******************************************************************************
*                                 全局函数(变量)声明
********************************************************************************/
 

/*******************************************************************************
*                                 静态函数(变量)声明
********************************************************************************/


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
unsigned long Multiplication(unsigned int Multiplicand, unsigned int Multiplier)
{
    int i = 0;

    unsigned long Product = 0;
   
    int BitBufIndex = 0;
    unsigned char BitBuf[16] = {0};    //-乘数是unsigned int类型,为16位整数-

    if (Multiplier == 0)
    {
        return 0;
    }

    while(Multiplier != 0) 
    {
        if (Multiplier & 0x01)
        {
            BitBuf[BitBufIndex] = BitBufIndex + 1;
        }

        Multiplier >>= 1;
        BitBufIndex++;
    }

    for (i = 0; i < 16; i++)
    {
        if (BitBuf[i] == 0)
        {
            continue; 
        }
        Product +=(unsigned long) Multiplicand << (BitBuf[i] - 1);
    }

    return Product;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
unsigned long Division(unsigned long Dividend, unsigned long Divisor)
{
     unsigned char ShifCount = 0;   //-记录移位的次数-
     unsigned long Quotient  = 0;   //-商-

     unsigned long TmpDividend = 0;

     if (Divisor == 0)
     {
         return 0;
     } 

     TmpDividend = Dividend;
Loop:
     ShifCount = 0;
     if (TmpDividend < Divisor)
     {
         return Quotient;
     }
     while(TmpDividend >= Divisor)
     {
         TmpDividend >>= 1; 
         ShifCount++;
     }

     ShifCount--;
     TmpDividend = Dividend >> ShifCount;

     TmpDividend -= Divisor;

     TmpDividend <<= ShifCount;
     Quotient    += (unsigned long)1 << ShifCount;

     TmpDividend += Dividend & (((unsigned long)1 << ShifCount) - 1);
     Dividend = TmpDividend;
     goto Loop;
}
 
/*************************************END OF FILE*******************************/


