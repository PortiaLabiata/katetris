#include "display.h"

#if !SITL
#include "chmtx.h"
#include "chprintf.h"
#include "hal_i2c.h"
#include "hal_streams.h"
#else
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <curses.h>
#endif

#include <string.h>

#define DISP_ADDR 60

#if !SITL
int *i2c_scan() {
	static int status[127] = {0};
	memset(status, 0, sizeof(status));

	uint8_t txbuf = 0;

	for (int i = 8; i < 127; i++) {
		i2cAcquireBus(&I2CD1);
		msg_t msg = i2cMasterTransmitTimeout(
			&I2CD1, i, &txbuf, 1,
			NULL, 0, TIME_MS2I(1));
		if(msg == MSG_OK) {
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

#define CMD_ON 0xAF
#define CMD_OFF 0xAE
#define CMD_CP_ON 0x8D
#define CMD_ALL_ON 0xA5

int disp_write_cmd(uint8_t *bs, int size) {
	uint8_t buf[size+1];
	buf[0] = 0x00;
	memcpy(buf+1, bs, size);

	return i2cMasterTransmit(&I2CD1, DISP_ADDR, buf, size+1, NULL, 0);
}

int disp_write_data(uint8_t *bs, int size) {
	uint8_t buf[size+1];
	buf[0] = 1 << 6;
	memcpy(buf+1, bs, size);

	return i2cMasterTransmit(&I2CD1, DISP_ADDR, buf, size+1, NULL, 0);
}

int disp_init() {
	uint8_t _b[4];

	disp_write_cmdb(CMD_OFF); // Sleep mode
	disp_write_cmdb(CMD_CP_ON); // Enable charge pump
	disp_write_cmdb(CMD_ALL_ON); // All pixels on

	// Set addressing mode to vertical
	_b[0] = 0x20;
	_b[1] = 0x01;
	disp_write_cmd(_b, 2);

	disp_write_cmdb(CMD_ON);
	return 0;
}

#else

#define ZOOM 3
static SDL_Window *window = NULL;
static SDL_Renderer *renderer  = NULL;
int disp_init() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		return -1;
	}
	window = SDL_CreateWindow("SITL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, \
					DISP_COLS, DISP_ROWS*8, \
					SDL_WINDOW_ALLOW_HIGHDPI);
	if (!window) {
		return -2;
	}

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		return -3;
	}
	initscr();
}

void sitl_render(vbuf_t *vbuf) {
	SDL_Event evt;
	SDL_PollEvent(&evt);
	if (evt.type == SDL_QUIT) {
		endwin();
		exit(0);
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	for (int i = 0; i < DISP_COLS; i++) {
		for (int j = 0; j < DISP_ROWS; j++) {
			uint8_t b = vbuf->buf[i][j];
			for (int k = 0; k < 8; k++) {
				if ((b >> k) & 0x01) {
					SDL_RenderDrawPoint(renderer, i, j*8+k);
				}
			}
		}
	}

	SDL_RenderPresent(renderer);
}

#endif

void vbuf_init(vbuf_t *buf) {
	chMtxObjectInit(&buf->mtx);
	for (int i = 0; i < DISP_COLS; i++) {
		for (int j = 0; j < DISP_ROWS; j++) {
			buf->buf[i][j] = 0xFF;
		}
	}
}

void vbuf_set_bit(vbuf_t *vbuf, int col, int row) {
	vbuf->buf[col][row/8] |= 1 << (row % 8);
}

void vbuf_reset_bit(vbuf_t *vbuf, int col, int row) {
	vbuf->buf[col][row/8] &= ~(1 << (row % 8));
}

void vbuf_clear(vbuf_t *vbuf) {
	chMtxLock(&vbuf->mtx);
	for (int i = 0; i < DISP_COLS; i++) {
		memset(vbuf->buf[i], 0x00, DISP_ROWS);
	}
	chMtxUnlock(&vbuf->mtx);
}

void vbuf_draw_rect(vbuf_t *vbuf, int x, int y, int h, int w) {
	chMtxLock(&vbuf->mtx);
	for (int i = x; i < DISP_COLS && i < x+w; i++) {
		for (int j = y; j < DISP_ROWS*8 && j < y+h; j++) {
			vbuf_set_bit(vbuf, i, j);
		}
	}
	chMtxUnlock(&vbuf->mtx);
}
