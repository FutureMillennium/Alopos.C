#pragma once
// VGA text mode 3

// ---------------------------------------------
// constants
// ---------------------------------------------

static const Index COLS_MAX_VGA = 80;
static const Index ROWS_MAX_VGA = 25;


// ---------------------------------------------
// enums
// ---------------------------------------------

enum Color_VGA {
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};


// ---------------------------------------------
// function definitions
// ---------------------------------------------

void Backspace_Terminal_VGA();
void PutChar_Terminal_VGA(char character);
void Echo_Terminal_VGA(const char* text, ...);
void Cursor2CurrentPos_Terminal_VGA();


// ---------------------------------------------
// vars
// ---------------------------------------------

Index rowCurrentTerminal_VGA;
Index colCurrentTerminal_VGA;
Byte colorCurrentTerminal_VGA;
volatile Byte2* bufferTerminal_VGA;
