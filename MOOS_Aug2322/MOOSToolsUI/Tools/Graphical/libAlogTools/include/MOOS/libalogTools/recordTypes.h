#ifndef __recordTypes_h
#define __recordTypes_h

#include <iostream>
#include <set>
#include <string>

const int FILE_FORMAT_VERSION = 0;

namespace aloglib
{

  class idxRec
  {
  public:
    idxRec() : time(0.0), lineBegin(0), len(0) {}

    bool  operator <(const  idxRec& b) const;

    friend std::ostream& operator<<(std::ostream& os, const idxRec & rec);
    friend std::istream& operator>>(std::istream& is, idxRec & rec);
    
  public:
    double time;
    long lineBegin;
    long len;

  };

  class idxHeader
  {
  public:
    idxHeader() : version(FILE_FORMAT_VERSION), recsBegin(0), numRecs(0), startTime(0.0) {}

    void clear()
    {
      version = FILE_FORMAT_VERSION;
      recsBegin = 0;
      numRecs = 0;
      startTime = 0.0;
    }

    friend std::ostream& operator<<(std::ostream& os, const idxHeader & header);
    friend std::istream& operator>>(std::istream& is, idxHeader & header);

  public:
    int version;
    long recsBegin;
    long numRecs;
    double startTime;
  };

  class idxMsgList : public std::set< std::string >
  {
    friend std::ostream& operator<<(std::ostream& os, const idxMsgList & msglist);
    friend std::istream& operator>>(std::istream& is, idxMsgList & msglist);
  };

  class idxSrcList : public std::set< std::string >
  {
    friend std::ostream& operator<<(std::ostream& os, const idxSrcList & msglist);
    friend std::istream& operator>>(std::istream& is, idxSrcList & msglist);
  };


}

#endif // __recordTypes_h
