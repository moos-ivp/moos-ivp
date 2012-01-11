#include "indexReader.h"
#include "FileNotFoundException.h"

#include <vector>
#include <string>
#include <iterator>
#include <iostream>
#include <fstream>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
indexReader::indexReader():
    m_alogHeader(),
    m_alogMsgList(),
    m_alogSrcList(),
    m_alogRecords()
{}

////////////////////////////////////////////////////////////////////////////////
indexReader::~indexReader()
{}

////////////////////////////////////////////////////////////////////////////////
void indexReader::clear()
{
  m_alogHeader.clear();
  m_alogMsgList.clear();
  m_alogSrcList.clear();
  m_alogRecords.clear();
}

////////////////////////////////////////////////////////////////////////////////
void indexReader::GetMsgTypes(vector<string> &msgTypes)
{
  msgTypes.clear();
  msgTypes.reserve(m_alogMsgList.size());
  std::copy(m_alogMsgList.begin(), m_alogMsgList.end(), inserter(msgTypes, msgTypes.begin()));
}

////////////////////////////////////////////////////////////////////////////////
void indexReader::ReadIndexFile( std::string alogIndexFilename )
{
    clear();

    std::ifstream idxFileStream( alogIndexFilename.c_str() );

    if(!idxFileStream.is_open())
    {
      throw FileNotFoundException(alogIndexFilename);
    }

    // Read in index file's header
    // This could throw a VersionException
    idxFileStream >> m_alogHeader;
    
    // Read in the list of message types contained in the alog
    idxFileStream >> m_alogMsgList;

    // Read in the list of sources contained in the alog
    idxFileStream >> m_alogSrcList;

    // Read all of the index records into a vector
    m_alogRecords.reserve(m_alogHeader.numRecs);
    for(int i = 0; i < m_alogHeader.numRecs; ++i)
    {
        aloglib::idxRec alogRecord;
        idxFileStream >> alogRecord;

        m_alogRecords.push_back( alogRecord );
    }

}

////////////////////////////////////////////////////////////////////////////////
const aloglib::idxRec& indexReader::GetLineRecord(unsigned int lineNum ) const
{ 
    return m_alogRecords.at(lineNum);
}

////////////////////////////////////////////////////////////////////////////////
double indexReader::GetTime( int i ) const
{
    const aloglib::idxRec &rec = GetLineRecord( i );
    return rec.time;
}

////////////////////////////////////////////////////////////////////////////////
double indexReader::GetStartTime() const
{
    return m_alogHeader.startTime;
}

////////////////////////////////////////////////////////////////////////////////
int indexReader::GetNumRecords() const
{
    return m_alogRecords.size();
}

////////////////////////////////////////////////////////////////////////////////
const aloglib::idxMsgList& indexReader::GetMsgList() const
{
    return m_alogMsgList;
}

////////////////////////////////////////////////////////////////////////////////
const aloglib::idxSrcList& indexReader::GetSrcList() const
{
    return m_alogSrcList;
}

////////////////////////////////////////////////////////////////////////////////
const std::vector<aloglib::idxRec>& indexReader::GetRecordList() const
{
    return m_alogRecords;
}

