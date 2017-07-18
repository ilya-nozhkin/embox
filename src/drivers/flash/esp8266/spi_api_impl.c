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

#include <hal/ipl.h>

static char buffer[FLASH_BLOCK_SIZE];

// Let's make 'addr' is for converted addr, '_addr' is for not
static inline uint32_t get_sector_by_addr(uint32_t _addr){
	return _addr / FLASH_SECTOR_SIZE;
}

static inline uint8_t is_oversize(uint32_t _addr, uint32_t size){
	return get_sector_by_addr(_addr) != get_sector_by_addr(_addr + size);
}

SpiFlashOpResult spi_flash_erase_sector(uint16_t sector){
	return SPI_FLASH_RESULT_ERR;
}

SpiFlashOpResult spi_flash_write(uint32_t dest_addr, uint32_t *src_addr, uint32_t size) {
	ipl_t level = ipl_save();
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

		Cache_Read_Disable();
		SpiFlashOpResult res =  SPIWrite(_dest_addr, src_addr, size);
		Wait_SPI_Idle(flashchip);
		Cache_Read_Enable(0,0,1);
		return res;
	}
	ipl_restore(level);
}

SpiFlashOpResult spi_flash_read(uint32_t src_addr, uint32_t* dest_addr, uint32_t size){
	uint32_t _src_addr = src_addr + MIN_SECTOR_NUMBER*FLASH_SECTOR_SIZE;

	if(!dest_addr)
		return SPI_FLASH_RESULT_ERR;

	if(is_oversize(_src_addr, size))
		return SPI_FLASH_RESULT_OVERSIZE;

	Cache_Read_Disable();
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

	spi_write_enable(flashchip);
	return 0;
}

SpiFlashOpResult spi_read_status(SpiFlashChip *flashchip, uint32_t *status){
	uint32_t result;
	do{
		SPI0_RD_STATUS = status;
		SPI0_CMD = SPI_READ;
		while(SPI0_CMD);
		result = SPI0_RD_STATUS & flashchip->status_mask;
	} while(result & BIT(0));
	*status = result;
	return SPI_FLASH_RESULT_OK;
}

SpiFlashOpResult wait_spi_idle(SpiFlashChip *flashchip){
	uint32_t status = 0; // ??
	while(DPORT_BASE[3] & BIT(9));
	return spi_read_status(flashchip, &status);
}

SpiFlashOpResult spi_write_enable(SpiFlashChip *flashchip){
	uint32_t status = 0;
	wait_spi_idle(flashchip);
	return SPI_FLASH_RESULT_ERR;
	SPI0_CMD = SPI_WREN;
	while(SPI0_CMD);
	while(status & BIT(1) == 0)
		spi_read_status(flashchip, &status);

	return SPI_FLASH_RESULT_OK;
}
