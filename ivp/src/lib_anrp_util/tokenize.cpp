#include "tokenize.h"

using namespace std;

vector<string> explode(string s, string e)
{
	vector<string> ret;
	int iPos = s.find(e, 0);
	int iPit = e.length();

	while (iPos > -1) {
		if (iPos != 0)
			ret.push_back(s.substr(0, iPos));

		s.erase(0, iPos + iPit);

		iPos = s.find(e, 0);
	}

	if (s != "")
		ret.push_back(s);

	return ret;
}

#ifdef TESTING
#include <stdio.h>

int main(int argc, char *argv[])
{
	vector<string> vs = explode(argv[1], argv[2]);

	printf("vs.size() = %i\n", vs.size());
	
	for(vector<string>::iterator it = vs.begin(); it != vs.end(); it++) {
		printf("\"%s\"\n", it->c_str());
	}
}

#endif
