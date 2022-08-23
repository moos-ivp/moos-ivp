#ifndef _ALogIndexWrapper_h_
#define _ALogIndexWrapper_h_

#include "MOOS/libalogTools/indexedAlogReader.h"

// wraps indexed alog reader so that uPlayBack can use this new
// indexed class without any modifications

class ALogIndexWrapper
{
    public:

        bool m_bInitialized;
        int m_nLineCount;
        indexedAlogReader m_ALog;
        std::string m_sFileName;

        ALogIndexWrapper();
        ~ALogIndexWrapper();
        bool Open(const std::string & sfName);
        std::string GetFileName();
        bool IsOpen();
        void Close();
        std::string GetLine(int nLine);
        bool GetNextToken(const std::string & s,int & nPos,std::string & sTk);
        int GetLineCount();
        int SeekToFindTime(double dfT);
        double GetEntryTime(int i);
        double GetStartTime();
        const std::set<std::string>& GetSourceNames();
};

#endif // _ALogIndexWrapper_h_

