#include "env.h"
#include "impl_stm32.h"

hwiface_t hwiface_stm32 = {
	.delay = delay_stm32,
	.display_cmd = display_cmd_stm32,
	.display_init = display_init_stm32,
	.display_update = display_update_stm32,
	.fatal_error = fatal_error_stm32,
	.init = init_stm32,
	.led_toggle = led_toggle_stm32,
	.millis = millis_stm32,
	.serial_getch = serial_getch_stm32
};

void delay_stm32(uint32_t ms) {
	osalThreadSleepMilliseconds(ms);
}

#define DISP_ADDR 60
#define CMD_ON 0xAF
#define CMD_OFF 0xAE
#define CMD_CP_ON 0x8D
#define CMD_ALL_ON 0xA5

void display_cmd_stm32(uint8_t *cmd, size_t size) {
	uint8_t buf[size+1];
	buf[0] = 0x00;
	memcpy(buf+1, cmd, size);

	i2cMasterTransmit(&I2CD1, DISP_ADDR, buf, size+1, NULL, 0);
}

#define disp_write_cmdb(b) do { \
	uint8_t _buf = b; \
	display_cmd_stm32(&_buf, 1); \
} while (0)

bool display_init_stm32(void) {
	uint8_t _b[4];

	disp_write_cmdb(CMD_OFF); // Sleep mode
	disp_write_cmdb(CMD_CP_ON); // Enable charge pump
	disp_write_cmdb(CMD_ALL_ON); // All pixels on

	// Set addressing mode to vertical
	_b[0] = 0x20;
	_b[1] = 0x01;
	display_cmd_stm32(_b, 2);

	disp_write_cmdb(CMD_ON);
	return true;
}

static int disp_write_data(uint8_t *bs, int size) {
	uint8_t buf[size+1];
	buf[0] = 1 << 6;
	memcpy(buf+1, bs, size);

	return i2cMasterTransmit(&I2CD1, DISP_ADDR, buf, size+1, NULL, 0);
}

#define disp_write_datab(b) do { \
	uint8_t _buf = b; \
	disp_write_data(&_buf, 1); \
} while (0)

void display_update_stm32(const vbuf_t *vbuf) {
	for (int i = 0; i < DISP_COLS; i++) {
		for (int j = 0; j < DISP_ROWS; j++) {
			disp_write_datab(vbuf->buf[i][j]);	
		}
	}
}

void fatal_error_stm32(const char *msg) {
	chSysHalt(msg);
}

#define GPIO_LED 13
static void gpio_init() {
	palSetPadMode(GPIOC, GPIO_LED, PAL_MODE_OUTPUT_PUSHPULL);

	palSetPadMode(GPIOA, 9, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	palSetPadMode(GPIOA, 10, PAL_MODE_INPUT);

	palSetPadMode(GPIOB, 6, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
	palSetPadMode(GPIOB, 7, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
}

bool init_stm32(void) {
	chSysInit();
	halInit();
	gpio_init();
	
	sdStart(&SD1, NULL);

	const I2CConfig cfg = {
		.op_mode = OPMODE_I2C,
		.clock_speed = 100000,
		.duty_cycle = STD_DUTY_CYCLE,
	};
	return !i2cStart(&I2CD1, &cfg);
}

void led_toggle_stm32(void) {
	palTogglePad(GPIOC, GPIO_LED);
}

uint32_t millis_stm32(void) {
	return TIME_I2MS(chVTGetSystemTime());
}

char serial_getch_stm32(void) {
	return sdGetTimeout(&SD1, TIME_MS2I(1));
}

thdiface_t thdiface_stm32 = {
	.create = thd_create_stm32,
	.exit = thd_exit_stm32,
	.lock = thd_lock_stm32,
	.unlock = thd_unlock_stm32,
	.mtx_init = thd_mtx_init_stm32,
};

void thd_create_stm32(thd_handle_t *handle) {
	chThdCreateStatic(handle->wa,
		handle->wa_size, handle->prio, *handle->func, NULL);
}

void thd_exit_stm32(int code) {
	(void)code;
	// Stub
}

void thd_lock_stm32(mutex_impl_t *mtx) {
	chMtxLock(mtx);
}

void thd_unlock_stm32(mutex_impl_t *mtx) {
	chMtxUnlock(mtx);
}

void thd_mtx_init_stm32(mutex_impl_t *mtx) {
	chMtxObjectInit(mtx);
}
