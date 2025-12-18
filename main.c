#include "ch.h"
#include "chthreads.h"
#include "hal.h"
#include "hal_pal.h"

static THD_WORKING_AREA(wa_serial, 128);
static THD_FUNCTION(thd_serial, arg) {
	while (1) {
		sdWrite(&SD1, (uint8_t*)"Hello, world!\n", 14);
		chThdSleepMilliseconds(1000);
	}
}

static THD_WORKING_AREA(wa_blink, 128);
static THD_FUNCTION(thd_blink, arg) {
	while (1) {
		palTogglePad(GPIOC, 13);
		chThdSleepMilliseconds(500);
	}
}

int main(int argc, char *argv[]) {
	chSysInit();
	halInit();

	palSetPadMode(GPIOC, 13, PAL_MODE_OUTPUT_PUSHPULL);

	palSetPadMode(GPIOA, 9, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	palSetPadMode(GPIOA, 10, PAL_MODE_INPUT);

	sdStart(&SD1, NULL);
	chThdSleepMilliseconds(1);
	thread_t *thread_serial = chThdCreateStatic(wa_serial, 
					sizeof(wa_serial),
					NORMALPRIO,
					thd_serial, NULL);
	chThdCreateStatic(wa_blink, 
					sizeof(wa_blink),
					NORMALPRIO,
					thd_blink, NULL);
	chThdWait(thread_serial);
}
