
// memory functions

#include "VarDefs.h"

Byte* MemorySet(Byte* destination, Byte setTo, Index count) {
	Byte* current = destination;
	while (count != 0) {
		*current = setTo;
		current++;
		count--;
	}
	return destination;
}
