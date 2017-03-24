/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @date    23.03.2017
 */
 
#include <assert.h>

#include <drivers/gpio.h>

#include <hal/reg.h>
#include <embox/unit.h>

#define GPIO_FUNC_MAP 0xFFFFF0CCul
#define DEFAULT_FUNC_MAP 0x3333111111000200ull

EMBOX_UNIT_INIT(esp8266_gpio_init);

static int esp8266_gpio_init(void) {
	MTDI_U = 1 << 7;
	MTCK_U = 1 << 7;
	MTMS_U = 1 << 7;
	MTDO_U = 1 << 7;
	U0RXD_U = 1 << 7;
	U0TXD_U = 1 << 7;
	SD_CLK_U = 0;
	SD_DATA0_U = 0;
	SD_DATA1_U = 0;
	SD_DATA2_U = 0;
	SD_DATA3_U = 0;
	SD_CMD_U = 0;
	GPIO0_U = 1 << 7;
	GPIO2_U = 1 << 7;
	GPIO4_U = 0;
	GPIO5_U = 0;
	
    GPIO_OUT_W1TC = 0xFFFF;
    GPIO_ENABLE_W1TC = 0xFFFF;
    
	return 0;
}

int gpio_settings(struct gpio *gpio, gpio_mask_t mask, int mode) {
	assert(gpio);
	
	if (mode & GPIO_MODE_DEFAULT) {
		
	}
}
