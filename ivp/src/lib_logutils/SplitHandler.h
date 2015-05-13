/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SplitHandler.h                                       */
/*    DATE: Feb 2nd, 2015                                        */
/*****************************************************************/

#ifndef ALOG_SPLIT_HANDLER_HEADER
#define ALOG_SPLIT_HANDLER_HEADER

#include <vector>
#include <string>
#include <map>
#include <set>

class SplitHandler
{
 public:
  SplitHandler(std::string);
  ~SplitHandler() {}

  bool handle();
  bool handlePreCheckALogFile();
  void setVerbose(bool v)          {m_verbose=v;}
  void setDirectory(std::string s) {m_given_dir=s;}
  
 protected:
  bool handlePreCheckSplitDir();
  bool handleMakeSplitFiles();
  bool handleMakeSplitSummary();
  
 protected: // Config variables
  std::string m_alog_file;
  std::string m_given_dir;
  bool        m_verbose;

 protected: // State variables
  std::string m_basedir;
  std::string m_logstart;
  std::string m_time_min;
  std::string m_time_max;
  std::string m_vname;
  std::string m_vtype;
  std::string m_vlength;

  bool m_alog_file_confirmed;

  bool m_split_dir_prior;

  std::string m_curr_helm_iter;

  // Each map key is a MOOS variable name
  std::map<std::string, FILE*>       m_file_ptr;
  std::map<std::string, std::string> m_var_type;
  std::map<std::string, std::set<std::string> > m_var_srcs;

  // Keep track of all unique bhv names for summary file
  std::set<std::string> m_bhv_names;
};

#endif

