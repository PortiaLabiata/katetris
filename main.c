#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#include "display.h"

#define GPIO_LED 13
static THD_WORKING_AREA(wa_blink, 128);
static THD_FUNCTION(thd_blink, arg) {
	chRegSetThreadName("thd_blink");
	while (1) {
		palTogglePad(GPIOC, GPIO_LED);
		osalThreadSleepMilliseconds(500);
	}
}

static THD_WORKING_AREA(wa_video, 512);
static THD_FUNCTION(thd_video, arg) {
	chRegSetThreadName("thd_video");

	BaseSequentialStream *s = (BaseSequentialStream*)&SD1;
	if (disp_init()) {
		chprintf(s, "Couldn't init I2C\r\n");
	} else {
		while (1) {
			chprintf(s, "\r\nScanning...\r\n");
			i2c_scan_print(s, i2c_scan());
			osalThreadSleepMilliseconds(1000);
		}
	}
}

int main(int argc, char *argv[]) {
	chSysInit();
	halInit();

	palSetPadMode(GPIOC, GPIO_LED, PAL_MODE_OUTPUT_PUSHPULL);

	palSetPadMode(GPIOA, 9, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	palSetPadMode(GPIOA, 10, PAL_MODE_INPUT);

	palSetPadMode(GPIOB, 6, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
	palSetPadMode(GPIOB, 7, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);

	sdStart(&SD1, NULL);
	thread_t *thd = chThdCreateStatic(wa_blink, 
				sizeof(wa_blink),
				NORMALPRIO,
				thd_blink, NULL);
	(void)chThdCreateStatic(wa_video,
				sizeof(wa_video),
				NORMALPRIO,
				thd_video, NULL);
	
	chThdWait(thd);
}

void HardFault_Handler(void) {
	while (1)
		;
}
