#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>

/* Check if the compiler thinks we are targeting the wrong operating system. */
#if defined(__linux__)
#error "Attempting to compile to Linux."
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "Needs to be compiled with a ix86-elf compiler"
#endif

/* Hardware text mode color constants. */
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

uint8_t ColorMake_VGA(enum Color_VGA fg, enum Color_VGA bg) {
	return fg | bg << 4;
}

uint16_t EntryMake_VGA(char c, uint8_t color) {
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}

size_t Length_String(const char* str) {
	size_t ret = 0;
	while (str[ret] != 0)
		ret++;
	return ret;
}

static const size_t COLS_VGA = 80;
static const size_t ROWS_VGA = 25;

size_t rowCurrentTerminal_VGA;
size_t colCurrentTerminal_VGA;
uint8_t colorCurrentTerminal_VGA;
uint16_t* bufferTerminal_VGA;

void Init_Terminal_VGA() {
	rowCurrentTerminal_VGA = 0;
	colCurrentTerminal_VGA = 0;
	colorCurrentTerminal_VGA = ColorMake_VGA(COLOR_LIGHT_GREY, COLOR_BLACK);
	bufferTerminal_VGA = (uint16_t*)0xB8000; // TODO monochrome monitors at 0xB0000?
	/*for (size_t y = 0; y < ROWS_VGA; y++) {
		for (size_t x = 0; x < COLS_VGA; x++) {
			const size_t index = y * COLS_VGA + x;
			bufferTerminal_VGA[index] = make_vgaentry(' ', colorCurrentTerminal_VGA);
		}
	}*/
}

void ColorSet_Terminal_VGA(uint8_t color) {
	colorCurrentTerminal_VGA = color;
}

void EntryPut_Terminal_VGA(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * COLS_VGA + x;
	bufferTerminal_VGA[index] = EntryMake_VGA(c, color);
}

void PutChar_Terminal_VGA(char c) {
	EntryPut_Terminal_VGA(c, colorCurrentTerminal_VGA, colCurrentTerminal_VGA, rowCurrentTerminal_VGA);
	if (++colCurrentTerminal_VGA == COLS_VGA) {
		colCurrentTerminal_VGA = 0;
		if (++rowCurrentTerminal_VGA == ROWS_VGA) {
			rowCurrentTerminal_VGA = 0;
		}
	}
}

void Write_Terminal_VGA(const char* data) {
	size_t datalen = Length_String(data);
	for (size_t i = 0; i < datalen; i++)
		PutChar_Terminal_VGA(data[i]);
}

#if defined(__cplusplus)
extern "C" /* Use C linkage for main_kernel. */
#endif
void Main_Kernel() {
	/* Initialize terminal interface */
	Init_Terminal_VGA();
	
	rowCurrentTerminal_VGA = ROWS_VGA - 1;
	Write_Terminal_VGA("Alopos says hello!");
}
