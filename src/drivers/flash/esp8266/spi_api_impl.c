/**
 * @file
 * @brief work with flash on esp8266
 *
 * @date 05.07.2017
 * @author Egor Shitov
 */

#include "spi_api_impl.h"

#include <embox/unit.h>
#include <kernel/printk.h>
#include <hal/reg.h>

static char buffer[FLASH_BLOCK_SIZE];

// Let's make 'addr' is for converted addr, '_addr' is for not
static inline uint32_t get_sector_by_addr(uint32_t _addr){
	return _addr / FLASH_SECTOR_SIZE;
}

static inline uint8_t is_oversize(uint32_t _addr, uint32_t size){
	return get_sector_by_addr(_addr) != get_sector_by_addr(_addr + size);
}

SpiFlashOpResult spi_flash_erase_sector(uint16_t sector){
	Cache_Read_Disable();
	SpiFlashOpResult result = SPIEraseSector(sector + MIN_SECTOR_NUMBER);
	Cache_Read_Enable(0, 0, 1);
	return result;
}

SpiFlashOpResult spi_flash_write(uint32_t dest_addr, uint32_t *src_addr, uint32_t size){
	uint32_t _dest_addr = dest_addr + MIN_SECTOR_NUMBER*FLASH_SECTOR_SIZE;

	if(!src_addr)
		return SPI_FLASH_RESULT_ERR;

	/* We should align it like 0x...000 */
	if(size & 3){
		// int new_size = (size | 3) + 1;
		//
		// if(is_oversize(_dest_addr, new_size)){
		// 	return SPI_FLASH_RESULT_OVERSIZE;
		// }
		//
		// void *tmp = malloc(new_size);
		// memcpy(tmp, src_addr, size);
		//
		// SpiFlashOpResult res = SPIWrite(_dest_addr, tmp, new_size);
		//
		// free(tmp);
		// return res;

		// We don't need align it right there (at this moment :p)
		return SPI_FLASH_RESULT_UNALIGNED_ADDR;
	} else {
		if(is_oversize(_dest_addr, size))
			return SPI_FLASH_RESULT_OVERSIZE;

		Cache_Read_Disable();
		Wait_SPI_Idle(flashchip);
		SpiFlashOpResult res =  SPIWrite(_dest_addr, src_addr, size);
		Cache_Read_Enable(0,0,1);
		return res;
	}

}

SpiFlashOpResult spi_flash_read(uint32_t src_addr, uint32_t* dest_addr, uint32_t size){
	uint32_t _src_addr = src_addr + MIN_SECTOR_NUMBER*FLASH_SECTOR_SIZE;

	if(!dest_addr)
		return SPI_FLASH_RESULT_ERR;

	if(is_oversize(_src_addr, size))
		return SPI_FLASH_RESULT_OVERSIZE;

	Cache_Read_Disable();
	Wait_SPI_Idle(flashchip);
	SpiFlashOpResult res = SPIRead(_src_addr, dest_addr, size);
	Cache_Read_Enable(0,0,1);
	return res;
}

EMBOX_UNIT_INIT(esp8266_spi_setup);
static int esp8266_spi_setup(void){

	printk("deviceId: %u\nchip_size: %u\nblock_size: %u\nsector_size: %u\npage_size: %u\nstatus_mask: %u\n",
		flashchip->deviceId,
		flashchip->chip_size,
		flashchip->block_size,
		flashchip->sector_size,
		flashchip->page_size,
		flashchip->status_mask);

	return 0;
}
