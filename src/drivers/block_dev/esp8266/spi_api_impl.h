/**
 * @file
 * @brief work with flash on esp8266
 *
 * @date 05.07.2017
 * @author Egor Shitov
 */

#ifndef DRIVERS_BLOCKDEV_ESP8266_SPI_API_IMPL_H_
#define DRIVERS_BLOCKDEV_ESP8266_SPI_API_IMPL_H_

#define FLASH_BLOCK_SIZE    4*1024
#define MIN_SECTOR_NUMBER   16
#define FLASH_MAX_SIZE      2*1024*1024

#include <stdint.h>

typedef enum{
    SPI_FLASH_RESULT_OK,
    SPI_FLASH_RESULT_ERR,
    SPI_FLASH_RESULT_TIMEOUT
} SpiFlashOpResult;

/* built-in esp8266 functions */
extern SpiFlashOpResult SPIEraseSector(uint16_t sector);
extern SpiFlashOpResult SPIWrite(uint32_t dest_addr, uint32_t *src_addr, uint32_t size);
extern SpiFlashOpResult SPIRead(uint32_t src_addr, uint32_t *dest_addr, uint32_t size);

SpiFlashOpResult spi_flash_erase_sector(uint16_t sector);
SpiFlashOpResult spi_flash_write(uint32_t dest_addr, uint32_t *src_addr, uint32_t size);
SpiFlashOpResult spi_flash_read(uint32_t src_addr, uint32_t *dest_addr, uint32_t size);

#endif /* DRIVERS_BLOCKDEV_ESP8266_SPI_API_IMPL_H_ */
