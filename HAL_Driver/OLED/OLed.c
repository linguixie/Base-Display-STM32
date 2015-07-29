/*******************************************************************************
* Copyright (c) 2015,�����Զ����Ƽ����޹�˾
 *            All rights reserved.
* �������ƣ�
* �� �� �ţ�1.0
* �� �� �ˣ�л�ֹ�
* ����ʱ�䣺2015-6-4
* �� �� ��:
* �޸�ʱ��:
* �޸�˵��:
* ��    ����
********************************************************************************/
/*******************************************************************************
*                                    ͷ  ��  ��
********************************************************************************/
#include "stm32f0xx_conf.h"
#include "GPIO.h"
#include "OLed.h"

/*******************************************************************************
*                               �ļ��ڲ�ʹ�ú궨��
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
*                                 ȫ�ֺ���(����)����
********************************************************************************/
//-LCDҪ��ʾ��1�����ݵĻ���,���������ܶ����1024-
unsigned char g_DisplayBuf[Display_Buf_Size];

/*******************************************************************************
*                                 ��̬����(����)����
********************************************************************************/

/*-�����0~9��%,����ͨ�������ַ�����:��������������1��-*/
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
* ��������:    Delay
* ��������:    ��ʱ
* �������:    ��ʱ��ֵ
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    OLED_WR_Byte
* ��������:    ��ʱ
* �������:    Byte,Ҫд����ֽ�;
               Command,0��ʾ����,1��ʾ����.
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
unsigned int Rom16_16GetAddress(unsigned char Code_H, unsigned char Code_L)
{
    unsigned int Address = 0;

    /*-Ϊ�˼��ٴ�����,���ֿ�оƬ�еĵ�ַ�����޹ز���ȥ����,�������ƺ����Ĵ������һ��-*/
    //-�����ַ�,�ο�GB2312�����-
    if (Code_H >= 0xB0 && Code_H <= 0xF7 && Code_L >= (unsigned char)0xA1)
    {
        Address = (Code_H - 0xB0) * 94;
        Address += (Code_L - 0xA1) + 846;
        Address <<= 5;

        return Address;
        //-�����-
        //return ((Code_H - 0xB0) * 94 + (Code_L - 0xA1) + 846) * 32;
    }
    
    if (Code_H >= 0xA1 && Code_H <= 0xA3 && Code_L >= (unsigned char)0xA1)
    {
        Address = (Code_H - 0xA1) * 94;
        Address += (Code_L - 0xA1);
        Address <<= 5;

        return Address;
        //-�����-
        //return ((Code_H - 0xB0) * 94 + (Code_L - 0xA1) + 846) * 32;
    }

    return 0x00;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
unsigned int Rom8_16GetAddress(unsigned char Code_H, unsigned char Code_L)
{
    unsigned int Code = 0x00;
    unsigned int Address = 0;
    
    Code = Code_H;
    Code <<= 8;
    Code += Code_L;

    //-�����ַ�,�ο�GB2312�����-
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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
static unsigned char RomGetData(void)
{
    unsigned char i;
    unsigned char Data = 0;

    ROM_CLK_H();
    for(i = 0; i < 8; i++)
    {
        //-����<JLX12864G-095-PC���ֿ�IC�ı��˵����>��ROM_MISO = 1Ӧ������,������-
        //-ʵ�������������ּ��ϴ�������Һ����Ϊ�հ�.ԭ��δ֪-
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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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

    //-�ո����⴦��-
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

    //-���Ƿ������⴦��-
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
* ��������:    SwapFontBuf2DisplayBuf
* ��������:    ����������ת��ΪOLED����,�������ʾ������
* �������:    FontBuf,  �������ݻ���
*              FontBufLen,  �������ݳ���
*              DisplayBufStartIndex,  OLED��ʾ��������
* �������:    DisplayBuf,  OLED��ʾ����
* �� �� ֵ:    ��
*******************************************************************************/
void SwapFontBuf2DisplayBuf(unsigned char *FontBuf, unsigned FontBufLen, unsigned char *DisplayBuf, int DisplayBufStartIndex)
{
    int i = 0;
    int Index = 0;
    int BytePerLine = 0;

    if (FontBufLen == 32)         //-�����ַ�ÿ��16���ֽ�-
    {
        BytePerLine = 16;
    }
    else if (FontBufLen == 16)    //-Ӣ���ַ�ÿ��8���ֽ�-
    {
        BytePerLine = 8;
    }
    else
    {
        return;
    }

    if (DisplayBufStartIndex & 0x7 != 0)    //-��8����-
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
* ��������:    OLED_SetPos
* ��������:    ������ʾ������λ��
* �������:    x,������λ��;y,������λ��.
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void OLED_SetPos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0 + y, OLED_CMD);
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
	OLED_WR_Byte((x & 0x0f) | 0x01, OLED_CMD); 
}  


/*******************************************************************************
* ��������:    OLED_DisplayOn
* ��������:    ����OLED
* �������:    ��
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/  
void OLED_DisplayOn(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14, OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF, OLED_CMD);  //DISPLAY ON
}


/*******************************************************************************
* ��������:    OLED_DisplayOff
* ��������:    �ر�OLED
* �������:    ��
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/   
void OLED_DisplayOff(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10, OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE, OLED_CMD);  //DISPLAY OFF
}	


/*******************************************************************************
* ��������:    OLED_Clear
* ��������:    ����
* �������:    ��
* �������:    ��
* �� �� ֵ:    ��
* ��    ��:    ������,������Ļ�Ǻ�ɫ��!��û����һ��!!!
*******************************************************************************/ 	   			 	  
void OLED_Clear(void)  
{  
	int i = 0;
    int j = 0;
		    
	for(i = 0; i < 8; i++)  
	{  
		OLED_WR_Byte (0xb0+i, OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x02, OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10, OLED_CMD);      //������ʾλ�á��иߵ�ַ 
  
		for(j = 0; j < 128; j++)
        {
            OLED_WR_Byte(0x00, OLED_DATA); 
        }
	} 
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    OLedInit
* ��������:    ��ʼ��OLED
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void OLED_SetAddress(unsigned char Page, unsigned char Column)
{
#if 0
    //Column -= 1;                    //����ƽ����˵�ĵ�1�У���LCD����IC���ǵ�0�С�
    //Page -= 1;

    /*-����ҳ��ַ��ÿҳ��8�С�һ�������64�б��ֳ�8��ҳ������ƽ��
     *-��˵�ĵ�1ҳ����LCD����IC���ǵ�0ҳ�������������ȥ1-*/
#endif
    OLED_WR_Byte(0xB0 + Page, OLED_CMD);                           //-����ҳ��ַ-
    OLED_WR_Byte(((Column >> 4) & 0x0f) + 0x10, OLED_CMD);         //-�����е�ַ�ĸ�4λ-
    OLED_WR_Byte((Column & 0x0f) + 2, OLED_CMD);                   //-�����е�ַ�ĵ�4λ-
}


/*******************************************************************************
* ��������:    LcdRefresh
* ��������:    Lcdˢ�º���,��LcdDisplayBuf�е����ݷ��͸�Lcd
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void OLED_Refresh(int CharacterLineIndex)
{
    int i,j;

    //-ÿ��ˢ��2��(1���ַ���)-
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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void ShowTest(int X_Offset)
{
    unsigned char FontBuf[32] = {0};
    unsigned char FontBufLen = 0;
  
    unsigned int Code = 0;

    //-����"*"- 
    Code = 0x2A;
    GetDotData(Code, EN_Character, &FontBuf[0], &FontBufLen);
    SwapFontBuf2DisplayBuf(&FontBuf[0], FontBufLen, &g_DisplayBuf[0], X_Offset);
    OLED_Refresh(0);
}


/*******************************************************************************
* ��������:    OLED_Test
* ��������:    ���Ժ���
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void OLED_Test(int CharacterLineIndex, int Code)
{
    ShowTest(0);
}


/*************************************END OF FILE*******************************/


