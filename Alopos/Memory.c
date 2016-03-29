
// memory functions

#include "VarDefs.h"

Byte* MemorySet(Byte* offset, Byte setTo, Index count) {
	Byte* current = offset;
	while (count != 0) {
		*current = setTo;
		current++;
		count--;
	}
	return offset;
}
