/**
 * @file
 * @brief work with flash on esp8266
 *
 * @date 05.07.2017
 * @author Egor Shitov
 */

#ifndef DRIVERS_BLOCKDEV_ESP8266_SPI_API_IMPL_H_
#define DRIVERS_BLOCKDEV_ESP8266_SPI_API_IMPL_H_

#define FLASH_BLOCK_SIZE 	16				// block size (bytes)
#define FLASH_SECTOR_SIZE   (4*1024)		// sector size 4KBytes (esp8266-documented)
#define FLASH_MAX_SIZE      (2*1024*1024)	// actually 4MBytes (esp8266-documented)
#define MIN_SECTOR_NUMBER   32				// reserved for executable code

#define IDX_SPI_CMD 0
#define IDX_SPI_ADDR 1

#define SPI0_CMD spi0_[IDX_SPI_CMD]
#define SPI0_ADDR spi0_[IDX_SPI_ADDR]

#include <stdint.h>

typedef struct {
	uint32_t deviceId;
	uint32_t chip_size;
	uint32_t block_size;
	uint32_t sector_size;
	uint32_t page_size;
	uint32_t status_mask;
} SpiFlashChip;							// esp8266-documented

typedef enum{
	SPI_FLASH_RESULT_OK,				// esp8266-documented
	SPI_FLASH_RESULT_ERR,				// esp8266-documented
	SPI_FLASH_RESULT_TIMEOUT,			// esp8266-documented
	SPI_FLASH_RESULT_UNALIGNED_ADDR, 	// happens when (addr & 3) != 0
	SPI_FLASH_RESULT_OVERSIZE			// happens when addr starts in one sector and addr+size ends in another one
} SpiFlashOpResult;

extern volatile uint32_t spi0_[64];

/* built-in esp8266 functions */
extern void Wait_SPI_Idle(SpiFlashChip*);
extern void Cache_Read_Disable(void);
extern void Cache_Read_Enable(uint32_t,uint32_t,uint32_t);
extern SpiFlashOpResult SPIEraseSector(uint16_t sector);
extern SpiFlashOpResult SPIWrite(uint32_t dest_addr, uint32_t *src_addr, uint32_t size);
extern SpiFlashOpResult SPIRead(uint32_t src_addr, uint32_t *dest_addr, uint32_t size);
/* built-in esp8266 struct */
extern SpiFlashChip *flashchip;

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
