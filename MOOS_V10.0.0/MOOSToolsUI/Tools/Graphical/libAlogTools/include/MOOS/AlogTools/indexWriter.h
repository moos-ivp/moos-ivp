#ifndef _indexWriter_h_
#define _indexWriter_h_

#include <string>

#include "recordTypes.h"

namespace MOOS {
namespace AlogTools {

class indexWriter
{
    public:
        indexWriter() {}

        idxHeader m_alogHeader;
        idxMsgList m_alogMsgList;
        idxSrcList m_alogSrcList;
        std::vector<idxRec> m_alogRecords;


        void parseAlogFile( std::string alogFileName );
        void writeIndexFile( std::string alogIndexName );
};

}  // namespace AlogTools
}  // namespace MOOS

#endif // _indexWriter_h_

