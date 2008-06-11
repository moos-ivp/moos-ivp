#include <string>
#include <iostream>
#include "sutil.h"
#include "esc_spc.h"

using namespace std;

string esc_spc(string in_s)
{
	string r = in_s;
	
	map<string, string> fr;

	fr["%"] = "%%";
	fr["\n"] = "%n";
	find_and_replace_many(r, fr);

	return r;
}

string reverse_esc_spc(string in_s)
{
	string r = in_s;

	map<string, string> fr;
	fr["%%"] = "%";
	fr["%n"] = "\n";

	find_and_replace_many(r, fr);

	return r;
}

bom reverse_esc_spc(bom in)
{
	bom r = in;

	map<bom, bom> fr;
	fr[bom("%%", 2)] = bom("%", 1);
	fr[bom("%n", 2)] = bom("\n", 1);
	fr[bom("%0", 2)] = bom("\0", 1);

	find_and_replace_many(r, fr);

	return r;
}

bom esc_spc(bom in)
{
	bom r = in;

	map<bom, bom> fr;
	fr[bom("%", 1)] = bom("%%", 2);
	fr[bom("\n", 1)] = bom("%n", 2);
	fr[bom("\0", 1)] = bom("%0", 2);

	find_and_replace_many(r, fr);

	return r;
}

