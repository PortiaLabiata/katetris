#include "ch.h"
#include "chthreads.h"
#include "hal.h"
#include "hal_pal.h"

int main(void) {
	halInit();
	chSysInit();

	palSetPadMode(GPIOC, 13U, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPad(GPIOC, 13U);

	while (1) {
		palTogglePad(GPIOC, 13U);
		chThdSleepMilliseconds(1000);
	}
}
