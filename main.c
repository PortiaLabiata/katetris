#include "env.h"
#include "tetris.h"
#include "common.h"
#include "impl.h"

#include "text.h"
#include "font.h"

#if SITL
#include <SDL2/SDL.h>
#endif

static THD_WORKING_AREA(wa_blink, 64);
static THD_FUNCTION(thd_blink, arg) {
	while (1) {
		hw->led_toggle();
		hw->delay(500);
	}
}

static vbuf_t vbuf;
static grid_t grid;

void draw(vbuf_t *vbuf) {
	if (vbuf->clear_flag) {
		hw->display_update(vbuf);
		vbuf->clear_flag = false;
		return;
	}
	while (vbuf->upd_stack.size) {
		bbox_t bbox = {0};
		bbox_stack_pop(&vbuf->upd_stack, &bbox);
		hw->display_update_rect(vbuf, &bbox);
	}
}

#define SCORE_X 0
#define SCORE_Y 0
#define SCORE_SIZEX GRID_STEP*CHAR_SIZE
#define SCORE_SIZEY GRID_STEP*CHAR_SIZE*3

static THD_WORKING_AREA(wa_video, 128);
static THD_FUNCTION(thd_video, arg) {
	while (1) {
#if !SITL
		thd->lock(&vbuf.mtx);
		draw(&vbuf);
		thd->unlock(&vbuf.mtx);
#endif
		hw->delay(1);
	}
}

thd_handle_t threads[] = {
	{.wa = wa_blink, 
	 .wa_size = sizeof(wa_blink), 
	 .prio = NORMALPRIO, 
	 .func = thd_blink},

	{.wa = wa_video,
	 .wa_size = sizeof(wa_video),
	 .prio = LOWPRIO,
	 .func = thd_video},
};

// TODO: fix rendering of gamma block in one of 
// orientation
int main(int argc, char *argv[]) {
	if (!hw->init()) {
		hw->fatal_error("system");
	}
	if (!hw->display_init()) {
		hw->fatal_error("display");
	}

	vbuf_init(&vbuf);
	grid_clear(grid);

	for (int i = 0; i < sizeof(threads)/sizeof(thd_handle_t); i++) {
		thd->create(&threads[i]);
	}

	vbuf_clear(&vbuf);
	static uint32_t t_last = 0;
	t_last = hw->millis();

	block_t blk = block_gamma;
	bbox_t bbox = get_bbox(blk.ptrs[blk.ori]);
	int score = 0;

	// We need to draw entire screen at least once
	vbuf_set_flag(&vbuf);
	while (1) {
		hw->loop(&vbuf, &blk);

		if (blk.x+bbox.x == GRID_ROWS-bbox.sizex) {
			goto collision;
		}

		vbuf_clear(&vbuf);

		uint32_t t_now = hw->millis();
		if (t_now - t_last >= 1000) {
			t_last = t_now;
			blk.x++;
			update_block(&vbuf, &blk);
		}
		if (block_collides(&blk, grid)) {
collision:
			block_add(&blk, grid);
			blk = block_gamma;
			bbox = get_bbox(blk.ptrs[blk.ori]);

			for (int i = GRID_ROWS-1; i >= 0; i--) {
				if (grid_check(grid, i)) {
					score++;
					grid_shift(grid, i);
				}
			}
#if SHIFT_CLEAR
			vbuf_set_flag(&vbuf);
#endif
		} // block_collides
		block_draw(&vbuf, &blk);
		grid_draw(&vbuf, grid);

		text_draw_number(&vbuf, score, SCORE_X, SCORE_Y);
		bbox_stack_push(&vbuf.upd_stack, 
			&(bbox_t){SCORE_X, SCORE_Y, SCORE_SIZEX, SCORE_SIZEY});
#if SITL
		draw(&vbuf);
#endif
		hw->delay(10);
	} // while (1)
}

void HardFault_Handler(void) {
	while (1)
		;
}
