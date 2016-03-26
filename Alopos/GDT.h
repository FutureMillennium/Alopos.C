#pragma once
// Global Descriptor Table (GDT)



// ---------------------------------------------
// structs
// ---------------------------------------------

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

typedef struct {
	GDTSegmentDescriptor entries[3]; // TODO variable number of entries
	GDTAddress address;
} GDT;


// ---------------------------------------------
// function definitions
// ---------------------------------------------

extern void GDTFlush(Byte4); // Load and flush the GDT

