#ifndef STRINGUTIL_HEADER
#define STRINGUTIL_HEADER

#include <vector>
#include <string>

// Splits the string s into the different substrings that are separated by
// the string 'separator'.  The separator will not appear in the returned
// strings.
std::vector<std::string> tokenize(const std::string & s, const std::string & separator);

// Prints the strings in the vector, one on each line, to the output string.
//  Actual for m on each line is <prefix><string><suffix>
std::string vect_to_string(std::string prefix, std::string suffix, const std::vector<std::string> & v);

#endif
