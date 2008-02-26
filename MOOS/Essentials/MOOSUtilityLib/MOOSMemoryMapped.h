#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4996)
#endif

#ifndef MOOSMemoryMappedH
#define MOOSMemoryMappedH


#include <stdlib.h>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <set>
#include <iterator>
#include <algorithm>
#include <iomanip>


#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
#endif 

class CMOOSMemMappedFile
{
public:
    /** simple constructor which opens a mem mapped file in a OS-dependent way*/
    CMOOSMemMappedFile()
    {
        m_hMapHandle = 0;
        m_pBuffer = NULL;
        m_nSize = -1;
    }
    
    bool Open(const std::string & sfName)
    {
        m_hMapHandle = 0;
        m_sName = sfName;
        m_pBuffer = NULL;
        m_nSize = -1;
        
#ifdef WIN32
        //Windows land
        m_hFileHandle = CreateFile(m_sName.c_str(),
                                       GENERIC_READ, 
                                       FILE_SHARE_READ,
                                       NULL,
                                       OPEN_EXISTING,
                                       FILE_ATTRIBUTE_NORMAL,
                                       NULL);
        
        if (m_hFileHandle!=INVALID_HANDLE_VALUE)
        {
            m_hMapHandle = CreateFileMapping(m_hFileHandle, NULL, PAGE_READONLY, 0,0, NULL);
            if (m_hMapHandle != NULL)
            {
                m_pBuffer = (char*)MapViewOfFile(m_hMapHandle, FILE_MAP_READ, 0, 0, 0);
            }
        }
        else 
        {
            
            PrintLastWin32Error();
            
            m_hFileHandle = NULL;
            m_hMapHandle = NULL;
        }
#else
        //linux side..
        m_hMapHandle = open(m_sName.c_str(), O_RDWR, 0);
        if (m_hMapHandle != -1)
        {
            //get file stats
            struct stat StatusBuffer;
            fstat(m_hMapHandle, &StatusBuffer);     
            m_pBuffer = (char*)mmap(NULL,
                StatusBuffer.st_size,
                PROT_READ,
                MAP_SHARED,
                m_hMapHandle , 0);
        }
        else
        {
            m_hMapHandle = 0;
        }
#endif 
        
        m_nSize = GetSize();
        
        //report out success
        if(IsOpen())
        {
            std::cout<<"Successfully Memory Mapped "<<double(m_nSize)/(1048576)<<"MB file \n";
            return true;
        }
        else
        {
            return MOOSFail("CATASTROPHE : Failed to open file %s",sfName.c_str());
        }
        
    }
    
    ~CMOOSMemMappedFile()
    {
        Close();
    }
    
    
    //helper function to print WIN32
    void PrintLastWin32Error()
    {
#ifdef _WIN32
        LPVOID lpMsgBuf;
        FormatMessage( 
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM | 
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR) &lpMsgBuf,
            0,
            NULL 
            );
        
        std::string s((LPCTSTR)lpMsgBuf);        
        MOOSTrace("Problem : %s\n",s.c_str());
        LocalFree( lpMsgBuf );
#endif
    }
    
    void Close()
    {
        if (m_hMapHandle)
        {
#ifdef WIN32
            UnmapViewOfFile(m_pBuffer);
            CloseHandle(m_hMapHandle);
            CloseHandle(m_hFileHandle);
            m_hFileHandle = 0;
            
#else
            munmap(m_pBuffer, 0);
            close(m_hMapHandle);
#endif 
            m_hMapHandle = 0;
            m_pBuffer = 0;
        }   
    }
    
    /** returns size in bytes of file, 0 if not open*/
    int GetSize()
    {
        if(!IsOpen())
            return 0;
        
        if(m_nSize<0)
        {
#ifdef WIN32
            m_nSize =  (int)GetFileSize(m_hFileHandle,NULL);
#else
            struct stat sbuf;
            fstat(m_hMapHandle, &sbuf);     
            m_nSize = sbuf.st_size;
#endif
        }
        return m_nSize;
    }
    
    /**provides [] operator for file*/
    char * operator[](int i)
    {
        if(i<GetSize())
            return (GetContents()+i);
        else
            return NULL;
    }
    
    
    /** returns true if file has been opened succesfully */
    bool IsOpen()
    {
        return m_hMapHandle!=0 && GetContents()!=NULL;
    }
    
    /** returns pointer to start of file contents */
    char* GetContents()
    {
        return m_pBuffer;
    }
    
 private:
     
     
#ifdef WIN32
     HANDLE m_hMapHandle;
     HANDLE m_hFileHandle;
#else
     int m_hMapHandle;
#endif 
     
     std::string m_sName;
     char* m_pBuffer;
     int m_nSize;
     
};



/** simple structure to hold text line info */
struct TextLineInfo
{
    TextLineInfo(){pLineStart=NULL;pLineEnd = NULL;};
    TextLineInfo(char * pStart,char * pEnd)
    {
        pLineStart = pStart;
        pLineEnd = pEnd;
    }
    char * Start(){return pLineStart;};
    char * End(){return pLineEnd;};
    bool IsWanted(){return true;};
    
    char * pLineStart;
    char * pLineEnd;
};



/** Specialised memory mapped file class to handle text files. 

  Default template parameter is type TextLineInfo. Other template types must support
  the following functions:
  T::T(char * pStart, char * pEnd) //constructor
  char * T.Start(); //return start of line
  char * T.End();   //return end of line
  bool IsWanted();  //return true if this line is wanted
  
    Main functionality of this class is to produce a index of lines each entry
    pointing to a instance of "T"
    
*/
template <class T = TextLineInfo >
class CMOOSMemMappedTextFile : public CMOOSMemMappedFile
{
private:
    typedef CMOOSMemMappedFile BASE;
public:
    
    /** we will refer to T as type LINE */
    typedef T LINE;
    
    /** constructor counts lines and buld line index */
    CMOOSMemMappedTextFile(){m_nLineCount = -1;};
    
    bool Open(const std::string & sName,int nMaxLines=-1)
    {
        if(!BASE::Open(sName))
            return false;
        
        if(nMaxLines<0)
            m_nLineCount = GetLineCount();
        else
            m_nLineCount = nMaxLines;

        BuildLineIndex(nMaxLines);
        
        return true;
    }
    
    /** simply returns size of file in lines looking for \n*/
    int GetLineCount()
    {
        if(IsOpen())
        {
            if(m_nLineCount<0)
            {
                //counting lines - 
                std::cout<<"Counting Lines....";
                int nc = std::count(GetContents(),GetContents()+GetSize(),'\n');
                /*
                char * pLook = GetContents();
                char * pStop = pLook+GetSize();
                int nc = 0;
                while(pLook<pStop)
                {
                    if(*pLook++=='\n')
                        nc++;
                }*/

                std::cout<<"done ("<<nc<<")\n";
                return nc;
            }
            else
            {
                return m_nLineCount;
            }
        }
        return 0;
    }
    
    /** iterate through the file building the line index*/
    bool BuildLineIndex(int nNumLines = -1)
    {
        std::cout<<"Indexing lines....";
        
        if(nNumLines==-1)
            nNumLines = m_nLineCount;

        m_LineIndex.resize(nNumLines);
        
        char * pNewLine = GetContents();
        char *pFileEnd = pNewLine+GetSize();
        int n = 0;
        while(pNewLine<pFileEnd && n<nNumLines)
        {
            char * pLineEnd = std::find(pNewLine,pFileEnd,'\n');
            if(pLineEnd<pFileEnd)
            {
                //this is where specialised constructors for the non default
                //line type template may get called...
                m_LineIndex[n] = LINE(pNewLine,pLineEnd);
                
                if(m_LineIndex[n].IsWanted())
                    n++;
            }
            pNewLine = pLineEnd+1;
        }
        
        std::cout<<"done\n";
        
        //we may have discounted some lines...
        std::cout<<"Pruning "<<std::distance(m_LineIndex.begin()+n,m_LineIndex.end())<<" elements from LineIndex (comments)\n";
        m_LineIndex.erase(m_LineIndex.begin()+n,m_LineIndex.end());
        
        //re measure the number of lines
        m_nLineCount = m_LineIndex.size();
        
        return true;
    }
    
    /** return the specified line as a string*/
    std::string GetLine(int nLine)
    {
        //note templated type nmust support Start() and End()
        if(nLine<static_cast<int>(m_LineIndex.size()))
            return std::string(m_LineIndex[nLine].Start(),m_LineIndex[nLine].End());
        else
        {
            std::cerr<<"OUCH - indexing past end of file index!\n";
            return "";
        }
    }
    
    /** a vector of LINES (templated type).*/ 
    std::vector< LINE > m_LineIndex;
    
    /** how many lines we have*/
    int m_nLineCount;
};






struct ALogLineInfo : public TextLineInfo
{
    ALogLineInfo(){};
    ALogLineInfo(char * pStart,char * pEnd):TextLineInfo(pStart,pEnd)
    {
        dfTimeField = FastATOF(pStart);
        //std::cout<<dfTimeField<<std::endl;
    }
    
    //returns true if comment
    bool IsComment() const
    {
        // alog are well formatted - first char in a line being a %
        //indicates a comment
        return *pLineStart=='%';
    }
    
    static bool Printable(char c)
    {
        return (isalnum(c) != 0);       
    }
    
    
    bool IsEmpty()
    {
        //        return std::find_if(pLineStart,pLineEnd,isalnum)==pLineEnd;
        return std::find_if(pLineStart,pLineEnd,Printable)==pLineEnd;
    }
    
    bool IsWanted()
    {
        if(IsComment())
            return false;
        
        if(IsEmpty())
            return false;
        
        return true;
    }
    
    double GetTimeField(double dfOffset=0) const
    {
        return dfTimeField+dfOffset;
    }
    
    double FastATOF(const char * pStart)
    {
        //quick and dirty  - looks for double in first 20 digits
        //beginnin at pStart.
        //prevents long call of strlen embedded in atof ..
        #define MAX_DOUBLE_STRING_LENGTH 20
        char Tmp[MAX_DOUBLE_STRING_LENGTH];
        std::copy(pStart,pStart+sizeof(Tmp),Tmp);
        Tmp[sizeof(Tmp)-1] = 0;
        
        return atof(Tmp);
        
    }
    
    
    
    double dfTimeField;
};


inline bool ALogLineInfoLessThan(const ALogLineInfo & L1,const ALogLineInfo & L2) 
{
/*static int nCalls = 0;    
if(nCalls++%10000==0)
    cout<<"nC:"<<nCalls<<std::endl;*/
    
    if(L1.IsComment() && L2.IsComment())
        return false;
    if(L1.IsComment())
        return true;
    if(L2.IsComment())
        return false;
    
    //look at first column of times...
    return L1.dfTimeField < L2.dfTimeField;        
}

/** specialisation of memory mapped ASCII file to swoop around MOOS alog files.
Main specialisation here is that the templated type is a ALogLineInfo struct
which contains time information. This is sorted during creation */
class CMOOSMemMappedAlogFile : public CMOOSMemMappedTextFile< ALogLineInfo >
{
    
private:
    typedef CMOOSMemMappedTextFile <ALogLineInfo>  BASE;
public:
    CMOOSMemMappedAlogFile(){m_dfLogStart = -1;}
    
    bool Open(const std::string & sName,bool bSummary = true, int nMaxLines=-1)
    {
        if(!BASE::Open(sName,nMaxLines))
            return false;
        
        if(!SortLineIndex())
            return false;
        
        if(bSummary && !ReadSourceAndTypeSets())
            return false;
        
        if(!ReadStartTime())
            return false;
        
        return true;
    }
    
    bool SortLineIndex()
    {
        std::cout<<"Sorting Lines....";
        std::sort(m_LineIndex.begin(),m_LineIndex.end(),ALogLineInfoLessThan);
        std::cout<<"done"<<std::endl;
        return true;
    }
    
    static bool TimePredicate(const LINE & L, double dfT)
    {
        return L.GetTimeField()<dfT;
    }
    
    static bool TimePredicate2(const LINE & L, const LINE &  L2)
    {
        return L.GetTimeField()<L2.GetTimeField();
    }
    
    int SeekToFindTime(double dfT)
    {       
        std::vector<LINE>::iterator p;
        LINE Q;
        Q.dfTimeField = dfT - m_dfLogStart;
        //GCC and VC<8 are OK with this but VC8 is less sure..
        //p = std::lower_bound(m_LineIndex.begin(),m_LineIndex.end(),dfT - m_dfLogStart,TimePredicate);
        p = std::lower_bound(m_LineIndex.begin(),m_LineIndex.end(),Q,TimePredicate2);
        if(p==m_LineIndex.end())
            return -1;
        else
            return std::distance(m_LineIndex.begin(),p);
    }
    
    //simply extract a space delimeted token from a string startign at position nPos
    static bool GetNextToken(const std::string & s,int & nPos,std::string & sTk)
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
    
    /** This function builds two sets , one conataining all the unique messgae names and the other
    containing the set of message sources (processes)*/
    bool ReadSourceAndTypeSets()
    {
        
        std::cout<<"extracting message names and sources.....";
        for(int i = 0; i<m_nLineCount;i++)
        {
            if(!m_LineIndex[i].IsComment())
            {
                std::string sLine = GetLine(i);
                std::string sT,sWhat,sWho;
                int n= 0;
                
                //time
                GetNextToken(sLine,n,sT);
                
                //what
                GetNextToken(sLine,n,sWhat);
                m_MessageNames.insert(sWhat);
                
                //who
                GetNextToken(sLine,n,sWho);
                m_SourceNames.insert(sWho);
                
            }
        }
        std::cout<<"done\n";
        
        //print out some warm fuzzies
        std::cout<<"\nThere are "<<m_MessageNames.size()<<" different message types:\n\t";
        std::copy(m_MessageNames.begin(),
            m_MessageNames.end(),
            std::ostream_iterator<std::string>(std::cout ,"\n\t"));
        
        std::cout<<"\nThere are "<<m_SourceNames.size()<<" different sources:\n\t";
        std::copy(m_SourceNames.begin(),
            m_SourceNames.end(),
            std::ostream_iterator<std::string>(std::cout ,"\n\t"));
        
        
        return true;
    }
    
    //simply fills in the log fills start time
    bool ReadStartTime()
    {
        int nHeaderBlock = 1000;
        std::string sHeader(GetContents(),GetContents()+nHeaderBlock);
        MOOSChomp(sHeader,"LOGSTART");
        m_dfLogStart = atof(sHeader.c_str());
        
        //this is unix time so it must be geqz
        return m_dfLogStart>=0;
    }
    
    double GetLogStart(){return m_dfLogStart;};
    
    double GetEntryTime(int i)
        {
            if(i>=static_cast<int> (m_LineIndex.size()))
                return -1;

            //return the time of the ith index offset by the 
            //start time of the whoel log
            return m_LineIndex[i].GetTimeField(GetLogStart());
        }

    std::set<std::string> GetSourceNames(){return m_SourceNames;};
    std::set<std::string> GetMessageNames(){return m_MessageNames;};
    
    std::set<std::string> m_MessageNames;
    std::set<std::string> m_SourceNames;
    double m_dfLogStart;
    
};



#endif

