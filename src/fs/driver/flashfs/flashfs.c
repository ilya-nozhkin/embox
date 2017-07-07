/**
 * @file
 * @brief Very simple file system
 *
 * @date 05.07.2017
 * @author Egor Shitov
 */

#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <fs/flashfs.h>
#include <fs/file_system.h>
#include <fs/file_desc.h>
#include <fs/file_operation.h>
#include <fs/path.h>

#include <util/err.h>
#include <util/indexator.h>

#include <mem/misc/pool.h>

#include <embox/unit.h>
#include <drivers/block_dev.h>
#include <drivers/esp8266/flashdisk.h>

/* flashfs filesystem description pool */
POOL_DEF(flashfs_fs_pool, struct flashfs_fs_info, OPTION_GET(NUMBER,flashfs_descriptor_quantity));

/* flashfs file description pool */
POOL_DEF(flashfs_file_pool, struct flashfs_file_info, OPTION_GET(NUMBER,inode_quantity));

INDEX_DEF(flashfs_file_idx,0,OPTION_GET(NUMBER,inode_quantity));

/* define sizes in 16 blocks (since 1 block is 4kbyte) */
#define MAX_FILE_SIZE OPTION_GET(NUMBER,flashfs_file_size)
#define FILESYSTEM_SIZE (MAX_FILE_SIZE * OPTION_GET(NUMBER,inode_quantity))

#define FLASHFS_DEV "/dev/ffs#"
#define FLASHFS_DIR "/"

static char flashfs_dev[] = FLASHFS_DEV;

static int flashfs_format(void *dev){
	return 0;
}

static int flashfs_mount(void *dev, void *dir){
	struct node *dir_node, *dev_node;
	struct nas *dir_nas, *dev_nas;
	struct flashfs_file_info *fi;
	struct flashfs_fs_info *fsi;
	struct node_fi *dev_fi;

	dev_node = dev;
	dev_nas = dev_node->nas;
	dir_node = dir;
	dir_nas = dir_node->nas;

	if (NULL == (dev_fi = dev_nas->fi)) {
		return -ENODEV;
	}

	if (NULL == (dir_nas->fs = filesystem_create("flashfs"))) {
		return -ENOMEM;
	}
	dir_nas->fs->bdev = dev_fi->privdata;

	/* allocate this fs info */
	if(NULL == (fsi = pool_alloc(&flashfs_fs_pool))) {
		filesystem_free(dir_nas->fs);
		return -ENOMEM;
	}

	memset(fsi, 0, sizeof(struct flashfs_fs_info));
	dir_nas->fs->fsi = fsi;

	fsi->block_per_file = MAX_FILE_SIZE / FLASH_BLOCK_SIZE;
	fsi->block_size = FLASH_BLOCK_SIZE;
	fsi->numblocks = block_dev(dev_fi->privdata)->size / FLASH_BLOCK_SIZE;

	/* allocate this directory info */
	if(NULL == (fi = pool_alloc(&flashfs_file_pool))) {
		return -ENOMEM;
	}

	memset(fi, 0, sizeof(struct flashfs_file_info));
	fi->index = fi->mode = 0;
	fi->pointer = 0;
	dir_nas->fi->privdata = (void *) fi;

	return 0;
}

EMBOX_UNIT_INIT(flashfs_init);
static int flashfs_init(void){
	struct path dir_node;
	vfs_lookup(FLASHFS_DIR, &dir_node);

	if(dir_node.node == NULL){
		return -ENOENT;
	}

	flashdisk_t *flashdisk;
	if(err(flashdisk = flashdisk_create(flashfs_dev, FILESYSTEM_SIZE))){
		return err(flashdisk);
	}

	struct node *dev_node = flashdisk->bdev->dev_vfs_info;
	assert(dev_node != NULL);

	flashfs_format(dev_node);
	return flashfs_mount(dev_node, dir_node.node);
}

static struct idesc *flashfs_open(struct node *node, struct file_desc *file_desc, int flags);
static int    flashfs_close(struct file_desc *desc);
static size_t flashfs_read(struct file_desc *desc, void *buf, size_t size);
static size_t flashfs_write(struct file_desc *desc, void *buf, size_t size);

static struct kfile_operations flashfs_fop = {
	.open = flashfs_open,
	.close = flashfs_close,
	.read = flashfs_read,
	.write = flashfs_write,
};

static struct idesc *flashfs_open(struct node *node, struct file_desc *desc, int flags) {
	struct nas *nas;
	flashfs_file_info_t *fi;

	nas = node->nas;
	fi = (flashfs_file_info_t *)nas->fi->privdata;

	fi->pointer = desc->cursor;

	return &desc->idesc;
}

static int flashfs_close(struct file_desc *desc) {
	return 0;
}

static int flashfs_read_sector(struct nas *nas, char *buffer,
		uint32_t count, uint32_t sector) {
	struct flashfs_fs_info *fsi;

	fsi = nas->fs->fsi;

	if(0 > block_dev_read(nas->fs->bdev, (char *) buffer,
			count * fsi->block_size, sector)) {
		return -1;
	}
	else {
		return count;
	}
}

static int flashfs_write_sector(struct nas *nas, char *buffer,
		uint32_t count, uint32_t sector) {
	struct flashfs_fs_info *fsi;

	fsi = nas->fs->fsi;

	if(0 > block_dev_write(nas->fs->bdev, (char *) buffer,
			count * fsi->block_size, sector)) {
		return -1;
	}
	else {
		return count;
	}
}

/*static size_t flashfs_read(struct file_desc *desc, void *buf, size_t size){
	struct nas *nas = desc->node->nas;
	struct flashfs_fs_info *fsi = nas->fs->fsi;


}
static size_t flashfs_write(struct file_desc *desc, void *buf, size_t size)*/
