#ifndef __ssp_h__
#define __ssp_h__

#include <string>
#include <stdarg.h>

std::string ssp(std::string s, ...);
std::string vssp(std::string s, va_list ap);


#endif
