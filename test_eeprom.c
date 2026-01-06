#include "hal.h"
#include "ch.h"
#include "osal.h"

#define EEP_ADDR 0b1010000
#define LED_LINE PAL_LINE(GPIOC, 13)

msg_t eep_writeb(uint8_t eep_addr, uint16_t addr, uint8_t data) {
	osalDbgCheck(((addr >> 8) & 0xF0) == 0);

	uint8_t txbuf[3] = {
		addr >> 8,
		addr & 0xFF,
		data,
	};
	return i2cMasterTransmitTimeout(&I2CD1, eep_addr, txbuf,
					3, NULL, 0, TIME_MS2I(1));
}

msg_t eep_read(uint8_t eep_addr, uint16_t addr, uint8_t *data, 
				size_t size) {
	osalDbgCheck(((addr >> 8) & 0xF0) == 0);
	osalDbgCheck(size > 1);

	uint8_t txbuf[2] = {
		addr >> 8,
		addr & 0xFF,
	};
	return i2cMasterTransmitTimeout(&I2CD1, eep_addr, txbuf,
					2, data, size, TIME_MS2I(1));
}

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

	msg_t ret = MSG_OK;
	ret = eep_writeb(EEP_ADDR, 0x0000, 0xDE);	
	osalDbgAssert(ret == MSG_OK, "write");
	osalThreadSleepMilliseconds(100);

	uint8_t res[2] = {0};
	ret = eep_read(EEP_ADDR, 0x0000, res, 2);
	osalDbgAssert(ret == MSG_OK, "read");
	osalDbgAssert(res[0] == 0xDE, "result");

	while (1) ;
}
