#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#define GPIO_LED 13
int flag = 1;

void cmd_on(BaseSequentialStream *s) {
	flag = 0;
	chprintf(s, "led on ok");
	palSetPad(GPIOC, GPIO_LED);
}

void cmd_off(BaseSequentialStream *s) {
	flag = 0;
	chprintf(s, "led off ok");
	palClearPad(GPIOC, GPIO_LED);
}

void cmd_blink(BaseSequentialStream *s) {
	flag = 1;
	chprintf(s, "led blink ok");
}

#define streq(a, b) (strcmp(a, b) == 0)
static THD_WORKING_AREA(wa_shell, 512);
static THD_FUNCTION(thd_shell, arg) {
	while (1) {
		char c = sdGet(&SD1);

		BaseSequentialStream *s = (BaseSequentialStream*)&SD1;
		switch (c) {
			case '1':
				cmd_on(s);
				break;
			case '0':
				cmd_off(s);
				break;
			case 'b':
				cmd_blink(s);
				break;
			default:
				break;
		}
	}
}

static THD_WORKING_AREA(wa_blink, 128);
static THD_FUNCTION(thd_blink, arg) {
	while (1) {
		while (!flag)
			chThdSleepMilliseconds(1);
		palTogglePad(GPIOC, GPIO_LED);
		chThdSleepMilliseconds(499);
	}
}


int main(int argc, char *argv[]) {
	chSysInit();
	halInit();

	palSetPadMode(GPIOC, 13, PAL_MODE_OUTPUT_PUSHPULL);

	palSetPadMode(GPIOA, 9, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	palSetPadMode(GPIOA, 10, PAL_MODE_INPUT);

	sdStart(&SD1, NULL);
	chThdCreateStatic(wa_blink, 
				sizeof(wa_blink),
				NORMALPRIO,
				thd_blink, NULL);
	thread_t *shthd = chThdCreateStatic(wa_shell,
				sizeof(wa_shell),
				NORMALPRIO,
				thd_shell, NULL);
	chThdWait(shthd);
}

void HardFault_Handler(void) {
	while (1)
		;
}
