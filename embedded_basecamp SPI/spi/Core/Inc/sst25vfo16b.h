#ifndef _SST25VF016B_H
#define _SST25VF016B_H


#include "main.h"

#define INIT_DEBUG               0

extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart3;

#define DEBUG_UART              &huart3
#define SST25_SPI_PTR           &hspi1

#define FLASH_CS_GPIO_Port GPIOD
#define FLASH_CS_Pin GPIO_PIN_7



#define SST25_WRITE_DISABLE     0x04
#define SST25_WRITE_ENABLE      0x06
#define SST25_CHIP_ERASE        0xC7 //0x60
#define SST25_SECTOR_ERASE      0x20
#define SST25_BLOCK_ERASE       0xD8
#define SST25_READ              0x03
#define SST25_PAGE_PROGRAMM     0xAD
#define SST25_BYTE_PROGRAMM     0x02
#define SST25_GET_JEDEC_ID      0x9F
#define SST25_READ_STATUS       0x05
#define SST25_WRITE_STATUS      0x01
#define ENABLE_WRITE_STATUS     0x50


typedef struct
{

	uint8_t		UniqID[8];
	uint16_t	PageSize;
	uint32_t	PageCount;
	uint32_t	SectorSize;
	uint32_t	SectorCount;
	uint32_t	BlockSize;
	uint32_t	BlockCount;
	uint32_t 	CapacityInKiloByte;
	uint8_t		StatusRegister;


}sst25vf016b_t;

extern sst25vf016b_t	dev;


uint8_t	SST25_Init(void);


void SST25_WriteStatusRegister(uint8_t Data);
uint8_t SST25_ReadStatusRegister();

void SST25_EraseChip(void);
void SST25_EraseSector(uint32_t SectorAddr);
void SST25_EraseBlock(uint32_t BlockAddr);

void SST25_WriteByte(uint8_t *pByte, uint32_t addr);
void SST25_WritePage(uint8_t *pBuffer, uint32_t Page_Address);


void SST25_ReadByte(uint8_t *pByte, uint32_t Byte_Address);
void SST25_ReadPage(uint8_t *pBuffer, uint32_t Page_Address);


#endif
