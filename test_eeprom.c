#include "hal.h"
#include "ch.h"
#include "osal.h"

#define EEP_ADDR 0b1010000
#define LED_LINE PAL_LINE(GPIOC, 13)
int main(void) {
	halInit();
	chSysInit();

	palSetLineMode(LED_LINE, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOB, 6, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
	palSetPadMode(GPIOB, 7, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);

	static const I2CConfig cfg = {
		.clock_speed = 100000,
		.duty_cycle = STD_DUTY_CYCLE,
		.op_mode = OPMODE_I2C,
	};
	i2cStart(&I2CD1, &cfg);

	while (1) {
		uint8_t res[2] = {0};
		msg_t ret = i2cMasterReceive(&I2CD1, EEP_ADDR, res, 2);
		osalDbgAssert(ret == MSG_OK, "io");

		palToggleLine(LED_LINE);
		osalThreadSleepMilliseconds(500);
	}
}
