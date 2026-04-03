/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "fonts.h"
#include "ssd1306.h"
//#include "test.h"
#include "scl3300.h"
#include "sht3x.h"
#include "math.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

CAN_HandleTypeDef hcan;

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/*
int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)ptr, (uint16_t)len, 10);
	return len;
}
*/


/*
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)ch,1, 10);
	return ch;
}
*/
int __io_putchar(int ch)
{
	if(HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 10) != HAL_OK)
		return -1;
	return ch;
}

//uint16_t len;
uint8_t  Rx_indx;
uint8_t  Rx_indx2;
uint8_t  Rx_data;
uint8_t  Rx_data2;
uint8_t  Tx_Flag;
uint8_t  Rx_cmd;
uint8_t  PSU_Control_cmd;
uint8_t  Rx_Buffer[7],EndOfTrans; //9  =>1 axy,    17 => 2 axy
uint8_t  Rx_Buffer2[7];
//uint8_t TmpBuffer[19];
uint8_t  id;
uint8_t  address;
uint32_t ENT_Pressed = 0;
uint16_t UP_Pressed = 0;
uint16_t DOWN_Pressed = 0;
uint32_t ENT_Pressed2 = 0;
uint32_t DSP_Toggle_Cnt = 0;
uint8_t set_mode = 0;
uint8_t DSP_Page = 0;
uint8_t PSU1_LF,PSU2_LF,PSU3_LF,PSU4_LF,PSU1_TF,PSU2_TF,PSU3_TF,PSU4_TF = 0;
float angZ1= 0.0;
float Zero_A,Zero_B,Zero_C,Zero_D;
int Mindata[3]; // 2023-05-10
int16_t xsum1=0, xsum2=0, ysum1=0, ysum2=0 ;
double xsum=0.0, ysum=0.0;
#define ON 1;
#define OFF 0;
#define OCCUR 1;
#define CLEAR 0;
unsigned char txpack[8];

/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)  // EXTERNAL 
{

	///////////////////// final  => delet

	if(huart->Instance == huart2.Instance)   //SOLAR SENSOR
	{
		if(Rx_indx==0 )
   		{
			for ( int i = 0 ; i < 18 ; i++)  //9=> 18
			{ //clear buffer
		      Rx_Buffer[i] = 0;
		    }
        } //printf("%c",Rx_data);
//+001.234, -004.321\r => +001.234 => x axy,  -004.321 => y axy
		Rx_Buffer[Rx_indx++] = Rx_data;

		if(Rx_data == '\r')
		{
			Rx_indx = 0; EndOfTrans= 1;
		}

		if(Rx_indx == 18) // 9=> 17
		{
			 Rx_indx = 0; EndOfTrans= 1;
		}
		HAL_UART_Receive_IT(&huart2, &Rx_data, 1);
	 }

	xsum1 =  ( ( (Rx_Buffer[1]-48) *100) + ( (Rx_Buffer[2]-48) *10) + (Rx_Buffer[3]-48) );
	xsum2 =  ( ( (Rx_Buffer[5]-48) *100) + ( (Rx_Buffer[6]-48) *10) + (Rx_Buffer[7]-48) );

	ysum1 =  ( ( (Rx_Buffer[10]-48) *100)+ ( (Rx_Buffer[11]-48) *10)+ (Rx_Buffer[12]-48) );
	ysum2 =  ( ( (Rx_Buffer[14]-48) *100)+ ( (Rx_Buffer[15]-48) *10)+ (Rx_Buffer[16]-48) );

    if     ( Rx_Buffer[0] == 0x2B ) xsum=  ( (xsum1 *1000.0) + xsum2 ) / 1000.0 ;
    else if( Rx_Buffer[0] == 0x2D ) xsum= -( (xsum1 *1000.0) + xsum2 ) / 1000.0 ;

    if     ( Rx_Buffer[9] == 0x2B ) ysum=  ( (ysum1 *1000.0) + ysum2 ) / 1000.0 ;
    else if( Rx_Buffer[9] == 0x2D ) ysum= -( (ysum1 *1000.0) + ysum2 ) / 1000.0 ;


   ////////////////////////////////   delet   ////////////////////////////////////////////

}
*/
void CheckByte(void)
{
	uint8_t CheckSum;
	switch(Rx_indx)
	{
		case 0 : 
			for ( int i = 0 ; i < 8 ; i++)  //9=> 18
			{ //clear buffer
				Rx_Buffer[i] = 0;
			}
			if(Rx_data == 0x02)
			{
				Rx_Buffer[Rx_indx++] = Rx_data;
				return;
			}
			else 	Rx_indx = 0;  return;
		

		case 1 : 
			if(Rx_data == 0x02)
			{
				Rx_Buffer[Rx_indx++] = Rx_data;
				return;
			}
			else 	Rx_indx = 0; return;		
		

		case 2 : 
			if(Rx_data == 0x01)
			{
				Rx_Buffer[Rx_indx++] = Rx_data;
				return;
			}
			else 	Rx_indx = 0;	 return;
		
		
		case 3 : 
				Rx_Buffer[Rx_indx++] = Rx_data;
				Rx_cmd = Rx_data;
				return;

		case 4 : 
			CheckSum = 0;
			CheckSum = Rx_Buffer[0]^Rx_Buffer[1]^Rx_Buffer[2]^Rx_Buffer[3];
			if(CheckSum == Rx_data)
			{
				Rx_Buffer[Rx_indx++] = Rx_data;
				return;
			}
			else		Rx_indx = 0; return;
			
		case 5 : 
			if(Rx_data == 0xFE)
			{
				Rx_Buffer[Rx_indx++] = Rx_data;
				return;
			}
			else 	Rx_indx = 0; return;
		
		case 6 : 
			if(Rx_data == 0xFE)
			{
				Rx_Buffer[Rx_indx++] = Rx_data;
				Rx_indx = 0;
				Tx_Flag=0x01;
				return;
			}
			else 	Rx_indx = 0; return;

		default:
		
		break;
	}
}

void CheckByte2(void)
{
	uint8_t CheckSum;
	switch(Rx_indx2)
	{
		case 0 : 
			for ( int i = 0 ; i < 8 ; i++)  //9=> 18
			{ //clear buffer
				Rx_Buffer2[i] = 0;
			}
			if(Rx_data2 == 0x02)
			{
				Rx_Buffer2[Rx_indx2++] = Rx_data2;
				return;
			}
			else 	Rx_indx2 = 0; return;

		case 1 : 
			if(Rx_data2 == address)
			{
				Rx_Buffer2[Rx_indx2++] = Rx_data2;
				return;
			}
			else 	Rx_indx2 = 0; return;

		case 2 : 
			Rx_Buffer2[Rx_indx2++] = Rx_data2;
			return;
		
		case 3 : 
			if(Rx_data2 == 0x00)
			{
				Rx_Buffer2[Rx_indx2++] = Rx_data2;
				switch(address)
				{
					case 0x01 : PSU1_LF = CLEAR; break;
					case 0x02 : PSU2_LF = CLEAR; break;
					case 0x03 : PSU3_LF = CLEAR; break;
					case 0x04 : PSU4_LF = CLEAR; break;					
					default :
					break;
				}
				return;
			}
			else
			{
				Rx_Buffer2[Rx_indx2++] = Rx_data2;
				switch(address)
				{
					case 0x01 : PSU1_LF = OCCUR; break;
					case 0x02 : PSU2_LF = OCCUR; break;
					case 0x03 : PSU3_LF = OCCUR; break;
					case 0x04 : PSU4_LF = OCCUR; break;					
					default :
					break;
				}
				return;
			}

		case 4 : 
			CheckSum = 0;
			CheckSum = Rx_Buffer2[0]^Rx_Buffer2[1]^Rx_Buffer2[2]^Rx_Buffer2[3];
			if(CheckSum == Rx_data2)
			{
				Rx_Buffer2[Rx_indx2++] = Rx_data2;
				return;
			}
			else		Rx_indx2 = 0; return;
  
		break;

		case 5 : 
			if(Rx_data2 == 0x03)
			{
				Rx_Buffer2[Rx_indx2++] = Rx_data2;
				Rx_indx2 = 0;				
				return;
			}
			else 	Rx_indx2 = 0; return;
  

		default:
		
		break;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)  // EXTERNAL 
{
	if(huart->Instance == huart2.Instance)   //RX interrupt Msg from WISUN module or SOLAR level developments sensor
	{
		HAL_UART_Receive_IT(&huart2, &Rx_data, 1); // 1Byte rx interrupt 
		CheckByte();	 // Check rx data by 1Byte
	}
	
	if(huart->Instance == huart1.Instance)   //RX interrupt Msg from PSU
	{
		HAL_UART_Receive_IT(&huart1, &Rx_data2, 1);
		CheckByte2();		
	}	
}
/////////////////  can  //////////////////////
CAN_FilterTypeDef canfil; //CAN Bus Filter
CAN_TxHeaderTypeDef   TxHeader;
uint8_t               TxData[8];
uint8_t               TxData2[8];
uint32_t              TxMailbox;

uint16_t CAN_ID;
/*******************************************************************************
* Function Name  : CanWriteData
* Description    : Can Write Date to CAN-BUS
*******************************************************************************/
/*
void CanWriteData(uint16_t CAN_ID)
{
  TxHeader.StdId = CAN_ID;//0x321;                 // Standard Identifier, 0 ~ 0x7FF
  TxHeader.ExtId = 0x01;                  // Extended Identifier, 0 ~ 0x1FFFFFFF
  TxHeader.RTR = CAN_RTR_DATA;            //  DATA or REMOTE
  TxHeader.IDE = CAN_ID_STD;              //   STD or EXT
  TxHeader.DLC = 8;                       // 길이, 0 ~ 8 byte
  TxHeader.TransmitGlobalTime = DISABLE;  //   timestamp counter 값을 capture.
}
*/

void ProcessRxmsg(void)
{
	uint8_t Tx_indx=0;
	char lv0=0,lv2,lv1;
	
	TxData2[Tx_indx++] = 0x02;
	TxData2[Tx_indx++] = 0x02;
	TxData2[Tx_indx++] = address;
	TxData2[Tx_indx++] = Rx_cmd;
	TxData2[Tx_indx++] = (PSU1_TF<<7)+(PSU2_TF<<6)+(PSU3_TF<<5)+(PSU4_TF<<4)
	                                  +(PSU1_LF<<3)+(PSU2_LF<<2)+(PSU3_LF<<1)+(PSU4_LF<<0);
	lv1 = 0;
	for(lv2=0;lv2<Tx_indx;lv2++) lv1 = TxData2[lv2]^lv1;
	
	TxData2[Tx_indx++] = lv1;
	TxData2[Tx_indx++] = 0xFE;
	TxData2[Tx_indx++] = 0xFE;	
	//if(Rx_cmd == 0xFF) set_mode =0x01 ;
}

void TxMsg_PSU(void)
{
	uint8_t ch, i, j,k;
	
	txpack[0] = 0x02;  //STX
	txpack[1] = address;
	if(PSU1_LF | PSU2_LF | PSU3_LF | PSU4_LF | PSU1_TF | PSU2_TF  | PSU3_TF  | PSU4_TF) txpack[2] = 0x00;
	else 	txpack[2] = Rx_cmd;	
	txpack[3] =  PSU1_TF | PSU2_TF  | PSU3_TF  | PSU4_TF | PSU1_LF | PSU2_LF  | PSU3_LF  | PSU4_LF;
	k=0;
	for(j=0;j<4;j++) k = txpack[j]^k;
	txpack[4] = k; //CRC
	txpack[5] = 0x03; //ETX
	for (i =0; i <6 ; i++)     // LED PAPI PSU
	{
		ch = txpack[i];
		HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 10);
	}
}

void TxMsg_Tilt(void)
{
	TxHeader.StdId = address;//CAN_ID;//0x321;   // Standard Identifier, 0 ~ 0x7FF
	TxHeader.ExtId = 0x01;                  // Extended Identifier, 0 ~ 0x1FFFFFFF
	TxHeader.RTR = CAN_RTR_DATA;            //  DATA or REMOTE
	TxHeader.IDE = CAN_ID_STD;              //   STD or EXT
	TxHeader.DLC = 8;                       // 길이, 0 ~ 8 byte
	TxHeader.TransmitGlobalTime = DISABLE;  //   timestamp counter 값을 capture.
	// TX[1]~ TX[4] => tilt, temp ...etc
	TxData[0] = 0x24;  // etc PAPI PSU Sensor
	if(PSU1_LF | PSU2_LF | PSU3_LF | PSU4_LF | PSU1_TF | PSU2_TF  | PSU3_TF  | PSU4_TF) TxData[1] = 0x00;
	else 	TxData[1] = Rx_cmd;	// cmd PAPI PSU Step from WISUN device
	TxData[2] = PSU1_LF | PSU2_LF | PSU3_LF | PSU4_LF; //Lamp Fault_err;
	TxData[3] = 0x0;
	TxData[4] = 0x0;
	TxData[5] = address;
	TxData[6] = PSU1_TF | PSU2_TF | PSU3_TF | PSU4_TF; //tilt_err;
	TxData[7] = 0x23;
	HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);

}

//uint8_t  id=0;
static uint8_t  tilt_state=0 , can_rx=0 ;//;//id,ch=0;
CAN_RxHeaderTypeDef RxHeader; //CAN Bus  Header
uint8_t             RxData[8];

//uint8_t  RxData[8];
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *CanHandle)
{
	uint8_t ch, i;
	//printf("%s\r\n", __FUNCTION__);
	// Get RX message
	if (HAL_CAN_GetRxMessage(CanHandle, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
	{
		/* Reception Error */
		Error_Handler();
	}
	if(address == 0x01)
	{
		switch(RxData[5])
		{
	 		case 0x02 : 
				if(RxData[6] == 0x01) {PSU2_TF = OCCUR;}
				else  {PSU2_TF = CLEAR;}
				
				if(RxData[2] == 0x01) {PSU2_LF = OCCUR;}
				else  {PSU2_LF = CLEAR;}
			break;
			
	 		case 0x03 : 
				if(RxData[6] == 0x01) {PSU3_TF = OCCUR;}
				else  {PSU3_TF = CLEAR;}
				
				if(RxData[2] == 0x01) {PSU3_LF = OCCUR;}
				else  {PSU3_LF = CLEAR;}
			break;		
			
	 		case 0x04 : 
				if(RxData[6] == 0x01) {PSU4_TF = OCCUR;}
				else  {PSU4_TF = CLEAR;}
				
				if(RxData[2] == 0x01) {PSU4_LF = OCCUR;}
				else  {PSU4_LF = CLEAR;}
			break;		

			default:
	
			break;
		}
	}
	else // PSU Slave get CMD from PSU Master Through 
	{
		switch(RxData[5])
		{
	 		case 0x01 : 
				if(RxData[6] == 0x01) {PSU1_TF = OCCUR;}
				else  {PSU1_TF = CLEAR;}
				
				if(RxData[2] == 0x01) {PSU2_LF = OCCUR;}
				else  {PSU2_LF = CLEAR;}
			break;
		
	 		case 0x02 : 
				if(RxData[6] == 0x01) {PSU2_TF = OCCUR;}
				else  {PSU2_TF = CLEAR;}
				
				if(RxData[2] == 0x01) {PSU2_LF = OCCUR;}
				else  {PSU2_LF = CLEAR;}
			break;
			
	 		case 0x03 : 
				if(RxData[6] == 0x01) {PSU3_TF = OCCUR;}
				else  {PSU3_TF = CLEAR;}
				
				if(RxData[2] == 0x01) {PSU3_LF = OCCUR;}
				else  {PSU3_LF = CLEAR;}
			break;		
			
	 		case 0x04 : 
				if(RxData[6] == 0x01) {PSU4_TF = OCCUR;}
				else  {PSU4_TF = CLEAR;}
				
				if(RxData[2] == 0x01) {PSU4_LF = OCCUR;}
				else  {PSU4_LF = CLEAR;}
			break;		

			default:
	
			break;
		}
	
		Rx_cmd = RxData[1];
	}
	
	// StdId  = id ;
	/*
	RxData[0]= '$';
	RxData[5]= id ;
	RxData[6]= tilt_state;
	RxData[7]= '#';
	*/
	//	  printf("StdID: %04lx, IDE: %ld, DLC: %ld\r\n", RxHeader.StdId, RxHeader.IDE, RxHeader.DLC);
	//  printf("Data: %X %X %X %X %X %X %X %X\r\n", RxData[0], RxData[1], RxData[2], RxData[3], RxData[4], RxData[5], RxData[6], RxData[7]);
	/*if (tilt_state ==0)
	{
		for (i =0; i <8 ; i++)
		{
			ch = RxData[i];
			//		HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 10);
			// printf("%X ",ch);
		}
		if( RxData[6] == 1) can_rx=1;//   tilt_state = 1;
		else         		can_rx=0;// tilt_state = 0;
	}*/
	//	else   can_rx=0;
}

/////////////////  can end    ///////////////////////////
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C2_Init(void);
static void MX_CAN_Init(void);
static void MX_ADC2_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

extern void oled_printf(uint16_t x, uint16_t y, FontDef_t* Font, uint8_t color, const char *fmt,...);

extern void SCL3300_Init(void);

extern float SCL3300_ReadAngle(uint8_t direction);

#define ANG_X                   0
#define ANG_Y                   1
#define ANG_Z                   2

uint32_t  Flash_Add = 0x0801FC00 ;//

void flash_save(int data) //int data)
{
	  uint32_t PageError = 0;

	  FLASH_EraseInitTypeDef My_Flash;  //
	  HAL_FLASH_Unlock();               // h

	  My_Flash.TypeErase = FLASH_TYPEERASE_PAGES;  //
	  My_Flash.PageAddress = Flash_Add ; //Flash_Add ;  //
	  My_Flash.NbPages = 1;

	  PageError = 0;                    //
	  HAL_FLASHEx_Erase(&My_Flash, &PageError);  //
	  HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,Flash_Add, data  );//Write_Flash_Data);
//	  HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,Flash_Add, data  );//Write_Flash_Data);	  

	  HAL_FLASH_Lock(); // Flash
	  data = * (__IO uint16_t*)Flash_Add;
}

////////////////

//I2C_HandleTypeDef hi2c2;   vcc scl sda gnd
#define add_w 0x80 // D address writing
#define add_r 0x81 // address read
#define measure_temp 0xe3 // read temperature
#define measure_hum 0xe5 // read humidity

//////////////////////
////  adc  //////

uint16_t value1=0 , value2=0;
float  vout=0.0;
float PAPI_temp=0.0;
//////

/*
// 100ms마다 ADC결과 ?��?��
void adc_task()
{
    static uint32_t time = 0;
    uint16_t value;

    if (HAL_GetTick() - time < 100) {
    	return;
    }

    time = HAL_GetTick();

    // ADC ?��?��
    HAL_ADC_Start(&hadc1);

    // ADC�????????????????? ?��료될 ?��까�? ??�?????????????????
    while(HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK);

    // ADC �????????????????? �??????????????????��?���?????????????????
    value = HAL_ADC_GetValue(&hadc1);

    // ADC 중�?
    HAL_ADC_Stop(&hadc1);

  //  printf("%d\r\n", value);
}
*/
//Read the temperature function as follows

#define	Bit0				1
#define	Bit1				2

//#define	SET			0

uint8_t sw_id(void)
{ // 1= 01111 2=1101 3=0101 4=1011
  	 uint8_t  idbf =0;
	 if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_14) == 0) idbf |= Bit0;//1
	 if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_15) == 0) idbf |= Bit1;//2

	 idbf  =  idbf+1;
	 return  (idbf & 0x0f);
}

float       tilt_x1=0.0;//,      tilt_x2=0 .0;
int         tilt_flash =100;// , tilt_flash2 =100  ;
//unsigned int  Flash_data=100,     Flash_data2=100;
void auto_zero(void)
{

/*	if ((angZ1 < 0.009)&&(angZ1 > -0.009)) 
	{
		flash_save(tilt_flash);
		//tilt_flash = * (__IO uint16_t*)Flash_Add ;
		//printf("Key up(B4)   => flash Save \r\n"  );		
		set_mode = 0;
	}
	else if ((angZ1 > -1)&&( angZ1 <= -0.009))
	{
		tilt_flash = tilt_flash+1;
	}
	
	else if( angZ1 < -1)
	{
		tilt_flash = tilt_flash+10;
	}

	else if( angZ1 > 1)
	{
		tilt_flash = tilt_flash-10;
	}
	else if(( angZ1 < 1)&&(  angZ1 >= 0.009))
	{
		tilt_flash = tilt_flash-1;
	}
	*/
	/* Unit A(2.5000)     A 2.509 B 2.491  C 2.40  D 2.6 */
	/* Unit B(2.8300)     A 2.839 B 2.821  C 2.73  D 2.93 */
	/* Unit C(3.1700)     A 3.179 B 3.161  C 3.07  D 3.27 */
	/* Unit D(3.5000)     A 3.509 B 2.491  C 3.40  D 3.7 */

	if ((angZ1 < Zero_A)&&(angZ1 > Zero_B))  // A 2.509 B 2.491  C 2.4  D 2.6 
	{
		flash_save(tilt_flash);
		//tilt_flash = * (__IO uint16_t*)Flash_Add ;
		//printf("Key up(B4)   => flash Save \r\n"  );		
		set_mode = 0;
	}
	else if ((angZ1 > Zero_C)&&( angZ1 <= Zero_B)) // C 2.4  B 2.491
	{
		tilt_flash = tilt_flash+1;
	}
	
	else if( angZ1 < Zero_B) // B 2.4
	{
		tilt_flash = tilt_flash+10;
	}

	else if( angZ1 > Zero_D) // D 2.6
	{
		tilt_flash = tilt_flash-10;
	}
	else if(( angZ1 < Zero_D)&&(  angZ1 >= Zero_A)) // D 2.6   A 2.509
	{
		tilt_flash = tilt_flash-1;
	}	
	if     ( tilt_flash >= 100 ) { tilt_x1=  ( tilt_flash-100 ) *0.01 ; }
	else if( tilt_flash < 100  ) { tilt_x1= -( 100- tilt_flash) *0.01 ; }

	

}

void key_scan(void)
{
	GPIO_PinState ENT_pin = HAL_GPIO_ReadPin(ENT_GPIO_Port, ENT_Pin);
//	GPIO_PinState pin_up  = HAL_GPIO_ReadPin(up_GPIO_Port, up_Pin);
//	GPIO_PinState pin_dn  = HAL_GPIO_ReadPin(dn_GPIO_Port, dn_Pin);	
	if( ( ENT_pin == GPIO_PIN_RESET) && (set_mode == 0) )
	{
		ENT_Pressed ++;
		if(ENT_Pressed > 500000)
		{
			set_mode = 1;
			ENT_Pressed = 0;
		}
	}
	else ENT_Pressed = 0;

//	if( pin_up == GPIO_PIN_RESET)
//	{
//		UP_Pressed++;
//		if(UP_Pressed > 1)
//		{
//		DSP_Page++;
//		UP_Pressed = 0;
//		if(DSP_Page == 2) DSP_Page = 0;
//		}
//	}
//	else UP_Pressed = 0;
}
void key(void)
{
///////////////////////////////////////////
	GPIO_PinState pin_up  = HAL_GPIO_ReadPin(up_GPIO_Port, up_Pin);
	GPIO_PinState pin_dn  = HAL_GPIO_ReadPin(dn_GPIO_Port, dn_Pin);
	GPIO_PinState pin_ENT = HAL_GPIO_ReadPin(ENT_GPIO_Port, ENT_Pin);

	if( pin_ENT == GPIO_PIN_RESET)
	{
		ENT_Pressed2 ++;
		if(ENT_Pressed2 >40)
		{
			flash_save(tilt_flash);
			tilt_flash = * (__IO uint16_t*)Flash_Add ;
			printf("Key up(B4)   => flash Save \r\n"  );
			ENT_Pressed2 = 0;
			set_mode = 0;
		}
	}
	else ENT_Pressed2 = 0;

	if( pin_up == GPIO_PIN_RESET)
	{
		if( 300 < tilt_flash)
		{
			tilt_flash= 100;
			// flash_save(100);
		}
		else
		{
			tilt_flash = tilt_flash+1;
			// flash_save(tilt_flash);
		}
 		printf("Key up(B9) => + 0.02,  tilt_flash =%d \r\n",  tilt_flash  );
	}

	if( pin_dn == GPIO_PIN_RESET) //&& (pin_ENT == GPIO_PIN_SET))
	{
		if( 300 < tilt_flash)
		{
			tilt_flash= 100;
				//flash_save(100);
		}
		else
		{
			tilt_flash = tilt_flash-1;
				// flash_save(tilt_flash);
		}
 		printf("Key dn(B8) => - 0.02,  tilt_flash =%d \r\n", tilt_flash );
	}

	if     ( tilt_flash >= 100 ) { tilt_x1=  ( tilt_flash-100 ) *0.02 ; }
	else if( tilt_flash < 100  ) { tilt_x1= -( 100- tilt_flash) *0.02 ; }

	//	 tilt_x1=  ( tilt_flash-100 ) *0.02 ;

}

float angX= 0.0, angY= 0.0, angZ= 0.0,   angY1= 0.0;
//float angX= 0.0, angY= 0.0, angZ= 0.0,   angY1= 0.0, angZ1= 0.0;

void id_angle_preset(void)
{
	/* Unit A(2.5000)     A 2.509 B 2.491  C 2.40  D 2.6 */
	/* Unit B(2.8300)     A 2.839 B 2.821  C 2.73  D 2.93 */
	/* Unit C(3.1700)     A 3.179 B 3.161  C 3.07  D 3.27 */
	/* Unit D(3.5000)     A 3.509 B 3.491  C 3.40  D 3.6 */

	id= sw_id();
	address = id;
	switch(address)
	{
		case 0x01 :  
		{
			Zero_A = 2.509;
			Zero_B = 2.491;				
			Zero_C = 2.4;
			Zero_D = 2.6;				
		}
		break;
		
		case 0x02 :  
			Zero_A = 2.839;
			Zero_B = 2.821;				
			Zero_C = 2.73;
			Zero_D = 2.93;			
			
		break;
		
		case 0x03 : 
			Zero_A = 3.179;
			Zero_B = 3.161;				
			Zero_C = 3.07;
			Zero_D = 3.27;			
			
		break;
		
		case 0x04 :  
			Zero_A = 3.509;
			Zero_B = 3.491;				
			Zero_C = 3.4;
			Zero_D = 3.6	;			
			
		break;
		
		default :
			
		break;
	}
}
void ang_dect(unsigned char id_no)
{
	static 	int err_cnt=0;
	float 	 ang1,ang2;
	//PAPI 4EA setting
	if (address==1)       {	 ang1=2.25, ang2=3.0; }
	else if (address==2)	{	 ang1=2.58, ang2=3.33; }
	else if (address==3)  {	 ang1=2.92, ang2=3.67; }
	else if (address==4)	{	 ang1=3.25, ang2=4.0; }

	//if( (angZ1 < ang1 )|| (ang2 < angZ1) ) //1.8  2.7   //error
	//if( (fabs(angZ1) < ang1 )|| (ang2 < fabs(angZ1)) )  //  1.8  2.7   //error
	if( (angZ1 < ang1 ) || (ang2 < angZ1) )  //  1.8  2.7   //error	
	{
		err_cnt++;
		//printf("1.  err_cnt  =%d  \r\n",err_cnt);
		if(err_cnt >= 5 )  // ERROR
		{
			tilt_state=1;  //ERROR
			switch(address)
			{
				case 0x01 : 
					PSU1_TF = OCCUR;
				break;
				case 0x02 : 
					PSU2_TF = OCCUR;					
				break;
				case 0x03 : 
					PSU3_TF = OCCUR;					
				break;				
				case 0x04 : 
					PSU4_TF = OCCUR;					
				break;
				default : 
				break;				
			}	
			//printf(" 2. err_cnt  =%d  \r\n",err_cnt);
			err_cnt =0;
		}     // err_cnt
	}
	else
	{
		err_cnt =0;
		//	if( RxData[6] == 1) tilt_state = 1;
		//else
		tilt_state=0;
		switch(address)
		{
			case 0x01 : 
				PSU1_TF = CLEAR;
			break;
			case 0x02 : 
				PSU2_TF = CLEAR;					
			break;
			case 0x03 : 
				PSU3_TF = CLEAR;					
			break;				
			case 0x04 : 
				PSU4_TF = CLEAR;					
			break;
			default : 
			break;				
		}			
	}

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim)
{
	uint8_t timer_cnt;

	timer_cnt++;
	if(timer_cnt  > 1)
	{
		//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
		
		angX = SCL3300_ReadAngle(ANG_X);
		angY = SCL3300_ReadAngle(ANG_Y); //mode4  =>y =>good
		angZ = SCL3300_ReadAngle(ANG_Z);

/*		if( ( angZ >= 0) && (angZ < 270.0)) angZ1= (angZ+(tilt_x1));
		else if(angZ > 270.0)               angZ1= (tilt_x1)-(360-angZ);  // - deg data 
		if( ( angZ >= 0) && (angZ < 270.0)) angZ1= -((angZ-(tilt_x1)));
		else if(angZ > 270.0)               angZ1= (tilt_x1)+(360-angZ);  // - deg data */
		if( ( angY >= 0) && (angY < 270.0)) angZ1= (angY+(tilt_x1));
		else if(angY > 270.0)               angZ1= ((tilt_x1)-(360-angY));  // - deg data 
		Mindata[0] = angZ1;
		Mindata[2] = (angZ1*100);
		Mindata[1] = ((Mindata[2]) % 100)*6/10;
		timer_cnt = 0;
	}
				
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	GPIO_PinState pin_up  = HAL_GPIO_ReadPin(up_GPIO_Port, up_Pin);
	GPIO_PinState pin_dn  = HAL_GPIO_ReadPin(dn_GPIO_Port, dn_Pin);	
	if( pin_up == GPIO_PIN_RESET)
	{
		DSP_Page++;
		if(DSP_Page >1)DSP_Page=0;
	}
	else if( pin_dn == GPIO_PIN_RESET)
	{
		DSP_Page++;
		if(DSP_Page >1)DSP_Page=0;
	}
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	uint32_t test_cnt1=0;
	uint32_t test_cnt2=0;
	uint32_t mode_cnt=0;	
	uint16_t cnt_bit=0;
	uint16_t seq_cnt=0;
	uint8_t ch, i , can_process=0;



/*
	 unsigned char txpack[4];
	 txpack[0] ='$'; //0x24 ;//'$'
	 txpack[4] ='#'; ///0x23 ;//''#';;
*/
  //   int MS_pin=0;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();
  MX_CAN_Init();
  MX_ADC2_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(GPIOB, RST_Pin,GPIO_PIN_SET);
  HAL_Delay(100);  
  HAL_GPIO_WritePin(GPIOB, RST_Pin,GPIO_PIN_RESET);
  HAL_Delay(100);  
  HAL_GPIO_WritePin(GPIOB, RST_Pin,GPIO_PIN_SET);
  HAL_Delay(100);    
  SSD1306_Init();  // initialise
  id_angle_preset();
//  oled_printf(5,  0, &Font_11x18, 1, "PAPI Tilt "  );
//  SSD1306_UpdateScreen(); //display

  //////////////// Sol  Tilt  input  /////////////
  HAL_UART_Receive_IT(&huart2, &Rx_data,1); //
  HAL_UART_Receive_IT(&huart1, &Rx_data2, 1);
  
  //printf(" ***  LED PAPI Tilt Test.\n\r");  //uart1
  ///////////// Init    Tilt deg up or down  Save   ////////
  tilt_flash  = *(__IO uint16_t*)Flash_Add;
  HAL_Delay(100);
	if (tilt_flash > 32768) tilt_flash = -(65535-tilt_flash);

   if     ( tilt_flash >= 100 ) { tilt_x1=  ( tilt_flash-100 ) *0.01 ; }
   else if( tilt_flash <= 100 ) { tilt_x1= -( 100- tilt_flash) *0.01 ; }

//	 tilt_x1=  ( tilt_flash-100 ) *0.02 ;


  //printf("\n\r  LED PAPI Tilt  Flash_data  = %d , tilt_x1 = %2.2f \n\r", tilt_flash, tilt_x1  );

//////////////  Tilt  Init  //////////////
 SCL3300_Init();

 ////////////    temp   ///////////////
 // Create the handle for the sensor.
 sht3x_handle_t handle = {
     .i2c_handle = &hi2c2,
    .device_address = SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_LOW
 };

 // Initialise sensor (tests connection by reading the status register).
 if (!sht3x_init(&handle)) {
    // printf("SHT3x access failed.\n\r");
 //    oled_printf(3,  0, &Font_11x18, 1, "SHT3x err "  );
 //    SSD1306_UpdateScreen(); //display
 }

 // Read temperature and humidity.
 float temperature, humidity;

 sht3x_read_temperature_and_humidity(&handle, &temperature, &humidity);
//	 printf("Initial temperature: %.2fC, humidity: %.2f%%RH\n\r", temperature, humidity);

 // Enable heater for two seconds.
 sht3x_set_header_enable(&handle, true);
 HAL_Delay(20);
 sht3x_set_header_enable(&handle, false);

 // Read temperature and humidity again.
 sht3x_read_temperature_and_humidity(&handle, &temperature, &humidity);

// printf("After heating temperature: %.2fC, humidity: %.2f%%RH\n\r", temperature, humidity);

 //////////////////////////////////////////////////
////////////////////  adc  temp   ///////////////
 while(!(HAL_ADCEx_Calibration_Start(&hadc1)==HAL_OK));
/////////////////////////////////////////////////

 ///////////////  can   /////////////////
 canfil.FilterBank = 0;
 canfil.FilterMode = CAN_FILTERMODE_IDMASK;
 canfil.FilterFIFOAssignment = CAN_RX_FIFO0;
 canfil.FilterIdHigh = 0;
 canfil.FilterIdLow = 0;
 canfil.FilterMaskIdHigh = 0;
 canfil.FilterMaskIdLow = 0;
 canfil.FilterScale = CAN_FILTERSCALE_32BIT;
 canfil.FilterActivation = ENABLE;
 canfil.SlaveStartFilterBank = 14;

 TxHeader.StdId = address;//0x202;            // Standard Identifier, 0 ~ 0x7FF
 TxHeader.ExtId = 0x01;                  // Extended Identifier, 0 ~ 0x1FFFFFFF
 TxHeader.RTR = CAN_RTR_DATA;            //  DATA or REMOTE
 TxHeader.IDE = CAN_ID_STD;              //   STD or EXT
 TxHeader.DLC = 8;                       // 길이, 0 ~ 8 byte
 TxHeader.TransmitGlobalTime = DISABLE;  //   timestamp counter 값을 capture.

 HAL_CAN_ConfigFilter(&hcan ,&canfil);
 HAL_CAN_Start(&hcan );
 HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING);
 HAL_TIM_Base_Start_IT(&htim2); // yhlee 2023.02.02

///////////////  can   /////////////////

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//////

	key_scan();
	if(Tx_Flag)
	{
//		PSU1_TF=PSU2_LF=OCCUR;
		ProcessRxmsg(); // Tx to WISUN Module
		for (i =0; i <8 ; i++)
		{
			ch = TxData2[i];
			HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 10);
		}
		Tx_Flag=0;
	}

	if(set_mode)
	{
//		key();
		auto_zero();
//		oled_printf(2,  5, &Font_16x26, 1, "%2.2f, %d ", xsum, id );
//		if(angZ1>0)	oled_printf(  5,  5, &Font_16x26, 1,"A: %2.2f ", angZ1);
//		oled_printf(5, 35, &Font_16x26, 1, "%d     ",tilt_flash );		
//		SSD1306_UpdateScreen(); //display
		
//		else 	oled_printf(  5,  5, &Font_16x26, 1,"A:%2.2f ", angZ1);
//		oled_printf(  5,  5, &Font_16x26, 1,"ANGLE ");
//		oled_printf(5, 35, &Font_16x26, 1, "SET > 0");

		DSP_Toggle_Cnt++;
		if(DSP_Toggle_Cnt == 5)
		{
			oled_printf(  5,  5, &Font_16x26, 1,"ANGLE  ");
			oled_printf(5, 35, &Font_16x26, 1, "SET   0");
		}
		else if(DSP_Toggle_Cnt == 10)
		{
			oled_printf(  5,  5, &Font_16x26, 1,"ANGLE  ");
			oled_printf(5, 35, &Font_16x26, 1, "SET>  0");
		}

		else if(DSP_Toggle_Cnt == 15)
		{
			oled_printf(  5,  5, &Font_16x26, 1,"ANGLE  ");
			oled_printf(5, 35, &Font_16x26, 1, "SET > 0");
			SSD1306_UpdateScreen(); //display			
		}		
		else if(DSP_Toggle_Cnt == 20)
		{
			oled_printf(  5,  5, &Font_16x26, 1,"ANGLE  ");
			oled_printf(5, 35, &Font_16x26, 1, "SET  >0");
			DSP_Toggle_Cnt = 0;
		}		
		SSD1306_UpdateScreen(); //display					
			
	}

	else
	{
		test_cnt1++;
		if(test_cnt1  > 10)//100 ->10 yhlee 2023.01.31
		{
			test_cnt1 = 0;
			test_cnt2++;
			
			if(test_cnt2 > 2500) //5
			{
				test_cnt2 = 0;
				cnt_bit ^=1 & 0x01;

				seq_cnt++;
				if( seq_cnt >  5)  seq_cnt = 0;

				if( seq_cnt == 0)
				{
					//////////////////////////////////
					sht3x_set_header_enable(&handle, true);
					//	HAL_Delay(1);//2000
					sht3x_set_header_enable(&handle, false);
					sht3x_read_temperature_and_humidity(&handle, &temperature, &humidity);

					/////////////////  기존  /////////////////
					/*
					if( ( angZ >= 0) && (angZ < 270.0)) angZ1= angZ+(2.222+tilt_x1);
					else if(angZ > 270.0)               angZ1= (2.222+tilt_x1)-(360-angZ);  // - deg data
					*/
	/*				angX = SCL3300_ReadAngle(ANG_X);
					angY = SCL3300_ReadAngle(ANG_Y); //mode4  =>y =>good
					angZ = SCL3300_ReadAngle(ANG_Z);

					if( ( angZ >= 0) && (angZ < 270.0)) angZ1= angZ+(tilt_x1);
					else if(angZ > 270.0)               angZ1= (tilt_x1)-(360-angZ);  // - deg data 
	/*				

					//355.86  2.45

					//printf(" \r\n SCL3300_ReadAngle = %3.3f,    angZ= %3.3f, angZ1=  %3.3f \r\n", SCL3300_ReadAngle(ANG_Z),  angZ, angZ1 );

					////////////////////////////////////////////
					/*
					// xx  test xxxx ////////////////////
					//  if( ( angX >= 0) && (angX < 270.0))
					//  {
					//	  angX = angX -0.445;// angz1= angZ+2.338 ;
					//      printf("\r\n 1 xxx  X0= %3.3f, X0= %3.3f \r\n",   angX, ( 90- xsum )    );
					//  }
					//  else if(angX > 270.0)
					//  {
					//	  angX = (360- angX )-1.2 ; // angZ1= 2.338 - (360-angZ);
					//	  printf(" 2 xxx X1= %3.3f, X1= %3.3f \r\n", angX,  ( 90- xsum )   );
					//  }
					// *****y    *********************
					printf(" \r\n     sol X1= %3.3f,     sol Y1= %3.3f,  \r\n", xsum, ysum    );
					printf("\r\n ***** yyyyyyyy ***********************\r\n ");
					if( ( angY >= 0) && (angY < 270.0))
					{
					angY1= angY+1.595;
					printf(" 3 YYY  Z0= %3.3f, Y0= %3.3f, f=> Y1= %3.3f \r\n",    angZ+2.21 , angY, angY+1.43);
					}
					else if(angY > 270.0)
					{
					angY1=(360-angY) - 1.595 ;
					printf(" 4 yyy  Z0= %3.3f, Y0= %3.3f, f=> Y1= %3.3f   \r\n",  2.21 - (360-angZ), angY,  1.33 - (360-angY)  );
					}
					printf(" **************************** ");
					*/
					//			printf(" Y= %3.3f, Z= %3.3f, S_X= %3.3f, S_Y= %3.3f\r\n", angY, angZ, xsum , ysum   );
					//			printf("*flash= %d, %d, tilt_x1= %2.3f, %2.3f** Z1x= %3.3f, Y1y= %3.3f\r\n",Flash_data,Flash_data2, tilt_x1,tilt_x2,  angZ1, angY1);
					//           printf(" ----------------------------------------------------------------------- \n\r" );
				}


/*				if( seq_cnt == 1)
				{
					// setting (deg => flash save ) or  normal
					GPIO_PinState MS_pin = HAL_GPIO_ReadPin(MS_GPIO_Port, MS_Pin);
					if(  MS_pin   == GPIO_PIN_RESET) //  pin_MS =0  flas save
					{
						key();
						///////////
						oled_printf(2,  5, &Font_16x26, 1, "%2.2f, %d ", xsum, id );

						if(0<= angZ1)
						oled_printf(2, 35, &Font_16x26, 1, "%2.2f,%d " , angZ1, tilt_flash  );
						else    oled_printf(2, 35, &Font_16x26, 1, "%2.2f,%d " , fabs(angZ1), tilt_flash  );
						//   oled_printf(78,  5, &Font_16x26, 1, " %d ", tilt_flash );
					}
					else
					{
						//	if( ( angZ >= 0) && (angZ < 270.0)) angZ1= angZ+(2.222+tilt_x1+tilt_x2);
						//	else if(angZ > 270.0)  angZ1= (2.222+tilt_x1+tilt_x2)-(360-angZ);
						if( (tilt_state== 0 && can_rx== 1)||(tilt_state== 1 && can_rx== 0) ||(tilt_state== 1 && can_rx== 1) )
						{
							SSD1306_InvertDisplay(1); //SSD1306_ToggleInvert();
							if(angZ1>0)	oled_printf(  5,  5, &Font_16x26, 1,"A: %2.2f ", angZ1);
							else 	oled_printf(  5,  5, &Font_16x26, 1,"A:%2.2f ", angZ1);
	//						oled_printf(  5,  5, &Font_16x26, 1,"%2.2f, %0d ", fabs(angZ1), id );						
						}
						else // led on   if(tilt_state== 0)
						{
							can_process=0;
							SSD1306_InvertDisplay(0); //SSD1306_ToggleInvert();
							if(angZ1>0)	oled_printf(  5,  5, &Font_16x26, 1,"A: %2.2f ", angZ1);
							else 	oled_printf(  5,  5, &Font_16x26, 1,"A:%2.2f ", angZ1);
	//						oled_printf(  5,  5, &Font_16x26, 1,"%2.2f, %0d ", fabs(angZ1), id );						
						}
						if ( -40 > temperature  || 0.0== humidity   ) //sht31 xxx =0 display
						{
							oled_printf( 5, 35, &Font_16x26, 1," x ,  x ", temperature-4, humidity);
						}
						else  //sht31  ok
						oled_printf(  5, 35, &Font_16x26, 1,"T%2.0f,H%2.0f", temperature-4, humidity);
						// 		printf("\r\n temperature=  %2.2f, humidity =  %2.2f  \r\n", temperature, humidity );
					}
					SSD1306_UpdateScreen(); //display
					ang_dect(id);  //  ang setting =>LED ON/OFF
					//	if( TxData[6] == 1) tilt_state = 0; //tilt_err;


					// printf("\r\n angX= %3.3f, angY= %3.3f, angZ= %3.3f, angZ1= %3.3f  \r\n",  angX, angY, angZ,  angZ1 );
					//  EXAMPLE
					//id=0 => 1.75  2.25  2.75  (1.8  2.25   2.7)
					//id=1 => 2.25  2.75  3.25  (2.3   2.75  3.2)
					//id=2 => 2.75  3.25  3.75  5(2.8  3.25  3.7)
					//id=3 => 3.25  3.75  4.24  (3.3  3.75  4.2)
				}*/
				if( seq_cnt == 1)
				{
					if( (tilt_state== 0 && can_rx== 1)||(tilt_state== 1 && can_rx== 0) ||(tilt_state== 1 && can_rx== 1) )
					{
						if(DSP_Page == 0)
						{
							SSD1306_InvertDisplay(0); //SSD1306_ToggleInvert();
							if(angZ1>=0) oled_printf(  5,  5, &Font_16x26, 1,"<:%d!%d' ", Mindata[0],Mindata[1]);
							else 	oled_printf(  5,  5, &Font_16x26, 1,"<:-%d!%d'", abs(Mindata[0]),abs(Mindata[1]));
							switch(address)
							{
								case 0x01 : oled_printf(  5, 35, &Font_16x26, 1,"UNIT: A"); break;
								case 0x02 : oled_printf(  5, 35, &Font_16x26, 1,"UNIT: B"); break;
								case 0x03 : oled_printf(  5, 35, &Font_16x26, 1,"UNIT: C"); break;
								case 0x04 : oled_printf(  5, 35, &Font_16x26, 1,"UNIT: D"); break;					
								default :
								break;
							}
							/*if(angZ1>0)	oled_printf(  5,  5, &Font_16x26, 1,"A: %2.2f ", angZ1);
							else 	oled_printf(  5,  5, &Font_16x26, 1,"A:%2.2f ", angZ1);
							oled_printf(  5, 35, &Font_16x26, 1,"ADDR: %d",address);*/
							//SSD1306_InvertDisplay(1); //SSD1306_ToggleInvert();
							//oled_printf(  5, 5, &Font_32x52, 1,"%d",address);

						}
						else
						{
							SSD1306_InvertDisplay(0); //SSD1306_ToggleInvert();
							oled_printf(  5, 5, &Font_16x26, 1,"TEMP:%2.0f", temperature-4);
							oled_printf(  5, 35, &Font_16x26, 1,"HUMI:%2.0f", humidity);							
						}
					SSD1306_UpdateScreen(); //display
					}
					else // led on   if(tilt_state== 0)
					{
						if(DSP_Page == 0)
						{
							can_process=0;
							SSD1306_InvertDisplay(0); //SSD1306_ToggleInvert();
							if(angZ1>=0) oled_printf(  5,  5, &Font_16x26, 1,"<:%d!%d' ", Mindata[0],Mindata[1]);
							else 	oled_printf(  5,  5, &Font_16x26, 1,"<:-%d!%d'", abs(Mindata[0]),abs(Mindata[1]));
							switch(address)
							{
								case 0x01 : oled_printf(  5, 35, &Font_16x26, 1,"UNIT: A"); break;
								case 0x02 : oled_printf(  5, 35, &Font_16x26, 1,"UNIT: B"); break;
								case 0x03 : oled_printf(  5, 35, &Font_16x26, 1,"UNIT: C"); break;
								case 0x04 : oled_printf(  5, 35, &Font_16x26, 1,"UNIT: D"); break;					
								default :
								break;
							}
//							if(angZ1>0)	oled_printf(  5,  5, &Font_16x26, 1,"A: %2.2f ", angZ1);
//							else 	oled_printf(  5,  5, &Font_16x26, 1,"A:%2.2f ", angZ1);
							//SSD1306_InvertDisplay(1); //SSD1306_ToggleInvert();
							//oled_printf(  5, 5, &Font_32x52, 1,"%d",address);

						}
						else
						{
							SSD1306_InvertDisplay(0); //SSD1306_ToggleInvert();
							oled_printf(  5, 5, &Font_16x26, 1,"TEMP:%2.0f", temperature-4);
							oled_printf(  5, 35, &Font_16x26, 1,"HUMI:%2.0f", humidity);							
						}
					SSD1306_UpdateScreen(); //display
					}
					/*if ( -40 > temperature  || 0.0== humidity   ) //sht31 xxx =0 display
					{
						oled_printf( 5, 35, &Font_16x26, 1," x ,  x ", temperature-4, humidity);
						SSD1306_UpdateScreen(); //display
						ang_dect(address);  //  ang setting =>LED ON/OFF
						
					}
					else  //sht31  ok*/
					//SSD1306_Clear();
					//SSD1306_UpdateScreen(); //display
					ang_dect(address);  //  ang setting =>LED ON/OFF
				}
				if( seq_cnt == 3)
				{
					//NTC   temp    =   		=>  CCR   on/off
					HAL_ADC_Start(&hadc1);
					while(HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK); // HAL_ADC_PollForConversion(&hadc1, 100);// HAL_MAX_DELAY);
					value1 = HAL_ADC_GetValue(&hadc1);
					HAL_ADC_Stop(&hadc1);

					///cds  phto sensor   =  BAT  used =>  LED on/off
					HAL_ADC_Start(&hadc2);
					while(HAL_ADC_PollForConversion(&hadc2, 10) != HAL_OK); // HAL_ADC_PollForConversion(&hadc1, 100);// HAL_MAX_DELAY);
					value2 = HAL_ADC_GetValue(&hadc2);
					HAL_ADC_Stop(&hadc2 );
				}
				if( seq_cnt == 4)
				{
					if(PSU1_LF | PSU2_LF | PSU3_LF | PSU4_LF | PSU1_TF | PSU2_TF  | PSU3_TF  | PSU4_TF)
						HAL_GPIO_WritePin(Relay_GPIO_Port, Relay_Pin,GPIO_PIN_SET );
					else HAL_GPIO_WritePin(Relay_GPIO_Port, Relay_Pin,GPIO_PIN_RESET );
					TxMsg_Tilt();
					TxMsg_PSU();
				}
			}	//test_cnt2
		}	//test_cnt1
	}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc2.Init.ContinuousConvMode = ENABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 2;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 9;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_4TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_3TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = ENABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */
  /*
  ***  36,000,000(APB1) / 9 / (1+4+3) = 500,000 bps
  */
  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 1000;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 7200;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 38400;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED1_Pin|LED2_Pin|SPI_NSS_Pin|Relay_Pin
                          |RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : S1_Pin S2_Pin */
  GPIO_InitStruct.Pin = S1_Pin|S2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LED1_Pin LED2_Pin SPI_NSS_Pin Relay_Pin */
  GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin|SPI_NSS_Pin|Relay_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : ORG_Pin MS_Pin */
  GPIO_InitStruct.Pin = ORG_Pin|MS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : RST_Pin */
  GPIO_InitStruct.Pin = RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ENT_Pin */
  GPIO_InitStruct.Pin = ENT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ENT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : up_Pin dn_Pin */
  GPIO_InitStruct.Pin = up_Pin|dn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
