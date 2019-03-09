/************************************************************/
/*   NAME: Michael Benjamin                                 */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA */
/*   FILE: LOCTester.h                                      */
/*   DATE: Jan 19th, 2018                                   */
/************************************************************/
 
#ifndef LOC_TESTER_HEADER
#define LOC_TESTER_HEADER

#include <string>
#include <map>

class LOCTester {
public:
  LOCTester();
  virtual ~LOCTester() {};

  bool setFile(std::string filename, unsigned int index);

  void setVerbose(bool v)   {m_verbose=v;}
  
  bool test();
  void print() const;

 protected:

  bool m_verbose;

  std::string m_file1;
  std::string m_file2;
  
  std::map<std::string, std::string> m_loc1;
  std::map<std::string, std::string> m_loc2;
  
};

#endif
