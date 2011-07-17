#ifndef FileNotFoundException_H
#define FileNotFoundException_H

#include <stdexcept>
#include <string>

class FileNotFoundException : public std::runtime_error {
  FileNotFoundException& operator = (const FileNotFoundException &other);

public:
  FileNotFoundException(const std::string &msg = "");
  FileNotFoundException(const FileNotFoundException &other);

protected:

};

#endif // FileNotFoundException_H
