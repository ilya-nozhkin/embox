/**
 * @file
 * @brief necessary test (was created to catch unknown error :p)
 *
 * @date 06.07.2017
 * @author Egor Shitov
 */

#include <drivers/esp8266/spi_api_impl.h>
#include <embox/test.h>
#include <stdlib.h>

#define ESP8266_SPI_TEST_DATA_SIZE 14

EMBOX_TEST_SUITE("esp8266 flash ops test");

static char data[ESP8266_SPI_TEST_DATA_SIZE] = "Hello, world!";

TEST_CASE("Write/read to/from flash") {
	char data_to_read[ESP8266_SPI_TEST_DATA_SIZE] = {0};

	// Write and read without offset
	test_assert_zero(spi_flash_write(0, data, ESP8266_SPI_TEST_DATA_SIZE));
	test_assert_zero(spi_flash_read(0, data_to_read, ESP8266_SPI_TEST_DATA_SIZE));

	// Since strcmp doesn't work :c
	bool fail = false;
	for(int i = 0; i < ESP8266_SPI_TEST_DATA_SIZE; i++)
		if(data[i] != data_to_read[i])
			fail = true;

	test_assert_zero(fail);
}

TEST_CASE("Write/read to/from flash with offset") {
	char data_to_read[ESP8266_SPI_TEST_DATA_SIZE] = {0};

	// Write and read with offset
	uint8_t offset = 16;

	test_assert_zero(spi_flash_write(offset, data, ESP8266_SPI_TEST_DATA_SIZE));
	test_assert_zero(spi_flash_read(offset, data_to_read, ESP8266_SPI_TEST_DATA_SIZE));

	bool fail = false;
	for(int i = 0; i < ESP8266_SPI_TEST_DATA_SIZE; i++)
		if(data[i] != data_to_read[i])
			fail = true;

	test_assert_zero(fail);
}

TEST_CASE("Read from unwritten place") {
	uint32_t dest[8] = {0};

	// if spi_flash_read fails, it will throw exception
	SpiFlashOpResult res = spi_flash_read(FLASH_BLOCK_SIZE*20, dest, sizeof(uint32_t) * 8);
	test_assert_zero(res);
}
