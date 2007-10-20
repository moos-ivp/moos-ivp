#ifndef __kevcs_h__
#define __kevcs_h__

#include <map>
#include <string>

using namespace std;

void kevcs_parse(string input, map<string, string> &output);
bool vpss_find(map<string, string> args, string key, string &value);

#endif
