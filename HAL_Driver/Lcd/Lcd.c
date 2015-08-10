/*******************************************************************************
* Copyright (c) 2015,xxx公司
 *            All rights reserved.
* 程序名称：
* 版 本 号：1.0
* 开 发 人：谢林贵
* 开发时间：2015-3-16
* 修 改 者:
* 修改时间:
* 修改说明:
* 其    他：   1、注意区分是8080时序还是6080时序.本文件采用8080时序.
               2、注意RD管脚必须在初始化时清零.否则白屏
********************************************************************************/
/*******************************************************************************
*                                    头  文  件
********************************************************************************/
#include "Valve.h"
#include "Lcd.h"

/*******************************************************************************
*                               文件内部使用宏定义
********************************************************************************/
#define LCD_RES_H()             Lcd_RESSet()
#define LCD_RES_L()             Lcd_RESClr()

#define LCD_CS_H()              Lcd_CSSet()
#define LCD_CS_L()              Lcd_CSClr()

#define LCD_RS_H()              Lcd_DCSet()
#define LCD_RS_L()              Lcd_DCClr()

#define LCD_RD_H()              Lcd_RDSet()
#define LCD_RD_L()              Lcd_RDClr()

#define LCD_WR_H()              Lcd_WRSet()
#define LCD_WR_L()              Lcd_WRClr()

#define LCD_Write(Byte)         Lcd_WriteDataBus(Byte)
#define LCD_Read()              Lcd_ReadDataBus()


#define ROM_CLK_H()              ROM_CLKSet()
#define ROM_CLK_L()              ROM_CLKClr()

#define ROM_CS_H()               ROM_CSSet()
#define ROM_CS_L()               ROM_CSClr()
 
#define ROM_MOSI_H()             ROM_MOSISet()
#define ROM_MOSI_L()             ROM_MOSIClr()

#define ROM_MISO_Get()           ROM_MISOGet()

/*******************************************************************************
*                                 全局函数(变量)声明
********************************************************************************/

//-LCD要显示的1行数据的缓冲,缓冲区不能定义成1024-
unsigned char g_DisplayBuf[Display_Buf_Size];

//-LCD要更新的数据行索引号从上到下0~3(由于LcdDisplayBuf缓冲区大小限制,只能每次更新1行)-  
//unsigned char g_LcdDisplayLineIndex = 0;  



unsigned char SpecicalCharacter[11][4][16] = 
{
  //-0-
  {
    {0x00, 0x00, 0x80, 0xE0, 0xF0, 0xF8, 0xF8, 0x38, 0x38, 0xF8, 0xF8, 0xF0, 0xE0, 0x80, 0x00, 0x00},
    {0x00, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00},
    {0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00},
    {0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F, 0x1F, 0x1C, 0x1C, 0x1F, 0x1F, 0x0F, 0x07, 0x01, 0x00, 0x00}
  },
  //-1-
  {
    {0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0xF0, 0xF8, 0xF8, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00}, 
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00}, 
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00}, 
    {0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1C, 0x18, 0x00, 0x00, 0x00}
  },
  //-2-
  {
    {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xF8, 0x98, 0x38, 0xF8, 0xF8, 0xF8, 0xF0, 0xE0, 0x80, 0x00},
    {0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x07, 0x00},
    {0x00, 0x00, 0x00, 0x80, 0xE0, 0xF0, 0xFC, 0x7F, 0x3F, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x1E, 0x1F, 0x1F, 0x1F, 0x1D, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1F, 0x1F, 0x1F, 0x00}
  },
  //-3-
  {
    {0x00, 0x00, 0xC0, 0xF0, 0xF0, 0xF8, 0xF8, 0x18, 0x38, 0xF8, 0xF8, 0xF0, 0xE0, 0xC0, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0xC1, 0xC0, 0xF0, 0xFF, 0x7F, 0x7F, 0x3F, 0x0F, 0x00, 0x00},
    {0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xF0, 0x00},
    {0x00, 0x00, 0x07, 0x0F, 0x1F, 0x1F, 0x1B, 0x18, 0x1C, 0x1F, 0x1F, 0x0F, 0x07, 0x03, 0x00, 0x00}
  },
  //-4-
  {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF8, 0xF8, 0xF8, 0xF8, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x80, 0xF0, 0xFC, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x70, 0x7E, 0x7F, 0x6F, 0x63, 0x60, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x60, 0x60, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x18, 0x00, 0x00}
  },
  //-5-
  {
    {0x00, 0x00, 0x00, 0xE0, 0xF8, 0xF8, 0xF8, 0x38, 0x38, 0x38, 0x3C, 0x3C, 0x3C, 0x3C, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x6F, 0x60, 0xE0, 0xE0, 0xE0, 0xC0, 0xC0, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x00},
    {0x00, 0x00, 0x07, 0x0F, 0x1F, 0x1F, 0x1D, 0x18, 0x1C, 0x1F, 0x1F, 0x0F, 0x07, 0x03, 0x00, 0x00}

  },
  //-6-
  {
    {0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0x78, 0x78, 0xF8, 0xF8, 0xF8, 0xF0, 0x60, 0x00},
    {0x00, 0x00, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0xE0, 0xE0, 0xE0, 0xC0, 0xC0, 0x00, 0x00},
    {0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00},
    {0x00, 0x00, 0x00, 0x03, 0x07, 0x0F, 0x1F, 0x1F, 0x18, 0x1F, 0x1F, 0x1F, 0x0F, 0x07, 0x01, 0x00}
  },
  //-7-
  {
    {0x00, 0x00, 0xF8, 0xF8, 0xF8, 0x78, 0x18, 0x18, 0x18, 0x18, 0xB8, 0xF8, 0xF8, 0xF8, 0x38, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF0, 0xFC, 0xFF, 0x7F, 0x0F, 0x01, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00}

  },
  //-8-
  {
    {0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF8, 0xF8, 0x38, 0x18, 0x38, 0xF8, 0xF0, 0xF0, 0xC0, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x1F, 0x7F, 0xFF, 0xFF, 0xFC, 0xF0, 0xF0, 0xFF, 0xBF, 0x0F, 0x07, 0x00, 0x00},
    {0x00, 0x00, 0xF0, 0xFC, 0xFE, 0xFF, 0x0F, 0x03, 0x07, 0x0F, 0x3F, 0xFF, 0xFF, 0xFE, 0xF8, 0x00},
    {0x00, 0x00, 0x03, 0x07, 0x0F, 0x1F, 0x1E, 0x1C, 0x18, 0x1C, 0x1E, 0x1F, 0x0F, 0x07, 0x01, 0x00}
  },
  //-9-
  {
    {0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF8, 0x78, 0x18, 0xF8, 0xF8, 0xF0, 0xE0, 0xC0, 0x00, 0x00},
    {0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x00},
    {0x00, 0x00, 0x00, 0x03, 0x03, 0x07, 0x07, 0x07, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00},
    {0x00, 0x00, 0x04, 0x0F, 0x1F, 0x1F, 0x1F, 0x1E, 0x1E, 0x1F, 0x0F, 0x07, 0x03, 0x00, 0x00, 0x00}

  },
  //-%-
  {
    {0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0xFF, 0xC3, 0x80, 0x00, 0xC3, 0xFF, 0xC0, 0xF0, 0xF0, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x01, 0x01, 0xE1, 0xF9, 0x1E, 0x07, 0xF9, 0x87, 0x03, 0x01, 0x87, 0xFE, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x06, 0x07, 0x01, 0x00, 0x00, 0x01, 0x03, 0x03, 0x02, 0x03, 0x01, 0x00, 0x00}
  } 
};
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
void DelayUs(int Count)
{
    int i = 0;
    int j = 0;

    for (i = 0; i < Count; i++)
        for (j = 0; j < 1; j++);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void Delay(int Count)
{
    int i = 0;
    int j = 0;

    for (i = 0; i < Count; i++)
        for (j = 0; j < 110; j++);
}

/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void LcdGPIOInit(void)
{
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void SetDataBusOutput(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = Lcd_D0_Pin | Lcd_D1_Pin | Lcd_D2_Pin | Lcd_D3_Pin | Lcd_D4_Pin | Lcd_D5_Pin | Lcd_D6_Pin | Lcd_D7_Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(Lcd_Data_Port, &GPIO_InitStructure); 
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void SetDataBusInput(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = Lcd_D0_Pin | Lcd_D1_Pin | Lcd_D2_Pin | Lcd_D3_Pin | Lcd_D4_Pin | Lcd_D5_Pin | Lcd_D6_Pin | Lcd_D7_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ; 
    GPIO_Init(Lcd_Data_Port, &GPIO_InitStructure);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void TransferCommand(int Command)
{
    LCD_RS_L();
    LCD_CS_L();
    LCD_WR_L();
    LCD_Write(Command);
    DelayUs(3);
    LCD_WR_H();
    LCD_CS_H();
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void TransferData(int Data)
{
    LCD_RS_H();
    LCD_CS_L();
    LCD_WR_L();
    LCD_Write(Data);
    DelayUs(3);
    LCD_WR_H();
    LCD_CS_H();
}


/*******************************************************************************
* 函数名称:    
* 函数功能:     
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
unsigned char ReadStatus(void)
{
    unsigned char Byte = 0;
    
    SetDataBusInput();
    LCD_RS_L();
    LCD_CS_L();
    LCD_RD_L();
    DelayUs(3);          //-必须加上延时,可能是输出后不能立马读输入-
    Byte = LCD_Read();
    DelayUs(3);
    LCD_RD_H();
    LCD_CS_H();

    SetDataBusOutput();

    return Byte;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void LcdReset(void)
{
    unsigned char Byte = 0;

    Byte = ReadStatus();
    if (((Byte & 0x10) != 0) || ((Byte & 0x20) != 0))
    {
        LCD_RES_L();              /*低电平复位*/
        Delay(200);
        LCD_RES_H();              /*复位完毕*/
        Delay(50);
        TransferCommand(0xe2);   /*软复位*/
        Delay(5);
        TransferCommand(0x2C);   /*升压步聚1*/
        Delay(5);
        TransferCommand(0x2E);   /*升压步聚2*/
        Delay(5);
        TransferCommand(0x2F);   /*升压步聚3*/
        Delay(5);
    }

    TransferCommand(0x23);   /*粗调对比度，可设置范围0x20～0x27*/
    TransferCommand(0x81);   /*微调对比度*/
    TransferCommand(0x30);   /*0x28,微调对比度的值，可设置范围0x00～0x3f*/
    TransferCommand(0xA2);   /*1/9偏压比（bias）*/
    TransferCommand(0xc8);   /*行扫描顺序：从上到下*/
    TransferCommand(0xa0);   /*列扫描顺序：从左到右*/
    TransferCommand(0x40);   /*起始行：第一行开始*/    
    TransferCommand(0xaf);   /*开显示*/

}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void LcdInit(void)
{
    int i = 0;

    LCD_RES_L();              /*低电平复位*/
    Delay(200);
    LCD_RES_H();              /*复位完毕*/
    Delay(50);

    TransferCommand(0xe2);   /*软复位*/
    Delay(5);
    TransferCommand(0x2c);   /*升压步聚1*/
    Delay(5);
    TransferCommand(0x2e);   /*升压步聚2*/
    Delay(5);
    TransferCommand(0x2f);   /*升压步聚3*/
    Delay(5);

    LcdReset();

    for (i = 0; i < Display_Buf_Size; i++)
    {
        g_DisplayBuf[i] = 0x00;
    }
    ClearScreen();
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void LcdSetAddress(unsigned char Page, unsigned char Column)
{
#if 0
    //Column -= 1;                    //我们平常所说的第1列，在LCD驱动IC里是第0列。
    //Page -= 1;

    /*-设置页地址。每页是8行。一个画面的64行被分成8个页。我们平常
     *-所说的第1页，在LCD驱动IC里是第0页，所以在这里减去1-*/
#endif
    TransferCommand(0xB0 + Page);   
    TransferCommand(((Column >> 4) & 0x0f) + 0x10);         //设置列地址的高4位
    TransferCommand(Column & 0x0f);                         //设置列地址的低4位
}
 


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
unsigned int Rom16_16GetAddress(unsigned char Code_H, unsigned char Code_L)
{
    unsigned int Address = 0;

    /*-为了减少代码量,将字库芯片中的地址计算无关部分去除了,其他类似函数的处理与此一致-*/
    //-中文字符,参考GB2312编码表-
    if (Code_H >= 0xB0 && Code_H <= 0xF7 && Code_L >= (unsigned char)0xA1)
    {
        Address = (Code_H - 0xB0) * 94;
        Address += (Code_L - 0xA1) + 846;
        Address <<= 5;

        return Address;
        //-溢出了-
        //return ((Code_H - 0xB0) * 94 + (Code_L - 0xA1) + 846) * 32;
    }

    return 0x00;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
unsigned int Rom8_16GetAddress(unsigned char Code_H, unsigned char Code_L)
{
    unsigned int Code = 0x00;
    unsigned int Address = 0;
    
    Code = Code_H;
    Code <<= 8;
    Code += Code_L;

    //-西文字符,参考GB2312编码表-
    if (Code >= 0xA3A1 && Code <= 0xA3FE)
    {
        Address = (Code - 0xA3A1);
        Address <<= 4;
        Address += 0x3B7D0;
       
        return Address;
        //return (Code - 0xA3A1) * 16 + 0x3B7D0;
    }

    if ((Code >= 0x20) && (Code <= 0x7E))
    {
        Address = (Code - 0x20);
        Address <<= 4;
        Address += 0x3B7C0;
       
        return Address;
    }

    return 0x00;
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
static void RomSendData(unsigned char Data)
{
    unsigned char i;
    for(i = 0; i < 8; i++ )
    {
        if(Data & 0x80)
            ROM_MOSI_H();
        else
            ROM_MOSI_L();
        Data = Data << 1;
        ROM_CLK_L();
        ROM_CLK_H();
    }
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
static unsigned char RomGetData(void)
{
    unsigned char i;
    unsigned char Data = 0;

    ROM_CLK_H();
    for(i = 0; i < 8; i++)
    {
        //-按照<JLX12864G-095-PC带字库IC的编程说明书>中ROM_MISO = 1应该添上,但是在-
        //-实际运行起来发现加上此语句后导致液晶屏为空白.原因未知-
        //ROM_MISO = 1;    
        ROM_CLK_L();
        Data = Data <<1;
        if( ROM_MISO_Get() )
            Data = Data + 1;
        else
            Data = Data + 0;
        ROM_CLK_H();
    }

    return(Data);
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void RomGetBytes(unsigned int Address, unsigned char *pBuff, unsigned char DataLen)
{
    unsigned char i;

    ROM_CS_L();
    LCD_CS_H();
    ROM_CLK_L();

    RomSendData(0x03);
    RomSendData(Address >> 16 & 0xFF);
    RomSendData(Address >> 8 & 0xFF);
    RomSendData(Address >> 0 & 0xFF);

    for(i = 0; i < DataLen; i++ )
    {
        *(pBuff+i) = RomGetData();
    }

    ROM_CS_H();
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void GetDotData(unsigned int Code, unsigned char CharacterType, unsigned char *FontBuf, unsigned char *FontBufLen)
{
    int i = 0;
    unsigned char Code_H = 0;
    unsigned char Code_L = 0;

    unsigned char CharacterMaxSize = 0;
    unsigned int Address = 0;

    Code_H = (Code >> 8) & 0xFF;
    Code_L = Code & 0xFF;

    //-空格特殊处理-
    if (((Code_H == 0xA3) && (Code_L == 0xA0)) || 
        ((Code_H == 0x00) && (Code_L == 0x00)))
    {

        for (i = 0; i < EN_Character_Size; i++)
        {
            FontBuf[i] = 0x00;
        }

        *FontBufLen = EN_Character_Size;

        return;
    }
     
    if(CharacterType == CN_Character)
    {
        Address = Rom16_16GetAddress(Code_H, Code_L);
        CharacterMaxSize = CN_Character_Size;
    }
    else
    {
        Address = Rom8_16GetAddress(Code_H, Code_L);
        CharacterMaxSize = EN_Character_Size;
    }

    RomGetBytes(Address, &FontBuf[0], CharacterMaxSize);

    *FontBufLen = CharacterMaxSize;
}



/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void SwapFontBuf2DisplayBuf(unsigned char *FontBuf, unsigned FontBufLen, unsigned char *DisplayBuf, int DisplayBufStartIndex)
{
    int i = 0;
    int Index = 0;
    int ColumnPerCharacter = 0;

    if (FontBufLen == 32)         //-每个中文字符占16列-
    {
        ColumnPerCharacter = 16;
    }
    else if (FontBufLen == 16)    //-每个英文字符占8列-
    {
        ColumnPerCharacter = 8;
    }
    else
    {
        return;
    }

    if (DisplayBufStartIndex & 0x7 != 0)    //-对8求余-
    {
        return;
    }

    Index = DisplayBufStartIndex;
    for (i = 0; i < ColumnPerCharacter; i++)
    {
        DisplayBuf[Index++] = FontBuf[i];
    }

    Index = DisplayBufStartIndex + 128;
    for (i = ColumnPerCharacter; i < ColumnPerCharacter + ColumnPerCharacter; i++)
    {
        DisplayBuf[Index++] = FontBuf[i];
    }
}


/*******************************************************************************
* 函数名称:    ClearScreen
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ClearScreen(void)
{
    unsigned char i = 0;
    unsigned char j = 0;

    for(i = 0;i < 8; i++)
    {
        LcdSetAddress(i, 0);
        for(j = 0;j < 128; j++)
        {
            TransferData(0x00);
        }
    }
}


/*******************************************************************************
* 函数名称:    LcdRefresh
* 函数功能:    Lcd刷新函数,将LcdDisplayBuf中的数据发送给Lcd
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void LcdRefresh(int CharacterLineIndex)
{
    int i,j;

    //-每次刷新2行(1个字符行)-
    for(j = 0; j < 2; j++)
    {
        LcdSetAddress(CharacterLineIndex * 2 + j, 0);

        for(i = 0; i < 128; i++)
        {
            TransferData(g_DisplayBuf[128 * j + i]);
        }
    }
}

/*************************************END OF FILE*******************************/


