#include "MOOS/libAlogTools/FileNotFoundException.h"

using namespace std;

FileNotFoundException::FileNotFoundException(const string &msg) : runtime_error(msg) {
}
