#include "ch.h"
#include "hal.h"

typedef bool button_state_t;
typedef struct {
	ioline_t line;
	button_state_t state;
	uint32_t counter;
	void (*cb)(void*);
} button_t;

ioline_t led_line = PAL_LINE(GPIOC, 13);

#define DEBOUNCE_TIMES 20
void button_cb(void *arg) {
	button_t *button = (button_t*)arg;
	chSysLockFromISR();

	if (button->counter >= DEBOUNCE_TIMES) {
		button->state = !button->state;
		palToggleLine(led_line);
	} 
	button->counter = 0;

	chSysUnlockFromISR();
}

button_state_t get_button_state(button_t *btn) {
	chSysLock();
	button_state_t state = btn->state;
	chSysUnlock();
	return state;
}

void button_tick(button_t *button) {
	chSysLock();
	button->counter++;
	chSysUnlock();
}

button_t buttons[] = {
	{PAL_LINE(GPIOB, 12), 0, 0, button_cb},
	{PAL_LINE(GPIOB, 13), 0, 0, button_cb},
	{PAL_LINE(GPIOB, 14), 0, 0, button_cb},
	{PAL_LINE(GPIOB, 15),  0, 0, button_cb},
	{PAL_LINE(GPIOA, 11),  0, 0, button_cb},
};
#define NBUTTONS sizeof(buttons)/sizeof(button_t)

int main(void) {
	halInit();
	chSysInit();
	palSetLineMode(led_line, PAL_MODE_OUTPUT_PUSHPULL);

	for (int i = 0; i < NBUTTONS; i++) {
		ioline_t line = buttons[i].line;
	
		palSetLineMode(line, PAL_MODE_INPUT_PULLDOWN);
		palEnableLineEvent(line, PAL_EVENT_MODE_BOTH_EDGES);
		if (buttons[i].cb)
			palSetLineCallback(line, buttons[i].cb, &buttons[i]);
	}

	while (1) {
		for (int i = 0; i < NBUTTONS; i++) {
			button_tick(&buttons[i]);
		}
		osalThreadSleepMilliseconds(1);	
	}
}
