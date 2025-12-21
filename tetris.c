#include "env.h"
#include "tetris.h"
#include "common.h"

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

bool block_collides(block_t *blka, block_t *blkb) {
	if (abs(blka->y - blkb->y) > 3 || \
		abs(blka->x - blkb->x) > 2) {
		return false;	
	}
	return blkb->ptrs[blka->ori][0] & blkb->ptrs[blkb->ori][2];
}

bool blockbuf_tick(blockbuf_t *buf) {
	block_t *last = blockbuf_last(buf);
	for (int i = 0; i < buf->idx-1; i++) {
		if (block_collides(last, &buf->buf[i])) {
			last->fell = true;
			return true;	
		}
	}
	if (last->x >= 13) {
		last->fell = true;
		return true;
	}
	last->x++;
	return false;
}

void blockbuf_push(blockbuf_t *buf, const block_t *blk) {
	if (buf->idx < BUFSIZE) {
		buf->buf[buf->idx++] = *blk;
	}
}

block_t *blockbuf_last(blockbuf_t *buf) {
	return &buf->buf[buf->idx-1];
}
