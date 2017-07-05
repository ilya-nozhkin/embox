#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <drivers/esp8266/spi_api_impl.h>

#define SECTOR_NUMBER 16
#define SECTOR_OFFSET 4 * 1024
#define DATA_SIZE 14

int main(int argc, char **argv) {
    int action = atoi(argv[1]);

    char data[DATA_SIZE] = "Hello, world!";

    if(action == 1){
        SpiFlashOpResult res = spi_flash_write(SECTOR_NUMBER*SECTOR_OFFSET, data, DATA_SIZE);
        printf("Write result: %d\n", res);
    } else if(action == 2) {
        SpiFlashOpResult res = spi_flash_erase_sector(SECTOR_NUMBER);
        printf("Erase result: %d\n", res);
    } else {
        char data_to_read[DATA_SIZE] = {0};
        SpiFlashOpResult res = spi_flash_read(SECTOR_NUMBER*SECTOR_OFFSET, data_to_read, DATA_SIZE);

        printf("Read result: %d\n", res);

        printf("Result: %s\n", data_to_read);
    }

	return 0;
}
