
#include "VarDefs.h"


// Check if the compiler thinks we are targeting the wrong operating system.
#if defined(__linux__)
#error "Attempting to compile to Linux."
#endif

// Will only work for the 32-bit ix86 targets.
#if !defined(__i386__)
#error "Needs to be compiled with a ix86-elf compiler"
#endif

// Hardware text mode color constants.
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

Byte ColorMake_VGA(enum Color_VGA foreground, enum Color_VGA background) {
	return foreground | background << 4;
}

Byte2 EntryMake_VGA(char character, Byte color) {
	Byte2 c16 = character;
	Byte2 color16 = color;
	return c16 | color16 << 8;
}

Index Length_String(const char* input) {
	Index length = 0;
	while (input[length] != 0)
		length++;
	return length;
}

static const Index COLS_MAX_VGA = 80;
static const Index ROWS_MAX_VGA = 25;

Index rowCurrentTerminal_VGA;
Index colCurrentTerminal_VGA;
Byte colorCurrentTerminal_VGA;
volatile Byte2* bufferTerminal_VGA;

void Init_Terminal_VGA() {
	rowCurrentTerminal_VGA = ROWS_MAX_VGA - 1;
	colCurrentTerminal_VGA = 0;

	colorCurrentTerminal_VGA = ColorMake_VGA(COLOR_LIGHT_GREY, COLOR_BLACK);

	// TODO monochrome monitors at 0xB0000?
	// TODO video address const
	bufferTerminal_VGA = (Byte2*)0xB8000;
}

void ColorSet_Terminal_VGA(Byte color) {
	colorCurrentTerminal_VGA = color;
}

void EntryPut_Terminal_VGA(char character, Byte color, Index x, Index y) {
	const Index index = y * COLS_MAX_VGA + x;
	bufferTerminal_VGA[index] = EntryMake_VGA(character, color);
}

void NewLine_Terminal_VGA() {
	colCurrentTerminal_VGA = 0;
	if (++rowCurrentTerminal_VGA == ROWS_MAX_VGA) {
		rowCurrentTerminal_VGA = 0;
		// TODO scroll screen
	}

	// clear line
	for (Index i = 0; i < COLS_MAX_VGA; i++) {
		EntryPut_Terminal_VGA(0, colorCurrentTerminal_VGA, i, rowCurrentTerminal_VGA);
	}
}

void PutChar_Terminal_VGA(char character) {
	if (character == '\n') {
		NewLine_Terminal_VGA();
	} else {
		EntryPut_Terminal_VGA(character, colorCurrentTerminal_VGA, colCurrentTerminal_VGA, rowCurrentTerminal_VGA);
		if ((colCurrentTerminal_VGA++) == COLS_MAX_VGA) {
			NewLine_Terminal_VGA();
		}
	}
}

void Echo_Terminal_VGA(const char* text) {
	Index index = 0;
	while (text[index] != 0) {
		PutChar_Terminal_VGA(text[index]);
		index++;
	}
}

#if defined(__cplusplus)
extern "C" // Use C linkage for main_kernel.
#endif
void Main_Kernel() {

	Init_Terminal_VGA();
	
	Echo_Terminal_VGA("Alopos says hello!\nCharset test:\n");

	// test charset
	char i = 0;

	do {
		EntryPut_Terminal_VGA(i, colorCurrentTerminal_VGA, colCurrentTerminal_VGA, rowCurrentTerminal_VGA);
		if ((colCurrentTerminal_VGA++) == COLS_MAX_VGA) {
			NewLine_Terminal_VGA();
		}
		i++;
	} while (i != 0);

}
