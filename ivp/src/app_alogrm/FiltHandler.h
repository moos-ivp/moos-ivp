/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: FiltHandler.h                                        */
/*    DATE: August 6th, 2008                                     */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef FILT_HANDLER_HEADER
#define FILT_HANDLER_HEADER

#include <set>
#include <vector>
#include <string>

class FiltHandler
{
 public:
  FiltHandler();
  ~FiltHandler() {};

  bool handle(const std::string&, const std::string& str="");
  bool setParam(const std::string& param, const std::string& value);
  void printReport();

 protected:

  std::vector<std::string> m_keys;
  std::vector<bool>        m_pmatch;

  bool  m_chuck_strings;
  bool  m_chuck_numbers;
  bool  m_timeshift;
  bool  m_clean;

  double m_lines_removed;
  double m_lines_retained;
  double m_chars_removed;
  double m_chars_retained;
  bool   m_file_overwrite;

  std::set<std::string> m_vars_retained;
  std::set<std::string> m_vars_removed;

  FILE *m_file_in;
  FILE *m_file_out;

  double m_logstart;
};

#endif
