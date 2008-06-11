#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <vector>

using namespace std;

void kevcs_parse(string input, map<string, string> &output)
{
	if(input.size() == 0) return;

	char *s = strdup(input.c_str()), *op, *p;
	
	vector<string> parts;

	op = s;
	while((p = strchr(op, ',')) != NULL) {
		*p++ = 0;
		parts.push_back(op);
		op = p;
	}
	if(*op != 0) parts.push_back(op);

	free(s);

	vector<string>::iterator it;
	for(it = parts.begin(); it != parts.end(); it++) {
		string k, v;
		if(it->find("=") != string::npos) {
			k = it->substr(0, it->find("="));
			v = it->substr(it->find("=")+1);
			output[k] = v;
		} else {
			k = *it;
			output[k] = "";
		}
	}
}

#if 0
int main()
{
	string ins = "sonar=start,sonar_resolution=2,sonar_gain=0,sonar_tvgain=0,sonar_maxrange=40";

	vector<pair<string, string> > out;

	kevcs_parse(ins, out);

	vector<pair<string, string> >::iterator it;

	for(it = out.begin(); it != out.end(); it++) {
		fprintf(stderr, "%s\n", it->second.c_str());
	}
}
#endif

bool vpss_find(map<string, string> args, string key, string &value)
{
	if(args.find(key) != args.end()) {
		value = args[key];
		return true;
	}

	return false;
}
