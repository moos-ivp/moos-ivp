#include "MOOS/libAlogTools/VersionException.h"

using namespace std;

VersionException::VersionException(const string &msg) : runtime_error(msg) {
}
