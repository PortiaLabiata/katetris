#include "ch.h"
#include "hal.h"
#include <string.h>

int main(void) {
	halInit();
	chSysInit();

	palSetPadMode(GPIOC, GPIOC_LED, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPad(GPIOC, GPIOC_LED);

	palSetPadMode(GPIOA, 9U, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	palSetPadMode(GPIOA, 10U, PAL_MODE_INPUT);

	const SerialConfig cfg = {
		9600,	
		0,
		USART_CR2_STOP1_BITS,
		0
	};
	sdStart(&SD1, &cfg);
	while (1) {
		palTogglePad(GPIOC, 13U);
		chThdSleepMilliseconds(100);
		sdWrite(&SD1, (uint8_t*)"Hello, world!\r\n", strlen("Hello, world!\r\n"));
	}
}
