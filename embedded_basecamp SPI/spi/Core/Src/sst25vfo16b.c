
#include <sst25vfo16b.h>

#if (INIT_DEBUG == 1)
#include "string.h"
#include "stdio.h"
char buf[64];
extern UART_HandleTypeDef huart3;
#endif


#define DUMMY_BYTE        0x00

sst25vf016b_t	dev;

#define HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET)    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET)



uint8_t	SST25_Spi(uint8_t	Data)
{

	uint8_t	ret;

	HAL_SPI_TransmitReceive(SST25_SPI_PTR, &Data, &ret, 1, 10);

	return ret;
}


uint32_t SST25_ReadID(void)
{
	uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);

	SST25_Spi(SST25_GET_JEDEC_ID);

	Temp0 = SST25_Spi(DUMMY_BYTE);
	Temp1 = SST25_Spi(DUMMY_BYTE);
	Temp2 = SST25_Spi(DUMMY_BYTE);

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_Delay(1);
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

	return Temp;
}

void SST25_WriteEnable(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
	SST25_Spi(SST25_WRITE_ENABLE);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_Delay(1);
}

void SST25_WriteDisable(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
	SST25_Spi(SST25_WRITE_DISABLE);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_Delay(1);
}


void SST25_WaitForWriteEnd(void)
{

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
	SST25_Spi(SST25_READ_STATUS);
	dev.StatusRegister = SST25_Spi(DUMMY_BYTE);
	while((dev.StatusRegister & 0x01) == 0x01)
	{
		HAL_Delay(1);
		dev.StatusRegister = SST25_Spi(DUMMY_BYTE);
	}
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_Delay(1);
}


void EnableWriteStatus(void)
{

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
	SST25_Spi(ENABLE_WRITE_STATUS);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_Delay(1);
}



uint8_t SST25_Init(void)
{
	uint32_t id;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_Delay(100);

	#if (INIT_DEBUG == 1)
	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Init Begin...\n", 14, 100);
	#endif

	id = SST25_ReadID();

	#if (INIT_DEBUG == 1)
	snprintf(buf, 64, "ID:0x%lX\n", id & 0x0000FFFF);
	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen(buf), 100);
	#endif

	if(id & 0x0000FFFF & 0x2541)
	{

			dev.BlockCount = 32;
			#if (INIT_DEBUG == 1)
			HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Chip: sst25vf016b\n", 18, 100);
			#endif
	}
	dev.PageSize = 256;
	dev.SectorSize = 0x1000;
	dev.SectorCount = dev.BlockCount * 16;
	dev.PageCount = (dev.SectorCount * dev.SectorSize) / dev.PageSize;
	dev.BlockSize = dev.SectorSize * 16;
	dev.CapacityInKiloByte = (dev.SectorCount * dev.SectorSize) / 1024;



	#if (INIT_DEBUG == 1)
	snprintf(buf, 64, "Page Size: %d Bytes\n", dev.PageSize);
	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen(buf), 100);

	snprintf(buf, 64, "Page Count: %lu\n", dev.PageCount);
	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen(buf), 100);

	snprintf(buf, 64, "Sector Size: %lu Bytes\n", dev.SectorSize);
	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen(buf), 100);

	snprintf(buf, 64, "Sector Count: %lu\n", dev.SectorCount);
	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen(buf), 100);

	snprintf(buf, 64, "Block Size: %lu Bytes\n", dev.BlockSize);
	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen(buf), 100);

	snprintf(buf, 64, "Block Count: %lu\n", dev.BlockCount);
	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen(buf), 100);

	snprintf(buf, 64, "Capacity: %lu KBytes\n", dev.CapacityInKiloByte);
	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen(buf), 100);

	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Init Done\n", 10, 100);
	#endif


	return 1;
}

void SST25_WriteStatusRegister(uint8_t Data)
{
	EnableWriteStatus();
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
	SST25_Spi(SST25_WRITE_STATUS);
	dev.StatusRegister = Data;
	SST25_Spi(Data);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	SST25_WaitForWriteEnd();

}

uint8_t SST25_ReadStatusRegister(void)
{
	uint8_t status;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
	SST25_Spi(SST25_READ_STATUS);
	status =  SST25_Spi(DUMMY_BYTE);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_Delay(1);

	return status;
}

void SST25_EraseChip(void)
{
	SST25_WriteEnable();

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
	SST25_Spi(SST25_CHIP_ERASE);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	SST25_WaitForWriteEnd();
}


void SST25_EraseSector(uint32_t SectorAddr)
{

	SectorAddr = SectorAddr * dev.SectorSize;

	SST25_WriteEnable();
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
	SST25_Spi(SST25_SECTOR_ERASE);
	SST25_Spi((SectorAddr & 0xFF0000) >> 16);
	SST25_Spi((SectorAddr & 0xFF00) >> 8);
	SST25_Spi(SectorAddr & 0xFF);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	SST25_WaitForWriteEnd();
}

void SST25_EraseBlock(uint32_t BlockAddr)
{
	BlockAddr = BlockAddr * dev.SectorSize * 16;
	SST25_WriteEnable();
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
	SST25_Spi(SST25_BLOCK_ERASE);
	SST25_Spi((BlockAddr & 0xFF0000) >> 16);
	SST25_Spi((BlockAddr & 0xFF00) >> 8);
	SST25_Spi(BlockAddr & 0xFF);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	SST25_WaitForWriteEnd();
}

void SST25_WriteByte(uint8_t * pByte,  uint32_t addr)
{

	SST25_WriteEnable();
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
	SST25_Spi(SST25_BYTE_PROGRAMM);
	SST25_Spi((addr & 0xFF0000) >> 16);
	SST25_Spi((addr & 0xFF00) >> 8);
	SST25_Spi(addr & 0xFF);
	SST25_Spi(*pByte);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	SST25_WriteDisable();
	SST25_WaitForWriteEnd();
}

void SST25_WritePage(uint8_t *pBuffer, uint32_t Page_Address)
{

	Page_Address = Page_Address*dev.PageSize;
	uint8_t count = 0;
	SST25_WriteEnable();
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
	SST25_Spi(SST25_PAGE_PROGRAMM);
	SST25_Spi((Page_Address & 0xFF0000) >> 16);
	SST25_Spi((Page_Address & 0xFF00) >> 8);
	SST25_Spi(Page_Address & 0xFF);
	HAL_SPI_Transmit(SST25_SPI_PTR, &pBuffer[count], 2, 10);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	for(count=1;count<128;count++)
	{
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
		SST25_Spi(SST25_PAGE_PROGRAMM);
		HAL_SPI_Transmit(SST25_SPI_PTR, &pBuffer[count<<1], 2, 10);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	}
	SST25_WriteDisable();
	SST25_WaitForWriteEnd();
}


void SST25_ReadByte(uint8_t *pByte, uint32_t Byte_Address)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
	SST25_Spi(SST25_READ);
	SST25_Spi((Byte_Address & 0xFF0000) >> 16);
	SST25_Spi((Byte_Address& 0xFF00) >> 8);
	SST25_Spi(Byte_Address & 0xFF);
	*pByte = SST25_Spi(DUMMY_BYTE);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_Delay(1);
}


void SST25_ReadPage(uint8_t *pBuffer, uint32_t Page_Address)
{

	Page_Address = Page_Address * dev.PageSize;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);

	SST25_Spi(SST25_READ);
	SST25_Spi((Page_Address & 0xFF0000) >> 16);
	SST25_Spi((Page_Address& 0xFF00) >> 8);
	SST25_Spi(Page_Address & 0xFF);
	HAL_SPI_Receive(SST25_SPI_PTR, pBuffer, 256, 100);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_Delay(1);
}
