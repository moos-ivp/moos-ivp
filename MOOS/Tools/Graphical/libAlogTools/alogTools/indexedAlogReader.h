#ifndef _indexedAlogReader_h_
#define _indexedAlogReader_h_

#include <string>
#include <vector>

#include "recordTypes.h"
#include "alogLineReader.h"
#include "indexReader.h"

class indexedAlogReader
{
    public:
        indexedAlogReader();
        ~indexedAlogReader();

        bool Init( std::string alogFilename );
        void GetNextLine(std::string & line);
        void GetPrevLine(std::string & line);
        
        void GetLine( int lineNum, std::string & line);

        int GetNumRecords() const;
        double GetTime( int i ) const;
        double GetStartTime() const;
        const aloglib::idxMsgList& GetMsgList() const;
        const aloglib::idxSrcList& GetSrcList() const;
        const std::vector<aloglib::idxRec>& GetRecordList() const;

        alogLineReader m_alogLineReader;
        indexReader m_indexReader;
        int m_CurrentLine;
};

#endif // _indexedAlogReader_h_

