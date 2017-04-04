/**
 * @file
 * @brief ESP8266's uart interface
 *
 * @author  Ilya Nozhkin
 * @date    22.03.2017
 */

#include <hal/reg.h>
#include <drivers/diag.h>
#include <drivers/diag.h>

#include <drivers/gpio.h>

struct uart_clkdiv {
	uint32_t value : 20;
	uint32_t reserved : 12;
};

struct uart_autobaud {
	uint32_t autobaud_en : 1;
	uint32_t glitch_filt_ro : 7;
	uint32_t glitch_filt_rw : 8;
	uint32_t reserved : 16;
};

struct uart_status {
	uint32_t rxfifo_cnt : 8;
	uint32_t reserved0 : 5;
	uint32_t dsrn : 1;
	uint32_t ctsn : 1;
	uint32_t rxd : 1;
	uint32_t txfifo_cnt : 8;
	uint32_t reserved1 : 5;
	uint32_t dtrn : 1;
	uint32_t rtsn : 1;
	uint32_t txd : 1;
};

struct uart_conf0 {
	uint32_t parity : 1;
	uint32_t parity_en : 1;
	uint32_t bit_num : 2;
	uint32_t stop_bit_num : 2;
	uint32_t sw_rts : 1;
	uint32_t sw_dtr : 1;
	uint32_t txd_brk : 1;
	uint32_t reserved0 : 5;
	uint32_t uart_loopback : 1;
	uint32_t tx_flow_en : 1;
	uint32_t reserved1 : 1;
	uint32_t rxfifo_rst : 1;
	uint32_t txfifo_rst : 1;
	uint32_t uart_rxd_inv : 1;
	uint32_t uart_cts_inv : 1;
	uint32_t uart_dsr_inv : 1;
	uint32_t uart_txd_inv : 1;
	uint32_t uart_rts_inv : 1;
	uint32_t uart_dtr_inv : 1;
	uint32_t reserved2 : 7;
};

struct uart_conf1 {
	uint32_t rxfifo_full_thrhd : 7;
	uint32_t reserved0 : 1;
	uint32_t txfifo_empty_thrhd : 7;
	uint32_t reserved1 : 1;
	uint32_t rx_flow_thrhd : 7;
	uint32_t rx_flow_en : 1;
	uint32_t rx_tout_thrhd : 7;
	uint32_t rx_tout_en : 1;
};

#define UART_ID 0
#define UART_BASE (0x60000000 + UART_ID*0xf00)

#define UART_FIFO (*((volatile uint32_t*) (UART_BASE + 0x0)))

#define UART_CLKDIV (((volatile struct uart_clkdiv*) (UART_BASE + 0x14)))
#define UART_AUTOBAUD (((volatile struct uart_autobaud*) (UART_BASE + 0x18)))
#define UART_STATUS (((volatile struct uart_status*) (UART_BASE + 0x1C)))
#define UART_CONF0 (((volatile struct uart_conf0*) (UART_BASE + 0x20)))
#define UART_CONF1 (((volatile struct uart_conf1*) (UART_BASE + 0x24)))

#define UART_CLK_FREQ 52*1024*1024
#define BAUD_RATE OPTION_GET(NUMBER, baud_rate)

static int esp8266_uart_diag_init(const struct diag *diag) {
	gpio_settings(U0TXD_U, 0, GPIO_MODE_DEFAULT);
	gpio_settings(U0RXD_U, 0, GPIO_MODE_DEFAULT);
	
	/**((uint32_t*) UART_AUTOBAUD) = 0;
	*((uint32_t*) UART_CONF0) = 0;
	*((uint32_t*) UART_CONF1) = 0;*/
	
	UART_CONF0->rxfifo_rst = 1;
	UART_CONF0->txfifo_rst = 1;
	UART_CONF0->rxfifo_rst = 0;
	UART_CONF0->txfifo_rst = 0;
	
	UART_CONF0->bit_num = 0b11;
	UART_CONF0->stop_bit_num = 0b1;
	UART_CONF0->parity_en = 0;
	
	UART_CLKDIV->value = UART_CLK_FREQ / BAUD_RATE;
	
	return 0;
}

static void esp8266_uart_diag_putc(const struct diag *diag, char ch) {
	while (UART_STATUS->txfifo_cnt > 126) {
		asm volatile ("nop");
	}
	UART_FIFO = (uint32_t) ch;
}

static char esp8266_uart_diag_getc(const struct diag *diag) {
	return UART_FIFO & 0xFF;
}

static int esp8266_uart_diag_kbhit(const struct diag *diag) {
	return UART_STATUS->rxfifo_cnt;
}

DIAG_OPS_DECLARE(
		.init = esp8266_uart_diag_init,
		.putc = esp8266_uart_diag_putc,
		.getc = esp8266_uart_diag_getc,
		.kbhit = esp8266_uart_diag_kbhit,
);
