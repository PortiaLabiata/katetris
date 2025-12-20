#pragma once

#include "ch.h"
#include "hal.h"

#define DISP_COLS 128
#define DISP_ROWS 8
typedef struct {
	uint8_t buf[DISP_COLS][DISP_ROWS];
	mutex_t mtx;
} vbuf_t;

int disp_init();

int *i2c_scan();
void i2c_scan_print(BaseSequentialStream *s, int status[]);

int disp_write_cmd(uint8_t *bs, int size);
int disp_write_data(uint8_t *bs, int size);

void vbuf_init(vbuf_t *buf);
void vbuf_set_bit(vbuf_t *vbuf, int col, int row);
void vbuf_reset_bit(vbuf_t *vbuf, int col, int row);
void vbuf_clear(vbuf_t *buf);
void vbuf_draw_rect(vbuf_t *buf, int x, int y, int h, int w);

#define disp_write_cmdb(b) do { \
	uint8_t _buf = b; \
	disp_write_cmd(&_buf, 1); \
} while (0)

#define disp_write_datab(b) do { \
	uint8_t _buf = b; \
	disp_write_data(&_buf, 1); \
} while (0)
