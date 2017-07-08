/**
 * @file
 * @brief work with flash on esp8266
 *
 * @date 05.07.2017
 * @author Egor Shitov
 */

#include "spi_api_impl.h"

#include <embox/unit.h>

// Let's make 'addr' is for converted addr, '_addr' is for not
static inline uint32_t get_sector_by_addr(uint32_t _addr){
	return _addr / FLASH_SECTOR_SIZE;
}

static inline uint8_t is_oversize(uint32_t _addr, uint32_t size){
	return get_sector_by_addr(_addr) != get_sector_by_addr(_addr + size);
}

SpiFlashOpResult spi_flash_erase_sector(uint16_t sector){
	SpiFlashOpResult result = SPIEraseSector(sector + MIN_SECTOR_NUMBER);
	return result;
}

SpiFlashOpResult spi_flash_write(uint32_t dest_addr, uint32_t *src_addr, uint32_t size){
	uint32_t _dest_addr = dest_addr + MIN_SECTOR_NUMBER*FLASH_SECTOR_SIZE;

	if(!src_addr)
		return SPI_FLASH_RESULT_ERR;

	/* We should align it like 0x...000 */
	if(size & 3){
		int new_size = (size | 3) + 1;

		if(is_oversize(_dest_addr, new_size)){
			return SPI_FLASH_RESULT_OVERSIZE;
		}

		void *tmp = malloc(new_size);
		memcpy(tmp, src_addr, size);

		SpiFlashOpResult res = SPIWrite(_dest_addr, tmp, new_size);

		free(tmp);
		return res;
	} else {
		if(is_oversize(_dest_addr, size))
			return SPI_FLASH_RESULT_OVERSIZE;

		return SPIWrite(_dest_addr, src_addr, size);
	}

}

SpiFlashOpResult spi_flash_read(uint32_t src_addr, uint32_t* dest_addr, uint32_t size){
	uint32_t _src_addr = src_addr + MIN_SECTOR_NUMBER*FLASH_SECTOR_SIZE;

	if(!dest_addr)
		return SPI_FLASH_RESULT_ERR;

	if(is_oversize(_src_addr, size))
		return SPI_FLASH_RESULT_OVERSIZE;

	return SPIRead(_src_addr, dest_addr, size);
}
