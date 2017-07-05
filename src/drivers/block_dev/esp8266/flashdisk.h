/**
 * @file
 * @brief
 *
 * @date 05.07.2017
 * @author Egor Shitov
 */

#ifndef DRIVERS_BLOCKDEV_ESP8266_FLASHDISK_H_
#define DRIVERS_BLOCKDEV_ESP8266_FLASHDISK_H_

#include <stddef.h>

typedef struct flashdisk {
    struct block_dev    *bdev;
    size_t              start_sector;
    size_t              sectors;
    int                 idx;
} flashdisk_t;

struct flashdisk *flashdisk_create(char *path, size_t size);
struct flashdisk *flashdisk_get_param(char *path);
int flashdisk_delete(const char *name);

#endif /* DRIVERS_BLOCKDEV_ESP8266_FLASHDISK_H_ */
