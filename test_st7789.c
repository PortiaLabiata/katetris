#include "ch.h"
#include "chschd.h"
#include "chthreads.h"
#include "hal.h"
#include "osal.h"

#define GPIO_LED 13
#define DC_PORT GPIOA
#define DC_PIN 3
#define RST_PORT GPIOA
#define RST_PIN 2
#define BLK_PORT GPIOA
#define BLK_PIN 1

#define disp_set_cd() palSetPad(DC_PORT, DC_PIN) // data
#define disp_clear_cd() palClearPad(DC_PORT, DC_PIN) // command
#define disp_set_blk() palSetPad(BLK_PORT, BLK_PIN)
#define disp_clear_blk() palClearPad(BLK_PORT, BLK_PIN)
#define disp_set_rst() palSetPad(RST_PORT, RST_PIN)
#define disp_clear_rst() palClearPad(RST_PORT, RST_PIN)

static inline void disp_hreset() {
	disp_clear_rst();
	osalThreadSleepMilliseconds(10); \
	disp_set_rst(); \
	osalThreadSleepMilliseconds(110); \
}

#define delay(t) osalThreadSleepMilliseconds(t)

static THD_WORKING_AREA(wa_blink, 128);
static THD_FUNCTION(thd_blink, arg) {
	while (1) {
		palTogglePad(GPIOC, GPIO_LED);
		delay(500);
	}
}

int send_cmd(uint8_t cmd) {
	disp_clear_cd();
	return spiSend(&SPID1, 1, &cmd);
}

int send_data(uint16_t data) {
	disp_set_cd();
	return spiSend(&SPID1, 2, &data);
}

int send_datab(uint8_t data) {
	disp_set_cd();
	return spiSend(&SPID1, 1, &data);
}

int main(void) {
	halInit();
	chSysInit();

	palSetPadMode(GPIOC, GPIO_LED, PAL_MODE_OUTPUT_PUSHPULL);

	palSetPadMode(GPIOA, 9, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	palSetPadMode(GPIOA, 10, PAL_MODE_INPUT);

	osalDbgAssert(sdStart(&SD1, NULL) == MSG_OK, "SD init");

	palSetPadMode(GPIOA, 5, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	palSetPadMode(GPIOA, 7, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	palSetPadMode(RST_PORT, RST_PIN, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(DC_PORT, DC_PIN, PAL_MODE_OUTPUT_PUSHPULL);

	static const SPIConfig cfg = {
		.circular = false,
		.data_cb = NULL,
		.error_cb = NULL,
		.slave = false,
		.ssport = GPIOA,
		.sspad = 4,
		.cr1 = SPI_CR1_CPOL | SPI_CR1_CPHA,
		.cr2 = 0
	};
	osalDbgAssert(spiStart(&SPID1, &cfg) == MSG_OK, "SPI start");

	(void)chThdCreateStatic(wa_blink, sizeof(wa_blink), NORMALPRIO,
					thd_blink, NULL);
	while (1) {
		msg_t status = MSG_OK;
		disp_hreset();	
		// Soft reset
		status |= send_cmd(0x01);
		delay(110);
		// Sleep out
		status |= send_cmd(0x11);
		delay(110);
		// Disp on
		status |= send_cmd(0x29);
		delay(10);
		// Disp colmode
		status |= send_cmd(0x3A);
		status |= send_datab(0b01010101);
		delay(10);

		send_cmd(0x2C);
		for (int i = 0; i < 1000; i++) {
			status |= send_data(0x0000);
		}
		osalDbgAssert(status == MSG_OK, "Runtime");
		delay(100);
	}
}
