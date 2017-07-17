/**
 * @file
 * @brief work with flash on esp8266
 *
 * @date 05.07.2017
 * @author Egor Shitov
 */

#include "spi_api_impl.h"

#include <hal/ipl.h>
#include <embox/unit.h>

static char spi_buff[FLASH_SECTOR_SIZE];

// Let's make 'addr' is for converted addr, '_addr' is for not
static inline uint32_t get_sector_by_addr(uint32_t _addr){
	return _addr / FLASH_SECTOR_SIZE;
}

static inline uint8_t is_oversize(uint32_t _addr, uint32_t size){
	uint32_t sec1 = get_sector_by_addr(_addr);
	uint32_t sec2 = get_sector_by_addr(_addr + size);

	return sec1 % FLASH_SECTOR_SIZE ? sec1 != sec2 : sec2 - sec1 > 1;
}

SpiFlashOpResult spi_flash_erase_sector(uint16_t sector){
	uint32_t old = ipl_save();
	Cache_Read_Disable();
	SpiFlashOpResult res = SPIEraseSector(sector + MIN_SECTOR_NUMBER);
	Cache_Read_Enable(0, 0, 1);
	ipl_restore(old);
	return res;
}

SpiFlashOpResult spi_flash_write(uint32_t dest_addr, uint32_t *src_addr, uint32_t size){
	if(!src_addr || !size)
		return SPI_FLASH_RESULT_ERR;

	if(is_oversize(dest_addr, size))
		return SPI_FLASH_RESULT_OVERSIZE;

	uint32_t sec_addr = (dest_addr / FLASH_SECTOR_SIZE) * FLASH_SECTOR_SIZE;
	SpiFlashOpResult res;
	res = spi_flash_read(sec_addr, spi_buff, FLASH_SECTOR_SIZE);
	if(res)
		return res;

	res = spi_flash_erase_sector(dest_addr/FLASH_SECTOR_SIZE);
	if(res)
		return res;

	uint32_t current = dest_addr%FLASH_SECTOR_SIZE;
	memcpy(spi_buff + current, src_addr, size);

	uint32_t old = ipl_save();
	Cache_Read_Disable();
	res =  SPIWrite(sec_addr + MIN_SECTOR_NUMBER*FLASH_SECTOR_SIZE, spi_buff, FLASH_SECTOR_SIZE);
	Cache_Read_Enable(0, 0, 1);
	ipl_restore(old);

	return res;
}

SpiFlashOpResult spi_flash_read(uint32_t src_addr, uint32_t* dest_addr, uint32_t size){
	uint32_t _src_addr = src_addr + MIN_SECTOR_NUMBER*FLASH_SECTOR_SIZE;

	if(!dest_addr)
		return SPI_FLASH_RESULT_ERR;

	if(is_oversize(_src_addr, size))
		return SPI_FLASH_RESULT_OVERSIZE;

	uint32_t sec_addr = (_src_addr / FLASH_SECTOR_SIZE) * FLASH_SECTOR_SIZE;

	uint32_t old = ipl_save();
	Cache_Read_Disable();
	SpiFlashOpResult res = SPIRead(sec_addr, spi_buff, FLASH_SECTOR_SIZE);
	Cache_Read_Enable(0, 0, 1);
	ipl_restore(old);
	if(res)
		return res;

	uint32_t current = src_addr % FLASH_SECTOR_SIZE;
	memcpy(dest_addr, spi_buff + current, size);

	return SPI_FLASH_RESULT_OK;
}

EMBOX_UNIT_INIT(spi_api_impl_init);
static int spi_api_impl_init(void){
	flashchip->sector_size = FLASH_SECTOR_SIZE;
	return 0;
}
