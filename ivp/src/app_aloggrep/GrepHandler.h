/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: GrepHandler.h                                        */
/*    DATE: August 6th, 2008                                     */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef ALOG_GREP_HANDLER_HEADER
#define ALOG_GREP_HANDLER_HEADER

#include <vector>
#include <string>
#include <set>

class GrepHandler
{
 public:
  GrepHandler();
  ~GrepHandler() {};

  bool handle(const std::string&, const std::string& str="");
  void addKey(std::string str);
  void printReport();
  void setFileOverWrite(bool v) {m_file_overwrite=v;};
  
 protected:
  std::vector<std::string> getMatchedKeys();
  std::vector<std::string> getUnMatchedKeys();
  
 protected:

  std::vector<std::string> m_keys;
  std::vector<bool>        m_pmatch;

  double m_lines_removed;
  double m_lines_retained;
  double m_chars_removed;
  double m_chars_retained;
  bool   m_file_overwrite;

  std::set<std::string> m_vars_retained;
  std::set<std::string> m_vars_removed;
  
  FILE *m_file_in;
  FILE *m_file_out;
};

#endif
