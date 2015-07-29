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
* 其    他：
********************************************************************************/
/*******************************************************************************
*                                    头  文  件
********************************************************************************/
#include "stm32f0xx_conf.h"
#include "GPIO.h"
#include "OLed.h"

/*******************************************************************************
*                               文件内部使用宏定义
********************************************************************************/
#define OLED_CMD                 0
#define OLED_DATA                1

#define OLED_RES_H()             OLed_RESSet()
#define OLED_RES_L()             OLed_RESClr()

#define OLED_CS_H()              OLed_CSSet()
#define OLED_CS_L()              OLed_CSClr()

#define OLED_DC_H()              OLed_DCSet()
#define OLED_DC_L()              OLed_DCClr()

#define OLED_RD_H()              OLed_RDSet()
#define OLED_RD_L()              OLed_RDClr()

#define OLED_WR_H()              OLed_WRSet()
#define OLED_WR_L()              OLed_WRClr()

#define OLED_Write(Byte)         OLed_WriteDataBus(Byte)


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

/*******************************************************************************
*                                 静态函数(变量)声明
********************************************************************************/

/*-特殊的0~9和%,和普通的西文字符区别:横向和纵向均扩大1倍-*/
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

unsigned char TriangleCode[16] = 
{
    0x00, 0x00, 0x80, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x02, 0x00
};


/*******************************************************************************
* 函数名称:    Delay
* 函数功能:    延时
* 输入参数:    延时数值
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void DelayMs(unsigned int Ms)
{                         
    unsigned int i;

    while(Ms)
    {
        i = 6000;
        while(i--);
        Ms--;
    }
}


/*******************************************************************************
* 函数名称:    OLED_WR_Byte
* 函数功能:    延时
* 输入参数:    Byte,要写入的字节;
               Command,0表示命令,1表示数据.
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void OLED_WR_Byte(unsigned char Byte, unsigned char Command)
{   
    OLED_Write(Byte);
              
    if(Command)
      OLED_DC_H();
    else 
      OLED_DC_L();        
    OLED_CS_L();
    OLED_WR_L();
    OLED_WR_H();           
    OLED_CS_H();
    OLED_DC_H();        
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
    
    if (Code_H >= 0xA1 && Code_H <= 0xA3 && Code_L >= (unsigned char)0xA1)
    {
        Address = (Code_H - 0xA1) * 94;
        Address += (Code_L - 0xA1);
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
    OLED_CS_H();
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

        *FontBufLen = 0;

        return;
    }

    //-三角符号特殊处理-
    if (Code == GBK_Triangle)
    {
        for (i = 0; i < EN_Character_Size; i++)
        {
            FontBuf[i] = TriangleCode[i];
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
* 函数名称:    SwapFontBuf2DisplayBuf
* 函数功能:    将点阵数据转化为OLED数据,存放在显示缓冲中
* 输入参数:    FontBuf,  点阵数据缓冲
*              FontBufLen,  点阵数据长度
*              DisplayBufStartIndex,  OLED显示缓冲索引
* 输出参数:    DisplayBuf,  OLED显示缓冲
* 返 回 值:    无
*******************************************************************************/
void SwapFontBuf2DisplayBuf(unsigned char *FontBuf, unsigned FontBufLen, unsigned char *DisplayBuf, int DisplayBufStartIndex)
{
    int i = 0;
    int Index = 0;
    int BytePerLine = 0;

    if (FontBufLen == 32)         //-中文字符每行16个字节-
    {
        BytePerLine = 16;
    }
    else if (FontBufLen == 16)    //-英文字符每行8个字节-
    {
        BytePerLine = 8;
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
    for (i = 0; i < BytePerLine; i++)
    {
        DisplayBuf[Index++] = FontBuf[i];
    }

    Index = DisplayBufStartIndex + 128;
    for (i = BytePerLine; i < BytePerLine + BytePerLine; i++)
    {
        DisplayBuf[Index++] = FontBuf[i];
    }
}


/*******************************************************************************
* 函数名称:    OLED_SetPos
* 函数功能:    设置显示的坐标位置
* 输入参数:    x,横坐标位置;y,纵坐标位置.
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void OLED_SetPos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0 + y, OLED_CMD);
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
	OLED_WR_Byte((x & 0x0f) | 0x01, OLED_CMD); 
}  


/*******************************************************************************
* 函数名称:    OLED_DisplayOn
* 函数功能:    开启OLED
* 输入参数:    无
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/  
void OLED_DisplayOn(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14, OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF, OLED_CMD);  //DISPLAY ON
}


/*******************************************************************************
* 函数名称:    OLED_DisplayOff
* 函数功能:    关闭OLED
* 输入参数:    无
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/   
void OLED_DisplayOff(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10, OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE, OLED_CMD);  //DISPLAY OFF
}	


/*******************************************************************************
* 函数名称:    OLED_Clear
* 函数功能:    清屏
* 输入参数:    无
* 输出参数:    无
* 返 回 值:    无
* 其    他:    清完屏,整个屏幕是黑色的!和没点亮一样!!!
*******************************************************************************/ 	   			 	  
void OLED_Clear(void)  
{  
	int i = 0;
    int j = 0;
		    
	for(i = 0; i < 8; i++)  
	{  
		OLED_WR_Byte (0xb0+i, OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x02, OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10, OLED_CMD);      //设置显示位置—列高地址 
  
		for(j = 0; j < 128; j++)
        {
            OLED_WR_Byte(0x00, OLED_DATA); 
        }
	} 
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void OLED_Reset(void)
{
	OLED_WR_Byte(0xA8,OLED_CMD); //Set Multiplex Ratio 
	OLED_WR_Byte(0x3F,OLED_CMD); // 
	
	OLED_WR_Byte(0xD3,OLED_CMD); //Set Display Offset
	OLED_WR_Byte(0X00,OLED_CMD); //
	
	OLED_WR_Byte(0x40,OLED_CMD); //Set Display Start Line 
	
	OLED_WR_Byte(0XA1,OLED_CMD); //Set Segment Re-Map
	OLED_WR_Byte(0xAF,OLED_CMD); //Set Display On 
}


/*******************************************************************************
* 函数名称:    OLedInit
* 函数功能:    初始化OLED
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void OLedInit(void)
{
    OLED_RES_H();
	DelayMs(100);
	OLED_RES_L();
	DelayMs(100);
	OLED_RES_H();

	OLED_WR_Byte(0xFD,OLED_CMD); //Command Lock
	OLED_WR_Byte(0x12,OLED_CMD); //		
  
	OLED_WR_Byte(0xAE,OLED_CMD); //Set Display Off 
	
	OLED_WR_Byte(0xD5,OLED_CMD); //Set Display Clock Divide Ratio/Oscillator Frequency
	OLED_WR_Byte(0xA0,OLED_CMD); //
	
	OLED_WR_Byte(0xA8,OLED_CMD); //Set Multiplex_Digit Ratio 
	OLED_WR_Byte(0x3F,OLED_CMD); // 
	
	OLED_WR_Byte(0xD3,OLED_CMD); //Set Display Offset
	OLED_WR_Byte(0X00,OLED_CMD); //
	
	OLED_WR_Byte(0x40,OLED_CMD); //Set Display Start Line 
	
	OLED_WR_Byte(0XA1,OLED_CMD); //Set Segment Re-Map

	OLED_WR_Byte(0xC8,OLED_CMD); //Set COM Output Scan Direction 
	
	OLED_WR_Byte(0xDA,OLED_CMD); //Set COM Pins Hardware Configuration
	OLED_WR_Byte(0x12,OLED_CMD); //
	
	OLED_WR_Byte(0x81,OLED_CMD); //Set Current Control 
	OLED_WR_Byte(0xDF,OLED_CMD); //
	
	OLED_WR_Byte(0xD9,OLED_CMD); //Set Pre-Charge Period  
	OLED_WR_Byte(0x82,OLED_CMD); //
	
	OLED_WR_Byte(0xDB,OLED_CMD); //Set VCOMH Deselect Level 	 
	OLED_WR_Byte(0x34,OLED_CMD); //
	
	OLED_WR_Byte(0xA4,OLED_CMD); //Set Entire Display On/Off
	
	OLED_WR_Byte(0xA6,OLED_CMD); //Set Normal/Inverse Display 

	
	OLED_WR_Byte(0xAF,OLED_CMD); //Set Display On 
	OLED_Clear();
	OLED_SetPos(0,0);  
}


/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void OLED_SetAddress(unsigned char Page, unsigned char Column)
{
#if 0
    //Column -= 1;                    //我们平常所说的第1列，在LCD驱动IC里是第0列。
    //Page -= 1;

    /*-设置页地址。每页是8行。一个画面的64行被分成8个页。我们平常
     *-所说的第1页，在LCD驱动IC里是第0页，所以在这里减去1-*/
#endif
    OLED_WR_Byte(0xB0 + Page, OLED_CMD);                           //-设置页地址-
    OLED_WR_Byte(((Column >> 4) & 0x0f) + 0x10, OLED_CMD);         //-设置列地址的高4位-
    OLED_WR_Byte((Column & 0x0f) + 2, OLED_CMD);                   //-设置列地址的低4位-
}


/*******************************************************************************
* 函数名称:    LcdRefresh
* 函数功能:    Lcd刷新函数,将LcdDisplayBuf中的数据发送给Lcd
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void OLED_Refresh(int CharacterLineIndex)
{
    int i,j;

    //-每次刷新2行(1个字符行)-
    for(j = 0; j < 2; j++)
    {
        OLED_SetAddress(CharacterLineIndex * 2 + j, 0);

        for(i = 0; i < 128; i++)
        {
            OLED_WR_Byte(g_DisplayBuf[128 * j + i], OLED_DATA);
        }
    }
}
 

/*******************************************************************************
* 函数名称:    
* 函数功能:    
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void ShowTest(int X_Offset)
{
    unsigned char FontBuf[32] = {0};
    unsigned char FontBufLen = 0;
  
    unsigned int Code = 0;

    //-补上"*"- 
    Code = 0x2A;
    GetDotData(Code, EN_Character, &FontBuf[0], &FontBufLen);
    SwapFontBuf2DisplayBuf(&FontBuf[0], FontBufLen, &g_DisplayBuf[0], X_Offset);
    OLED_Refresh(0);
}


/*******************************************************************************
* 函数名称:    OLED_Test
* 函数功能:    测试函数
* 输入参数:    
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void OLED_Test(int CharacterLineIndex, int Code)
{
    ShowTest(0);
}


/*************************************END OF FILE*******************************/


