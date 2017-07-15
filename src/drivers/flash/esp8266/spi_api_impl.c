/**
 * @file
 * @brief work with flash on esp8266
 *
 * @date 05.07.2017
 * @author Egor Shitov
 */

#include "spi_api_impl.h"

#include <hal/ipl.h>

// Let's make 'addr' is for converted addr, '_addr' is for not
static inline uint32_t get_sector_by_addr(uint32_t _addr){
	return _addr / FLASH_SECTOR_SIZE;
}

static inline uint8_t is_oversize(uint32_t _addr, uint32_t size){
	return get_sector_by_addr(_addr) != get_sector_by_addr(_addr + size);
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
	uint32_t _dest_addr = dest_addr + MIN_SECTOR_NUMBER*FLASH_SECTOR_SIZE;

	if(!src_addr)
		return SPI_FLASH_RESULT_ERR;

	/* We should align it like 0x...000 */
	if(size & 3){
		// We don't need align it right there (at this moment :p)
		return SPI_FLASH_RESULT_UNALIGNED_ADDR;
	} else {
		if(is_oversize(_dest_addr, size))
			return SPI_FLASH_RESULT_OVERSIZE;

		uint32_t old = ipl_save();
		Cache_Read_Disable();
		SpiFlashOpResult res =  SPIWrite(_dest_addr, src_addr, size);
		Cache_Read_Enable(0,0,1);
		ipl_restore(old);
		return res;
	}

}

SpiFlashOpResult spi_flash_read(uint32_t src_addr, uint32_t* dest_addr, uint32_t size){
	uint32_t _src_addr = src_addr + MIN_SECTOR_NUMBER*FLASH_SECTOR_SIZE;

	if(!dest_addr)
		return SPI_FLASH_RESULT_ERR;

	if(is_oversize(_src_addr, size))
		return SPI_FLASH_RESULT_OVERSIZE;

	uint32_t old = ipl_save();
	Cache_Read_Disable();
	SpiFlashOpResult res = SPIRead(_src_addr, dest_addr, size);
	Cache_Read_Enable(0,0,1);
	ipl_restore(old);
	return res;
}
