
#ifndef __SCL3300_H_
#define __SCL3300_H_

#include <stdio.h>

#define SPIT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))

#define READ_ACC_X 0x040000F7
#define READ_ACC_Y 0x080000FD
#define READ_ACC_Z 0x0C0000FB
#define READ_STO 0x100000E9
#define ENABLE_ANGLE_OUTPUTS 0xB0001F6F
#define READ_ANG_X 0x240000C7
#define READ_ANG_Y 0x280000CD
#define READ_ANG_Z 0x2C0000CB
#define READ_TEMPERATURE 0x140000EF
#define READ_STATUS_SUMMARY 0x180000E5
#define READ_ERR_FLAG1 0x1C0000E3
#define READ_ERR_FLAG2 0x200000C1
#define READ_CMD 0x340000DF
#define CHANGE_TO_MODE1 0xB400001F //默认模式，1.8g full-scale 40 Hz 1st order low pass filter
#define CHANGE_TO_MODE2 0xB4000102 //23.6g full-scale 70 Hz 1st order low pass filter
#define CHANGE_TO_MODE3 0xB4000225 //Inclination mode 13 Hz 1st order low pass filter
#define CHANGE_TO_MODE4 0xB4000338 //Inclination mode 13 Hz 1st order low pass filter Low noise mode
#define SET_POWER_DOWN_MODE 0xB400046B
#define WAKE_UP 0xB400001F
#define SW_RESET 0xB4002098
#define READ_ID 0x40000091 //WHOAMI

#define READ_SERIAL1 0x640000A7
#define READ_SERIAL2 0x680000AD
#define READ_CURRENT_BANK 0x7C0000B3
#define SWITCH_TO_BANK_0 0xFC000073 //默认，读bank1后自动返回
#define SWITCH_TO_BANK_1 0xFC00016E
//

//u8 SPI_SCL3300_SendByte(u8 byte);
uint8_t SPI_SCL3300_SendByte(uint8_t byte);
void SET_SPI(void);
void SCL3300_Init(void);
//u32 SPI_SCL3300_CMD(u32 cmd);
//void SPI_SCL3300_SendCmd(uint32_t cmd)
uint32_t SPI_SCL3300_CMD(uint32_t cmd);


// spi driver function
//void spi_nss_init(void);
void spi_nss_hi(void);
void spi_nss_low(void);
//void spi_nss_rev(void);

/*
Read temperature data register(RWTR) reads the temperature data register during nomal
operation without effecting the operation. Temperature data register is updated every 150us. The
load operation is disabled whenever the CSB signal is low, hence CSB must stay high at least 150
us prior to the PWTR command in order to guarantee correct data. The data is transferred MSB first.
In normal operation, it does not matter what data is writted into temperature data register
during the PWTR command and hence writing all zeros is recommended.
*/

/*
 * During normal operation, acceleration data register are reloaded every 150us.
 * The load operation is disabled whenever the CSB signal is low, hence CSB must
 * stay high at least 150us prior to he RADX command in order to guarantee correct
 * data. Data output is an 11-bit digital word that is out MSB first and LSB last.
 */


#endif
