#pragma once
#include "tetris.h"

#define CHAR_SIZE 5
typedef uint8_t char_t[CHAR_SIZE];
typedef struct {
	char c;
	char_t ptr;
} font_map_t;

static font_map_t font_map[] = {
	{0,
	{0b11111,
	 0b10001,
	 0b10001,
	 0b10001,
	 0b11111}},

	{1,
	{0b00100,
	 0b00110,
	 0b00100,
	 0b00100,
	 0b01110}},

	{2,
	{0b00010,
	 0b00101,
	 0b00100,
	 0b00010,
	 0b00111}},

	{3,
	{0b11110,
	 0b10000,
	 0b11110,
	 0b10000,
	 0b11110}},

	{4,
	{0b10010,
	 0b10010,
	 0b11110,
	 0b10000,
	 0b10000}},

	{5,
	{0b11110,
	 0b00010,
	 0b11110,
	 0b10000,
	 0b11110}},

	{6,
	{0b11110,
	 0b10000,
	 0b11110,
	 0b10010,
	 0b11110}},

	{7,
	{0b11110,
	 0b10000,
	 0b01000,
	 0b00100,
	 0b00010}},

	{8,
	{0b11110,
	 0b10010,
	 0b11110,
	 0b10010,
	 0b11110}},

	{9,
	{0b11110,
	 0b10010,
	 0b11110,
	 0b10000,
	 0b11110}},
};

#define FONT_MAP_SIZE sizeof(font_map)/sizeof(font_map_t)
