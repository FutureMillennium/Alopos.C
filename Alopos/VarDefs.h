#pragma once

#ifndef __cplusplus

#define bool	_Bool
#define true	1
#define false	0

#else // __cplusplus

#define _Bool	bool
#define bool	bool
#define false	false
#define true	true

#endif /* __cplusplus */


typedef __UINT8_TYPE__ Uint8;
typedef __UINT8_TYPE__ Byte;

typedef __UINT16_TYPE__ Uint16;
typedef __UINT16_TYPE__ Byte2;

typedef __UINT32_TYPE__ Uint32;
typedef __UINT32_TYPE__ Byte4;

typedef __UINT64_TYPE__ Uint64;
typedef __UINT64_TYPE__ Byte8;

typedef __SIZE_TYPE__ Index;


// Multiboot info

struct multiboot_aout_symbol_table {
	Byte4 tabsize;
	Byte4 strsize;
	Byte4 addr;
	Byte4 reserved;
};
typedef struct multiboot_aout_symbol_table multiboot_aout_symbol_table_t;

struct multiboot_elf_section_header_table {
	Byte4 num;
	Byte4 size;
	Byte4 addr;
	Byte4 shndx;
};
typedef struct multiboot_elf_section_header_table multiboot_elf_section_header_table_t;

struct multiboot_info {
	/* Multiboot info version number */
	Byte4 flags;

	/* Available memory from BIOS */
	Byte4 mem_lower;
	Byte4 mem_upper;

	/* "root" partition */
	Byte4 boot_device;

	/* Kernel command line */
	Byte4 cmdline;

	/* Boot-Module list */
	Byte4 mods_count;
	Byte4 mods_addr;

	union {
		struct multiboot_aout_symbol_table aout_sym;
		struct multiboot_elf_section_header_table elf_sec;
	} u;

	/* Memory Mapping buffer */
	Byte4 mmap_length;
	Byte4 mmap_addr;

	/* Drive Info buffer */
	Byte4 drives_length;
	Byte4 drives_addr;

	/* ROM configuration table */
	Byte4 config_table;

	/* Boot Loader Name */
	Byte4 boot_loader_name;

	/* APM table */
	Byte4 apm_table;

	/* Video */
	Byte4 vbe_control_info;
	Byte4 vbe_mode_info;
	Byte2 vbe_mode;
	Byte2 vbe_interface_seg;
	Byte2 vbe_interface_off;
	Byte2 vbe_interface_len;
};
typedef struct multiboot_info multiboot_info_t;

struct multiboot_mod_list {
	/* the memory used goes from bytes 'mod_start' to 'mod_end-1' inclusive */
	Byte4 mod_start;
	Byte4 mod_end;

	/* Module command line */
	Byte4 cmdline;

	/* padding to take it to 16 bytes (must be zero) */
	Byte4 pad;
};
typedef struct multiboot_mod_list multiboot_module_t;

struct multiboot_mmap_entry {
	Byte4 size;
	Byte8 addr;
	Byte8 len;
	Byte4 type;
} __attribute__((packed));
typedef struct multiboot_mmap_entry multiboot_memory_map_t;