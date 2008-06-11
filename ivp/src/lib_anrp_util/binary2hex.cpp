#include "binary2hex.h"
#include <string>

using namespace std;

void binary2hex(int len, const char *bin, char **out)
{
	*out = (char *)malloc((len*2)+1);
	**out = 0;
	for(int i=0; i<len; i++) {
		snprintf((*out) + strlen(*out), 3, "%02x", (int)(*(bin+i)) & 0xFF);
	}
}

string binary2hex(bom &b)
{
	return binary2hex(b.size(), b.c());
}

string binary2hex(int len, const char *bin)
{
	char *out;
	binary2hex(len, bin, &out);

	string s = out;

	free(out);

	return s;
}

void hex2binary(const char *in, int *len, unsigned char **out)
{
	*len = strlen(in)/2;
	*out = (unsigned char *)malloc(*len);

	for(int i=0; i<*len; i++) {
		char hex[3];
		hex[0] = tolower(*(in+(i*2)));
		hex[1] = tolower(*(in+(i*2)+1));
		hex[2] = 0;
		
		int t;
		sscanf(hex, "%x", &t);
		
		(*out)[i] = t & 0xFF;
	}
}

bom hex2binary(string s)
{
	unsigned char *p;
	int l;
	hex2binary(s.c_str(), &l, &p);

	bom ret(l);
	memcpy(ret.p(), p, l);

	free(p);

	return ret;
}

