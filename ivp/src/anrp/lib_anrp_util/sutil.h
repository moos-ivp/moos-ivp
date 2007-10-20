#ifndef __sutil_h__
#define __sutil_h__

#include <string>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static inline std::string clean_spaces(const std::string &s) {
	std::string ret = s;
	while(ret.size() && isspace(ret[0])) ret.erase(ret.begin());
	while(ret.size() && isspace(ret[ret.length()-1])) ret.erase(ret.length()-1, 1);
	return ret;
}

static inline bool strtobool(const std::string &s)
{
	if(strcasecmp(s.c_str(), "true") == 0 ||
		strcasecmp(s.c_str(), "yes") == 0 ||
		strcasecmp(s.c_str(), "y") == 0 ||
		strcasecmp(s.c_str(), "t") == 0 ||
		strcasecmp(s.c_str(), "1") == 0) return true;
	else return false;
}

static inline double stof(const std::string &s) {
	return atof(s.c_str());
}

#endif
