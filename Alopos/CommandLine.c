
// command line

#include "VarDefs.h"
#include "Global.h"
#include "VGATextMode.h"
#include "PS2Keyboard.h"
#include "Multiboot.h"

// commands:

void Help() {
	Echo_Terminal_VGA("TODO help");
}

void CharsetTest() {
	Echo_Terminal_VGA("VGA character set (charset) test:\n");

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
}

void ColourTest() {
	// test VGA colours
	Echo_Terminal_VGA("VGA colours test:\n");

	{
		for (Byte i = 0; i < 16; i++) {
			for (Byte j = 0; j < 16; j++) {
				EntryPut_Terminal_VGA(((j > 9) ? 'A' + j - 10 : '0' + j), ColorMake_VGA(j, i), colCurrentTerminal_VGA, rowCurrentTerminal_VGA);
				colCurrentTerminal_VGA++;
			}
			if (i % 4 == 3) {
				NewLine_Terminal_VGA();
			}
		}
	}
}

void VarSizes() {
	Echo_Terminal_VGA("Variable sizes: "
		"char: %i, short: %i, int: %i, long: %i, Bool: %i, Byte: %i, Byte2: %i, Byte4: %i, Byte8: %i", sizeof(char), sizeof(short), sizeof(int), sizeof(long), sizeof(Bool), sizeof(Byte), sizeof(Byte2), sizeof(Byte4), sizeof(Byte8));
}

void GDTInfo() {
	// GDT data test
	for (Byte i = 0; i < 3; i++) {
		Echo_Terminal_VGA("%i: gran %b (0x%x), access %b (0x%x)\n", i, gdt.entries[i].granularity, gdt.entries[i].granularity, gdt.entries[i].access, gdt.entries[i].access);
	}
}

void BootInfo() {
	if (magicNumber_boot != MULTIBOOT_BOOTLOADER_MAGIC) {
		Echo_Terminal_VGA("Not booted by multiboot: 0x%x\n", magicNumber_boot);
	} else {
		Echo_Terminal_VGA("Booted by multiboot: 0x%x\n", magicNumber_boot);
	}

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

	// Is the symbol table of a.out valid?
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 4)) {
		MultibootAoutSymbolTable* multiboot_aout_sym = &(multibootInfo->u.aout_sym);

		Echo_Terminal_VGA("MultibootAoutSymbolTable: tabsize = 0x%0x, "
			"strsize = 0x%x, addr = 0x%x\n",
			(unsigned)multiboot_aout_sym->tabsize,
			(unsigned)multiboot_aout_sym->strsize,
			(unsigned)multiboot_aout_sym->addr);

		// Is the section header table of ELF valid?
	} else if (FlagCheck_MultibootInfo(multibootInfo->flags, 5)) {
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

		for (mmap = (MultibootMemoryMapEntry*)multibootInfo->mmap_addr;
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
}

void CommandInput() {
	Echo_Terminal_VGA("\n>");
	// TODO cursor enable
	Cursor2CurrentPos_Terminal_VGA();
	isKeyboardAcceptingInput = true;
}

void CommandTryAccept() {
	isKeyboardAcceptingInput = false;
	isAcceptCommand = false;
	// TODO cursor disable
	PutChar_Terminal_VGA('\n');

	if (EqualStrings(keyboardBuffer, "help")) {
		Help();
	}  else if (EqualStrings(keyboardBuffer, "GDTInfo")) {
		GDTInfo();
	} else if (EqualStrings(keyboardBuffer, "BootInfo")) {
		BootInfo();
	} else if (EqualStrings(keyboardBuffer, "VarSizes")) {
		VarSizes();
	} else if (EqualStrings(keyboardBuffer, "ColourTest")) {
		ColourTest();
	} else if (EqualStrings(keyboardBuffer, "CharsetTest")) {
		CharsetTest();
	} else {
		Echo_Terminal_VGA("Invalid command. Type 'help' for a list of commands.");
	}

	keyboardBuffer[0] = 0;
	posKeyboardBuffer = 0;

	CommandInput();
}
