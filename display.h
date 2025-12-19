#pragma once

#include "ch.h"
#include "hal.h"

int disp_init();
int *i2c_scan();
void i2c_scan_print(BaseSequentialStream *s, int status[]);
