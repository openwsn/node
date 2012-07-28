/***********************************************************************************
    Filename:     hal_lcd.c

    Description:  Functions for accessing the LCD on SmartRF04EB (Nokia 3310 LCD)

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include "../hal_comp_foundation.h"
#include "hal_board.h"
#include "../interface/hal_lcd.h"
#include "../interface/hal_lcd_srf04.h"
#include "../../common/cc8051/hal_cc8051.h"
#include "../../utils/util.h"

/***********************************************************************************
 * MACROS, CONSTANTS AND DEFINES
 */


/******************************************************************************
* LCD
*
* See lcd.h for lcd fuctions
******************************************************************************/
#define LCD_DC                 P1_7
#define LCD_SDA                P1_6
#define LCD_SCL                P1_5
#define LCD_CE                 P1_4

/***********************************************************************************
 * LOCAL DATA
 */
static char pLcdLineBuffer[LCD_LINE_LENGTH];

/***********************************************************************************
 * LOCAL FUNCTIONS
 */
static void  lcdWait(void);

/**************************************/
//english_6x8_pixel
// 6 x 8 font
// 1 pixel space at left and bottom
// index = ASCII - 32
__code const unsigned char font6x8[][6] =
{
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // sp  0
  { 0x00, 0x00, 0x00, 0x2f, 0x00, 0x00 },   // !   1
  { 0x00, 0x00, 0x07, 0x00, 0x07, 0x00 },   // "   2
  { 0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14 },   // #   3
  { 0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12 },   // $   4
  { 0x00, 0x62, 0x64, 0x08, 0x13, 0x23 },   // %   5
  { 0x00, 0x36, 0x49, 0x55, 0x22, 0x50 },   // &   6
  { 0x00, 0x00, 0x05, 0x03, 0x00, 0x00 },   // '   7
  { 0x00, 0x00, 0x1c, 0x22, 0x41, 0x00 },   // (   8
  { 0x00, 0x00, 0x41, 0x22, 0x1c, 0x00 },   // )   9
  { 0x00, 0x14, 0x08, 0x3E, 0x08, 0x14 },   // *   10
  { 0x00, 0x08, 0x08, 0x3E, 0x08, 0x08 },   // +   11
  { 0x00, 0x00, 0x00, 0xA0, 0x60, 0x00 },   // ,   12
  { 0x00, 0x08, 0x08, 0x08, 0x08, 0x08 },   // -   13
  { 0x00, 0x00, 0x60, 0x60, 0x00, 0x00 },   // .   14
  { 0x00, 0x20, 0x10, 0x08, 0x04, 0x02 },   // /   15
  { 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 0   16
  { 0x00, 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 1   17
  { 0x00, 0x42, 0x61, 0x51, 0x49, 0x46 },   // 2   18
  { 0x00, 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 3   19
  { 0x00, 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 4   20
  { 0x00, 0x27, 0x45, 0x45, 0x45, 0x39 },   // 5   21
  { 0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 6   22
  { 0x00, 0x01, 0x71, 0x09, 0x05, 0x03 },   // 7   23
  { 0x00, 0x36, 0x49, 0x49, 0x49, 0x36 },   // 8   24
  { 0x00, 0x06, 0x49, 0x49, 0x29, 0x1E },   // 9   25
  { 0x00, 0x00, 0x36, 0x36, 0x00, 0x00 },   // :   26
  { 0x00, 0x00, 0x56, 0x36, 0x00, 0x00 },   // ;   27
  { 0x00, 0x08, 0x14, 0x22, 0x41, 0x00 },   // <   28
  { 0x00, 0x14, 0x14, 0x14, 0x14, 0x14 },   // =   29
  { 0x00, 0x00, 0x41, 0x22, 0x14, 0x08 },   // >   30
  { 0x00, 0x02, 0x01, 0x51, 0x09, 0x06 },   // ?   31
  { 0x00, 0x32, 0x49, 0x59, 0x51, 0x3E },   // @   32
  { 0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C },   // A   33
  { 0x00, 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B   34
  { 0x00, 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C   35
  { 0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D   36
  { 0x00, 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E   37
  { 0x00, 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F   38
  { 0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G   39
  { 0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H   40
  { 0x00, 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I   41
  { 0x00, 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J   42
  { 0x00, 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K   43
  { 0x00, 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L   44
  { 0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M   45
  { 0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N   46
  { 0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O   47
  { 0x00, 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P   48
  { 0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q   49
  { 0x00, 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R   50
  { 0x00, 0x46, 0x49, 0x49, 0x49, 0x31 },   // S   51
  { 0x00, 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T   52
  { 0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U   53
  { 0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V   54
  { 0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W   55
  { 0x00, 0x63, 0x14, 0x08, 0x14, 0x63 },   // X   56
  { 0x00, 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y   57
  { 0x00, 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z   58
  { 0x00, 0x00, 0x7F, 0x41, 0x41, 0x00 },   // [   59
  { 0x00, 0x55, 0x2A, 0x55, 0x2A, 0x55 },   // 55  60
  { 0x00, 0x00, 0x41, 0x41, 0x7F, 0x00 },   // ]   61
  { 0x00, 0x04, 0x02, 0x01, 0x02, 0x04 },   // ^   62
  { 0x00, 0x40, 0x40, 0x40, 0x40, 0x40 },   // _   63
  { 0x00, 0x00, 0x01, 0x02, 0x04, 0x00 },   // '   64
  { 0x00, 0x20, 0x54, 0x54, 0x54, 0x78 },   // a   65
  { 0x00, 0x7F, 0x48, 0x44, 0x44, 0x38 },   // b   66
  { 0x00, 0x38, 0x44, 0x44, 0x44, 0x20 },   // c   67
  { 0x00, 0x38, 0x44, 0x44, 0x48, 0x7F },   // d   68
  { 0x00, 0x38, 0x54, 0x54, 0x54, 0x18 },   // e   69
  { 0x00, 0x08, 0x7E, 0x09, 0x01, 0x02 },   // f   70
  { 0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C },   // g   71
  { 0x00, 0x7F, 0x08, 0x04, 0x04, 0x78 },   // h   72
  { 0x00, 0x00, 0x44, 0x7D, 0x40, 0x00 },   // i   73
  { 0x00, 0x40, 0x80, 0x84, 0x7D, 0x00 },   // j   74
  { 0x00, 0x7F, 0x10, 0x28, 0x44, 0x00 },   // k   75
  { 0x00, 0x00, 0x41, 0x7F, 0x40, 0x00 },   // l   76
  { 0x00, 0x7C, 0x04, 0x18, 0x04, 0x78 },   // m   77
  { 0x00, 0x7C, 0x08, 0x04, 0x04, 0x78 },   // n   78
  { 0x00, 0x38, 0x44, 0x44, 0x44, 0x38 },   // o   79
  { 0x00, 0xFC, 0x24, 0x24, 0x24, 0x18 },   // p   80
  { 0x00, 0x18, 0x24, 0x24, 0x18, 0xFC },   // q   81
  { 0x00, 0x7C, 0x08, 0x04, 0x04, 0x08 },   // r   82
  { 0x00, 0x48, 0x54, 0x54, 0x54, 0x20 },   // s   83
  { 0x00, 0x04, 0x3F, 0x44, 0x40, 0x20 },   // t   84
  { 0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C },   // u   85
  { 0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C },   // v   86
  { 0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C },   // w   87
  { 0x00, 0x44, 0x28, 0x10, 0x28, 0x44 },   // x   88
  { 0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C },   // y   89
  { 0x00, 0x44, 0x64, 0x54, 0x4C, 0x44 },   // z   90
  { 0x00, 0x00, 0xff, 0xff, 0x00, 0x00 },   // ||  91
  { 0x08, 0x1c, 0x2a, 0x08, 0x08, 0x08 },   // <-  92
  { 0x08, 0x08, 0x08, 0x2a, 0x1c, 0x08 },   // ->  93
  { 0x04, 0x02, 0xff, 0x02, 0x04, 0x00 },   // up  94
  { 0x00, 0x20, 0x40, 0xff, 0x40, 0x20 },   // down  95
  { 0x00, 0x01, 0x01, 0xFF, 0x81, 0x81 },   //ADC Series上用的上划线
  { 0x00, 0x80, 0x80, 0xFF, 0x80, 0x80 }    //ADC Series上用的下划线
};

/******** 发送一个字节的数据到液晶控制器*************************/
void halLcdSendByte(unsigned char Data)
{
  unsigned char i;
  LCD_CE=0;
  for(i=0;i<8;i++)
  {
    LCD_SCL = 0;
    if(Data & 0x80)
    {
      LCD_SDA = 1;
    }
    else LCD_SDA = 0;
    LCD_SCL = 1;
    Data<<=1;
  }
  LCD_CE=1;
}
/*-----------------------------------------------------------------------
LCD_write_byte    : 使用SPI接口写数据到LCD

输入参数：data    ：写入的数据；
command ：写数据/命令选择；

编写日期          ：2004-8-10
最后修改日期      ：2004-8-13
-----------------------------------------------------------------------*/
void halLcdWriteByte(unsigned char data, unsigned char command)
{
  LCD_CE = 1 ;		 // 使能LCD

  if (command == 0)
    LCD_DC = 0;	        // 传送命令
  else
    LCD_DC = 1;		// 传送数据
  halLcdSendByte(data);
  LCD_CE = 0;	        // 关闭LCD
  LCD_DC = 1;	
}

/*-----------------------------------------------------------------------
LCD_set_XY        : 设置LCD坐标函数

输入参数：X       ：0－83
Y       ：0－5

编写日期          ：2004-8-10
最后修改日期      ：2004-8-10
-----------------------------------------------------------------------*/
void halLcdSetXY(unsigned char X, unsigned char Y)
{
  halLcdWriteByte(0x40 | Y, 0);	// column
  halLcdWriteByte(0x80 | X, 0);  // row
}

/*******************************************************************************
 * @fn          halLcdInit
 *
 * @brief       Initialize Nokia3310LCD
 *
 * @param       none
 *
 * @return      none
 */
void halLcdInit(void)
{
  // CC2430 中，I/O 口做普通 I/O 使用时和每个 I/O 端口相关的寄存器有 3 个，分别是 //PxSEL
  //功能选择寄存器，PxDIR 方向寄存器，PxINP 输入模式寄存器，其中 x 为 0，1，2 。

  P1SEL &= 0x0F; //P1高4位 为普通 I/O 口
  P1DIR |= 0xF0; //P1高4位 设置为输出

  LCD_CE = 0;    // 关闭LCD
  lcdWait();
  LCD_CE = 1;    // 使能LCD
  lcdWait();

  halLcdWriteByte(0x21, 0);	// 使用扩展命令设置LCD模式
  halLcdWriteByte(0xc8, 0);	// 设置偏置电压
  halLcdWriteByte(0x06, 0);	// 温度校正
  halLcdWriteByte(0x13, 0);	// 1:48
  halLcdWriteByte(0x20, 0);	// 使用基本命令
  //halLcdWriteByte();	                // 清屏
  halLcdWriteByte(0x0c, 0);	// 设定显示模式，正常显示
  LCD_CE=0 ;                    // 关闭LCD
  
  halLcdClear();
}


/***********************************************************************************
 * @fn          halLcdClear
 *
 * @brief       Clear all lines on LCD
 *
 * @param       none
 *
 * @return      none
 */
void halLcdClear(void)
{
  //halLcdWriteLines(NULL, NULL, NULL);
  unsigned int i;

  halLcdWriteByte(0x0c, 0);
  halLcdWriteByte(0x80, 0);

  for (i=0; i<504; i++)
    halLcdWriteByte(0, 1);
}

/***********************************************************************************
 * @fn          halLcdGetLineLength
 *
 * @brief       Get max number of characters on each line
 *
 * @param       none
 *
 * @return      uint8 - number of characters on a line
 */
uint8 halLcdGetLineLength(void)
{
    return LCD_LINE_LENGTH;
}

/***********************************************************************************
 * @fn          halLcdGetNumLines
 *
 * @brief       Get the number of lines on the LCD.
 *
 * @param       none
 *
 * @return      uint8 - number of lines
 */
uint8 halLcdGetNumLines(void)
{
    return LCD_LINE_COUNT;
}


/***********************************************************************************
 * @fn          halLcdWriteChar
 *
 * @brief       Write a character on LCD.
 *
 * @param       line - line on display
 *              col - column
 *              text - character to display
 *
 * @return      none
 */
void halLcdWriteChar(uint8 line, uint8 col, char text)
{
  unsigned char i;
  halLcdSetXY(col*6,line);
  text -= 32;
  for (i=0; i<6; i++)
    halLcdWriteByte(font6x8[text][i], 1);
}


/***********************************************************************************
 * @fn          halLcdWriteLine
 *
 * @brief       Write line on LCD.
 *
 * @param       line - line on display
 *              text - text string
 *
 * @return      none
 */
void halLcdWriteLine(uint8 line, const char XDATA *text)
{
  halLcdWriteEnglishString(0,line,text);
}


/***********************************************************************************
 * @fn          halLcdWriteLine
 *
 * @brief       Write all lines on LCD.
 *
 * @param       line1 - text string to display on line 1
 *              line2 - text string to display on line 2
 *              line3 - ignored for this display
 *
 * @return      none
 */
void halLcdWriteLines(const char* line1, const char* line2, const char* line3)
{
   halLcdWriteLine(HAL_LCD_LINE_1, line1);
   halLcdWriteLine(HAL_LCD_LINE_2, line2);
   return;
}

/***********************************************************************************
 * @fn          halLcdDisplayValue
 *
 * @brief       Display value on display with optional text on right and left side
 *
 * @param       uint8 line
 *              char *pLeft
 *              int32 value
 *              char *pRight
 *
 * @return      none
 */
void halLcdDisplayValue(uint8 line, char __xdata *pLeft, int32 value, char __xdata *pRight)
{
    uint8 n;
    char *pValue = convInt32ToText(value);
    for (n = 0; n < LCD_LINE_LENGTH; n++) {
        if (pLeft && *pLeft) {
            pLcdLineBuffer[n] = *(pLeft++);
        } else if (*pValue) {
            pLcdLineBuffer[n] = *(pValue++);
        } else if (pRight && *pRight) {
            pLcdLineBuffer[n] = *(pRight++);
        } else {
            pLcdLineBuffer[n] = ' ';
        }
    }
    halLcdWriteLine(line, pLcdLineBuffer);
}

/***********************************************************************************
 * @fn          lcdWait
 *
 * @brief       This function does the timing of clk
 *
 * @param       none
 *
 * @return      none
 */
static void lcdWait(void)
{
   uint8 i = 0x01;
   while(i--);
}

/*-----------------------------------------------------------------------
LCD_write_english_String  : 英文字符串显示函数

输入参数：*s      ：英文字符串指针；
X、Y    : 显示字符串的位置,x 0-83 ,y 0-5

编写日期          ：2004-8-10
最后修改日期      ：2004-8-10 		
-----------------------------------------------------------------------*/
void halLcdWriteEnglishString(unsigned char X,unsigned char Y,const char *s)
{
  unsigned char i=0;
  halLcdSetXY(0,Y);//把第Y行清零
  for(i=0;i<84;i++)
  {
    halLcdWriteByte(0,1);
  }
  halLcdSetXY(X,Y);
  i=0;
  while (*s)
  {
    halLcdWriteChar(Y,i,*s);
    s++;
    i++;
    if(i>=LCD_LINE_LENGTH)return;
  }
}

/*-----------------------------------------------------------------------
LCD_draw_map      : 位图绘制函数

输入参数：X、Y    ：位图绘制的起始X、Y坐标；
*map    ：位图点阵数据；
Pix_x   ：位图像素（长）
Pix_y   ：位图像素（宽）

编写日期          ：2004-8-13
最后修改日期      ：2004-8-13
-----------------------------------------------------------------------*/
void LCD_draw_bmp_pixel(unsigned char X,unsigned char Y,unsigned char *map,
                        unsigned char Pix_x,unsigned char Pix_y)
{
  unsigned int i,n;
  unsigned char row;

  if (Pix_y%8==0) row=Pix_y/8;      //计算位图所占行数
  else
    row=Pix_y/8+1;

  for (n=0;n<row;n++)
  {
    halLcdSetXY(X,Y);
    for(i=0; i<Pix_x; i++)
    {
      halLcdWriteByte(map[i+n*Pix_x], 1);
    }
    Y++;                         //换行
  }
}

/*-----------------------------------------------------------------------
LCD_write_chinese_string: 在LCD上显示汉字
------------------------------------------------------------------------*/
void ShowHz16(unsigned char Y,unsigned char X,unsigned char *address)
{
    unsigned char i;
    halLcdSetXY(X,Y);
    for(i=0;i<16;i++) {halLcdWriteByte(*address,1);address++;}
    halLcdSetXY(X,++Y);
    for(i=0;i<16;i++) {halLcdWriteByte(*address,1);address++;}
}

/******************************************************************************
* See lcd.h for a description of this function.滚动显示
******************************************************************************/
/*
void scrollText(char *string, UINT8 length){
  UINT8 i = 0;

  while(getJoystickDirection() != CENTRED);

  do{
    lcdUpdateLine(string, &string[i]);
    i++;

    if(getJoystickDirection() != CENTRED)
    {
      lcdUpdateLine(string, &(string[length-15]));//cywu 17->15
      return;
    }

    halWait(100);
  } while((string[i+13] != '\0') && (i + 13) < length);//cywu 15->13
}
*/
/******************************************************************************/

