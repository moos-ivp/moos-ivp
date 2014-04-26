#include "MOOS/AlogTools/alogLineReader.h"
#include "MOOS/AlogTools/exceptions.h"

namespace MOOS {
namespace AlogTools {

////////////////////////////////////////////////////////////////////////////////
alogLineReader::alogLineReader() : m_alogFileStream()
{
}

////////////////////////////////////////////////////////////////////////////////
alogLineReader::~alogLineReader()
{
}

////////////////////////////////////////////////////////////////////////////////
void alogLineReader::Open( std::string alogFilename )
{
    if( m_alogFileStream.is_open() )
    {
        m_alogFileStream.close();
    }

    m_alogFileStream.open( alogFilename.c_str() );
    if(!m_alogFileStream.is_open())
    {
        throw exceptions::CannotOpenFileForReadingException(alogFilename);
    }
}

////////////////////////////////////////////////////////////////////////////////
void alogLineReader::Read( idxRec alogRec, std::string &line )
{
    m_alogFileStream.seekg( alogRec.lineBegin, std::ios_base::beg );
    
    std::getline (m_alogFileStream,line);
}

}  // namespace AlogTools
}  // namespace MOOS
