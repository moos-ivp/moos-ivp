#include "MOOS/libAlogTools/indexedAlogReader.h"

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
bool indexedAlogReader::Init( std::string alogFilename )
{
    std::string alogIndexFilename = alogFilename + ".idx";

    try
    {
      m_indexReader.ReadIndexFile( alogIndexFilename );
    }
    catch (...)
    {
      return false;
    }
      
    if( m_alogLineReader.Open( alogFilename ) == false )
    {
        return false;
    }

    return true;
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
    aloglib::idxRec curRec = m_indexReader.GetLineRecord( lineNum );
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
const aloglib::idxMsgList& indexedAlogReader::GetMsgList() const
{
    return m_indexReader.GetMsgList();
}

////////////////////////////////////////////////////////////////////////////////
const aloglib::idxSrcList& indexedAlogReader::GetSrcList() const
{
    return m_indexReader.GetSrcList();
}

////////////////////////////////////////////////////////////////////////////////
const std::vector<aloglib::idxRec>& indexedAlogReader::GetRecordList() const
{
    return m_indexReader.GetRecordList();
}

