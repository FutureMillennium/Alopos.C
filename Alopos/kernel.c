
// Check if the compiler thinks we are targeting the wrong operating system.
#if defined(__linux__)
#error "Attempting to compile to Linux."
#endif

// Will only work for the 32-bit ix86 targets.
#if !defined(__i386__)
#error "Needs to be compiled with a ix86-elf compiler"
#endif


#include "VarDefs.h"
#include "Multiboot.h"
#include "Global.h"

#include "Memory.c"
#include "StringFunctions.c"
#include "GDT.c"
#include "IDT.c"
#include "Multiboot.c"
#include "IO.c"
#include "VGATextMode.c"
#include "PS2Keyboard.c"
#include "CommandLine.c"



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

	magicNumber_boot = magicNumber;
	isKeyboardAcceptingInput = false;
	isAcceptCommand = false;

	GDTInit();
	IDTInit(); // interrupts, exceptions and IRQ
	Init_Terminal_VGA();
	InterruptsEnable();

	GetCursorPosition_Terminal_VGA();
	
	ColorSet_Terminal_VGA(ColorMake_VGA(COLOR_WHITE, COLOR_BLACK));
	Echo_Terminal_VGA("\n%s v%i.%i\n", OS_NAME, OS_MAJOR_VERSION, OS_MINOR_VERSION);

	ColorReset_Terminal_VGA();

	// Set multibootInfo to the address of the Multiboot information structure.
	multibootInfo = (MultibootInfo*)multibootInfoAddress;

	// Tests:

	if (magicNumber != MULTIBOOT_BOOTLOADER_MAGIC) {
		ColorSet_Terminal_VGA(ColorMake_VGA(COLOR_RED, COLOR_BLACK));
		Echo_Terminal_VGA("Warning: not booted correctly. System may not work. (0x%x)\n", magicNumber);
		ColorReset_Terminal_VGA();
	} else {
		//Echo_Terminal_VGA("Booted by multiboot: 0x%x\n", magicNumber);
	}

	// Bits 4 and 5 are mutually exclusive!
	if (FlagCheck_MultibootInfo(multibootInfo->flags, 4) && FlagCheck_MultibootInfo(multibootInfo->flags, 5)) {
		ColorSet_Terminal_VGA(ColorMake_VGA(COLOR_RED, COLOR_BLACK));
		Echo_Terminal_VGA("Warning: both Multiboot bits 4 and 5 are set.\n");
		ColorReset_Terminal_VGA();
		// TODO error?
	}

	Echo_Terminal_VGA("Type 'help' for a list of commands.\n");

	CommandInput();

kernelLoop:
	HaltExecution();

	if (isAcceptCommand) {
		CommandTryAccept();
	}

	goto kernelLoop;

}
