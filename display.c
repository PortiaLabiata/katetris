#include "display.h"
#include "chprintf.h"
#include "hal_i2c.h"
#include "hal_streams.h"
#include <string.h>

#define DISP_CMD_MODe() palClearPad(DISP_A0_PORT, DISP_A0_PIN)
#define DISP_DATA_MODE() palSetPad(DISP_A0_PORT, DISP_A0_PIN)

int disp_init() {
	const I2CConfig cfg = {
		.op_mode = OPMODE_I2C,
		.clock_speed = 100000,
		.duty_cycle = STD_DUTY_CYCLE,
	};
	return i2cStart(&I2CD1, &cfg);
}

int *i2c_scan() {
	static int status[127] = {0};
	memset(status, 0, sizeof(status));

	uint8_t txbuf = 0;

	for (int i = 8; i < sizeof(status); i++) {
		i2cAcquireBus(&I2CD1);
		msg_t msg = i2cMasterTransmitTimeout(
			&I2CD1, i, &txbuf, 1,
			NULL, 0, TIME_MS2I(1));
		if(msg != MSG_TIMEOUT) {
			status[i] = 1;
		} else {
			I2CD1.state = I2C_READY;
		}
		i2cReleaseBus(&I2CD1);
	}
	return status;
}

void i2c_scan_print(BaseSequentialStream *s, int status[]) {
	chprintf(s, "Found I2C devices: ");
	for (int i = 8; i < 127; i++) {
		if (status[i]) {
			chprintf(s, "%d ", i);	
		}
	}
}

