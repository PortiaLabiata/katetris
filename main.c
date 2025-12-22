#include "env.h"
#include "tetris.h"
#include "common.h"
#include "impl.h"

static THD_WORKING_AREA(wa_blink, 64);
static THD_FUNCTION(thd_blink, arg) {
	while (1) {
		hw->led_toggle();
		hw->delay(500);
	}
}

static vbuf_t vbuf;
static grid_t grid;

static THD_WORKING_AREA(wa_video, 128);
static THD_FUNCTION(thd_video, arg) {
	while (1) {
#if !SITL
		thd->lock(&vbuf.mtx);
		hw->display_update(&vbuf);	
		thd->unlock(&vbuf.mtx);
#endif
		hw->delay(10);
	}
}

thd_handle_t threads[] = {
	{.wa = wa_blink, 
	 .wa_size = sizeof(wa_blink), 
	 .prio = NORMALPRIO, 
	 .func = thd_blink},

	{.wa = wa_video,
	 .wa_size = sizeof(wa_video),
	 .prio = NORMALPRIO,
	 .func = thd_video},
};

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
	int score = 0;
	while (1) {
		char c = hw->serial_getch();
		switch (c) {
			case 'a':
				blk.y -= 1;
				break;
			case 'd':
				blk.y += 1;
				break;
			case 's':
				blk.x += 1;
				break;
			case 'q':
				block_rotr(&blk);
				break;
			case 'e':
				block_rotl(&blk);
				break;
			default:
				break;
		}
		// FIXME: duct tape!!!
		blk.y = bound(blk.y, -1, 
						GRID_COLS-BLOCK_HEIGHT+1);
		vbuf_clear(&vbuf);

		uint32_t t_now = hw->millis();
		if (t_now - t_last >= 1000) {
			t_last = t_now;
			blk.x += 1;
		}
		if (blk.x >= GRID_ROWS-BLOCK_HEIGHT || \
				block_collides(&blk, grid)) {
			block_add(&blk, grid);
			blk = block_gamma;
			for (int i = GRID_ROWS-1; grid[i] && i >= 0; i--) {
				if (grid_check(grid, i)) {
					score++;
					grid_shift(grid, i);
				}
			}
		}
		block_draw(&vbuf, &blk);
		grid_draw(&vbuf, grid);
#if SITL
		hw->display_update(&vbuf);
#endif
		hw->delay(10);
	}
}

void HardFault_Handler(void) {
	while (1)
		;
}
