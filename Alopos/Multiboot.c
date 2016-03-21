
// Multiboot functions

#include "VarDefs.h"
#include "Multiboot.h"

Bool FlagCheck_MultibootInfo(Byte4 flags, Byte bit) {
	return ((flags)& (1 << (bit)));
}
