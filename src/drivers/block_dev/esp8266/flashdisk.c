#include "flashdisk.h"

#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <util/err.h>
#include <embox/unit.h>
#include <fs/vfs.h>

#include <drivers/block_dev.h>
#include <drivers/esp8266/spi_api_impl.h>

#include <util/indexator.h>
#include <util/binalign.h>

#define MAX_DEV_QUANTITY OPTION_GET(NUMBER,flashdisk_quantity)
INDEX_DEF(flashdisk_idx,0,MAX_DEV_QUANTITY);

static int read_sectors(struct block_dev *bdev, char *buffer, size_t count, blkno_t blkno);
static int write_sectors(struct block_dev *bdev, char *buffer, size_t count, blkno_t blkno);
static int ram_ioctl(struct block_dev *bdev, int cmd, void *args, size_t size);

block_dev_driver_t flashdisk_pio_driver = {
	"flashdisk_drv",
	ram_ioctl,
	read_sectors,
	write_sectors
};

static int flashdisk_get_index(char *path) {
	char *dev_name;
	int idx;

	if(NULL == (dev_name = strstr(path, "flash"))) {
		return -1;
	}
	dev_name += sizeof("flash");

	if(!isdigit((int)dev_name[0])) {
		return -1;
	}

	sscanf(dev_name, "%d", &idx);

	return idx;
}

/* XXX not stores index if path have no index placeholder, like * or # */
struct flashdisk *flashdisk_create(char *path, size_t size) {
    struct flashdisk *flashdisk;
	int idx;
	int err;

	const size_t flashdisk_size = binalign_bound(size, FLASH_MAX_SIZE);
	const size_t sectors_n = (flashdisk_size + FLASH_BLOCK_SIZE - 1) / FLASH_BLOCK_SIZE;

	flashdisk->sectors = sectors_n;
    flashdisk->start_sector = MIN_SECTOR_NUMBER;

	if (0 > (idx = block_dev_named(path, &flashdisk_idx))) {
		err = -idx;
		goto err_out;
	}

	flashdisk->bdev = block_dev_create(path, &flashdisk_pio_driver, flashdisk);
	if (NULL == flashdisk->bdev) {
		err = EIO;
		goto err_free_bdev_idx;
	}

	flashdisk->bdev->size = flashdisk_size;
	flashdisk->bdev->block_size = FLASH_BLOCK_SIZE;
	return flashdisk;

err_free_bdev_idx:
	index_free(&flashdisk_idx, idx);
err_out:
	return err_ptr(err);
}

int flashdisk_delete(const char *name) {
	struct path flashdisk_node;
    flashdisk_t *flashdisk;
	struct nas *nas;
	struct node_fi *node_fi;
	int idx;

	if (vfs_lookup(name, &flashdisk_node)) {
		return -1;
	}

	if (NULL == flashdisk_node.node) {
		return -1;
	}

	nas = flashdisk_node.node->nas;
	node_fi = nas->fi;
	if (NULL != (flashdisk = (flashdisk_t *) block_dev(node_fi->privdata)->privdata)) {
		if (-1 != (idx = flashdisk_get_index((char *)name))) {
			index_free(&flashdisk_idx, idx);
		}
		block_dev_destroy (node_fi->privdata);
		vfs_del_leaf(flashdisk_node.node);
	}
	return 0;
}

static int read_sectors(struct block_dev *bdev,
		char *buffer, size_t count, blkno_t blkno) {
	flashdisk_t *flashdisk = (flashdisk_t *) bdev->privdata;
	size_t read_addr = (flashdisk->start_sector + blkno) * bdev->block_size;

    spi_flash_read(read_addr, buffer, count);

	return count;
}

static int write_sectors(struct block_dev *bdev,
		char *buffer, size_t count, blkno_t blkno) {
	flashdisk_t *flashdisk = (flashdisk_t *) bdev->privdata;
	size_t write_addr = (flashdisk->start_sector + blkno) * bdev->block_size;

    spi_flash_write(write_addr, buffer, count);
	return count;
}

static int ram_ioctl(struct block_dev *bdev, int cmd, void *args, size_t size) {
    flashdisk_t *flashdisk = (flashdisk_t *) bdev->privdata;

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return flashdisk->sectors;

	case IOCTL_GETBLKSIZE:
		return FLASH_BLOCK_SIZE;
	}
	return -ENOSYS;
}
