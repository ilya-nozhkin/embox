/**
 * @file
 * @brief work with flash on esp8266
 *
 * @date 05.07.2017
 * @author Egor Shitov
 */

#ifndef DRIVERS_BLOCKDEV_ESP8266_SPI_API_IMPL_H_
#define DRIVERS_BLOCKDEV_ESP8266_SPI_API_IMPL_H_

#define FLASH_BLOCK_SIZE 	8				// block size 8Bytes
#define FLASH_SECTOR_SIZE   (4*1024)		// sector size 4KBytes
#define FLASH_MAX_SIZE      \
					(FLASH_SECTOR_SIZE*64)	// actually 4MBytes
#define MIN_SECTOR_NUMBER   16				// reserved for executable code

#include <stdint.h>

typedef enum{
	SPI_FLASH_RESULT_OK,				// esp8266-documented
	SPI_FLASH_RESULT_ERR,				// esp8266-documented
	SPI_FLASH_RESULT_TIMEOUT,			// esp8266-documented
	SPI_FLASH_RESULT_UNALIGNED_ADDR, 	// happens when (addr & 3) != 0
	SPI_FLASH_RESULT_OVERSIZE			// happens when addr starts in one sector and addr+size ends in another one
} SpiFlashOpResult;

/* built-in esp8266 functions */
extern SpiFlashOpResult SPIEraseSector(uint16_t sector);
extern SpiFlashOpResult SPIWrite(uint32_t dest_addr, uint32_t *src_addr, uint32_t size);
extern SpiFlashOpResult SPIRead(uint32_t src_addr, uint32_t *dest_addr, uint32_t size);

/* documented esp8266 functions */
// Doesn't work at all
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
