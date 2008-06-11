#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>

void *mymemmem(char *haystack, int hayl, const char *needle, int nl)
{
	if (!haystack || !hayl || !needle || !nl)
		return NULL;

	void *ret = NULL;

	for (int i = 0; i < hayl && ret == NULL; i++) {
		if (haystack[i] == needle[0] &&
		                hayl - (hayl - i) + nl <= hayl) {
			if (memcmp(haystack + i, needle, nl) == 0) {
				ret = haystack + i;
			}
		}
	}

	return ret;
}

uint8_t *mymemmem(uint8_t *h, int hl, const uint8_t *n, int nl)
{
	return (uint8_t *)mymemmem((char *)h, hl, (char *)n, nl);
}
