#include <string>
#include <iostream>

using namespace std;


string esc_spc(string in_s)
{
	string r = in_s;
	unsigned int loc;

	while((loc = r.find("%")) != string::npos) {
		r.erase(loc, 1);
		r.insert(loc, string("%%"));
	}

	while((loc = r.find("\n")) != string::npos) {
		r.erase(loc, 1);
		r.insert(loc, string("%n"));
	}

	return r;
}

string reverse_esc_spc(string in_s)
{
	string r = in_s;
	unsigned int loc;

	while((loc = r.find("%%")) != string::npos) {
		r.erase(loc, 2);
		r.insert(loc, string("%"));
	}

	while((loc = r.find("%n")) != string::npos) {
		r.erase(loc, 2);
		r.insert(loc, string("\n"));
	}

	return r;
}
