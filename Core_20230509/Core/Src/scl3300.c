 
// file: sca103t.c

#include "stm32f1xx_hal.h"
#include "scl3300.h"

//#include "ssd1306.h"

#define SPI_NSS_PORT     GPIOB
#define SPI_NSS_PIN      GPIO_PIN_12

void spi_nss_hi(void)
{
  HAL_GPIO_WritePin(SPI_NSS_PORT, SPI_NSS_PIN, GPIO_PIN_SET);
}

void spi_nss_low(void)
{
  HAL_GPIO_WritePin(SPI_NSS_PORT, SPI_NSS_PIN, GPIO_PIN_RESET);
}



/**
  * @brief  Write a byte on the SD.
  * @param  Data: byte to send.
  * @retval None
  */
extern SPI_HandleTypeDef hspi2;


/*
#define delay_ms     HAL_Delay
#define millis()     HAL_GetTick()
#define SYS_CLOCK    72
#define SYSTICK_LOAD 71999
extern __IO uint32_t uwTick;

uint32_t micros() {
  return (uwTick&0x3FFFFF)*1000 + (SYSTICK_LOAD-SysTick->VAL)/SYS_CLOCK;
}


void delay_us(uint32_t us) {
  uint32_t temp = micros();
  uint32_t comp = temp + us;
  uint8_t  flag = 0;
  while(comp > temp){
    if(((uwTick&0x3FFFFF)==0)&&(flag==0)){
      flag = 1;
    }
    if(flag) temp = micros() + 0x400000UL * 1000;
    else     temp = micros();
  }
}

 */
void HAL_Delay(__IO uint32_t Delay) {
  uint32_t tickstart = HAL_GetTick();

  while((HAL_GetTick() - tickstart) < Delay);
}

/**
  * @brief  Read a byte from the SD.
  * @param  None
  * @retval The received byte.
  */

static __IO uint32_t SPITimeout = SPIT_LONG_TIMEOUT;
//static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
//{
//  return 0;
//}
//   2.38
uint8_t SPI_SCL3300_SendByte(uint8_t byte)
{
	  uint8_t dat=0;
	  uint8_t reg_val=0;



//	  while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY);
  //// https://www.bilibili.com/read/cv4062279/ 出处：bilibili
	   //////  xxxxxxxx
  	//   HAL_SPI_Transmit(&hspi2,  &byte, 1, 0xFFFF); // dat;
 	//   HAL_SPI_Receive(&hspi2,  &reg_val, 1, 0xFFFF); // dat;
 	//   HAL_Delay(1);

       HAL_SPI_TransmitReceive(&hspi2,  &byte, &reg_val, 1,0xFFFF);
  	   dat  = reg_val ;//Spi_ReadByte();

  	 //  HAL_Delay(1);

      return dat; // HAL_SPI_Receive(&hspi2,  &reg_val, 1, 0xFFFF); // dat;

/*
 *
// https://www.bilibili.com/read/cv4062279/  // HAL xxx
 //https://blog.csdn.net/qq_42680253/article/details/101199713  datasheet 설명
 //https://blog.csdn.net/u012846795/article/details/119271129   //nrf52832
u8 recv_data;
while(SPI_GetFlagStatus(SPI_FLAG_TXE)== RESET); //while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); //等待发送缓冲为空，发送数据
SPI_SendData(sen_data); //SPI_I2S_SendData(SPI1, sen_data);

while(SPI_GetFlagStatus(SPI_FLAG_RXNE)== RESET); //while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); //等待接收缓冲非空，接受数据
recv_data = SPI_ReceiveData();
return recv_data;



*/

}


uint32_t SPI_SCL3300_CMD(uint32_t cmd)
//u32 SPI_SCL3300_CMD(u32 cmd)
{

  uint8_t retemp[4] ={0};
  //uint32_t Temp = 0;
 // uint32_t Temp = 0;
// SPI_SCL3300_SendCmd(cmd);

  	spi_nss_low(); // SPI_SCL3300_CS_LOW();

	retemp[0] = SPI_SCL3300_SendByte((cmd >> 24) & 0xFF);
	retemp[1] = SPI_SCL3300_SendByte((cmd >> 16) & 0xFF);
	retemp[2] = SPI_SCL3300_SendByte((cmd >>  8) & 0xFF);
	retemp[3] = SPI_SCL3300_SendByte((cmd >>  0) & 0xFF);

//	 printf(" retemp[0]=%X, [1]=%X, [2]=%X, [3]=%X \n\r", retemp[0], retemp[1], retemp[2], retemp[3]);

	//Temp = (uint32_t)(retemp[0] << 24) + (uint32_t)(retemp[1] << 16) + (uint32_t)(retemp[2] << 8) + (uint32_t)retemp[3];

	uint16_t angle = (retemp[1] << 8) + retemp[2];  // The second and third bytes are Data

	spi_nss_hi(); // SPI_SCL3300_CS_HIGH();
// 	delay_us(10);  //DE
//	HAL_Delay(1);

    return angle;
}


void SCL3300_Init(void)
{
  spi_nss_hi();

//  delay_ms(1);  //DE
	HAL_Delay(1); //

  SPI_SCL3300_CMD(SW_RESET);
  //delay_ms(1);
	HAL_Delay(1);

  SPI_SCL3300_CMD(CHANGE_TO_MODE4);

 // delay_ms(25);
	HAL_Delay(25);

  SPI_SCL3300_CMD(READ_STATUS_SUMMARY);
  SPI_SCL3300_CMD(READ_STATUS_SUMMARY);
  SPI_SCL3300_CMD(READ_STATUS_SUMMARY);
  SPI_SCL3300_CMD(ENABLE_ANGLE_OUTPUTS);

}

#define ANG_X                   0
#define ANG_Y                   1
#define ANG_Z                   2

float SCL3300_ReadAngle(uint8_t direction)
{
	uint16_t angle =0;

	float    deg_sum=0.0,  deg_avg=0.0;
	int  i= 0, avgSamples=10;

    if(direction == ANG_X)
    {
    	angle  =  SPI_SCL3300_CMD(READ_ANG_X);  //writeRegister
    }
    else if(direction == ANG_Y)
    {
    	angle = SPI_SCL3300_CMD(READ_ANG_Y);
    }
    else if(direction == ANG_Z)
    {
    	angle =  SPI_SCL3300_CMD(READ_ANG_Z);
    }

   // readRegister();
   // SPI devuelve la estructura del primer byte OP + RS,
   // uint16_t angle = (s_readData[1] << 8) + s_readData[2];  // 2nd y 3RD bytes para datos
   // oled_printf(5, 18, &Font_11x18, 1, "fangle=%d",  angle );

   // El 4to byte es CRC   2^14= 16384

     float fangle =  (float)angle  / 16384.0 * 90.0;           //, como 3976/2 ^ 14 * 90 = 21.84 °

		for ( i=0; i<avgSamples; i++)
		{
			deg_sum+= fangle ; 	// HAL_Delay(1);  // 1ms
		}
		deg_avg=  deg_sum / (float)avgSamples;  //
		deg_sum =0.0;

		return deg_avg ;

	// return fangle;


/*
    readRegister();         // SPI OP+RS，
    uint16_t angle = (s_readData[1] << 8) + s_readData[2];  //  CRC
	_angle_x = (double)result[0].data / (1 << 14) * 90.0;
	_angle_y = (double)result[1].data / (1 << 14) * 90.0;
	_angle_z = (double)result[2].data / (1 << 14) * 90.0;

 */

}


/*
uint8_t SPI_ReadOneByte(SPI_ID_t SPIx)
{
	uint8_t r_byte,temp = 0xff;
	SPI_Write(SPIx, &temp, 1);
	while(!SPI_IsTxDone(SPIx));
	SPI_Read(SPIx, &r_byte, 1);
	return r_byte;
}

uint32_t SPI_ReadWriteData(SPI_ID_t SPIx, uint8_t *s_data, uint8_t *r_data, uint32_t length)
{
	uint8_t *r_data_ptr = r_data;
	for(uint32_t i=0; i<length; i++)
	{
		SPI_Write(SPIx, s_data++, 1);
		while(!SPI_IsTxDone(SPIx));
		SPI_Read(SPIx, r_data_ptr++, 1);
	}
	return length;
}
*/

/*
//#define SENDBUFF_SIZE 4
//static u8 retemp[SENDBUFF_SIZE];

//static uint8_t retemp[SENDBUFF_SIZE];

void SPI_SCL3300_SendCmd(uint32_t cmd)  //writeRegister
//void SPI_SCL3300_SendCmd(u32 cmd)
{

}

*/

// https://www.ccsinfo.com/forum/viewtopic.php?t=59437
//  https://www.csdn.net/tags/MtTaEgysMzU4MDc3LWJsb2cO0O0O.html
//uint8_t SPI_ReadWriteData(uint8_t TxData)

/*
uint8_t SPI_ReadWriteData(uint8_t byte, uint8_t reg_val, uint16_t Size )
{

	//(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)

    //uint8_t SPI_SCL3300_SendByte(uint8_t byte)

		uint8_t dat=0;
	//  uint8_t reg_val=0;
		spi_nss_low();
		delay_us(1);

	//  HAL_SPI_Transmit(&hspi2, &byte,    1, 0xFFFF);
	//  HAL_SPI_Receive(&hspi2,  &reg_val, 1, 0xFFFF);

	//  HAL_SPI_TransmitReceive(&hspi2,   &byte,  &reg_val, 1,0xFFFF);
	    HAL_SPI_TransmitReceive(&hspi2, (uint8_t*) &byte, (uint8_t*) &reg_val, 2,0xFFFF);
	  	dat  = reg_val ;//Spi_ReadByte();
		spi_nss_hi();
		delay_us(10);

	// oled_printf(3, 40, &Font_11x18, 1, " B=%3d, D=%3d ", byte, dat );
	// SSD1306_UpdateScreen(); //display
	   return dat;

}

*/


/*  //https://arabicprogrammer.com/article/60562466726/
void SPI_ReadWriteData(uint8 *pWriteData, uint8 *pReadData, uint8 writeDataLen)
{
	s_transferOk = false;

	APP_ERROR_CHECK(nrf_drv_spi_transfer(&s_spiHandle, pWriteData, writeDataLen, pReadData, writeDataLen));

	while(!s_transferOk)
	{
		__WFE();
	}						 			// Error in SPI or transfer already in progress.
}


static void writeRegister(uint32_t cmd)
{
	uint8_t writeData[4]= {0};

	writeData[0] = ((cmd >> 24) & 0xFF);
	writeData[1] = ((cmd >> 16) & 0xFF);
	writeData[2] = ((cmd >>  8) & 0xFF);
	writeData[3] = ((cmd >>  0) & 0xFF);

    spi_nss_low();
    SPI_ReadWriteData(writeData ,  NULL, 4);
    spi_nss_hi();
    HAL_Delay(10);
}


static uint8_t s_readData[4] = {0};
static void readRegister(void)
{
    spi_nss_low();
    SPI_ReadWriteData(NULL, s_readData, 4);
    spi_nss_hi();
    HAL_Delay(10);
}
*/

/*
uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//

     SPI_I2S_SendData(SPI1, TxData); //
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //

     return SPI_I2S_ReceiveData(SPI1); /
}
*/

    //	HAL_StatusTypeDef  state;
    //  while(HAL_SPI_GetState(&hspi2)==HAL_SPI_STATE_BUSY_TX)
    //  {}

    // state =
    //		  HAL_SPI_Transmit(&hspi2,  &byte,1,0xFFFF);

    //   < Return the byte read from the SPI bus
    //  while(HAL_SPI_GetState(&hspi2)==HAL_SPI_STATE_BUSY_RX)
    //  {}

    // state =
    //	 HAL_SPI_Receive(&hspi2,  &reg_val,1,0xFFFF);
    //	return recvdata;
