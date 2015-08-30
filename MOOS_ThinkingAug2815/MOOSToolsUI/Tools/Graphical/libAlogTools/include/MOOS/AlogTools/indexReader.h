#ifndef _indexReader_h_
#define _indexReader_h_

#include "recordTypes.h"

#include <vector>
#include <string>

namespace MOOS {
namespace AlogTools {

class indexReader
{
    public:
        indexReader();
        ~indexReader();

        void clear();

        void ReadIndexFile( std::string alogIndexFilename );
        
        const idxRec& GetLineRecord( unsigned int lineNum ) const;
        void GetMsgTypes(std::vector<std::string> & msgTypes);

        double GetTime( int i ) const;
        double GetStartTime() const;
        int GetNumRecords() const;
        const idxMsgList& GetMsgList() const;
        const idxSrcList& GetSrcList() const;
        const std::vector<idxRec>& GetRecordList() const;
        
    private:
        idxHeader m_alogHeader;
        idxMsgList m_alogMsgList;
        idxSrcList m_alogSrcList;
        std::vector<idxRec> m_alogRecords;

};

}  // namespace AlogTools
}  // namespace MOOS

#endif // _indexReader_h_

