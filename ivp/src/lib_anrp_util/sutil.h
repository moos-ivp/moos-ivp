#ifndef __sutil_h__
#define __sutil_h__

#include <string>
#include <vector>
#include <map>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "bom.h"

static inline std::string clean_spaces(const std::string &s) {
	std::string ret = s;
	while(ret.size() && isspace(ret[0])) ret.erase(ret.begin());
	while(ret.size() && isspace(ret[ret.length()-1])) ret.erase(ret.length()-1, 1);
	return ret;
}

static inline void strzero_space(char *s)
{
	while(*s && isspace(*s)) memmove(s, s+1, strlen(s));
	while(isspace(s[strlen(s)-1])) s[strlen(s)-1] = 0;
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

static inline int stoi(const std::string &s) {
	return atoi(s.c_str());
}

template<class container, class difftype>
void T_find_and_replace(container &s, const container &f, const container &r)
{
	difftype loc = 0;
	while((loc = s.find(f, loc)) != container::npos) {
		s.erase(loc, f.length());
		s.insert(loc, r);
		loc += r.length();
	}
}

template<class container, class difftype>
void T_find_and_replace_many(container &s, std::map<container, container> &fr, difftype orig = 0) {
	difftype loc = orig;
	bool done = false;
	while(!done) { 
		typename std::vector<difftype> curloc(fr.size());
		typename std::map<container, container>::iterator it;

		int i = 0;
		for(it = fr.begin(); it != fr.end(); it++, i++) {
			curloc[i] = s.find(it->first, loc);
		}
		int whichmin = -1, min = s.length(); i = 0;
		for(i = 0; i<fr.size(); i++) {
			if(curloc[i] != container::npos) {
				if(curloc[i] < min) {
					min = curloc[i];
					whichmin = i;
				}
			}
		}
		
		if(whichmin == -1) done = true;
		else {
			it = fr.begin();
			for(int i=0; i<whichmin; i++) it++;

			s.erase(curloc[whichmin], it->first.size());
			s.insert(curloc[whichmin], it->second);
			loc = curloc[whichmin] + it->second.size();
		}
	}
}

static inline void find_and_replace_many(bom &s, std::map<bom, bom> &fr, unsigned int orig = 0)
{ T_find_and_replace_many<bom, unsigned int>(s, fr, orig); }

static inline void find_and_replace_many(std::string &s, std::map<std::string, std::string> &fr, unsigned int orig = 0)
{ T_find_and_replace_many<std::string, unsigned int>(s, fr, orig); }

static inline void find_and_replace(bom &b, const bom &f, const bom &r)
{ T_find_and_replace<bom, unsigned int>(b, f, r); }

static inline void find_and_replace(std::string &s, const std::string &f, const std::string &r)
{ T_find_and_replace<std::string, unsigned int>(s, f, r); }

#endif
