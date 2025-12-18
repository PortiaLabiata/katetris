#include "ch.h"
#include "hal.h"
#include "hal_pal.h"

int main(int argc, char *argv[]) {
	chSysInit();
	halInit();

	palSetPadMode(GPIOC, 13, PAL_MODE_OUTPUT_PUSHPULL);

	while (1) {
		palTogglePad(GPIOC, 13);
		chThdSleepMilliseconds(1000);
	}
}
