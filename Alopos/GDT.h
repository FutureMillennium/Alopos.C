#pragma once
// Global Descriptor Table (GDT)

#include "VarDefs.h"

typedef struct { // 6 bytes, goes in GDTR (GDT register)
	Byte2 size; // size is actual size - 1
	Byte4 address;
} __attribute__((packed)) GDTAddress;

typedef struct { // 8 bytes, GDT entry
	Byte2 limit_low;
	Byte2 base_low;
	Byte base_middle;
	Byte access;
	Byte granularity;
	Byte base_high;
} __attribute__((packed)) GDTSegmentDescriptor;

void SegmentDescriptorSet_GDT(Index index, Byte8 base, Byte4 limit, Bool isReadableWriteable, Bit directionConforming, Bool isExecutable, Uint8 privilegeRing, Bool is32bit, Bit granularity) {
	Byte access, granularityFlags;

	// access
	access = 0b10010000; // bit 0, "accessed" is 0
	if (isReadableWriteable)
		access |= 0b00000010; // bit 1, readable (code) / writeable (data) bit
	if (directionConforming == 1)
		access |= 0b00000100; // bit 2, conforming (code) / direction (data) bit
	if (isExecutable)
		access |= 0b00001000; // bit 3
							  // bit 4 is 1
	access |= (privilegeRing << 5); // bit 5, 6, executable = code, non-executable = data
									// bit 7, "present" must be 1
	
	// granularity flags
	granularityFlags = 0;
	if (is32bit)
		granularityFlags |= 0b0100;
	if (granularity == 1)
		granularityFlags |= 0b1000;

	/* Base Address */
	gdt.entries[index].base_low = (base & 0xFFFF);
	gdt.entries[index].base_middle = (base >> 16) & 0xFF;
	gdt.entries[index].base_high = (base >> 24) & 0xFF;
	/* Limits */
	gdt.entries[index].limit_low = (limit & 0xFFFF);
	gdt.entries[index].granularity = (limit >> 16) & 0x0F;
	/* Granularity */
	gdt.entries[index].granularity |= (granularityFlags << 4);
	/* Access flags */
	gdt.entries[index].access = access;
}

typedef struct {
	GDTSegmentDescriptor entries[3];
	GDTAddress address;
} GDT;

GDT gdt;

void GDTInit() {
	gdt.address.size = sizeof(gdt.entries) - 1; // size is actual size - 1
	gdt.address.address = (Byte4) &gdt.entries[0];

	SegmentDescriptorSet_GDT(0,
		0, 0, 0, 0, 0, 0, 0, 0); // NULL segment
	SegmentDescriptorSet_GDT(1,
		0, 0xFFFFFFFF, true, 0, true, 0, true, 1); // kernel code segment
	SegmentDescriptorSet_GDT(1,
		0, 0xFFFFFFFF, true, 0, false, 0, true, 1); // kernel data segment
}

