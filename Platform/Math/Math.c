/*******************************************************************************
* Copyright (c) 2014,xxx��˾
 *            All rights reserved.
* �������ƣ�
* �� �� �ţ�1.0
* �� �� �ˣ�л�ֹ�
* ����ʱ�䣺2015-5-15
* �� �� ��:
* �޸�ʱ��:
* �޸�˵��:
* ��    �������һЩͨ�õĺ���ʵ��,����˳�����ʵ�֡�
********************************************************************************/
/*******************************************************************************
*                                    ͷ  ��  ��
********************************************************************************/


/*******************************************************************************
*                               �ļ��ڲ�ʹ�ú궨��
********************************************************************************/
 

/*******************************************************************************
*                                 ȫ�ֺ���(����)����
********************************************************************************/
 

/*******************************************************************************
*                                 ��̬����(����)����
********************************************************************************/


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
unsigned long Multiplication(unsigned int Multiplicand, unsigned int Multiplier)
{
    int i = 0;

    unsigned long Product = 0;
   
    int BitBufIndex = 0;
    unsigned char BitBuf[16] = {0};    //-������unsigned int����,Ϊ16λ����-

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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
unsigned long Division(unsigned long Dividend, unsigned long Divisor)
{
     unsigned char ShifCount = 0;   //-��¼��λ�Ĵ���-
     unsigned long Quotient  = 0;   //-��-

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


