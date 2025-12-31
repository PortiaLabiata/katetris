#include "impl_sitl.h"

#include <unistd.h>
#include <time.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <curses.h>
#include "common.h"
#include "tetris.h"
#include "buttons.h"

hwiface_t hwiface_sitl = {
	.delay = delay_sitl,
	.display_cmd = display_cmd_sitl,
	.display_init = display_init_sitl,
	.display_update = display_update_sitl,
	.fatal_error = fatal_error_sitl,
	.init = init_sitl,
	.led_toggle = led_toggle_sitl,
	.millis = millis_sitl,
	.serial_getch = serial_getch_sitl,
	.display_update_rect = update_rect_sitl,
	.loop = loop_sitl,
};

void delay_sitl(uint32_t ms) {
	SDL_Delay(ms);
}

static uint32_t _time_ms = 0;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

static uint32_t gettime(void) {
	struct timespec t = {0};
	clock_gettime(CLOCK_MONOTONIC, &t);
	return t.tv_sec*100+t.tv_nsec/1000000;
}

static void draw_pixel(int x, int y, bool color) {
	if (color)
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	else
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &(SDL_Rect){x*ZOOM, y*ZOOM, ZOOM, ZOOM});
}

uint32_t millis_sitl(void) {
	return gettime() - _time_ms;
}

bool init_sitl(void) {
	_time_ms = gettime();	
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, TRUE);
	return true;
}

static bool button_states[BUTTON_COUNT] = {0};
struct char_map_t {
	SDL_KeyCode c;
	button_e idx;
} char_map[] = {
	{'d', BUTTON_RIGHT},
	{'a', BUTTON_LEFT},
	{'w', BUTTON_UP},
	{'s', BUTTON_DOWN},
	{'e', BUTTON_ENTER},
};

bool display_init_sitl(void) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		return false;
	}
	window = SDL_CreateWindow("SITL", 
					SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, \
					DISP_ROWS*8*ZOOM, DISP_COLS*ZOOM, \
					SDL_WINDOW_ALLOW_HIGHDPI);
	if (!window) {
		return false;
	}

    renderer = SDL_CreateRenderer(window, -1, 
					SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		return false;
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	return true;
}

void display_cmd_sitl(uint8_t *cmd, size_t size) {
	(void)cmd; (void)size;
}

void display_update_sitl(const vbuf_t *vbuf) {
	update_rect_sitl(vbuf, &(bbox_t){0, 0, DISP_COLS, DISP_ROWS*8});
}

static button_e _map_button(SDL_KeyCode code) {
	for (int i = 0; i < BUTTON_COUNT; i++) {
		if (code == char_map[i].c) {
			return char_map[i].idx;
		}
	}
	return BUTTON_INVALID;
}

void loop_sitl(vbuf_t *vbuf, block_t *blk) {
	SDL_Event evt;
	SDL_PollEvent(&evt);
	if (evt.type == SDL_QUIT) {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		endwin();
		exit(0);
	} else if (evt.type == SDL_KEYDOWN || evt.type == SDL_KEYUP) {
		button_e button = _map_button(evt.key.keysym.sym);	
		if (button != BUTTON_INVALID) {
			if (evt.type == SDL_KEYDOWN)
				button_states[button] = true;
			else
				button_states[button] = false;
		}
		if (buttons_do_action(blk, button_states)) {
			update_block(vbuf, blk);
		}
	}	
}

void update_rect_sitl(const vbuf_t *vbuf, bbox_t *bbox) {
	int x, x_end;
	int y, y_end;

	x_end = MIN(bbox->x + bbox->sizex, DISP_COLS);
	y_end = MIN(bbox->y + bbox->sizey, DISP_ROWS*8)/8;

	for (x = bbox->x; x < x_end; x++) {
		for (y = bbox->y/8; y < y_end; y++) {
			for (int k = 0; k < 8; k++) {
				uint8_t byte = (vbuf->buf[x][y] >> k) & 0x01;	
				draw_pixel(k+8*y, x, byte);	
			}
		}
	}

#if SITL_OVERLAY
	if (!(bbox->sizex == DISP_COLS && bbox->sizey == DISP_ROWS*8)) {
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 50);
		SDL_Rect rect = {bbox->y*ZOOM, 
				bbox->x*ZOOM, bbox->sizey*ZOOM, 
				bbox->sizex*ZOOM};
		SDL_RenderFillRect(renderer, &rect);
	}
#endif

	SDL_RenderPresent(renderer);
}

void fatal_error_sitl(const char *msg) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	endwin();

	fprintf(stderr, "%s\n", msg);
	exit(-1);
}

void led_toggle_sitl(void) {

}

char serial_getch_sitl(void) {
	return getch();
}

thdiface_t thdiface_sitl = {
	.create = thd_create_sitl,
	.exit = thd_exit_sitl,
	.lock = thd_lock_sitl,
	.unlock = thd_unlock_sitl,
	.mtx_init = thd_mtx_init_sitl,
};

void thd_create_sitl(thd_handle_t *handle) {
	SDL_CreateThreadWithStackSize(handle->func, "", 
					handle->wa_size, NULL);
}

void thd_exit_sitl(int code) {
	exit(code);
}

void thd_lock_sitl(mutex_impl_t *mtx) {
	SDL_LockMutex(*mtx);
}

void thd_unlock_sitl(mutex_impl_t *mtx) {
	SDL_UnlockMutex(*mtx);
}

void thd_mtx_init_sitl(mutex_impl_t *mtx) {
	*mtx = SDL_CreateMutex();
}
