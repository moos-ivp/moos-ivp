#ifndef __bom_h__
#define __bom_h__

#include <stdlib.h>
#include <stdexcept>
#include <stdint.h>
#include <string.h>
#include <stdint.h>
#include <limits>

#include "mymemmem.h"

//#define BOM_PTRDIFF_MAX std::numeric_limits<unsigned int>::max()

// Brick of Memory

class bom {
public:
	static const unsigned int npos = UINT_MAX;

	bom(const void *p, int l) : ip(NULL), il(0), fillchar(0) {
		internal_realloc(l);
		memcpy(ip, p, l);
	}
		
	bom(int l, uint8_t fill = 0) : ip(NULL), il(0), fillchar(fill) {
		internal_realloc(l);
	}

	bom() : ip(NULL), il(0), fillchar(0) {}

	~bom() {
		if(il) {
			free(ip);
		}
	}

	bom(const bom &o) : ip(NULL), il(0), fillchar(o.fillchar) { // copy
		internal_realloc(o.il);
		memcpy(ip, o.ip, il);
	}

	unsigned int find(const bom &f, unsigned int start = 0) {
		if(start > il) throw std::runtime_error("Access past end of BoM in find");
		uint8_t *res = mymemmem(u8()+start, il-start, (uint8_t *)f.ip, f.il);
		if(res == NULL) return npos;
		else return res - u8();
	}

	void erase(unsigned int start, int len) {
		printf("start: %i len: %i\n", start, len);

		if(start + len > il) throw std::runtime_error("Attempted to erase too much");
		memmove(u8()+start, u8()+start+len, il - (start + len));
		internal_realloc(il - len);
	}

	void insert(unsigned int loc, const bom &b) {
		if(loc > il) throw std::runtime_error("Attempted to insert into BoM past end");
		internal_realloc(il+b.size());

		memmove(u8()+loc+b.il, u8()+loc, il - (loc + b.il));
		memcpy(u8()+loc, b.ip, b.il);
	}

	void operator=(const bom &o) {
		fillchar = o.fillchar;
		internal_realloc(o.il);
		memcpy(ip, o.ip, il);
	}

	uint8_t &operator[](const int &idx) {
		if(idx >= il) throw std::runtime_error("Access past end of BoM");
		return u8()[idx];
	}

	uint8_t *operator+(const unsigned int &off) {
		return u8()+off;
	}

	uint8_t *operator-(const unsigned int &off) {
		return u8()-off;
	}

	uint8_t &operator*() {
		return u8()[0];
	}

	bool operator<(const bom &o) const {
		if(memcmp(ip, o.ip, o.il < il? o.il : il) < 0) return true;
		else return false;
	}

	char *c() { return (char*)ip; }
	uint8_t *u8() { return (uint8_t*)ip; }
	void *p() { return ip; }

	bom c_str(void) { bom b; b = *this; b.resize(b.size()+1); b[b.size()-1] = 0; return b; }

	int size() const { return il; }
	int length() const { return il; }
	void resize(int newsize) {
		int olds = il;
		internal_realloc(newsize);
	}

private:
	void *ip;
	int il;

	uint8_t fillchar;

	void internal_realloc(int newl) {
		//printf("IR: %p, %i old, %i req\n", ip, il, newl);
		ip = realloc(ip, newl);
		if(ip == NULL) {
			if(newl == 0) {
				il = 0;
				return;
			} else {
				throw std::runtime_error("Out of Memory");
			}
		} else {
			if(newl > il) {
			//	printf("Oldl %i, newl %i, fillchar %i\n", il, newl, (int)fillchar);
				memset((uint8_t*)ip+il, fillchar, newl-il);
			} else if(newl == il) {
				//printf("Same size\n");
			} else {
				//printf("Shrinking\n");
			}
			il = newl;
		}
	}
};

#endif


