
// Interrupt Descriptor Table (IDT)

#include "VarDefs.h"

typedef struct IDTGateEntry {
	Byte2 offset_low; // offset bits 0..15
	Byte2 selector; // a code segment selector in GDT or LDT
	Byte zero;      // unused, set to 0
	Byte type_attr; // type and attributes, see below
	Byte2 offset_high; // offset bits 16..31
} IDTGateEntry;
