#include "ssp.h"
#include <stdio.h>
#include <stdarg.h>

using namespace std;

std::string ssp(std::string s, ...)
{
	va_list ap;

	va_start(ap, s);
	string ret = vssp(s, ap);
	va_end(ap);

	return ret;
}

std::string vssp(std::string s, va_list ap)
{
	char tmp[65536];
	vsnprintf(tmp, 65535, s.c_str(), ap);
	return std::string(tmp);
}

