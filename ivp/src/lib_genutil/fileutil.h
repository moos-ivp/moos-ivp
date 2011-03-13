#ifndef FILEUTIL_HEADER
#define FILEUTIL_HEADER

#include <vector>
#include <string>

// Returns 0 if 'files' has been appended with all files in the specified
// directory, otherwise returns an error code returned by the 'opendir'
// function.
int listdir (std::string dir, std::vector<std::string> &files);

bool isDirectory(std::string filename);
bool isRegularFile(std::string filename);

#endif
