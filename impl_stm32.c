#include "env.h"
#include "impl_stm32.h"
#include "common.h"
#include "tetris.h"

hwiface_t hwiface_stm32 = {
	.delay = delay_stm32,
	.display_cmd = display_cmd_stm32,
	.display_init = display_init_stm32,
	.display_update = display_update_stm32,
	.fatal_error = fatal_error_stm32,
	.init = init_stm32,
	.led_toggle = led_toggle_stm32,
	.millis = millis_stm32,
	.serial_getch = serial_getch_stm32,
	.display_update_rect = update_rect_stm32,
};

void delay_stm32(uint32_t ms) {
	osalThreadSleepMilliseconds(ms);
}

#define CMD_ON 0xAF
#define CMD_OFF 0xAE
#define CMD_CP_ON 0x8D
#define CMD_ALL_ON 0xA5

#define NSS_PORT GPIOA
#define NSS_PIN 4
#define DC_PORT GPIOA
#define DC_PIN 3
#define RST_PORT GPIOA
#define RST_PIN 2
#define BLK_PORT GPIOA
#define BLK_PIN 1

#define disp_set_cd() palSetPad(DC_PORT, DC_PIN) // data
#define disp_clear_cd() palClearPad(DC_PORT, DC_PIN) // command
#define disp_set_blk() palSetPad(BLK_PORT, BLK_PIN)
#define disp_clear_blk() palClearPad(BLK_PORT, BLK_PIN)
#define disp_set_rst() palSetPad(RST_PORT, RST_PIN)
#define disp_clear_rst() palClearPad(RST_PORT, RST_PIN)

void display_cmd_stm32(uint8_t *cmd, size_t size) {
	disp_clear_cd();	
	spiSend(&SPID1, size, cmd);
}

#define disp_write_cmdb(b) do { \
	uint8_t _buf = b; \
	display_cmd_stm32(&_buf, 1); \
} while (0)

static int disp_write_data(uint8_t *bs, int size) {
	disp_set_cd();
	spiSend(&SPID1, size, bs);
	return 0;
}

#define disp_write_datab(b) do { \
	uint8_t _buf = b; \
	disp_write_data(&_buf, 1); \
} while (0)

void display_clear(void) {
	for (int i = 0; i < DISP_COLS*DISP_ROWS; i++) {
		disp_write_datab(0x0000);
	}
}

#define disp_normal_mode() disp_write_cmdb(0x13)
#define disp_partial_mode() disp_write_cmdb(0x12)
#define disp_raddr_set(start, end) do { \
	disp_write_cmdb(0x2B); \
	uint8_t _buf[] = { \
		(start) >> 8, (start) & 0xFF, \
		(end) >> 8, (end) & 0xFF, \
	}; \
	disp_write_data(_buf, 4); \
} while (0)

#define disp_caddr_set(start, end) do { \
	disp_write_cmdb(0x2A); \
	uint8_t _buf[] = { \
		(start) >> 8, (start) & 0xFF, \
		(end) >> 8, (end) & 0xFF, \
	}; \
	disp_write_data(_buf, 4); \
} while (0)

bool display_init_stm32(void) {
	// Hard reset
	disp_clear_rst();
	hw->delay(100);
	disp_set_rst();
	hw->delay(150);

	// Soft reset
	disp_write_cmdb(0x01);
	hw->delay(500);
	// Sleep out
	disp_write_cmdb(0x11);
	hw->delay(500);
	// Inv on
	disp_write_cmdb(0x21);
	hw->delay(10);
	// Disp on
	disp_write_cmdb(0x29);
	hw->delay(10);
	// Disp colmode - 65k, 16bit/px
	disp_write_cmdb(0x3A);
	disp_write_datab(0b01010101);
	hw->delay(10);
	// Brightness
	disp_write_cmdb(0x51);
	disp_write_datab(0xFF);
	hw->delay(10);
	// MADCTL
	disp_write_cmdb(0x36);
	disp_write_datab(1 << 5);
	hw->delay(10);
	return true;
}

void display_update_stm32(const vbuf_t *vbuf) {
	update_rect_stm32(vbuf, &(bbox_t){0, 0, DISP_COLS, 
					DISP_ROWS*8});
}

// TODO: buffer rows for more efficient transfer
void update_rect_stm32(const vbuf_t *vbuf, bbox_t *bbox) {
	int x, x_end;
	int y, y_end, y_start_bytes, y_end_bytes;

	x_end = MIN(bbox->x + bbox->sizex, DISP_COLS);
	y_end = MIN(bbox->y + bbox->sizey, DISP_ROWS*8);

	y_start_bytes = bbox->y / 8;
	y_end_bytes = y_end / 8;

	disp_caddr_set(bbox->x, x_end-1);
	disp_raddr_set(bbox->y, y_end-1);

	// RAMWR
	disp_write_cmdb(0x2C);

	for (y = y_start_bytes; y < y_end_bytes; y++) {
		for (int rem = 0; rem < 8; rem++) {
			for (x = bbox->x; x < x_end; x++) {
				uint16_t buf = 0x0000;
				if ((vbuf->buf[x][y] >> rem) & 0x01) {
					buf = 0xFFFF;	
				}
				disp_write_data((uint8_t*)&buf, 2);
			}
		}
	}
	disp_write_cmdb(0x00);
}

void fatal_error_stm32(const char *msg) {
	chSysHalt(msg);
}

#define GPIO_LED 13
static void gpio_init() {
	palSetPadMode(GPIOC, GPIO_LED, PAL_MODE_OUTPUT_PUSHPULL);

	palSetPadMode(GPIOA, 9, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	palSetPadMode(GPIOA, 10, PAL_MODE_INPUT);

	palSetPadMode(GPIOA, 5, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	palSetPadMode(GPIOA, 7, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	palSetPadMode(RST_PORT, RST_PIN, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(DC_PORT, DC_PIN, PAL_MODE_OUTPUT_PUSHPULL);
}

bool init_stm32(void) {
	chSysInit();
	halInit();
	gpio_init();
	
	sdStart(&SD1, NULL);

	static const SPIConfig cfg = {
		.circular = false,
		.data_cb = NULL,
		.error_cb = NULL,
		.slave = false,
		.sspad = 4,
		.ssport = GPIOA,
		.cr1 = SPI_CR1_CPOL | SPI_CR1_CPHA,
		.cr2 = 0
	};
	return !spiStart(&SPID1, &cfg);
}

void led_toggle_stm32(void) {
	palTogglePad(GPIOC, GPIO_LED);
}

uint32_t millis_stm32(void) {
	return TIME_I2MS(chVTGetSystemTime());
}

char serial_getch_stm32(void) {
	return sdGetTimeout(&SD1, TIME_US2I(1));
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
	chThdExit(code);
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
