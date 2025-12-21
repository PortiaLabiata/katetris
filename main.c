#include "env.h"

#if !SITL
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#else
#include <time.h>
#include <stdio.h>
#include <poll.h>
#include <curses.h>
#include <stdlib.h>

#define SD1 0
#define osalThreadSleepMilliseconds(v) do { \
		struct timespec t = { \
			.tv_sec = 0, \
			.tv_nsec = v*1000, \
		}; \
		nanosleep(&t, NULL); \
} while (0)
#define sdGet(v) getch()
#endif

#if SITL
typedef time_t mytime_t;
#else
typedef systime_t mytime_t;
#endif

#include "display.h"
#include "tetris.h"
#include "common.h"

#if !SITL
#define GPIO_LED 13
static THD_WORKING_AREA(wa_blink, 64);
static THD_FUNCTION(thd_blink, arg) {
	chRegSetThreadName("thd_blink");
	while (1) {
		palTogglePad(GPIOC, GPIO_LED);
		osalThreadSleepMilliseconds(500);
	}
}
#endif // SITL

static vbuf_t vbuf;

#if !SITL
static THD_WORKING_AREA(wa_video, 128);
static THD_FUNCTION(thd_video, arg) {
	chRegSetThreadName("thd_video");

	while (1) {
		chMtxLock(&vbuf.mtx);
		for (int i = 0; i < DISP_COLS; i++) {
			for (int j = 0; j < DISP_ROWS; j++) {
				disp_write_datab(vbuf.buf[i][j]);	
			}
		}
		chMtxUnlock(&vbuf.mtx);
		osalThreadSleepMilliseconds(20);
	}
}

void gpio_init() {
	palSetPadMode(GPIOC, GPIO_LED, PAL_MODE_OUTPUT_PUSHPULL);

	palSetPadMode(GPIOA, 9, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	palSetPadMode(GPIOA, 10, PAL_MODE_INPUT);

	palSetPadMode(GPIOB, 6, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
	palSetPadMode(GPIOB, 7, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
}
#endif // SITL

mytime_t gettime() {
#if SITL
	return time(NULL);
#else
	return chTimeI2S(chVTGetSystemTime());
#endif
}

static blockbuf_t bbuf = {.idx = 0};
int main(int argc, char *argv[]) {
#if !SITL
	chSysInit();
	halInit();
	gpio_init();
	
	sdStart(&SD1, NULL);

	const I2CConfig cfg = {
		.op_mode = OPMODE_I2C,
		.clock_speed = 100000,
		.duty_cycle = STD_DUTY_CYCLE,
	};
	i2cStart(&I2CD1, &cfg);
#endif

	disp_init();
	vbuf_init(&vbuf);

#if !SITL
	chThdCreateStatic(wa_blink, 
				sizeof(wa_blink),
				NORMALPRIO,
				thd_blink, NULL);
	(void)chThdCreateStatic(wa_video,
				sizeof(wa_video),
				NORMALPRIO,
				thd_video, NULL);
#endif // SITL
	vbuf_clear(&vbuf);
	blockbuf_push(&bbuf, &block_gamma);
	static mytime_t t_last = 0;
	t_last = gettime();
	while (1) {
		char c = 0;
#if SITL
		sitl_render(&vbuf);
#endif
		//c = sdGet(&SD1);
		block_t *blk = blockbuf_last(&bbuf);
		if (!blk->fell) {
			switch (c) {
				case 'a':
					blk->y -= 1;
					break;
				case 'd':
					blk->y += 1;
					break;
				case 's':
					blk->x += 1;
					break;
				case 'q':
					block_rotr(blk);
					break;
				case 'e':
					block_rotl(blk);
					break;
				default:
					break;
			}
		}
		vbuf_clear(&vbuf);
		for (int i = 0; i < bbuf.idx; i++) {
			block_draw(&vbuf, &bbuf.buf[i]);
		}

		mytime_t t_now = gettime();
		if (abs(t_now - t_last) >= 1) {
			t_last = t_now;
			if (blockbuf_tick(&bbuf)) {
				blockbuf_push(&bbuf, &block_gamma);		
			}
		}
		osalThreadSleepMilliseconds(20);
	}
}

void HardFault_Handler(void) {
	while (1)
		;
}
