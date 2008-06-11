#include "bitpacker.h"

using namespace std;

vector<bool> unbitpack(bom b, bool flipped_byte, int bpb)
{
	vector<bool> res;

	int nbits = b.size()*bpb;
	uint8_t *p = b.u8();

	res.resize(nbits);

	for(int i=0; i<nbits; i++) {
		uint8_t *lp = p;
		int bp = i % bpb; // position in byte (bit offset)
		int byp = (i - bp)/bpb;

		lp += byp;

		bool cond = false;
		
		if(flipped_byte) {
			if(*lp & (0x80>>bp)) cond = true;
		} else {
			if(*lp & (1<<bp)) cond = true;
		}
		
		res[i] = cond;
	}

	return res;
}
		
bom bitpack(vector<bool> in, bool flipped_byte)
{
	int s = in.size();
	int rem = s % 8;
	if(rem == 0) { // exact size
		s /= 8;
	} else {
		s -= rem;
		s /= 8;
		s++;
	}

	bom rv(s);

	for(int i=0; i<in.size(); i++) {
		uint8_t *lp = rv.u8();
		int lrem = i % 8;
		
		lp += ((i-lrem)/8); // figure out byte pos

		if(in[i]) {
			if(flipped_byte) {
				*lp |= (0x80) >> lrem;
			} else {
				*lp |= 1 << lrem;
			}
		}
	}

	return rv;
}

#ifdef TEST
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	uint8_t stor[6] = { 0xff, 0, 0xaa, 0x55, 0x80, 0x01 };

	vector<bool> bits = unbitpack(stor, sizeof(stor)*8, true);

	for(int i=0; i<bits.size(); i++) {
		printf("%c", bits[i]? '1' : '0');
	}
	printf("\n");
	
	uint8_t *p; int s;
	bitpack(bits, &p, s, true);

	if(memcmp(stor, p, sizeof(stor)) == 0) printf("pass\n");
	else printf("fail\n");
}

#endif

