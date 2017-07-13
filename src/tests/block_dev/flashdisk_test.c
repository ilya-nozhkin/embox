/**
 * @file
 * @brief
 *
 * @date 06.07.2017
 * @author Egor Shitov
 */

#include <drivers/esp8266/spi_api_impl.h>
#include <drivers/block_dev/esp8266/flashdisk.h>
#include <embox/test.h>

#include <util/err.h>

EMBOX_TEST_SUITE("esp8266/flashdisk test");

#define FS_DEV  "/dev/flash#"
#define FS_BLOCKS  124

TEST_CASE("Create and delete flash") {

	char path[] = FS_DEV;

	test_assert_zero(err(flashdisk_create(path, FS_BLOCKS * FLASH_BLOCK_SIZE)));

	test_assert_zero(flashdisk_delete(path));
}
