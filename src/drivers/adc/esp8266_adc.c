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
	uint32_t reserved1 : 18;
	uint32_t completed : 3;
	uint32_t reserved2 : 6;
};

#define SAR_CFG ((volatile struct sar_cfg*) 0x60000D50)
#define SAR_DATA ((volatile uint32_t*) 0x60000D80)

#define SAR_SAMPLE_RATE 375000
#define SAMPLE_RATE 35
#define MAX_MEASURES 8

extern void rom_sar_init(void);

static int esp8266_adc_init(void) {
	//sar_init(MAX_MEASURES, SAR_SAMPLE_RATE / SAMPLE_RATE);
	rom_sar_init();
	return 0;
}

EMBOX_UNIT_INIT(esp8266_adc_init);

void adc_start(uint32_t measures) {
	SAR_CFG->new_measure = 0;
	SAR_CFG->completed = 0;
	SAR_CFG->num_measures = measures;
	SAR_CFG->new_measure = 1;
}

uint32_t adc_completed(void) {
	return SAR_CFG->completed;
}

uint32_t *adc_get_raw(void) {
	return SAR_DATA;
}

uint32_t adc_get_corrected(void)
{
	uint32_t result = 0;
	for (int i = 0; i < SAR_CFG->completed; i++) {
		int x = SAR_DATA[i];
		int z = (x & 0xFF) - 21;
		x &= 0x700;
		if(z > 0) x = ((z * 279) >> 8) + x;
		result += x;
	}
	return result;
}
