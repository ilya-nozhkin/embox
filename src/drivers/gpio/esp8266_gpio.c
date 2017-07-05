/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @date    23.03.2017
 */
 
//TODO: input: schmitt
//TODO: output: alternate
//TODO: interruptions
 
#include <assert.h>

#include <drivers/gpio.h>

#include <hal/reg.h>
#include <embox/unit.h>

#define GPIO_OUT_W1TS (*((volatile uint32_t*) 0x60000304))
#define GPIO_OUT_W1TC (*((volatile uint32_t*) 0x60000308))

#define GPIO_ENABLE_W1TS (*((volatile uint32_t*) 0x60000310))
#define GPIO_ENABLE_W1TC (*((volatile uint32_t*) 0x60000314))

#define GPIO_IN (*((volatile uint32_t*) 0x60000318))

#define GPIO_FUNC_MAP 0xFFFFF0CCul
#define DEFAULT_FUNC_MAP 0x3333111111000200ull
#define REGISTER_TO_ID_MAP 0x5420BA987613FEDCull
#define ID_TO_REGISTER_MAP 0x3210BA9876FE4D5Cull

#define REGISTER_TO_ID(X) ((REGISTER_TO_ID_MAP >> ((uint32_t) X - GPIO_START_ADDRESS)) & 0xF)
#define ID_TO_REGISTER(X) ((struct gpio *) (((uint32_t) ((ID_TO_REGISTER_MAP >> (X << 2)) & 0xF) << 2) + GPIO_START_ADDRESS))
#define ID_TO_PIN(X) ((struct gpio_pin *) (GPIO_PIN_START_ADDRESS + (X << 2)))

EMBOX_UNIT_INIT(esp8266_gpio_init);

static int esp8266_gpio_init(void) {
	*((uint32_t*) MTDI_U) = 0;
	*((uint32_t*) MTCK_U) = 0;
	*((uint32_t*) MTMS_U) = 0;
	*((uint32_t*) MTDO_U) = 0;
	*((uint32_t*) U0RXD_U) = 0;
	*((uint32_t*) U0TXD_U) = 0;
	/* *((uint32_t*) SD_CLK_U) = 0;
	*((uint32_t*) SD_DATA0_U) = 0;
	*((uint32_t*) SD_DATA1_U) = 0;
	*((uint32_t*) SD_DATA2_U) = 0;
	*((uint32_t*) SD_DATA3_U) = 0;
	*((uint32_t*) SD_CMD_U) = 0;*/
	*((uint32_t*) GPIO0_U) = 0;
	*((uint32_t*) GPIO2_U) = 0;
	*((uint32_t*) GPIO4_U) = 0;
	*((uint32_t*) GPIO5_U) = 0;
	
    //GPIO_OUT_W1TC = 0xFFFF;
    //GPIO_ENABLE_W1TC = 0xFFFF;
    
	return 0;
}

static inline void set_function(struct gpio *gpiop, uint8_t function) {
	gpiop->function_select10 = function & 0b11;
	gpiop->function_select2 = (function >> 2) & 0b1;
}

static inline void gpio_enable(uint8_t pin_number) {
	GPIO_ENABLE_W1TS |= 1 << pin_number;
}

static inline void gpio_disable(uint8_t pin_number) {
	GPIO_ENABLE_W1TC |= 1 << pin_number;
}

static inline void gpio_set(uint8_t pin_number) {
	GPIO_OUT_W1TS |= 1 << pin_number;
}

static inline void gpio_clear(uint8_t pin_number) {
	GPIO_OUT_W1TC |= 1 << pin_number;
}

static inline uint8_t gpio_get(uint8_t pin_number) {
	return (GPIO_IN >> pin_number) & 0b1;
}

struct gpio *gpio_by_num(int num_port) {
	return ID_TO_REGISTER(num_port);
}

int gpio_settings(struct gpio *gpiop, gpio_mask_t mask, int mode) {
	assert(gpiop);
	
	uint8_t id = REGISTER_TO_ID(gpiop);
	struct gpio_pin *pin = ID_TO_PIN(id);
	
	if (mode & GPIO_MODE_DEFAULT) {
		uint8_t function = (DEFAULT_FUNC_MAP >> (id << 2)) & 0xF;
		set_function(gpiop, function);
	} else if (mode & (GPIO_MODE_INPUT | GPIO_MODE_OUTPUT)) {
		uint8_t function = (GPIO_FUNC_MAP >> (id << 1)) & 0b11;
		set_function(gpiop, function);
		
		if (mode & GPIO_MODE_INPUT) {
			gpio_disable(id);
		} else {
			gpio_enable(id);
		}
	} else if (mode & (GPIO_MODE_BY_ID)) {
		uint8_t function = (mode >> GPIO_MODE_FUNC_ID_POS) & 0b111;
		set_function(gpiop, function);
	}
	
	if (mode & GPIO_MODE_IN_PULL_UP) {
		gpiop->pullup = 1;
	} else {
		gpiop->pullup = 0;
	}
	
	if (mode & GPIO_MODE_OUT_OPEN_DRAIN) {
		pin->driver = 1;
	} else {
		pin->driver = 0;
	}
	
	return 0;
}

void gpio_set_level(struct gpio *gpiop, gpio_mask_t mask, char level) {
	uint8_t id = REGISTER_TO_ID(gpiop);
	if (level) {
		gpio_set(id);
	} else {
		gpio_clear(id);
	}
}

gpio_mask_t gpio_get_level(struct gpio *gpiop, gpio_mask_t mask) {
	uint8_t id = REGISTER_TO_ID(gpiop);
	return gpio_get(id);
}
