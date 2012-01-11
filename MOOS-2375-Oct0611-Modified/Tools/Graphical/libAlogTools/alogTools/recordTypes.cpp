#include "recordTypes.h"
#include "utils.h"
#include "VersionException.h"

#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <stdexcept>

using namespace std;

namespace aloglib
{

  bool  idxRec::operator <(const idxRec& b) const
  {
    return time < b.time;
  }

  std::ostream& operator<<(std::ostream& os, const idxRec & rec)
  {
    os.write((char*) &rec.time, sizeof(rec.time));
    os.write((char*) &rec.lineBegin, sizeof(rec.lineBegin));
    os.write((char*) &rec.len, sizeof(rec.len));
    return os;
  }

  std::istream& operator>>(std::istream& is, idxRec & rec)
  {
    is.read((char*)(&rec.time), sizeof(rec.time));
    is.read((char*)(&rec.lineBegin), sizeof(rec.lineBegin));
    is.read((char*)(&rec.len), sizeof(rec.len));
    return is;
  }


  std::ostream& operator<<(std::ostream& os, const idxHeader & header)
  {
    os.write((char*) &header.version, sizeof(header.version));
    os.write((char*) &header.recsBegin, sizeof(header.recsBegin));
    os.write((char*) &header.numRecs, sizeof(header.numRecs));
    os.write((char*) &header.startTime, sizeof(header.startTime));
    return os;
    
  }


  std::istream& operator>>(std::istream& is, idxHeader & header)
  {
    // Store where we are, in case we decide we can't read the whole header
    int streampos = is.tellg();

    is.read((char*)(&header.version), sizeof(header.version));

    if (header.version != FILE_FORMAT_VERSION)
    {
      is.seekg(streampos);
      throw VersionException("Incorrect index file version.");
    }

    is.read((char*)(&header.recsBegin), sizeof(header.recsBegin));
    is.read((char*)(&header.numRecs), sizeof(header.numRecs));
    is.read((char*)(&header.startTime), sizeof(header.startTime));
    return is;
  }


  std::ostream& operator<<(std::ostream& os, const idxMsgList & msglist)
  {
    if (msglist.empty()) return os;

    copy(msglist.begin(), msglist.end(), ostream_iterator<string>(os, ","));
    os << std::endl;

    return os;
  }


  std::istream& operator>>(std::istream& is, idxMsgList & msglist)
  {
    // Read in comma separated list of strings
    std::string line;
    getline(is, line);

    // Push each one into the msglist
    vector<string> tokens;
    Tokenize(line, tokens, ",");
    std::copy(tokens.begin(), tokens.end(), inserter(msglist, msglist.begin()));

    return is;
  }

  std::ostream& operator<<(std::ostream& os, const idxSrcList & srcList)
  {
    if (srcList.empty()) return os;

    copy(srcList.begin(), srcList.end(), ostream_iterator<string>(os, ","));
    os << std::endl;

    return os;
  }


  std::istream& operator>>(std::istream& is, idxSrcList & srcList)
  {
    // Read in comma separated list of strings
    std::string line;
    getline(is, line);

    // Push each one into the srcList
    vector<string> tokens;
    Tokenize(line, tokens, ",");
    std::copy(tokens.begin(), tokens.end(), inserter(srcList, srcList.begin()));

    return is;
  }



}
