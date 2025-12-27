#pragma once
#include "env.h"
#include "chibios/os/common/ports/ARM-common/chtypes.h"

#if !SITL
#include "ch.h"
typedef mutex_t mutex_impl_t;
#else
#include <SDL2/SDL.h>
typedef SDL_mutex *mutex_impl_t;
#endif

typedef struct {
	int x, y;
	int sizex, sizey;
} bbox_t;

#define STACKSIZE 16
typedef struct {
	size_t size;
	bbox_t arr[STACKSIZE];
} bbox_stack_t;

#define DISP_COLS 240
#define DISP_ROWS 30
typedef struct {
	uint8_t buf[DISP_COLS][DISP_ROWS];
	bbox_stack_t upd_stack;
	bool clear_flag;
	mutex_impl_t mtx;
} vbuf_t;

typedef struct {
	bool (*init)(void);

	uint32_t (*millis)(void);
	void (*delay)(uint32_t);

	bool (*display_init)(void);
	void (*display_update)(const vbuf_t*);
	void (*display_update_rect)(const vbuf_t*, bbox_t *);
	void (*display_cmd)(uint8_t*, size_t);

	char (*serial_getch)(void);

	void (*led_toggle)(void);
	void (*fatal_error)(const char *msg);
} hwiface_t;

#if SITL
typedef uint32_t tprio_t;
typedef int (*tfunc_t)(void *);
#define ZOOM 3
#else
#define ZOOM 1
#endif

typedef struct {
	port_stkalign_t *wa;
	size_t wa_size;
	tprio_t prio;
	tfunc_t func;	
} thd_handle_t;

typedef struct {
	void (*create)(thd_handle_t*);
	void (*exit)(int);

	void (*mtx_init)(mutex_impl_t*);
	void (*lock)(mutex_impl_t*);
	void (*unlock)(mutex_impl_t*);
} thdiface_t;

#if SITL

extern hwiface_t hwiface_sitl;
static hwiface_t *hw = &hwiface_sitl;
extern thdiface_t thdiface_sitl;
static thdiface_t *thd = &thdiface_sitl;

#define THD_WORKING_AREA(name, size) port_stkalign_t name[size]
#define THD_FUNCTION(name, arg) int name(void *arg)
#define NORMALPRIO 0

#else

extern hwiface_t hwiface_stm32;
static hwiface_t *hw = &hwiface_stm32;
extern thdiface_t thdiface_stm32;
static thdiface_t *thd = &thdiface_stm32;

#endif
