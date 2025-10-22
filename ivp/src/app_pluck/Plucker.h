/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: MIT Dept of Mechanical Eng                           */
/*    FILE: Plucker.h                                            */
/*    DATE: Mar 8th, 2024                                        */
/*****************************************************************/

#ifndef PLUCKER_HEADER
#define PLUCKER_HEADER

#include <vector>
#include <string>

class Plucker
{
 public:
  Plucker();
  virtual ~Plucker() {}

  bool   setFile(std::string);
  bool   setLNum(std::string);
  bool   setComment(std::string);
  bool   setIndex(std::string);
  bool   setFld(std::string);
  bool   setGSep(std::string);
  bool   setLSep(std::string);
  bool   setGrep(std::string);
  
  std::string pluck();

 protected: // Config vars
  unsigned int m_lnum;
  unsigned int m_index;
  std::string  m_file;
  std::string  m_comment;
  std::string  m_fld;
  std::string  m_grep;
  char         m_gsep;
  char         m_lsep;
  
protected: // State variables

};

#endif 

