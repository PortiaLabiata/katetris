#include "common.h"

int abs(int x) {
	return x >= 0 ? x : -x;
}

int bound(int x, int left, int right) {
	if (x < left) x = left;
	if (x > right) x = right;
	return x;
}
