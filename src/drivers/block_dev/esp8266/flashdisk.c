#include "flashdisk.h"

#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <util/err.h>
#include <embox/unit.h>
#include <fs/vfs.h>

#include <mem/misc/pool.h>

#include <drivers/block_dev.h>
#include <drivers/esp8266/spi_api_impl.h>

#include <util/indexator.h>

#define MAX_DEV_QUANTITY OPTION_GET(NUMBER,flashdisk_quantity)

POOL_DEF(flashdisk_pool,struct flashdisk,MAX_DEV_QUANTITY);
INDEX_DEF(flashdisk_idx,0,MAX_DEV_QUANTITY);

static int read_blocks(struct block_dev *bdev, char *buffer, size_t count, blkno_t blkno);
static int write_blocks(struct block_dev *bdev, char *buffer, size_t count, blkno_t blkno);
static int flash_ioctl(struct block_dev *bdev, int cmd, void *args, size_t size);

block_dev_driver_t flashdisk_pio_driver = {
	"flashdisk_drv",
	flash_ioctl,
	read_blocks,
	write_blocks
};

static int flashdisk_get_index(char *path) {
	char *dev_name;
	int idx;

	dev_name = path + strlen(path) - 1;

	if(!isdigit(dev_name[0])){
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

    if((flashdisk = pool_alloc(&flashdisk_pool)) == NULL){
		pool_free(&flashdisk_pool, flashdisk);
		return err_ptr(err);
    }

	if(size > FLASH_MAX_SIZE) {
		err = E2BIG;

		pool_free(&flashdisk_pool, flashdisk);
		return err_ptr(err);
	}

    flashdisk->begin_block = 0;
	flashdisk->blocks = size % FLASH_BLOCK_SIZE == 0 ? size : size/FLASH_BLOCK_SIZE + 1;

	if (0 > (idx = block_dev_named(path, &flashdisk_idx))) {
		err = -idx;

		pool_free(&flashdisk_pool, flashdisk);
		return err_ptr(err);
	}

	flashdisk->bdev = block_dev_create(path, &flashdisk_pio_driver, flashdisk);
	if (NULL == flashdisk->bdev) {
		err = EIO;

		index_free(&flashdisk_idx, idx);
		pool_free(&flashdisk_pool, flashdisk);
		return err_ptr(err);
	}

	flashdisk->bdev->size = size;
	flashdisk->bdev->block_size = FLASH_BLOCK_SIZE;
	return flashdisk;
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
        pool_free(&flashdisk_pool, flashdisk);

		if (-1 != (idx = flashdisk_get_index((char *)name))) {
			index_free(&flashdisk_idx, idx);
		}
		block_dev_destroy (node_fi->privdata);
		vfs_del_leaf(flashdisk_node.node);
	}
	return 0;
}

static int read_blocks(struct block_dev *bdev,
		char *buffer, size_t count, blkno_t blkno) {
	flashdisk_t *flashdisk = (flashdisk_t *) bdev->privdata;
	size_t block_size = bdev->block_size;
	size_t read_addr = (flashdisk->begin_block + blkno) * block_size;

	size_t done = 0; // read blocks
	for(; done < count; done++){
		size_t offset = done * block_size;
		SpiFlashOpResult res = spi_flash_read(read_addr + offset, buffer + offset, block_size);
		if(res)
			break;
	}

	return done;
}

static int write_blocks(struct block_dev *bdev,
		char *buffer, size_t count, blkno_t blkno) {
	flashdisk_t *flashdisk = (flashdisk_t *) bdev->privdata;
	size_t block_size = bdev->block_size;
	size_t write_addr = (flashdisk->begin_block + blkno) * block_size;

	size_t done = 0; // written blocks

	for(; done < count; done++){
		size_t offset = done * block_size;
		SpiFlashOpResult res = spi_flash_write(write_addr + offset, buffer + offset, block_size);
		if(res)
			break;
	}

	return count;
}

static int flash_ioctl(struct block_dev *bdev, int cmd, void *args, size_t size) {
    flashdisk_t *flashdisk = (flashdisk_t *) bdev->privdata;

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return flashdisk->blocks;

	case IOCTL_GETBLKSIZE:
		return bdev->block_size;
	}
	return -ENOSYS;
}
