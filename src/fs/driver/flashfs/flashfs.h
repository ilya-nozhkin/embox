/**
 * @file
 * @brief file system based on flashdisk module
 *
 * @date 05.07.2017
 * @author Egor Shitov
 */

#ifndef FS_DRIVER_FLASHFS_H_
#define FS_DRIVER_FLASHFS_H_

#include <stdint.h>

/* DOS attribute bits  */
#define ATTR_READ_ONLY	0x01
#define ATTR_HIDDEN		0x02
#define ATTR_SYSTEM		0x04
#define ATTR_VOLUME_ID	0x08
#define ATTR_DIRECTORY	0x10
#define ATTR_ARCHIVE	0x20
#define ATTR_LONG_NAME	(ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | \
ATTR_VOLUME_ID)

typedef struct flashfs_fs_info {
	uint32_t numblocks;			/* number of block in volume */
	uint32_t block_size;		/* size of block */
	uint32_t block_per_file;	/* max number of blocks filesize*/
} ramfs_fs_info_t;

typedef struct flashfs_file_info {
	int     index;		        /* number of file in FS*/
	//int     mode;				/* mode in which this file was opened */
	uint32_t pointer;			/* current (BYTE) pointer */

#endif /* FS_DRIVER_FLASHFS_H_ */
