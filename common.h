#pragma once
#include <stdint.h>

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

int abs(int x);
int bound(int x, int left, int right);
