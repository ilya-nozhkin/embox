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
#include <util/math.h>

#include <mem/misc/pool.h>

#include <embox/unit.h>
#include <drivers/esp8266/spi_api_impl.h>
#include <drivers/block_dev.h>
#include <drivers/block_dev/esp8266/flashdisk.h>

/* flashfs filesystem description pool */
POOL_DEF(flashfs_fs_pool, struct flashfs_fs_info, OPTION_GET(NUMBER,flashfs_descriptor_quantity));

/* flashfs file description pool */
POOL_DEF(flashfs_file_pool, struct flashfs_file_info, OPTION_GET(NUMBER,inode_quantity));

INDEX_DEF(flashfs_file_idx,0,OPTION_GET(NUMBER,inode_quantity));

/* define sizes in 16 blocks (since 1 block is 4kbyte) */
#define MAX_FILE_SIZE OPTION_GET(NUMBER,flashfs_file_size)
#define FILESYSTEM_SIZE (MAX_FILE_SIZE * OPTION_GET(NUMBER,inode_quantity))

#define FLASHFS_DEV "/dev/ffs#"
#define FLASHFS_DIR "/flash"

static char sector_buff[FLASH_BLOCK_SIZE];
static char flashfs_dev[] = FLASHFS_DEV;

static int flashfs_format(void *dev){
	node_t *dev_node;
	struct nas *dev_nas;
	struct node_fi *dev_fi;

	if (NULL == (dev_node = dev)) {
		return -ENODEV;/*device not found*/
	}

	if(!node_is_block_dev(dev_node)) {
		return -ENODEV;
	}
	dev_nas = dev_node->nas;
	dev_fi = dev_nas->fi;

	if(MAX_FILE_SIZE > block_dev(dev_fi->privdata)->size) {
		return -ENOSPC;
	}
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

static size_t flashfs_read(struct file_desc *desc, void *buf, size_t size){
	struct nas *nas = desc->node->nas;
	flashfs_fs_info_t *fsi = nas->fs->fsi;
	flashfs_file_info_t *fi = nas->fi->privdata;
	uint32_t start_block = fi->index * fsi->block_per_file;

	void *pbuf, *ebuf;

	pbuf = buf;
	ebuf = buf + min(nas->fi->ni.size - desc->cursor, size);

	size_t counter = 0;

	char debug_buff[FLASH_BLOCK_SIZE+1];
	debug_buff[FLASH_BLOCK_SIZE] = '\0';

	while (pbuf < ebuf) {
		blkno_t blk = desc->cursor / fsi->block_size;
		int offset = desc->cursor % fsi->block_size;
		int read_n;

		assert (blk < fsi->block_per_file);
		assert (blk < fsi->numblocks);

		assert(sizeof(sector_buff) == fsi->block_size);

		blkno_t blk_to_read = blk + start_block;

		if(1 != flashfs_read_sector(nas, sector_buff, 1, blk_to_read)) {
			break;
		}

		memcpy(debug_buff, sector_buff, FLASH_BLOCK_SIZE);

		read_n = min(fsi->block_size - offset, ebuf - pbuf);
		memcpy (pbuf, sector_buff + offset, read_n);

		desc->cursor += read_n;
		pbuf += read_n;

		counter++;
	}

	return pbuf - buf;
}

static size_t flashfs_write(struct file_desc *desc, void *buf, size_t size){
	flashfs_file_info_t *fi;
	size_t len;
	size_t current, cnt;
	uint32_t end_pointer;
	blkno_t blk;
	uint32_t bytecount;
	uint32_t start_block;
	struct nas *nas;
	struct flashfs_fs_info *fsi;

	nas = desc->node->nas;
	fi = (flashfs_file_info_t *)nas->fi->privdata;
	fsi = nas->fs->fsi;

	bytecount = 0;

	fi->pointer = desc->cursor;
	len = size;
	end_pointer = fi->pointer + len;
	start_block = fi->index * fsi->block_per_file;


	while(1) {
		if(0 == fsi->block_size) {
			break;
		}
		blk = fi->pointer / fsi->block_size;
		/* check if block over the file */
		if(blk >= fsi->block_per_file) {
			bytecount = 0;
			break;
		}
		else {
			blk += start_block;
		}
		/* calculate pointer in scratch buffer */
		current = fi->pointer % fsi->block_size;

		/* set the counter how many bytes written in block */
		if(end_pointer - fi->pointer > fsi->block_size) {
			if(current) {
				cnt = fsi->block_size - current;
			}
			else {
				cnt = fsi->block_size;
			}
		}
		else {
			cnt = end_pointer - fi->pointer;
			/* over the block ? */
			if((current + cnt) > fsi->block_size) {
				cnt -= (current + cnt) % fsi->block_size;
			}
		}

		/* one 4096-bytes block read operation */
		if(1 != flashfs_read_sector(nas, sector_buff, 1, blk)) {
			bytecount = 0;
			break;
		}
		/* set new data in block */
		memcpy (sector_buff + current, buf, cnt);

		/* write one block to device */
		if(1 != flashfs_write_sector(nas, sector_buff, 1, blk)) {
			bytecount = 0;
			break;
		}
		bytecount += cnt;
		buf = (void*) (((uint8_t*) buf) + cnt);
		/* shift the pointer */
		fi->pointer += cnt;
		if(end_pointer <= fi->pointer) {
			break;
		}
	}
	/* if we write over the last EOF, set new filelen */
	if (nas->fi->ni.size < fi->pointer) {
		nas->fi->ni.size = fi->pointer;
	}

	desc->cursor = fi->pointer;
	return bytecount;
}

static flashfs_file_info_t *flashfs_create_file(struct nas *nas){
	flashfs_file_info_t *fi;
	size_t fi_index;

	fi = pool_alloc(&flashfs_file_pool);
	if(!fi){
		return NULL;
	}

	fi_index = index_alloc(&flashfs_file_idx, INDEX_MIN);
	if(fi_index == INDEX_NONE){
		pool_free(&flashfs_file_pool, fi);
		return NULL;
	}

	fi->index = fi_index;
	nas->fi->ni.size = fi->pointer = 0;

	return fi;
}

static int flashfs_create(struct node *parent_node, struct node* node){
	struct nas *nas;

	nas = node->nas;

	if(!node_is_directory(node)){
		if(!(nas->fi->privdata = flashfs_create_file(nas))){
			return -ENOMEM;
		}
	}

	nas->fs = parent_node->nas->fs;

	return 0;
}

static int flashfs_delete(struct node *node) {
	struct flashfs_file_info *fi;
	struct nas *nas;

	nas = node->nas;
	fi = nas->fi->privdata;

	if(!node_is_directory(node)) {
		index_free(&flashfs_file_idx, fi->index);
		pool_free(&flashfs_file_pool, fi);
	}

	vfs_del_leaf(node);

	return 0;
}

static int flashfs_truncate(struct node *node, off_t length){
	struct nas *nas = node->nas;

	if(length > MAX_FILE_SIZE){
		return -EFBIG;
	}

	nas->fi->ni.size = length;

	return 0;
}

static struct fsop_desc flashfs_fsop = {
	.init = flashfs_init,
	.format = flashfs_format,
	.mount = flashfs_mount,
	.create_node = flashfs_create,
	.delete_node = flashfs_delete,
	.truncate = flashfs_truncate,
};

static struct fs_driver flashfs_driver = {
	.name = "flashfs",
	.file_op = &flashfs_fop,
	.fsop = &flashfs_fsop
};

DECLARE_FILE_SYSTEM_DRIVER(flashfs_driver);
