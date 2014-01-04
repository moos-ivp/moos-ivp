#include "MOOS/AlogTools/indexedAlogReader.h"
#include "MOOS/AlogTools/exceptions.h"

namespace MOOS {
namespace AlogTools {


////////////////////////////////////////////////////////////////////////////////
indexedAlogReader::indexedAlogReader() :
    m_alogLineReader(),
    m_indexReader(),
    m_CurrentLine(0)
{}

////////////////////////////////////////////////////////////////////////////////
indexedAlogReader::~indexedAlogReader()
{}

////////////////////////////////////////////////////////////////////////////////
void indexedAlogReader::Init( std::string alogFilename )
{
    // Throws exceptions::CannotOpenFileForReadingException, but we'll pass
    // it on
    m_alogLineReader.Open( alogFilename );

    std::string alogIndexFilename = alogFilename + ".idx";

    try
    {
        m_indexReader.ReadIndexFile( alogIndexFilename );
    }
    catch (exceptions::CannotOpenFileForReadingException& e)
    {
        throw exceptions::CannotOpenIndexFileForReadingException(e.FileName());
    }
}

////////////////////////////////////////////////////////////////////////////////
void indexedAlogReader::GetNextLine(std::string & line)
{
    GetLine( m_CurrentLine++, line );
}

////////////////////////////////////////////////////////////////////////////////
void indexedAlogReader::GetPrevLine(std::string & line)
{
    GetLine( --m_CurrentLine, line );
}

////////////////////////////////////////////////////////////////////////////////
void indexedAlogReader::GetLine( int lineNum, std::string & line)
{
    idxRec curRec = m_indexReader.GetLineRecord( lineNum );
    m_alogLineReader.Read( curRec, line );
}

////////////////////////////////////////////////////////////////////////////////
int indexedAlogReader::GetNumRecords() const
{
    return m_indexReader.GetNumRecords();
}

////////////////////////////////////////////////////////////////////////////////
double indexedAlogReader::GetTime( int i ) const
{
    return m_indexReader.GetTime( i );
}

////////////////////////////////////////////////////////////////////////////////
double indexedAlogReader::GetStartTime() const
{
    return m_indexReader.GetStartTime();
}

////////////////////////////////////////////////////////////////////////////////
const idxMsgList& indexedAlogReader::GetMsgList() const
{
    return m_indexReader.GetMsgList();
}

////////////////////////////////////////////////////////////////////////////////
const idxSrcList& indexedAlogReader::GetSrcList() const
{
    return m_indexReader.GetSrcList();
}

////////////////////////////////////////////////////////////////////////////////
const std::vector<idxRec>& indexedAlogReader::GetRecordList() const
{
    return m_indexReader.GetRecordList();
}

}  // namespace AlogTools
}  // namespace MOOS
