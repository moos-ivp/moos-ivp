#include "crc.h"
#include <stdio.h>
#include "crc_tab.h"

int crc_fn(unsigned long start_poly, char *data, int len, unsigned long *out)
{
  char                  *p;
  unsigned long         crc = ~start_poly;
	
  for (p = data; p - data < len; p++) {
      crc = (crc >> 8) ^ crctable[(crc ^ *p) & 0xff];
  }

  *out = ~crc;
  return 0;
}
