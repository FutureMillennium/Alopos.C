
// Interrupt Descriptor Table (IDT)

#include "VarDefs.h"
#include "Memory.h"
#include "VGATextMode.h"
#include "IO.h"

// ---------------------------------------------
// constants
// ---------------------------------------------

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define PIC_EOI		0x20		/* End-of-interrupt command code */

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

char* exceptionMessages[] =
{
	"Division By Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Into Detected Overflow",
	"Out of Bounds",
	"Invalid Opcode",
	"No Coprocessor",

	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",

	"Coprocessor Fault",
	"Alignment Check",
	"Machine Check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};


// ---------------------------------------------
// enums
// ---------------------------------------------

enum IDTGate_Type {
	IDT_GATE_TYPE_TASK_GATE_32 = 5,
	IDT_GATE_TYPE_INTERRUPT_GATE_16 = 6,
	IDT_GATE_TYPE_TRAP_GATE_16 = 7,
	IDT_GATE_TYPE_INTERRUPT_GATE_32 = 14,
	IDT_GATE_TYPE_TRAP_GATE_32 = 15,
};

// ---------------------------------------------
// structs
// ---------------------------------------------

typedef struct IDTGateEntry {
	Byte2 offset_low; // offset bits 0..15
	Byte2 selector; // a code segment selector in GDT or LDT
	Byte zero;      // unused, set to 0
	Byte flags; // type and attributes, see below
	Byte2 offset_high; // offset bits 16..31
} __attribute__((packed)) IDTGateEntry;

typedef struct IDTAddress { // 6 bytes, goes in IDTR (IDT register)
	Byte2 size; // size is actual size - 1
	Byte4 address;
} __attribute__((packed)) IDTAddress;

typedef struct IDT {
	IDTGateEntry entries[256];
	IDTAddress address;
} IDT;

typedef struct InterruptState {
	unsigned int gs, fs, es, ds;
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned int interruptNumber, errorCode;
	unsigned int eip, cs, eflags, useresp, ss;
} InterruptState;


// ---------------------------------------------
// function definitions
// ---------------------------------------------

extern void ISRoutine0();
extern void ISRoutine1();
extern void ISRoutine2();
extern void ISRoutine3();
extern void ISRoutine4();
extern void ISRoutine5();
extern void ISRoutine6();
extern void ISRoutine7();
extern void ISRoutine8();
extern void ISRoutine9();
extern void ISRoutine10();
extern void ISRoutine11();
extern void ISRoutine12();
extern void ISRoutine13();
extern void ISRoutine14();
extern void ISRoutine15();
extern void ISRoutine16();
extern void ISRoutine17();
extern void ISRoutine18();
extern void ISRoutine19();
extern void ISRoutine20();
extern void ISRoutine21();
extern void ISRoutine22();
extern void ISRoutine23();
extern void ISRoutine24();
extern void ISRoutine25();
extern void ISRoutine26();
extern void ISRoutine27();
extern void ISRoutine28();
extern void ISRoutine29();
extern void ISRoutine30();
extern void ISRoutine31();

extern void IRQHandler0();
extern void IRQHandler1();
extern void IRQHandler2();
extern void IRQHandler3();
extern void IRQHandler4();
extern void IRQHandler5();
extern void IRQHandler6();
extern void IRQHandler7();
extern void IRQHandler8();
extern void IRQHandler9();
extern void IRQHandler10();
extern void IRQHandler11();
extern void IRQHandler12();
extern void IRQHandler13();
extern void IRQHandler14();
extern void IRQHandler15();

extern void IDTLoad(Byte4);


// ---------------------------------------------
// vars
// ---------------------------------------------

IDT idt;


// ---------------------------------------------
// functions
// ---------------------------------------------

void GateEntrySet_IDT(Index index, Byte4 offset, Byte2 selector, enum IDTGate_Type type, Bit storageSegment, Bit2 privilegeRing) {

	idt.entries[index].offset_low = (offset & 0xFFFF);
	idt.entries[index].offset_high = (offset >> 16);

	idt.entries[index].selector = selector;
	idt.entries[index].zero = 0; // always 0

	idt.entries[index].flags = (type & 0xF); // bit 0–3, gate type
	idt.entries[index].flags |= ((storageSegment & 1) << 4); // bit 4, storage segment
	idt.entries[index].flags |= ((privilegeRing & 0b11) << 5); // bit 5–6, Descriptor Privilege Level
	idt.entries[index].flags |= (1 << 7); // bit 7, Present always 1

}

void ExceptionHandler(InterruptState* state) {
	Echo_Terminal_VGA("Woop, ExceptionHandler happened\n");

	if (state->interruptNumber < 32) {
		Echo_Terminal_VGA("%s exception: %i\n", exceptionMessages[state->interruptNumber], state->errorCode);
	}
	return;
}

void IRQHandler(InterruptState* state) {
	/* This is a blank function pointer */
	//void(*handler)(struct regs *r);

	/* Find out if we have a custom handler to run for this
	*  IRQ, and then finally, run it */
	/*handler = irq_routines[r->int_no - 32];
	if (handler) {
		handler(r);
	}*/

	Echo_Terminal_VGA("Woop, IRQHandler happened\n");

	/* If the IDT entry that was invoked was greater than 40
	*  (meaning IRQ8 - 15), then we need to send an EOI to
	*  the slave controller */
	if (state->interruptNumber >= 40) {
		OutByte_IO(PIC2, PIC_EOI);
	}

	/* In either case, we need to send an EOI to the master
	*  interrupt controller too */
	OutByte_IO(PIC1, PIC_EOI);
}

void IDTInit() {
	idt.address.size = sizeof(idt.entries) - 1; // size is actual size - 1
	idt.address.address = (Byte4)&idt.entries[0];

	MemorySet((Byte*)&idt.entries, 0, sizeof(idt.entries));

	// exceptions 0–31
	GateEntrySet_IDT(0, (Byte4)ISRoutine0, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(1, (Byte4)ISRoutine1, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(2, (Byte4)ISRoutine2, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(3, (Byte4)ISRoutine3, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(4, (Byte4)ISRoutine4, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(5, (Byte4)ISRoutine5, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(6, (Byte4)ISRoutine6, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(7, (Byte4)ISRoutine7, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);

	GateEntrySet_IDT(8, (Byte4)ISRoutine8, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(9, (Byte4)ISRoutine9, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(10, (Byte4)ISRoutine10, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(11, (Byte4)ISRoutine11, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(12, (Byte4)ISRoutine12, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(13, (Byte4)ISRoutine13, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(14, (Byte4)ISRoutine14, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(15, (Byte4)ISRoutine15, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);

	GateEntrySet_IDT(16, (Byte4)ISRoutine16, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(17, (Byte4)ISRoutine17, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(18, (Byte4)ISRoutine18, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(19, (Byte4)ISRoutine19, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(20, (Byte4)ISRoutine20, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(21, (Byte4)ISRoutine21, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(22, (Byte4)ISRoutine22, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(23, (Byte4)ISRoutine23, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);

	GateEntrySet_IDT(24, (Byte4)ISRoutine24, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(25, (Byte4)ISRoutine25, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(26, (Byte4)ISRoutine26, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(27, (Byte4)ISRoutine27, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(28, (Byte4)ISRoutine28, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(29, (Byte4)ISRoutine29, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(30, (Byte4)ISRoutine30, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(31, (Byte4)ISRoutine31, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);

	// remap IRQ 0–15 from 8–15 to 32–47
	{
		Byte a1, a2;

		a1 = InByte_IO(PIC1_DATA); // save masks
		a2 = InByte_IO(PIC2_DATA);

		// TODO does this need io_wait()?
		OutByte_IO(PIC1_COMMAND, ICW1_INIT + ICW1_ICW4); // starts the initialization sequence (in cascade mode)
		OutByte_IO(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);
		OutByte_IO(PIC1_DATA, 32); // ICW2: Master PIC vector offset
		OutByte_IO(PIC2_DATA, 40); // ICW2: Slave PIC vector offset
		OutByte_IO(PIC1_DATA, 4); // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
		OutByte_IO(PIC2_DATA, 2); // ICW3: tell Slave PIC its cascade identity (0000 0010)
		
		OutByte_IO(PIC1_DATA, ICW4_8086);
		OutByte_IO(PIC2_DATA, ICW4_8086);
		
		OutByte_IO(PIC1_DATA, a1);   // restore saved masks.
		OutByte_IO(PIC2_DATA, a2);
	}

	// IRQ 0–15 = interrupt 32–47
	GateEntrySet_IDT(32, (Byte4)IRQHandler0, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(33, (Byte4)IRQHandler1, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(34, (Byte4)IRQHandler2, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(35, (Byte4)IRQHandler3, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(36, (Byte4)IRQHandler4, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(37, (Byte4)IRQHandler5, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(38, (Byte4)IRQHandler6, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(39, (Byte4)IRQHandler7, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);

	GateEntrySet_IDT(40, (Byte4)IRQHandler8, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(41, (Byte4)IRQHandler9, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(42, (Byte4)IRQHandler10, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(43, (Byte4)IRQHandler11, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(44, (Byte4)IRQHandler12, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(45, (Byte4)IRQHandler13, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(46, (Byte4)IRQHandler14, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);
	GateEntrySet_IDT(47, (Byte4)IRQHandler15, 0x08, IDT_GATE_TYPE_INTERRUPT_GATE_32, 0, 0);

	IDTLoad((Byte4)&idt.address);
}
