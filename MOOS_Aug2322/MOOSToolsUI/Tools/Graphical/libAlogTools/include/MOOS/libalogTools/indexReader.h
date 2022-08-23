#ifndef _indexReader_h_
#define _indexReader_h_

#include "recordTypes.h"

#include <vector>
#include <string>

class indexReader
{
    public:
        indexReader();
        ~indexReader();

        void clear();

        void ReadIndexFile( std::string alogIndexFilename );
        
        const aloglib::idxRec& GetLineRecord( unsigned int lineNum ) const;
        void GetMsgTypes(std::vector<std::string> & msgTypes);

        double GetTime( int i ) const;
        double GetStartTime() const;
        int GetNumRecords() const;
        const aloglib::idxMsgList& GetMsgList() const;
        const aloglib::idxSrcList& GetSrcList() const;
        const std::vector<aloglib::idxRec>& GetRecordList() const;
        
    private:
        aloglib::idxHeader m_alogHeader;
        aloglib::idxMsgList m_alogMsgList;
        aloglib::idxSrcList m_alogSrcList;
        std::vector<aloglib::idxRec> m_alogRecords;

};

#endif // _indexReader_h_

