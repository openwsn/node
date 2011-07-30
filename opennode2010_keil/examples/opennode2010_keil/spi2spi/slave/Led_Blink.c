#include "stm32f10x_heads.h"
#include "HelloRobot.h"

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BufferSize  32

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SPI_InitTypeDef  SPI_InitStructure;
u16 SPI1_Buffer_Tx[BufferSize] = {0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B0C,
                                  0x0D0E, 0x0F10, 0x1112, 0x1314, 0x1516, 0x1718,
                                  0x191A, 0x1B1C, 0x1D1E, 0x1F20, 0x2122, 0x2324,
                                  0x2526, 0x2728, 0x292A, 0x2B2C, 0x2D2E, 0x2F30,
                                  0x3132, 0x3334, 0x3536, 0x3738, 0x393A, 0x3B3C,
                                  0x3D3E, 0x3F40};
u16 SPI2_Buffer_Tx[BufferSize] = {0x5152, 0x5354, 0x5556, 0x5758, 0x595A, 0x5B5C,
                                  0x5D5E, 0x5F60, 0x6162, 0x6364, 0x6566, 0x6768,
                                  0x696A, 0x6B6C, 0x6D6E, 0x6F70, 0x7172, 0x7374,
                                  0x7576, 0x7778, 0x797A, 0x7B7C, 0x7D7E, 0x7F80,
                                  0x8182, 0x8384, 0x8586, 0x8788, 0x898A, 0x8B8C,
                                  0x8D8E, 0x8F90};
u16 SPI1_Buffer_Rx[BufferSize], SPI2_Buffer_Rx[BufferSize];
u8 TxIdx = 0, RxIdx = 0;
vu16 CRC1Value = 0, CRC2Value = 0;
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;
ErrorStatus HSEStartUpStatus;


TestStatus Buffercmp(u16* pBuffer1, u16* pBuffer2, u16 BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;
}

void spi_config()
{

  SPI_I2S_DeInit(SPI1);//Reset SPI2
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;

GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

GPIO_Init(GPIOA, &GPIO_InitStructure);


  SPI_Cmd(SPI1, DISABLE);

	/* SPI1 configuration ------------------------------------------------------*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* SPI2 configuration ------------------------------------------------------*/
  //SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
  //SPI_Init(SPI2, &SPI_InitStructure);

  /* Enable SPI1 CRC calculation */
  SPI_CalculateCRC(SPI1, ENABLE);
  /* Enable SPI2 CRC calculation */
  //SPI_CalculateCRC(SPI2, ENABLE);

  /* Enable SPI1 */
  SPI_Cmd(SPI1, ENABLE);
  /* Enable SPI2 */
  //SPI_Cmd(SPI2, ENABLE);

  /* Transfer procedure */
    /* Wait for SPI1 Tx buffer empty */
    //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    /* Send SPI2 data */
    //SPI_I2S_SendData(SPI2, SPI2_Buffer_Tx[TxIdx]);
    /* Send SPI1 data */
    //SPI_I2S_SendData(SPI1, 0x1234);
    /* Wait for SPI2 data reception */
    //while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    /* Read SPI2 received data */
    //SPI2_Buffer_Rx[RxIdx] = SPI_I2S_ReceiveData(SPI2);
    /* Wait for SPI1 data reception */
    //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    /* Read SPI1 received data */
    //SPI1_Buffer_Rx[RxIdx++] = SPI_I2S_ReceiveData(SPI1);
  

  /* Wait for SPI1 Tx buffer empty */
  //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  /* Wait for SPI2 Tx buffer empty */
  //while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

  /* Send last SPI2_Buffer_Tx data */
  //SPI_I2S_SendData(SPI2, SPI2_Buffer_Tx[TxIdx]);
  /* Enable SPI2 CRC transmission */
  //SPI_TransmitCRC(SPI2);
  /* Send last SPI1_Buffer_Tx data */
  //SPI_I2S_SendData(SPI1, SPI1_Buffer_Tx[TxIdx]);
  /* Enable SPI1 CRC transmission */
  //SPI_TransmitCRC(SPI1);

  /* Wait for SPI1 last data reception */
  //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  /* Read SPI1 last received data */
  //SPI1_Buffer_Rx[RxIdx] = SPI_I2S_ReceiveData(SPI1);

  /* Wait for SPI2 last data reception */
  //while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
  /* Read SPI2 last received data */
  //SPI2_Buffer_Rx[RxIdx] = SPI_I2S_ReceiveData(SPI2);

  /* Wait for SPI1 data reception: CRC transmitted by SPI2 */
  //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  /* Wait for SPI2 data reception: CRC transmitted by SPI1 */
  //while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

  /* Check the received data with the send ones */
  //TransferStatus1 = Buffercmp(SPI2_Buffer_Rx, SPI1_Buffer_Tx, BufferSize);
  //TransferStatus2 = Buffercmp(SPI1_Buffer_Rx, SPI2_Buffer_Tx, BufferSize);
  /* TransferStatus1, TransferStatus2 = PASSED, if the data transmitted and received
     are correct */
  /* TransferStatus1, TransferStatus2 = FAILED, if the data transmitted and received
     are different */

  /* Test on the SPI1 CRC Error flag */
  //if ((SPI_I2S_GetFlagStatus(SPI1, SPI_FLAG_CRCERR)) == SET)
  //{
    //TransferStatus2 = FAILED;
  //}

  /* Test on the SPI2 CRC Error flag */
  //if ((SPI_I2S_GetFlagStatus(SPI2, SPI_FLAG_CRCERR)) == SET)
  //{
    //TransferStatus1 = FAILED;
  //}

  /* Read SPI1 received CRC value */
  //CRC1Value = SPI_I2S_ReceiveData(SPI1);
  /* Read SPI2 received CRC value */
  //CRC2Value = SPI_I2S_ReceiveData(SPI2);

}

int main(void)
{
   BSP_Init();	//开发板初始化函数
   spi_config();
   //cc2520_config();
   while (1)
   {
     SPI1_Buffer_Rx[RxIdx]=0;
     GPIO_SetBits(GPIOA, GPIO_Pin_8);  // PA8输出高电平
   	 delay_nms(30);                    // 延时50ms		 	
     /* Wait for SPI1 last data reception */
     while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
	  GPIO_SetBits(GPIOA, GPIO_Pin_8);  // PA8输出高电平
     /* Read SPI1 received data */
     SPI1_Buffer_Rx[RxIdx] = SPI_I2S_ReceiveData(SPI1);
	 if (SPI1_Buffer_Rx[RxIdx]<0xffff)	{GPIO_ResetBits(GPIOA,GPIO_Pin_8);delay_nms(30);}
	
   }
}
