#include <exception>
#include <string>

namespace MOOS {
namespace AlogTools {
namespace exceptions {

class AlogToolsException : virtual public std::exception {};

class FileIOException : virtual public AlogToolsException {
public:
    FileIOException() {}

    explicit FileIOException(const std::string& filename) :
        _filename(filename) {
    }

    virtual ~FileIOException() throw () {}

    const std::string& FileName() const {
        return _filename;
    }
private:
    std::string _filename;
};


class CannotOpenFileForWritingException : virtual public FileIOException {
public:
    explicit CannotOpenFileForWritingException(const std::string& filename) :
        FileIOException(filename) {}
};


class CannotOpenFileForReadingException : virtual public FileIOException {
public:
    explicit CannotOpenFileForReadingException(const std::string& filename) :
        FileIOException(filename) {}
};


class CannotOpenIndexFileForReadingException : virtual public CannotOpenFileForReadingException {
public:
    explicit CannotOpenIndexFileForReadingException(const std::string& filename) :
        CannotOpenFileForReadingException(filename) {}
};

class IncorrectAlogIndexVersionException : virtual public AlogToolsException {};

}  // namespace exceptions
}  // namespace AlogTools
}  // namespace MOOS
