
// VGA text mode 3

#include "VarDefs.h"

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
// vars
// ---------------------------------------------

Index rowCurrentTerminal_VGA;
Index colCurrentTerminal_VGA;
Byte colorCurrentTerminal_VGA;
volatile Byte2* bufferTerminal_VGA;


// ---------------------------------------------
// functions
// ---------------------------------------------


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
		colCurrentTerminal_VGA++;
		if (colCurrentTerminal_VGA == COLS_MAX_VGA) {
			NewLine_Terminal_VGA();
		}
	}
}


// Echo("Foo");
// Echo("String: %s", "foo");
// Echo("String: %s%", "foo", "\nAnd another: %s", "bar");
void Echo_Terminal_VGA(const char* text, ...) {

	char** args = (char **)&text;
	char* arg = *args;

	char c;

	while ((c = *arg++) != 0) {
		if (c == '%') {
			c = *arg++;
			char* tmp;

			switch (c) {
				case '%':
					PutChar_Terminal_VGA(c);
					break;

				case 'i': // signed int
				case 'u': // unsigned int
				case 'x': // hex
				case 'b': // binary
				{
					char buffer[17];
					args++;
					Number2String(*(int*)args, buffer, c);
					tmp = buffer;
					goto echoString;
				}
				break;

				case 's':
					args++;
					tmp = *args;

				echoString:
					while (*tmp)
						PutChar_Terminal_VGA(*tmp++);
					break;
			}
			tmp = arg;
			if (*tmp++ == '%' && *tmp == 0) { // go to next string
				args++;
				arg = *args;
			}

		} else {
			PutChar_Terminal_VGA(c);
		}
	}
}

void Init_Terminal_VGA() {
	colorCurrentTerminal_VGA = ColorMake_VGA(COLOR_LIGHT_GREY, COLOR_BLACK);

	rowCurrentTerminal_VGA = 0;
	colCurrentTerminal_VGA = 0;

	// TODO monochrome monitors at 0xB0000?
	// TODO video address const
	// TODO is it always 0xB8000?
	bufferTerminal_VGA = (Byte2*)0xB8000; // VGA text mode 3 video buffer address

	for (Byte y = 0; y < ROWS_MAX_VGA; y++) {
		for (Byte x = 0; x < COLS_MAX_VGA; x++) {
			EntryPut_Terminal_VGA(0, colorCurrentTerminal_VGA, x, y);
		}
	}
}

void CursorSet_Terminal_VGA(Index row, Index col) {
	unsigned short position = (row * 80) + col;

	// TODO get cursor port address from BIOS info?

	// cursor LOW port to vga INDEX register
	OutByte_IO(0x3D4, 0x0F);
	OutByte_IO(0x3D5, (Byte)(position & 0xFF));
	// cursor HIGH port to vga INDEX register
	OutByte_IO(0x3D4, 0x0E);
	OutByte_IO(0x3D5, (Byte)((position >> 8) & 0xFF));
}
