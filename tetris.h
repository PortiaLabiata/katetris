#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "display.h"

#define GRID_STEP 16
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

typedef struct {
	int x, y;
	int sizex, sizey;
} bbox_t;

static const pattern_t patterns_gamma[ORI_SIZE] = {
		{0b110, 
		 0b100, 
		 0b100},

		{0b000, 
		 0b111, 
		 0b001},

		{0b001, 
		 0b001, 
		 0b011},

		{0b000, 
		 0b100, 
		 0b111},
};
static const block_t block_gamma = {
	.ptrs = patterns_gamma,
	.ori = ORI_NORTH,
	.x = 0,
	.y = 0,
};

// TODO: add normal grid size calculation
#if DISP_COLS % GRID_STEP > 0
	#define GRID_ROWS DISP_COLS/GRID_STEP+1
#else
	#define GRID_ROWS DISP_COLS/GRID_STEP
#endif
#if DISP_ROWS % GRID_STEP > 0
	#define GRID_COLS DISP_ROWS*8/GRID_STEP+1
#else
	#define GRID_COLS DISP_ROWS*8/GRID_STEP
#endif

#if GRID_COLS <= 16
typedef uint16_t grid_row_t;
#else
typedef uint32_t grid_row_t;
#endif

typedef grid_row_t grid_t[GRID_ROWS];

void block_rotr(block_t *blk);
void block_rotl(block_t *blk);
void block_draw(vbuf_t *vbuf, block_t *blk);
bool block_collides(block_t *blk, grid_t grid);
void block_add(block_t *blk, grid_t grid);

void grid_clear(grid_t grid);
void grid_draw(vbuf_t *vbuf, grid_t grid);
void grid_shift(grid_t grid, size_t start_idx);
bool grid_check(grid_t grid, size_t idx);
bbox_t get_bbox(const pattern_t ptr);
