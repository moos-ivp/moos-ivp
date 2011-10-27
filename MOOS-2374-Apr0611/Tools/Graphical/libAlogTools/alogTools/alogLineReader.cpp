#include "alogLineReader.h"
#include "FileNotFoundException.h"

////////////////////////////////////////////////////////////////////////////////
alogLineReader::alogLineReader() : m_alogFileStream()
{
}

////////////////////////////////////////////////////////////////////////////////
alogLineReader::~alogLineReader()
{
}

////////////////////////////////////////////////////////////////////////////////
bool alogLineReader::Open( std::string alogFilename )
{
    if( m_alogFileStream.is_open() )
    {
        m_alogFileStream.close();
    }

    m_alogFileStream.open( alogFilename.c_str() );
    if(!m_alogFileStream.is_open())
    {
        throw FileNotFoundException(alogFilename);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void alogLineReader::Read( aloglib::idxRec alogRec, std::string &line )
{
    m_alogFileStream.seekg( alogRec.lineBegin, std::ios_base::beg );
    
    std::getline (m_alogFileStream,line);
}

