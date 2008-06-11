#include "sutil.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include "esc_spc.h"

#ifdef TESTING

using namespace std;

int main(int argc, char *argv[])
{
	string inp = argv[1];
	int num = (argc - 1) / 2;
	
	map<string, string> fr;
	for(int i=0; i<num; i++) {
		fr[argv[2+i*2+0]] = argv[2+i*2+1];
	}

	string nullstr = " ";
	nullstr[0] = 0;

	fr["%0"] = nullstr;

	printf("Input: \"%s\"\n", inp.c_str());
	find_and_replace_many(inp, fr);
	//printf("Output: \"%s\"\n", inp.c_str());
	cout << "Output: \"" << inp << "\"" << endl;
}

#endif

#ifdef TESTING2

using namespace std;

int main(int argc, char *argv[])
{
	bom start(5);

	start[0] = ' ';
	start[1] = '\n';
	start[2] = 0;
	start[3] = '%';
	start[4] = 'k';

	printf("IL %i\n", start.size());
	
	bom out = esc_spc(start);

	printf("OL %i c: %s\n", out.size(), out.c_str().c());
	printf("?\n");

	bom conv = reverse_esc_spc(out);

	printf("OLc %i\n", conv.size());
}

#endif
