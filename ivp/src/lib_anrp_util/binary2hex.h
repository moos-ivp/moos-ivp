#ifndef __binary2hex_h__
#define __binary2hex_h__

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "bom.h"
#include <string>

/**
   converts a binary chunk to hex
   @param len input binary length
   @param bin input binary memory pointer
   @param out output pointer (len*2)+1 bytes, null terminated
  */

void binary2hex(int len, const char *bin, char **out);
std::string binary2hex(bom &b);
std::string binary2hex(int len, const char *bin);

/**
   converts a null-terminated hex chunk back to binary
   @param in null-terminated input string
   @param len pointer to output length
   @param out output pointer
  */

void hex2binary(const char *in, int *len, unsigned char **out);
bom hex2binary(std::string in);

#endif /* __binary2hex_h__ */
