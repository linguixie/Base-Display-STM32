/*******************************************************************************
* Copyright (c) 2015,xxx��˾
 *            All rights reserved.
* �������ƣ�
* �� �� �ţ�1.0
* �� �� �ˣ�л�ֹ�
* ����ʱ�䣺2015-3-16
* �� �� ��:
* �޸�ʱ��:
* �޸�˵��:
* ��    ����   1��ע��������8080ʱ����6080ʱ��.���ļ�����8080ʱ��.
               2��ע��RD�ܽű����ڳ�ʼ��ʱ����.�������
********************************************************************************/
/*******************************************************************************
*                                    ͷ  ��  ��
********************************************************************************/
#include "Valve.h"
#include "Lcd.h"

/*******************************************************************************
*                               �ļ��ڲ�ʹ�ú궨��
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
*                                 ȫ�ֺ���(����)����
********************************************************************************/

//-LCDҪ��ʾ��1�����ݵĻ���,���������ܶ����1024-
unsigned char g_DisplayBuf[Display_Buf_Size];

//-LCDҪ���µ������������Ŵ��ϵ���0~3(����LcdDisplayBuf��������С����,ֻ��ÿ�θ���1��)-  
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
*                                 ��̬����(����)����
********************************************************************************/


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void DelayUs(int Count)
{
    int i = 0;
    int j = 0;

    for (i = 0; i < Count; i++)
        for (j = 0; j < 1; j++);
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void Delay(int Count)
{
    int i = 0;
    int j = 0;

    for (i = 0; i < Count; i++)
        for (j = 0; j < 110; j++);
}

/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void LcdGPIOInit(void)
{
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    
* ��������:     
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
unsigned char ReadStatus(void)
{
    unsigned char Byte = 0;
    
    SetDataBusInput();
    LCD_RS_L();
    LCD_CS_L();
    LCD_RD_L();
    DelayUs(3);          //-���������ʱ,����������������������-
    Byte = LCD_Read();
    DelayUs(3);
    LCD_RD_H();
    LCD_CS_H();

    SetDataBusOutput();

    return Byte;
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void LcdReset(void)
{
    unsigned char Byte = 0;

    Byte = ReadStatus();
    if (((Byte & 0x10) != 0) || ((Byte & 0x20) != 0))
    {
        LCD_RES_L();              /*�͵�ƽ��λ*/
        Delay(200);
        LCD_RES_H();              /*��λ���*/
        Delay(50);
        TransferCommand(0xe2);   /*��λ*/
        Delay(5);
        TransferCommand(0x2C);   /*��ѹ����1*/
        Delay(5);
        TransferCommand(0x2E);   /*��ѹ����2*/
        Delay(5);
        TransferCommand(0x2F);   /*��ѹ����3*/
        Delay(5);
    }

    TransferCommand(0x23);   /*�ֵ��Աȶȣ������÷�Χ0x20��0x27*/
    TransferCommand(0x81);   /*΢���Աȶ�*/
    TransferCommand(0x30);   /*0x28,΢���Աȶȵ�ֵ�������÷�Χ0x00��0x3f*/
    TransferCommand(0xA2);   /*1/9ƫѹ�ȣ�bias��*/
    TransferCommand(0xc8);   /*��ɨ��˳�򣺴��ϵ���*/
    TransferCommand(0xa0);   /*��ɨ��˳�򣺴�����*/
    TransferCommand(0x40);   /*��ʼ�У���һ�п�ʼ*/    
    TransferCommand(0xaf);   /*����ʾ*/

}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void LcdInit(void)
{
    int i = 0;

    LCD_RES_L();              /*�͵�ƽ��λ*/
    Delay(200);
    LCD_RES_H();              /*��λ���*/
    Delay(50);

    TransferCommand(0xe2);   /*��λ*/
    Delay(5);
    TransferCommand(0x2c);   /*��ѹ����1*/
    Delay(5);
    TransferCommand(0x2e);   /*��ѹ����2*/
    Delay(5);
    TransferCommand(0x2f);   /*��ѹ����3*/
    Delay(5);

    LcdReset();

    for (i = 0; i < Display_Buf_Size; i++)
    {
        g_DisplayBuf[i] = 0x00;
    }
    ClearScreen();
}


/*******************************************************************************
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void LcdSetAddress(unsigned char Page, unsigned char Column)
{
#if 0
    //Column -= 1;                    //����ƽ����˵�ĵ�1�У���LCD����IC���ǵ�0�С�
    //Page -= 1;

    /*-����ҳ��ַ��ÿҳ��8�С�һ�������64�б��ֳ�8��ҳ������ƽ��
     *-��˵�ĵ�1ҳ����LCD����IC���ǵ�0ҳ�������������ȥ1-*/
#endif
    TransferCommand(0xB0 + Page);   
    TransferCommand(((Column >> 4) & 0x0f) + 0x10);         //�����е�ַ�ĸ�4λ
    TransferCommand(Column & 0x0f);                         //�����е�ַ�ĵ�4λ
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
* ��������:    
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void SwapFontBuf2DisplayBuf(unsigned char *FontBuf, unsigned FontBufLen, unsigned char *DisplayBuf, int DisplayBufStartIndex)
{
    int i = 0;
    int Index = 0;
    int ColumnPerCharacter = 0;

    if (FontBufLen == 32)         //-ÿ�������ַ�ռ16��-
    {
        ColumnPerCharacter = 16;
    }
    else if (FontBufLen == 16)    //-ÿ��Ӣ���ַ�ռ8��-
    {
        ColumnPerCharacter = 8;
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
* ��������:    ClearScreen
* ��������:    
* �������:    
* �������:    ��
* �� �� ֵ:    ��
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
* ��������:    LcdRefresh
* ��������:    Lcdˢ�º���,��LcdDisplayBuf�е����ݷ��͸�Lcd
* �������:    
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void LcdRefresh(int CharacterLineIndex)
{
    int i,j;

    //-ÿ��ˢ��2��(1���ַ���)-
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


