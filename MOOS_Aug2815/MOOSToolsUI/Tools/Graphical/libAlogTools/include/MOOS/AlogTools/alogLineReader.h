#ifndef _alogLineReader_h_
#define _alogLineReader_h_

#include <fstream>
#include <string>

#include "recordTypes.h"

namespace MOOS {
namespace AlogTools {

// This class uses index records to find lines in the alog
class alogLineReader
{
    public:
        alogLineReader();
        ~alogLineReader();

        std::ifstream  m_alogFileStream;

        // Throws: exceptions::CannotOpenFileForReadingException
        void Open( std::string alogFilename );
        void Read( idxRec alogRec, std::string & line );
};

}  // namespace AlogTools
}  // namespace MOOS

#endif // _alogLineReader_h_

