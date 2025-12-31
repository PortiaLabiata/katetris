#pragma once
#include "tetris.h"

typedef enum {
	BUTTON_INVALID = -1,
	BUTTON_LEFT  = 0,
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_RIGHT,
	BUTTON_ENTER,
	BUTTON_COUNT
} button_e;

bool buttons_do_action(block_t *blk, bool button_states[]) {
	#define PRESSED(b) button_states[b]
	// Potential overhead
	bool ret = false;
	bbox_t bbox = get_bbox(blk->ptrs[blk->ori]);
	if (PRESSED(BUTTON_LEFT) && blk->y+bbox.y > 0) {
		blk->y--;			
		ret = true;
	}
	if (PRESSED(BUTTON_RIGHT) && 
			blk->y+bbox.y < GRID_COLS-bbox.sizey) {
		blk->y++;
		ret = true;
	}
	if (PRESSED(BUTTON_DOWN) && 
			blk->x+bbox.x < GRID_ROWS-bbox.sizex) {
		blk->x++;
		ret = true;
	}
	if (PRESSED(BUTTON_ENTER)) {
		block_rotr(blk);
		ret = true;
	}
	return ret;
}
