
// PS/2 keyboard driver

#include "VarDefs.h"
#include "Global.h"
#include "VGATextMode.h"

// ---------------------------------------------
// enums
// ---------------------------------------------

enum Keys_Keyboard {
	KEY_LSHIFT = 42,
	KEY_RSHIFT = 54,
	KEY_CTRL = 29,
	KEY_ALT = 59,
	KEY_ENTER = 0x1c,
	KEY_BACKSPACE = 0x0E,
};


// ---------------------------------------------
// constants
// ---------------------------------------------

#define KEYBOARD_BUFFER_SIZE 32

char ukKeyboardLayout[128] = {
	0,  0, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
	'9', '0', '-', '=', 0,	/* Backspace */
	'\t',			/* Tab */
	'q', 'w', 'e', 'r',	/* 19 */
	't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',		/* Enter key */
	0,			/* 29   - Control */
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
	'\'', '`',   0,		/* Left shift */
	'\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
	'm', ',', '.', '/',   0,					/* Right shift */
	'*',
	0,	/* Alt */
	' ',	/* Space bar */
	0,	/* Caps lock */
	0,	/* 59 - F1 key ... > */
	0,   0,   0,   0,   0,   0,   0,   0,
	0,	/* < ... F10 */
	0,	/* 69 - Num lock*/
	0,	/* Scroll Lock */
	0,	/* Home key */
	0,	/* Up Arrow */
	0,	/* Page Up */
	'-',
	0,	/* Left Arrow */
	0,
	0,	/* Right Arrow */
	'+',
	0,	/* 79 - End key*/
	0,	/* Down Arrow */
	0,	/* Page Down */
	0,	/* Insert Key */
	0,	/* Delete Key */
	0,   0,   0,
	0,	/* F11 Key */
	0,	/* F12 Key */
	0,	/* All other keys are undefined */
};
char ukShiftedKeyboardLayout[128] = {
	0,  0, '!', '@', '#', '$', '%', '^', '&', '*',
	'(', ')', '_', '+', 0,	/* BACKSPACE */
	'\t',			/* TAB */
	'Q', 'W', 'E', 'R',	/* 19 */
	'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', /* ENTER KEY */
	0, /* 29   - CONTROL */
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 39 */
	'"', '~',   0,		/* LEFT SHIFT */
	'|', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
	'M', '<', '>', '?',   0,					/* RIGHT SHIFT */
	'*',
	0,	/* Alt */
	' ',	/* Space bar */
	0,	/* Caps lock */
	0,	/* 59 - F1 key ... > */
	0,   0,   0,   0,   0,   0,   0,   0,
	0,	/* < ... F10 */
	0,	/* 69 - Num lock*/
	0,	/* Scroll Lock */
	0,	/* Home key */
	0,	/* Up Arrow */
	0,	/* Page Up */
	'-',
	0,	/* Left Arrow */
	0,
	0,	/* Right Arrow */
	'+',
	0,	/* 79 - End key*/
	0,	/* Down Arrow */
	0,	/* Page Down */
	0,	/* Insert Key */
	0,	/* Delete Key */
	0,   0,   0,
	0,	/* F11 Key */
	0,	/* F12 Key */
	0,	/* All other keys are undefined */
};


// ---------------------------------------------
// vars
// ---------------------------------------------

Bool isShiftPressed = false;
char keyboardBuffer[KEYBOARD_BUFFER_SIZE];
Index posKeyboardBuffer = 0;


// ---------------------------------------------
// functions
// ---------------------------------------------

void Keyboard_Init() {
	posKeyboardBuffer = 0;
	keyboardBuffer[0] = 0;
}

void KeyboardIRQ() {
	Byte key = InByte_IO(0x60);
	if (key & 0b10000000) { // bit 7, key released

		key &= ~(1 << 7); // set bit 7 to 0
		if (key == KEY_LSHIFT || key == KEY_RSHIFT) {
			isShiftPressed = false;
		}
		
	} else {
		if (key == KEY_LSHIFT || key == KEY_RSHIFT) {
			isShiftPressed = true;
		} else if (key == KEY_ALT) {

		} else if (key == KEY_CTRL) {

		} else if (isKeyboardAcceptingInput) {
			if (key == KEY_ENTER) {
				isAcceptCommand = true;
			}  else if (key == KEY_BACKSPACE && posKeyboardBuffer > 0) {
				posKeyboardBuffer--;
				keyboardBuffer[posKeyboardBuffer] = 0;
				Backspace_Terminal_VGA();
				Cursor2CurrentPos_Terminal_VGA();
			} else {
				Byte character;
				if (isShiftPressed)
					character = ukShiftedKeyboardLayout[key];
				else
					character = ukKeyboardLayout[key];

				if (character != 0 && posKeyboardBuffer < KEYBOARD_BUFFER_SIZE) {
					keyboardBuffer[posKeyboardBuffer] = character;
					posKeyboardBuffer++;
					keyboardBuffer[posKeyboardBuffer] = 0;
					PutChar_Terminal_VGA(character);
					Cursor2CurrentPos_Terminal_VGA();
				}
			}
		}
	}
}
