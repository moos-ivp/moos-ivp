#ifndef _indexWriter_h_
#define _indexWriter_h_

#include <string>

#include "recordTypes.h"

class indexWriter
{
    public:
        indexWriter() {}

        aloglib::idxHeader m_alogHeader;
        aloglib::idxMsgList m_alogMsgList;
        aloglib::idxSrcList m_alogSrcList;
        std::vector<aloglib::idxRec> m_alogRecords;


        void parseAlogFile( std::string alogFileName );
        void writeIndexFile( std::string alogIndexName );
};

#endif // _indexWriter_h_

