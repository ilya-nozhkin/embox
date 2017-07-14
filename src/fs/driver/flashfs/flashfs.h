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

#define FLAG_EXIST		0xF0F0F0F0
#define FLAG_DELETED	0x0

typedef struct flashfs_fs_info {
	uint32_t numblocks;			/* number of block in volume */
	uint32_t block_size;		/* size of block */
	uint32_t block_per_file;	/* max number of blocks filesize*/
} flashfs_fs_info_t;

typedef struct flashfs_file_info {
	uint32_t index;		        /* number of file in FS*/
	uint32_t mode;				/* mode in which this file was opened */
	uint32_t pointer;			/* current (BYTE) pointer */
} flashfs_file_info_t;

typedef struct flashfs_store_info {
	uint32_t 	exist;			/* helps find out if this is garbage */
	char		name[NAME_MAX]; /* name itself */
	char 		ph[28];			/* placeholder */
} flashfs_store_info_t;

#endif /* FS_DRIVER_FLASHFS_H_ */
