#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "display.h"

#define GRID_STEP 8
enum ori_e {
	ORI_NORTH = 0,
	ORI_EAST,
	ORI_SOUTH,
	ORI_WEST,
};

typedef uint8_t pattern_t[3];
typedef struct {
	const pattern_t *ptrs; // North, east, south and west
	enum ori_e ori;
	int x, y;
	bool fell;
} block_t;

static const pattern_t patterns_gamma[4] = {
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

#define BUFSIZE 50
typedef struct {
	block_t buf[BUFSIZE];
	size_t idx;
} blockbuf_t;

void block_rotr(block_t *blk);
void block_rotl(block_t *blk);
void block_draw(vbuf_t *vbuf, block_t *blk);
bool blockbuf_tick(blockbuf_t *buf);
