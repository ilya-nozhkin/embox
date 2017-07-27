/**
 * @file
 * @brief work with flash on esp8266
 *
 * @date 05.07.2017
 * @author Egor Shitov
 */

#ifndef DRIVERS_BLOCKDEV_ESP8266_SPI_API_IMPL_H_
#define DRIVERS_BLOCKDEV_ESP8266_SPI_API_IMPL_H_

#include <stdint.h>

#define FLASH_BLOCK_SIZE 	256				// block size (bytes)
#define FLASH_SECTOR_SIZE   (4*1024)		// default sector size is 4KBytes
#define FLASH_MAX_SIZE      (1*1024*1024)	// actually 4MBytes (esp8266-documented)
#define MIN_SECTOR_NUMBER	(16)			// reserved sectors for executable code

/* Addr we can rw:
 * 1048576 - 2093056 (256-511 with 4KByte sector)
 * 3145728 - 4190208 (768-1023 with 4KByte sector) */

typedef struct {
	uint32_t deviceId;
	uint32_t chip_size;
	uint32_t block_size;
	uint32_t sector_size;
	uint32_t page_size;
	uint32_t status_mask;
} SpiFlashChip;

typedef enum{
	SPI_FLASH_RESULT_OK,				// esp8266-documented
	SPI_FLASH_RESULT_ERR,				// esp8266-documented
	SPI_FLASH_RESULT_TIMEOUT,			// esp8266-documented
	SPI_FLASH_RESULT_UNALIGNED_ADDR, 	// happens when (addr & 3) != 0
	SPI_FLASH_RESULT_OVERSIZE			// happens when addr starts in one sector and addr+size ends in another one
} SpiFlashOpResult;

/* built-in esp8266 functions */
extern void Cache_Read_Disable(void);
extern void Cache_Read_Enable(uint32_t,uint32_t,uint32_t);
extern SpiFlashOpResult SPIEraseSector(uint16_t sector);
extern SpiFlashOpResult SPIWrite(uint32_t dest_addr, uint32_t *src_addr, uint32_t size);
extern SpiFlashOpResult SPIRead(uint32_t src_addr, uint32_t *dest_addr, uint32_t size);
/* built-in esp8266 struct */
extern SpiFlashChip *flashchip;

/* documented esp8266 functions */
SpiFlashOpResult spi_flash_erase_sector(uint16_t sector);

/* Write to sector (even with offset)
 * dest_addr must be like 0x..000 (000-end)
 */
SpiFlashOpResult spi_flash_write(uint32_t dest_addr, uint32_t *src_addr, uint32_t size);

/* Read from sector (even with offset)
 * src_addr must be like 0x..000 (000-end)
 */
SpiFlashOpResult spi_flash_read(uint32_t src_addr, uint32_t *dest_addr, uint32_t size);

#endif /* DRIVERS_BLOCKDEV_ESP8266_SPI_API_IMPL_H_ */
