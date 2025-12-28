#pragma once
#include <string.h>
#include "ch.h"
#include "hal.h"

#include "impl.h"

void display_cmd_stm32(uint8_t *cmd, size_t size);
bool display_init_stm32(void);
void display_update_stm32(const vbuf_t *vbuf);
void update_rect_stm32(const vbuf_t *vbuf, bbox_t *bbox);

void fatal_error_stm32(const char *msg);
bool init_stm32(void);

void led_toggle_stm32(void);

void delay_stm32(uint32_t ms);
uint32_t millis_stm32(void);

char serial_getch_stm32(void);

void thd_create_stm32(thd_handle_t *handle);
void thd_exit_stm32(int code);
void thd_lock_stm32(mutex_impl_t *mtx);
void thd_unlock_stm32(mutex_impl_t *mtx);
void thd_mtx_init_stm32(mutex_impl_t *mtx);
