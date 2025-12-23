#include "env.h"
#include "tetris.h"
#include "common.h"
#include <limits.h>

#if SITL
#include <SDL2/SDL.h>
#endif

void block_rotr(block_t *blk) {
	blk->ori = (blk->ori + 1) % 4;
}

void block_rotl(block_t *blk) {
	blk->ori = (blk->ori - 1) % 4;
}

static void suppx_draw(vbuf_t *vbuf, int x, int y) {
	vbuf_draw_rect(vbuf, x*GRID_STEP, y*GRID_STEP, GRID_STEP, GRID_STEP);
}

void block_draw(vbuf_t *vbuf, block_t *blk) {
	for (int i = 0; i < 3; i++) {
		for (int j = 2; j >= 0; j--) {
			if (blk->ptrs[blk->ori][i] & (1 << j)) {
				suppx_draw(vbuf, blk->x+i, blk->y+j);
			}
		}
	}
}

void grid_clear(grid_t grid) {
	for (int i = 0; i < GRID_ROWS; i++) {
		grid[i] = 0;
	}
}

bool block_collides(block_t *blk, grid_t grid) {
	for (int i = 0; i < BLOCK_HEIGHT; i++) {
		pattern_row_t block_cur = blk->ptrs[blk->ori][i];
		grid_row_t grid_cur = grid[blk->x+i+1];

		if ((block_cur << blk->y) & grid_cur) {
			return true;
		}
	}
	return false;
}

void block_add(block_t *blk, grid_t grid) {
	for (int i = blk->x; i < blk->x+BLOCK_HEIGHT; i++) {
		grid[i] |= (blk->ptrs[blk->ori][i-blk->x] << blk->y);
	}
}

void grid_draw(vbuf_t *vbuf, grid_t grid) {
	for (int i = 0; i < GRID_ROWS; i++) {
		for (int j = 0; j < GRID_COLS; j++) {
			if (grid[i] & (1 << j)) {
				suppx_draw(vbuf, i, j);
			}
		}
	}
}

void grid_shift(grid_t grid, size_t start_idx) {
	for (int i = start_idx; i > 0 && grid[i] != 0; i--) {
		grid[i] = grid[i-1];
	}
}

bool grid_check(grid_t grid, size_t idx) {
	int res = grid[idx] & 0x01;
	for (int i = 0; i < GRID_COLS; i++) {
		res &= ((grid[idx] >> i) & 0x01);
	}
	return res;
}

bbox_t get_bbox(const pattern_t ptr) {
	bbox_t res = {0};
	res.x = res.y = INT_MAX;
	res.sizex = res.sizey = INT_MIN;

	for (int i = 0; i < BLOCK_HEIGHT; i++) {
		for (int j = 0; j < BLOCK_HEIGHT; j++) {
			if ((ptr[i] >> j) & 0x01) {
				res.x = MIN(res.x, i);		
				res.y = MIN(res.y, j);

				res.sizex = MAX(res.sizex, i);
				res.sizey = MAX(res.sizey, j);
			}
		}
	}
	res.sizex -= res.x-1;
	res.sizey -= res.y-1;
	return res;
}
