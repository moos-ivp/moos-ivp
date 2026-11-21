#include "ALogIndexWrapper.h"

#include <vector>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
ALogIndexWrapper::ALogIndexWrapper() : m_bInitialized(false), m_nLineCount(-1)
{ }

////////////////////////////////////////////////////////////////////////////////
ALogIndexWrapper::~ALogIndexWrapper()
{ Close(); }

////////////////////////////////////////////////////////////////////////////////
bool ALogIndexWrapper::Open(const std::string & sfName)
{
    if( m_ALog.Init( sfName ) == false )
    {
        //printf("Initialisation of indexedAlogReader failed. Ensure that the alog path is correct:\n \t'%s'\n\n and that the corresponding index file exists:\n \t'%s'\n\nIf not, you can build an index file:\n \talogIndexWriter /path/to/file.alog\n\n",sfName.c_str(),(sfName+".idx").c_str());
        return false;
    }

    m_sFileName = sfName;
    m_nLineCount = m_ALog.GetNumRecords();

    m_bInitialized = true;
    return true;
}


std::string ALogIndexWrapper::GetFileName()
{
    return m_sFileName;
}
       
////////////////////////////////////////////////////////////////////////////////
bool ALogIndexWrapper::IsOpen()
{
    return m_bInitialized;
}

////////////////////////////////////////////////////////////////////////////////
void ALogIndexWrapper::Close()
{
    //m_ALog.Close();
}

////////////////////////////////////////////////////////////////////////////////
std::string ALogIndexWrapper::GetLine(int nLine)
{
    std::string line;
    m_ALog.GetLine( nLine, line );
    return line;
}

////////////////////////////////////////////////////////////////////////////////
bool ALogIndexWrapper::GetNextToken(const std::string & s,int & nPos,std::string & sTk)
{
    std::string::size_type Start = s.find_first_not_of(" \t", nPos);
    nPos     = s.find_first_of(" \t", Start);
    if( Start!=std::string::npos)
    {
        sTk =s.substr(Start,nPos-Start);
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
int ALogIndexWrapper::GetLineCount()
{
    return m_nLineCount;
}

////////////////////////////////////////////////////////////////////////////////
int ALogIndexWrapper::SeekToFindTime(double dfT)
{
    aloglib::idxRec seekRec;

    // dfT is global time, shift to be relative to log start
    seekRec.time = dfT - m_ALog.GetStartTime();

    std::vector<aloglib::idxRec> recs = m_ALog.GetRecordList();
    std::vector<aloglib::idxRec>::iterator result;

    // find first record which does not compare < seekRec.time
    result = std::lower_bound( recs.begin(), recs.end(), seekRec );

    if( result == recs.end() )
    {
        return -1;
    }
    else
    {
        // return line number rather than record
        return std::distance( recs.begin(), result );
    }
}

////////////////////////////////////////////////////////////////////////////////
double ALogIndexWrapper::GetEntryTime(int i)
{
    return m_ALog.GetTime( i ) + m_ALog.GetStartTime();
}

////////////////////////////////////////////////////////////////////////////////
double ALogIndexWrapper::GetStartTime()
{
    return m_ALog.GetStartTime();
}

////////////////////////////////////////////////////////////////////////////////
const std::set<std::string>& ALogIndexWrapper::GetSourceNames()
{
    return m_ALog.GetSrcList();
}

