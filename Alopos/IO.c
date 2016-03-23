
// IO, I/O, input/output functions

#include "VarDefs.h"

inline Byte InByte_IO(Byte2 port) {
	Byte ret;
#if defined(__GNUC__)
	asm volatile ("inb %1, %0"
		: "=a"(ret)
		: "Nd"(port));
#endif
	return ret;
}

inline void OutByte_IO(Byte2 port, Byte val) {
#if defined(__GNUC__)
	asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
#endif
	/* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
	* Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
	* The  outb  %al, %dx  encoding is the only option for all other cases.
	* %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}
