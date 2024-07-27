#ifndef VERSIONEXCEPTION_H
#define VERSIONEXCEPTION_H

#include <stdexcept>
#include <string>

class VersionException : public std::runtime_error {
  VersionException& operator = (const VersionException &other);

public:
  VersionException(const std::string &msg = "");
  VersionException(const VersionException &other);

protected:

};

#endif // VERSIONEXCEPTION_H
