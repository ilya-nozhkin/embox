/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @date    22.04.2017
 */

#include <drivers/adc.h>
#include <stdint.h>

#include <embox/unit.h>

struct sar_cfg {
	uint32_t reserved0 : 1;
	uint32_t new_measure : 1;
	uint32_t num_measures : 3;
	uint32_t reserved1 : 3;
	uint32_t clk_div : 15;
	uint32_t waiting : 3;
	uint32_t reserved3 : 6;
};

struct sar_tim {
	uint32_t value : 24;
	uint32_t reserved : 8;
};

struct sar_cfg1 {
	uint32_t reserved0 : 21;
	uint32_t value : 1;
	uint32_t reserved1 : 10;
};

struct sar_cfg2 {
	uint32_t value : 1;
	uint32_t reserved0 : 31;
};

#define SAR_CFG ((volatile struct sar_cfg*) 0x60000D50)
#define SAR_TIM1 ((volatile struct sar_tim*) 0x60000D54)
#define SAR_TIM2 ((volatile struct sar_tim*) 0x60000D58)
#define SAR_CFG1 ((volatile struct sar_cfg1*) 0x60000D5C)
#define SAR_CFG2 ((volatile struct sar_cfg2*) 0x60000D60)
#define SAR_DATA ((volatile uint32_t*) 0x60000D80)

#define IO_RTC_4 (*((volatile uint32_t*) 0x60000710))

#define CLK_DIV 8
#define WIN_CNT 8

static inline void dport_magic(void) {
	*((volatile uint32_t*) 0x3FF00018) |= 0x038f0000;
}

static inline void hdrf_magic(void) {
	*((volatile uint32_t*) 0x600005e8) |= 0x01800000;
}

extern void rom_sar_init(void);
extern void rom_i2c_writeReg(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t pData);
extern void rom_i2c_writeReg_Mask(uint8_t block, uint8_t host_id, uint8_t reg_add,
                                  uint8_t msb, uint8_t lsb, uint8_t indata);

static uint32_t current_measures = 0;

static int esp8266_adc_init(void) {
	IO_RTC_4 = (IO_RTC_4 & (~1)) | 0x06000000; //disable WiFi and enable SAR
	*((volatile uint32_t*) 0x60000834) = 0; //TODO: GPIO0_MUX = 0

	rom_sar_init();

	rom_i2c_writeReg_Mask(98, 1, 3, 7, 4, 15);

	dport_magic();
	hdrf_magic();

	return 0;
}

EMBOX_UNIT_INIT(esp8266_adc_init);


void adc_start(struct ad_converter *adc, uint32_t measures) {
	//WTF??? Copy-pasted from pvvx's SDK
	SAR_TIM1->value = (CLK_DIV * 5 + ((CLK_DIV - 1) << 16) + ((CLK_DIV - 1) << 8) - 1);
	SAR_TIM2->value = (CLK_DIV * 11 + ((CLK_DIV * 3 - 1) << 8) + ((CLK_DIV * 11 - 1) << 16) - 1);

	SAR_CFG1->value = 1;

	SAR_CFG2->value = 0;
	SAR_CFG2->value = 1;

	SAR_CFG->num_measures = measures - 1;
	SAR_CFG->clk_div = CLK_DIV;
	SAR_CFG->waiting = 0;

	rom_i2c_writeReg_Mask(108, 2, 0, 5, 5, 1);

	current_measures = measures;

	SAR_CFG->new_measure = 0;
	SAR_CFG->new_measure = 1;
}

void adc_stop(struct ad_converter *adc) {
	rom_i2c_writeReg_Mask(108, 2, 0, 5, 5, 0);
}

uint32_t adc_waiting(struct ad_converter *adc) {
	return SAR_CFG->waiting;
}

uint32_t *adc_get_raw(struct ad_converter *adc) {
	return SAR_DATA;
}

uint32_t adc_get_corrected(struct ad_converter *adc){
	while (adc_waiting(adc));

	uint32_t result = 0;
	for (int i = 0; i < current_measures; i++) {
		int x = SAR_DATA[i];
		int z = (x & 0xFF) - 21;
		x &= 0x700;
		if(z > 0) x = ((z * 279) >> 8) + x;
		result += x;
	}

	return MAX_ADC_VALUE - result / current_measures;
}

struct ad_converter *adc_by_id(uint32_t adc_id){
	if (adc_id == 0){
		struct ad_converter adc = {.id = 0};
		return &adc;
	}
	else{
		return 0;
	}
}
