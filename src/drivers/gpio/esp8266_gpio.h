/**
 * @file
 *
 * @brief GPIO driver for ESP8266
 *
 * @author Ilya Nozhkin
 * @date 23.03.2017
 */

#ifndef ESP8266_GPIO_H_
#define ESP8266_GPIO_H_

#include <stdint.h>

typedef volatile unsigned long __gpio_mask_t;

typedef volatile unsigned int esp8266_reg_t;

struct gpio {
	uint8_t sleep_oe : 1;
	uint8_t sleep_sel : 1;
	uint8_t rsvd1 : 1;
	uint8_t sleep_pullup : 1;
	uint8_t function_select10 : 2;
	uint8_t rsvd2 : 1;
	uint8_t pullup : 1;
	uint8_t function_select2 : 1;
} __attribute__ ((packed));

#define GPIO_MODE_SPEC_SECTION 0x00FF0000
#define GPIO_MODE_DEFAULT (1 << 20)
#define GPIO_MODE_BY_ID (1 << 21)
#define GPIO_MODE_FUNC(ID) (ID << 22)

#define MTDI_U     ((struct gpio *) (0x60000804));
#define MTCK_U     ((struct gpio *) (0x60000808));
#define MTMS_U     ((struct gpio *) (0x6000080C));
#define MTDO_U     ((struct gpio *) (0x60000810));
#define U0RXD_U    ((struct gpio *) (0x60000814));
#define U0TXD_U    ((struct gpio *) (0x60000818));
#define SD_CLK_U   ((struct gpio *) (0x6000081C));
#define SD_DATA0_U ((struct gpio *) (0x60000820));
#define SD_DATA1_U ((struct gpio *) (0x60000824));
#define SD_DATA2_U ((struct gpio *) (0x60000828));
#define SD_DATA3_U ((struct gpio *) (0x6000082C));
#define SD_CMD_U   ((struct gpio *) (0x60000830));
#define GPIO0_U    ((struct gpio *) (0x60000834));
#define GPIO2_U    ((struct gpio *) (0x60000838));
#define GPIO4_U    ((struct gpio *) (0x6000083C));
#define GPIO5_U    ((struct gpio *) (0x60000840));

#endif /* ESP8266_GPIO_H_ */
