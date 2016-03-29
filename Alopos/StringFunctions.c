
// string functions

#include "VarDefs.h"


Index Length_String(const char* input) {
	Index length = 0;
	while (input[length] != 0)
		length++;
	return length;
}

void Number2String(int number, char* buffer, char base) {
	char *tmpbuffer = buffer;
	char *p1, *p2;
	unsigned long unsignedNumber = number;
	int divisor = 10;

	// %i is assumed to be signed, otherwise unsigned
	if (base == 'i' && number < 0) {
		*tmpbuffer++ = '-';
		buffer++;
		unsignedNumber = -number;
	} else if (base == 'x') {
		divisor = 16;
	} else if (base == 'b') {
		divisor = 2;
	}

	// divide by divisor until 0
	do {
		int remainder = unsignedNumber % divisor;

		*tmpbuffer++ = (remainder < 10) ? (remainder + '0') : (remainder + 'A' - 10);
	} while (unsignedNumber /= divisor);

	// 0-terminated string
	*tmpbuffer = 0;

	/* Reverse BUF. */
	p1 = buffer;
	p2 = tmpbuffer - 1;
	while (p1 < p2) {
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
}

Bool EqualStrings(const char* string1, const char* string2) {
	Index i = 0;

	if (string1[i] != string2[i])
		return false;

	while (string1[i] != 0) {
		i++;
		if (string1[i] != string2[i])
			return false;
	}

	return true;
}

void ToLower_String(char* target) {
	char* tmp = target;
	while (*tmp != 0) {
		if (*tmp >= 'A' && *tmp <= 'Z')
			*tmp -= ('A' - 'a');
		tmp++;
	}
	return;
}