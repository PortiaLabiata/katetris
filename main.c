#include "ch.h"

int main(void) {
	const char *test = "fuck";
	int a = 2+2;
	int b = 2+a;

	chSysInit();

	while (1) 
		a++;
}
