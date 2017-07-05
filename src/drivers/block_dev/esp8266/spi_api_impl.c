#include "spi_api_impl.h"

SpiFlashOpResult spi_flash_erase_sector(uint16_t sector){
    SpiFlashOpResult result = SPIEraseSector(sector);
    return result;
}

SpiFlashOpResult spi_flash_write(uint32_t dest_addr, uint32_t *src_addr, uint32_t size){
    if(!src_addr)
        return SPI_FLASH_RESULT_ERR;

    /* We should align it like 0x...000 */
    if(size & 3){
        int new_size = (size | 3) + 1;

        void *tmp = malloc(new_size);
        memcpy(tmp, src_addr, size);

        return SPIWrite(dest_addr, tmp, new_size);
    }
    else
        return SPIWrite(dest_addr, src_addr, size);

}

SpiFlashOpResult spi_flash_read(uint32_t src_addr, uint32_t* dest_addr, uint32_t size){
    if(!dest_addr)
        return SPI_FLASH_RESULT_ERR;

    return SPIRead(src_addr, dest_addr, size);
}
