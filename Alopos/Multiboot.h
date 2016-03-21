#pragma once
// Multiboot info

// ---------------------------------------------
// constants
// ---------------------------------------------

static const Byte4 MULTIBOOT_BOOTLOADER_MAGIC = 0x2BADB002; // Multiboot bootloader magic number


// ---------------------------------------------
// structs
// ---------------------------------------------

// bit 4
struct MultibootAoutSymbolTable {
	Byte4 tabsize;
	Byte4 strsize;
	Byte4 addr;
	Byte4 reserved;
};
typedef struct MultibootAoutSymbolTable MultibootAoutSymbolTableType;

// bit 5
struct MultibootELFSymbolTable {
	Byte4 num;
	Byte4 size;
	Byte4 addr;
	Byte4 shndx;
};
typedef struct MultibootELFSymbolTable MultibootELFSymbolTableType;

struct MultibootInfo {
	/* Multiboot info version number */
	Byte4 flags;

	// bit 0 – available memory from BIOS
	Byte4 mem_lower;
	Byte4 mem_upper;

	// bit 1 – "root" partition
	Byte4 boot_device;

	// bit 2 – Kernel command line
	Byte4 cmdline;

	// bit 3 – Boot-Module list
	Byte4 mods_count;
	Byte4 mods_addr;

	// bit 4 or 5
	union {
		struct MultibootAoutSymbolTable aout_sym; // bit 4
		struct MultibootELFSymbolTable elf_sec; // bit 5
	} u;

	// bit 6 – Memory Mapping buffer
	Byte4 mmap_length;
	Byte4 mmap_addr;

	// bit 7 – Drive Info buffer
	Byte4 drives_length;
	Byte4 drives_addr;

	// bit 8 – ROM configuration table
	Byte4 config_table;

	// bit 9 – Boot Loader Name
	Byte4 boot_loader_name;

	// bit 10 – APM (Advanced Power Management) table
	Byte4 apm_table;

	// bit 11 – VESA BIOS Extensions (VBE)
	Byte4 vbe_control_info;
	Byte4 vbe_mode_info;
	Byte2 vbe_mode;
	Byte2 vbe_interface_seg;
	Byte2 vbe_interface_off;
	Byte2 vbe_interface_len;
};
typedef struct MultibootInfo MultibootInfoType;

// bit 3 – Boot-Module list
struct MultibootModule {
	/* the memory used goes from bytes 'mod_start' to 'mod_end-1' inclusive */
	Byte4 mod_start;
	Byte4 mod_end;

	/* Module command line */
	Byte4 cmdline;

	/* padding to take it to 16 bytes (must be zero) */
	Byte4 pad;
};
typedef struct MultibootModule MultibootModuleType;

// bit 6 – Memory Mapping
struct MultibootMemoryMapEntry {
	Byte4 size;
	Byte8 base_addr;
	Byte8 length;
	Byte4 type;
}
#if defined(__GNUC__)
__attribute__((packed))
#endif
;
typedef struct MultibootMemoryMapEntry MultibootMemoryMapEntryType;
