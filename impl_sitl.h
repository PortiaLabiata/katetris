#pragma once
#include "env.h"
#include "impl.h"

void delay_sitl(uint32_t ms);
bool init_sitl(void);
void loop_sitl(vbuf_t *vbuf, block_t *blk);
uint32_t millis_sitl(void);
void display_cmd_sitl(uint8_t *cmd, size_t size);
bool display_init_sitl(void);
void display_update_sitl(const vbuf_t *vbuf);
void update_rect_sitl(const vbuf_t *vbuf, bbox_t *bbox);

void fatal_error_sitl(const char *msg);
void led_toggle_sitl(void);
char serial_getch_sitl(void);

void thd_create_sitl(thd_handle_t *handle);
void thd_exit_sitl(int code);
void thd_lock_sitl(mutex_impl_t *mtx);
void thd_unlock_sitl(mutex_impl_t *mtx);
void thd_mtx_init_sitl(mutex_impl_t *mtx);
