#include "impl_sitl.h"

#include <unistd.h>
#include <time.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <curses.h>
#include "common.h"

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
	update_rect_sitl(vbuf, &(bbox_t){0, 0, DISP_COLS, DISP_ROWS});
}

void update_rect_sitl(const vbuf_t *vbuf, bbox_t *bbox) {
	SDL_Event evt;
	SDL_PollEvent(&evt);
	if (evt.type == SDL_QUIT) {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		endwin();
		exit(0);
	}

	int x, x_end;
	int y, y_end;

	x_end = bbox->x + bbox->sizex;
	y_end = bbox->y + bbox->sizey;

	for (x = bbox->x; x < x_end; x++) {
		for (y = bbox->y; y < MIN(y_end, DISP_ROWS); y++) {
			for (int k = 0; k < 8; k++) {
				uint8_t byte = (vbuf->buf[x][y] >> k) & 0x01;	
				draw_pixel(k+8*y, x, byte);	
			}
		}
	}

#if SITL_OVERLAY
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 50);
	SDL_Rect rect = {bbox->y*ZOOM, 
			bbox->x*ZOOM, bbox->sizey*ZOOM, 
			bbox->sizex*ZOOM};
	SDL_RenderFillRect(renderer, &rect);
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
