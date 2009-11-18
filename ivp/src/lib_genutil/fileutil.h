#ifndef FILEUTIL_HEADER
#define FILEUTIL_HEADER

#include <vector>
#include <string>

// Returns 0 if 'files' has been appended with all files in the specified
// directory, otherwise returns an error code returned by the 'opendir'
// function.
int listdir (std::string dir, std::vector<std::string> &files);

// Calls exit() if it fails.  Sometime error handling might want to be
// changed.
bool isdir(std::string filename);

// Calls exit() if it fails.  Sometime error handling might want to be
// changed.  The file in question must exist.
bool is_regular_file(std::string filename);

#endif
