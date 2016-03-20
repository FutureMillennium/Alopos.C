
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
static const char* OS_NAME = "Alopos";
static const Byte OS_VERSION_MAJOR = 0;
static const Byte OS_VERSION_MINOR = 1;
static const Byte4 MULTIBOOT_BOOTLOADER_MAGIC = 0x2BADB002;

Index rowCurrentTerminal_VGA;
Index colCurrentTerminal_VGA;
Byte colorCurrentTerminal_VGA;
volatile Byte2* bufferTerminal_VGA;

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

void Number2String(int number, char* buffer, char base) {
	char *tmpbuffer = buffer;
	char *p1, *p2;
	unsigned long unsignedNumber = number;
	int divisor = 10;

	// %i is assumed to be signed, otherwise unsigned
	if (base == 'i' && number < 0) {
		*tmpbuffer++ = '-';
		buffer++;
		unsignedNumber = -number;
	} else if (base == 'x') {
		divisor = 16;
	} else if (base == 'b') {
		divisor = 2;
	}

	// divide by divisor until 0
	do {
		int remainder = unsignedNumber % divisor;

		*tmpbuffer++ = (remainder < 10) ? (remainder + '0') : (remainder + 'A' - 10);
	} while (unsignedNumber /= divisor);

	// 0-terminated string
	*tmpbuffer = 0;

	/* Reverse BUF. */
	p1 = buffer;
	p2 = tmpbuffer - 1;
	while (p1 < p2) {
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
}

// Echo("Foo");
// Echo("String: %s", "foo");
// Echo("String: %s=", "foo", "\nAnd another: %s", "bar");
void Echo_Terminal_VGA(const char* text, ...) {

	char** args = (char **) &text;
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
					// TODO itoa
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
	bufferTerminal_VGA = (Byte2*)0xB8000;

	for (Byte y = 0; y < ROWS_MAX_VGA; y++) {
		for (Byte x = 0; x < COLS_MAX_VGA; x++) {
			EntryPut_Terminal_VGA(0, colorCurrentTerminal_VGA, x, y);
		}
	}
}

static inline void OutByte_IO(Byte2 port, Byte val) {
	asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
	/* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
	* Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
	* The  outb  %al, %dx  encoding is the only option for all other cases.
	* %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
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

Bool FlagCheck_MultibootInfo(Byte4 flags, Byte bit) {
	return ((flags) & (1 << (bit)));
}

#if defined(__cplusplus)
extern "C" // Use C linkage for main_kernel.
#endif
void Main_Kernel(Byte4 magicNumber, Byte4 multibootInfoAddress) {

	MultibootInfoType* multibootInfo;

	Init_Terminal_VGA();
	
	ColorSet_Terminal_VGA(ColorMake_VGA(COLOR_WHITE, COLOR_BLACK));
	Echo_Terminal_VGA("%s v%i.%i\n", OS_NAME, OS_VERSION_MAJOR, OS_VERSION_MINOR);

	ColorSet_Terminal_VGA(ColorMake_VGA(COLOR_LIGHT_GREY, COLOR_BLACK));

	if (magicNumber != MULTIBOOT_BOOTLOADER_MAGIC) {
		Echo_Terminal_VGA("Not booted by multiboot: 0x%x\n", magicNumber);
	} else {
		Echo_Terminal_VGA("Booted by multiboot: 0x%x\n", magicNumber);
	}
	
	// test charset
	Echo_Terminal_VGA("Charset test:\n");
	{
		char i = 0;

		do {
			EntryPut_Terminal_VGA(i, colorCurrentTerminal_VGA, colCurrentTerminal_VGA, rowCurrentTerminal_VGA);
			if ((colCurrentTerminal_VGA++) == COLS_MAX_VGA) {
				NewLine_Terminal_VGA();
			}
			i++;
		} while (i != 0);
	}

	// test VGA colours
	Echo_Terminal_VGA("\nColours test:\n");

	{
		for (Byte i = 0; i < 16; i++) {
			for (Byte j = 0; j < 16; j++) {
				EntryPut_Terminal_VGA('0' + j, ColorMake_VGA(j, i), colCurrentTerminal_VGA, rowCurrentTerminal_VGA);
				colCurrentTerminal_VGA++;
			}
			if (i % 4 == 3) {
				NewLine_Terminal_VGA();
			}
		}
	}

	// variable sizes
	Echo_Terminal_VGA("Variable sizes:\n"
		"char: %i, int: %i, long: %i, Bool: %i, Byte: %i, Byte2: %i, Byte4: %i, Byte8: %i\n", sizeof(char), sizeof(int), sizeof(long), sizeof(Bool), sizeof(Byte), sizeof(Byte2), sizeof(Byte4), sizeof(Byte8));

	/* Set multibootInfo to the address of the Multiboot information structure. */
	multibootInfo = (MultibootInfoType *)multibootInfoAddress;

	/* Print out the flags. */
	Echo_Terminal_VGA("flags = %b, ", (unsigned)multibootInfo->flags);

	/* Are mem_* valid? */
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 0))
		Echo_Terminal_VGA("mem_lower = %u KB, mem_upper = %u KB\n",
			(unsigned)multibootInfo->mem_lower, (unsigned)multibootInfo->mem_upper);

	/* Is boot_device valid? */
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 1))
		Echo_Terminal_VGA("boot_device = 0x%x, ", (unsigned)multibootInfo->boot_device);

	// boot loader name
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 9))
		Echo_Terminal_VGA("bootloader = %s, ", (char *)multibootInfo->boot_loader_name);

	/* Is the command line passed? */
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 2))
		Echo_Terminal_VGA("cmdline = %s\n", (char *)multibootInfo->cmdline);

	/* Are mods_* valid? */
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 3)) {
		MultibootModuleType *mod;
		unsigned int i;

		Echo_Terminal_VGA("mods_count = %i, mods_addr = 0x%x\n",
			(int)multibootInfo->mods_count, (int)multibootInfo->mods_addr);
		for (i = 0, mod = (MultibootModuleType *)multibootInfo->mods_addr;
		i < multibootInfo->mods_count;
			i++, mod++)
			Echo_Terminal_VGA(" mod_start = 0x%x, mod_end = 0x%x, cmdline = %s\n",
				(unsigned)mod->mod_start,
				(unsigned)mod->mod_end,
				(char *)mod->cmdline);
	}

	/* Bits 4 and 5 are mutually exclusive! */
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 4) && FlagCheck_MultibootInfo(multibootInfo->flags, 5)) {
		Echo_Terminal_VGA("Both bits 4 and 5 are set.\n");
		// TODO error?
	}

	/* Is the symbol table of a.out valid? */
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 4)) {
		MultibootAoutSymbolTableType *multiboot_aout_sym = &(multibootInfo->u.aout_sym);

		Echo_Terminal_VGA("MultibootAoutSymbolTable: tabsize = 0x%0x, "
			"strsize = 0x%x, addr = 0x%x\n",
			(unsigned)multiboot_aout_sym->tabsize,
			(unsigned)multiboot_aout_sym->strsize,
			(unsigned)multiboot_aout_sym->addr);
	}

	/* Is the section header table of ELF valid? */
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 5)) {
		MultibootELFSymbolTableType *multiboot_elf_sec = &(multibootInfo->u.elf_sec);

		Echo_Terminal_VGA("multiboot_elf_sec: num = %u, size = 0x%x,"
			" addr = 0x%x, shndx = 0x%x\n",
			(unsigned)multiboot_elf_sec->num, (unsigned)multiboot_elf_sec->size,
			(unsigned)multiboot_elf_sec->addr, (unsigned)multiboot_elf_sec->shndx);
	}

	/* Are mmap_* valid? */
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 6)) {
		MultibootMemoryMapEntryType *mmap;

		Echo_Terminal_VGA("mmap_addr = 0x%x, mmap_length = 0x%x\n", (unsigned)multibootInfo->mmap_addr, (unsigned)multibootInfo->mmap_length);

		for (mmap = (MultibootMemoryMapEntryType *)multibootInfo->mmap_addr;
		(unsigned long)mmap < multibootInfo->mmap_addr + multibootInfo->mmap_length;
			mmap = (MultibootMemoryMapEntryType *)((unsigned long)mmap
				+ mmap->size + sizeof(mmap->size)))
			Echo_Terminal_VGA("size = 0x%x, base_addr = 0x%x%x,"
				" length = 0x%x%x, type = 0x%x\n",
				(unsigned)mmap->size,
				(Byte4)(mmap->base_addr >> 32),
				(Byte4)(mmap->base_addr & 0xffffffff),
				(Byte4)(mmap->length >> 32),
				(Byte4)(mmap->length & 0xffffffff),
				(unsigned)mmap->type);
	}

	CursorSet_Terminal_VGA(rowCurrentTerminal_VGA, colCurrentTerminal_VGA);

}
