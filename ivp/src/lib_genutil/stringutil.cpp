#include "stringutil.h"

#include <iostream>
#include <sstream>

using namespace std;

//==============================================================================

std::vector<std::string> tokenize(const std::string & s, const std::string & separator) {
  vector<string> v;

  if (s.length() == 0) {
    return v;
  }

  string::size_type pos1 = 0;

  bool done = false;
  while (! done) {
    string::size_type pos2 = s.find(separator, pos1);
    string token = s.substr(pos1, pos2 - pos1);
    v.push_back(token);

    if (pos2 == string::npos) {
      done = true;
    }
    else {
      pos1 = pos2 + separator.length();
    }
  }

  return v;
}

//==============================================================================

std::string vect_to_string(std::string prefix, std::string suffix, 
    const std::vector<std::string> & v) {

  ostringstream os;

  for (vector<string>::const_iterator pos = v.begin(); pos != v.end(); ++pos) {
    os << prefix << (*pos) << suffix << endl;
  }

  return os.str();
}

