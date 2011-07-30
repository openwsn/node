通用MSP430—I2C程序 
 

＃i nclude "MSP430x14x.h"
//P3.2  SCL
//P3.3  SDA

#define SDA_1       P3OUT |=  BIT3              //SDA = 1
#define SDA_0       P3OUT &=~ BIT3              //SDA = 0
#define SCL_1       P3OUT |=  BIT2              //SCL = 1
#define SCL_0       P3OUT &=~ BIT2              //SCL = 0
#define DIR_IN      P3DIR &=~ BIT3;  SDA_1      //I/O口为输入
#define DIR_OUT     P3DIR |=  BIT3              //I/0口为输出
#define SDA_IN      ((P3IN >> 3) & 0x01)        //Read SDA

unsigned int a[50];

static void Delay(unsigned int n)
{
   unsigned int i;
   for (i=0; i<n; i++) {;}
}

void Init(void)
{
  SCL_1;
  Delay(5);
  SDA_1;
  Delay(5);
}

void Start(void)
{
  SDA_1;
  Delay(5);
  SCL_1;
  Delay(5);
  SDA_0;
  Delay(5);
  SCL_0;
  Delay(5);
}

void Stop(void)
{
  SDA_0;
  Delay(5);
  SCL_1;
  Delay(5);
  SDA_1;
  Delay(5);
}

void WriteByte(unsigned char WriteData)
{
  unsigned char i;
  for (i=0; i<8; i++)
  {
    SCL_0;
    Delay(5);
    if (((WriteData >> 7) & 0x01) == 0x01)
    {
      SDA_1;
    }
    else
    {
      SDA_0;
    }
    Delay(5);
    SCL_1;
    WriteData = WriteData << 1;
    Delay(5);
  }
  SCL_0;
  SDA_1;
  Delay(5);
  Delay(5);
}

unsigned char ReadByte(void)
{
  unsigned char i;
  unsigned char TempBit  = 0;
  unsigned char TempData = 0;
  SCL_0;
  Delay(5);
  SDA_1;
  for (i=0; i<8; i++)
  {
    Delay(5);
    SCL_1;
    Delay(5);
    DIR_IN;
    if (SDA_IN == 0x01 /*sda==1*/)
    {
      TempBit = 1;
    }
    else
    {
      TempBit = 0;
    }
    DIR_OUT;
    TempData = (TempData << 1) | TempBit;
    SCL_0;
  }
  Delay(5);
  return(TempData);
}

void ReceiveAck(void)
{
  unsigned char i = 0;
  SCL_1;
  Delay(5);
  DIR_IN;
  while ((SDA_IN == 0x01 /*sda==1*/) && (i < 255)/*调试方便,可以不要*/)
  {
    i++;
  }
  DIR_OUT;
  SCL_0;
  Delay(5);
}

void Acknowledge(void)
{
  SCL_0;
  Delay(5);
  DIR_OUT;
  SDA_0;
  SCL_1;
  Delay(5);
  SCL_0;
}

unsigned int ReadWord(unsigned char unit/*address*/)
{
  unsigned char HighData = 0;
  unsigned char LowData  = 0;
  unsigned int  TempData = 0;
  Start();
  WriteByte(0xa0);
  ReceiveAck();
  WriteByte(unit);
  ReceiveAck();
  Start();
  WriteByte(0xa1);
  ReceiveAck();
  LowData  = ReadByte();
  Acknowledge();
  HighData = ReadByte();
  Stop();
  TempData = (HighData << 8) + LowData;
  Delay(1000);
  return(TempData);
}

void ReadWords(unsigned char unit/*address*/)
{
  unsigned char i;
  unsigned char HighData = 0;
  unsigned char LowData  = 0;
  unsigned int  TempData = 0;
  Start();
  WriteByte(0xa0);
  ReceiveAck();
  WriteByte(unit);
  ReceiveAck();
  Start();
  WriteByte(0xa1);
  ReceiveAck();
  for (i=0; i<49; i++)
  {
    LowData  = ReadByte();
    Acknowledge();
    HighData = ReadByte();
    Acknowledge();
    a[i]= (HighData << 8) + LowData;
  }
  LowData  = ReadByte();
  Acknowledge();
  HighData = ReadByte();
  Stop();
  a[49] = (HighData << 8) + LowData;
  Delay(1000);
}

void WriteWord(unsigned char unit/*address*/, unsigned int WriteData)
{
  unsigned char LowData  = 0;
  unsigned char HighData = 0;
  LowData  = (unsigned char)WriteData;
  HighData = (unsigned char)(WriteData >> 8);
  Start();
  WriteByte(0xa0);
  ReceiveAck();
  WriteByte(unit);
  ReceiveAck();
  WriteByte(LowData);
  ReceiveAck();
  WriteByte(HighData);
  ReceiveAck();
  Stop();
  Delay(2000);
}