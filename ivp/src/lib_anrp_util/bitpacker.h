#ifndef __bitpacker_h__
#define __bitpacker_h__

#include <vector>
#include <stdint.h>
#include "bom.h"

std::vector<bool> unbitpack(bom, bool flipped_byte = false, int bits_per_byte = 8);
bom bitpack(std::vector<bool>, bool flipped_byte = false);

#endif
