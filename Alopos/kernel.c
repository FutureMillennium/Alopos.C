
// Check if the compiler thinks we are targeting the wrong operating system.
#if defined(__linux__)
#error "Attempting to compile to Linux."
#endif

// Will only work for the 32-bit ix86 targets.
#if !defined(__i386__)
#error "Needs to be compiled with a ix86-elf compiler"
#endif


#include "VarDefs.h"
#include "Global.h"

#include "Memory.c"
#include "StringFunctions.c"
#include "GDT.c"
#include "IDT.c"
#include "Multiboot.c"
#include "IO.c"
#include "VGATextMode.c"


inline void InterruptsEnable() {
#if defined(__GNUC__)
	asm volatile ("sti");
#endif
}

inline void HaltExecution() {
#if defined(__GNUC__)
	asm volatile ("hlt");
#endif
}


#if defined(__cplusplus)
extern "C" // Use C linkage for main_kernel.
#endif
void Main_Kernel(Byte4 magicNumber, Byte4 multibootInfoAddress) {

	MultibootInfo* multibootInfo;

	GDTInit();
	IDTInit(); // interrupts, exceptions and IRQ
	Init_Terminal_VGA();
	InterruptsEnable();

	GetCursorPosition_Terminal_VGA();

	/*{
		Byte i = 0;
		Echo_Terminal_VGA("%i: 0x%x %x = 0x%x 0x%x %b\n", i, idt.entries[i].offset_high, idt.entries[i].offset_low, (Byte4)ISRoutine0, idt.entries[i].selector, idt.entries[i].flags);
	}*/

	ColorSet_Terminal_VGA(ColorMake_VGA(COLOR_WHITE, COLOR_BLACK));
	Echo_Terminal_VGA("\n%s v%i.%i\n", OS_NAME, OS_MAJOR_VERSION, OS_MINOR_VERSION);

	ColorSet_Terminal_VGA(ColorMake_VGA(COLOR_LIGHT_GREY, COLOR_BLACK));


	// Tests:

	if (magicNumber != MULTIBOOT_BOOTLOADER_MAGIC) {
		Echo_Terminal_VGA("Not booted by multiboot: 0x%x\n", magicNumber);
	} else {
		Echo_Terminal_VGA("Booted by multiboot: 0x%x\n", magicNumber);
	}
	
	// test charset
	/*Echo_Terminal_VGA("Charset test:\n");

	{
		char i = 0;

		do {
			EntryPut_Terminal_VGA(i, colorCurrentTerminal_VGA, colCurrentTerminal_VGA, rowCurrentTerminal_VGA);
			colCurrentTerminal_VGA++;
			if (colCurrentTerminal_VGA == COLS_MAX_VGA) {
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
	*/

	/*// variable sizes
	Echo_Terminal_VGA("Variable sizes: "
		"char: %i, short: %i, int: %i, long: %i, Bool: %i, Byte: %i, Byte2: %i, Byte4: %i, Byte8: %i\n", sizeof(char), sizeof(short), sizeof(int), sizeof(long), sizeof(Bool), sizeof(Byte), sizeof(Byte2), sizeof(Byte4), sizeof(Byte8));
		*/

	// Set multibootInfo to the address of the Multiboot information structure.
	multibootInfo = (MultibootInfo*)multibootInfoAddress;

	// Print out the flags.
	Echo_Terminal_VGA("flags = %b, ", (unsigned)multibootInfo->flags);

	// Are mem_* valid?
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 0))
		Echo_Terminal_VGA("mem_lower = %u KB, mem_upper = %u KB\n",
			(unsigned)multibootInfo->mem_lower, (unsigned)multibootInfo->mem_upper);

	// Is boot_device valid?
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 1))
		Echo_Terminal_VGA("boot_device = 0x%x, ", (unsigned)multibootInfo->boot_device);

	// boot loader name
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 9))
		Echo_Terminal_VGA("bootloader = %s, ", (char *)multibootInfo->boot_loader_name);

	// Is the command line passed?
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 2))
		Echo_Terminal_VGA("cmdline = %s\n", (char *)multibootInfo->cmdline);

	// Are mods_* valid?
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 3)) {
		MultibootModule* mod;
		unsigned int i;

		Echo_Terminal_VGA("mods_count = %i, mods_addr = 0x%x\n",
			(int)multibootInfo->mods_count, (int)multibootInfo->mods_addr);
		for (i = 0, mod = (MultibootModule*)multibootInfo->mods_addr;
		i < multibootInfo->mods_count;
			i++, mod++)
			Echo_Terminal_VGA(" mod_start = 0x%x, mod_end = 0x%x, cmdline = %s\n",
				(unsigned)mod->mod_start,
				(unsigned)mod->mod_end,
				(char *)mod->cmdline);
	}

	// Bits 4 and 5 are mutually exclusive!
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 4) && FlagCheck_MultibootInfo(multibootInfo->flags, 5)) {
		Echo_Terminal_VGA("Both bits 4 and 5 are set.\n");
		// TODO error?
	}

	// Is the symbol table of a.out valid?
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 4)) {
		MultibootAoutSymbolTable* multiboot_aout_sym = &(multibootInfo->u.aout_sym);

		Echo_Terminal_VGA("MultibootAoutSymbolTable: tabsize = 0x%0x, "
			"strsize = 0x%x, addr = 0x%x\n",
			(unsigned)multiboot_aout_sym->tabsize,
			(unsigned)multiboot_aout_sym->strsize,
			(unsigned)multiboot_aout_sym->addr);
	}

	// Is the section header table of ELF valid?
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 5)) {
		MultibootELFSymbolTable* multiboot_elf_sec = &(multibootInfo->u.elf_sec);

		Echo_Terminal_VGA("multiboot_elf_sec: num = %u, size = 0x%x,"
			" addr = 0x%x, shndx = 0x%x\n",
			(unsigned)multiboot_elf_sec->num, (unsigned)multiboot_elf_sec->size,
			(unsigned)multiboot_elf_sec->addr, (unsigned)multiboot_elf_sec->shndx);
	}

	// Are mmap_* valid?
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 6)) {
		MultibootMemoryMapEntry* mmap;

		Echo_Terminal_VGA("mmap_addr = 0x%x, mmap_length = 0x%x\n", (unsigned)multibootInfo->mmap_addr, (unsigned)multibootInfo->mmap_length);

		for (mmap = (MultibootMemoryMapEntry*) multibootInfo->mmap_addr;
		(unsigned long)mmap < multibootInfo->mmap_addr + multibootInfo->mmap_length;
			mmap = (MultibootMemoryMapEntry*)((unsigned long)mmap
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

	// GDT data test
	/*for (Byte i = 0; i < 3; i++) {
		Echo_Terminal_VGA("%i: gran %b %x, access %b %x\n", i, gdt.entries[i].granularity, gdt.entries[i].granularity, gdt.entries[i].access, gdt.entries[i].access);
	}*/

	CursorSet_Terminal_VGA(rowCurrentTerminal_VGA, colCurrentTerminal_VGA);

kernelLoop:
	HaltExecution();
	goto kernelLoop;

}
