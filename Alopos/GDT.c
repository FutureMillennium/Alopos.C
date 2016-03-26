
// Global Descriptor Table (GDT)

#include "VarDefs.h"
#include "GDT.h"

// ---------------------------------------------
// vars
// ---------------------------------------------

GDT gdt;


// ---------------------------------------------
// functions
// ---------------------------------------------

void SegmentDescriptorSet_GDT(Index index, Byte8 base, Byte4 limit, Bool isReadableWriteable, Bit directionConforming, Bool isExecutable, Uint2 privilegeRing, Bool is32bit, Bit granularity) {
	Byte access, granularityFlags;

	// access
	access = 0b10010000;
	// bit 0, "accessed" is 0
	// bit 4 is 1
	// bit 7, "present" must be 1
	if (isReadableWriteable)
		access |= (1 << 1); // bit 1, readable (code) / writeable (data) bit
	if (directionConforming == 1)
		access |= (1 << 2); // bit 2, conforming (code) / direction (data) bit
	if (isExecutable)
		access |= (1 << 3); // bit 3
	access |= ((privilegeRing & 0b11) << 5); // bit 5, 6, executable = code, non-executable = data

	// granularity flags
	granularityFlags = 0;
	if (is32bit)
		granularityFlags |= 0b0100;
	if (granularity == 1)
		granularityFlags |= 0b1000;

	// base address
	gdt.entries[index].base_low = (base & 0xFFFF);
	gdt.entries[index].base_middle = (base >> 16) & 0xFF;
	gdt.entries[index].base_high = (base >> 24) & 0xFF;
	// limit
	gdt.entries[index].limit_low = (limit & 0xFFFF);
	gdt.entries[index].granularity = (limit >> 16) & 0x0F;
	// granularity flags
	gdt.entries[index].granularity |= (granularityFlags << 4);
	// access flags
	gdt.entries[index].access = access;
}

void GDTInit() {
	gdt.address.size = sizeof(gdt.entries) - 1; // size is actual size - 1
	gdt.address.address = (Byte4)&gdt.entries[0];

	SegmentDescriptorSet_GDT(0,
		0, 0, 0, 0, 0, 0, 0, 0); // NULL segment
	SegmentDescriptorSet_GDT(1,
		0, 0xFFFFFFFF, true, 0, true, 0, true, 1); // kernel code segment
	SegmentDescriptorSet_GDT(2,
		0, 0xFFFFFFFF, true, 0, false, 0, true, 1); // kernel data segment

	GDTFlush((Byte4)&gdt.address);
}
