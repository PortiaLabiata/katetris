#pragma once

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

#define GRID_STEP 8
enum ori_e {
	ORI_NORTH = 0,
	ORI_EAST,
	ORI_SOUTH,
	ORI_WEST,
	ORI_SIZE,
};

#define BLOCK_HEIGHT 3
typedef uint8_t pattern_row_t;
typedef pattern_row_t pattern_t[BLOCK_HEIGHT];
typedef struct {
	const pattern_t *ptrs; // North, east, south and west
	enum ori_e ori;
	int x, y;
} block_t;


