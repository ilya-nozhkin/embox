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

typedef volatile uint8_t __gpio_mask_t;

typedef volatile uint32_t esp8266_reg_t;

struct gpio {
	esp8266_reg_t sleep_oe : 1;
	esp8266_reg_t sleep_sel : 1;
	esp8266_reg_t rsvd1 : 1;
	esp8266_reg_t sleep_pullup : 1;
	esp8266_reg_t function_select10 : 2;
	esp8266_reg_t rsvd2 : 1;
	esp8266_reg_t pullup : 1;
	esp8266_reg_t function_select2 : 1;
};

struct gpio_pin {
	esp8266_reg_t source : 1;
	esp8266_reg_t reserved0 : 1;
	esp8266_reg_t driver : 1; // 1 - open drain, 0 - normal
	esp8266_reg_t reserved1 : 4;
	esp8266_reg_t int_type : 3;
	esp8266_reg_t wakeup_enable : 1;
};

#define GPIO_MODE_SPEC_SECTION 0x00FF0000
#define GPIO_MODE_DEFAULT (1 << 20)
#define GPIO_MODE_BY_ID (1 << 21)

#define GPIO_MODE_FUNC_ID_POS 22
#define GPIO_MODE_FUNC(ID) (ID << GPIO_MODE_FUNC_ID_POS)

#define GPIO_START_ADDRESS 0x60000804
#define GPIO_PIN_START_ADDRESS 0x60000328

#define MTDI_U     ((struct gpio *) (0x60000804))
#define MTCK_U     ((struct gpio *) (0x60000808))
#define MTMS_U     ((struct gpio *) (0x6000080C))
#define MTDO_U     ((struct gpio *) (0x60000810))
#define U0RXD_U    ((struct gpio *) (0x60000814))
#define U0TXD_U    ((struct gpio *) (0x60000818))
#define SD_CLK_U   ((struct gpio *) (0x6000081C))
#define SD_DATA0_U ((struct gpio *) (0x60000820))
#define SD_DATA1_U ((struct gpio *) (0x60000824))
#define SD_DATA2_U ((struct gpio *) (0x60000828))
#define SD_DATA3_U ((struct gpio *) (0x6000082C))
#define SD_CMD_U   ((struct gpio *) (0x60000830))
#define GPIO0_U    ((struct gpio *) (0x60000834))
#define GPIO2_U    ((struct gpio *) (0x60000838))
#define GPIO4_U    ((struct gpio *) (0x6000083C))
#define GPIO5_U    ((struct gpio *) (0x60000840))

#endif /* ESP8266_GPIO_H_ */
