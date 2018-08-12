   

Alopos.C
=========

A hobby operating system / OS kernel for x86 (32-bit) written in C and some assembly (GAS). Relies on a Multiboot 1 (0.6.96) compliant bootloader (GRUB, etc.).

Implements
-----------

- Multiboot 1 (0.6.96) bootloader compliance
- as-small-as-possible assembly code for jumping into C and managing interrupts
- GDT
- interrupts (IDT)
- reads the memory map and other info from the Multiboot bootloader
- VGA text mode output / display
- keyboard input with UK keyboard layout

User-facing features
---------------------

- Command line – case insensitive. Available commands:
	- `help` – prints an unhelpful message.
	- `BootInfo` – writes out info and memory map from the bootloader
	- `CharsetTest` – writes out all 256 characters in the VGA text mode character set
	- `ColourTest` – writes out all the combinations of background and foreground text colours.
	- `GDTInfo` – writes out info about the Global Descriptor Table (GDT)
	- `VarSizes` – writes out the sizes of the used C variable types.

Build dependencies
-------------------

- MinGW
- i686-elf GCC
- QEMU

How to build
-------------

Copy `/_local_templates/vars.cmd` to `/vars.cmd` and edit the dependency directories to match where you've installed them.

Run `build-run.bat` to build and run in QEMU  
Run `build.bat` to just build
Run `run.bat` to just run in QEMU
