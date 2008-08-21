/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ScanReport.h                                         */
/*    DATE: June 4th, 2008                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef ALOG_SCAN_REPORT_HEADER
#define ALOG_SCAN_REPORT_HEADER

#include <vector>
#include <map>
#include <string>

class ScanReport
{
 public:
  ScanReport() {m_lines=0;};
  ~ScanReport() {};
  
  void addLine(double timestamp, const std::string& varname, 
	       const std::string& source, const std::string &value);

  bool         containsVar(const std::string& varname);
  int          getVarIndex(const std::string& varname);
  unsigned int size() {return(m_var_names.size());};

  std::string getVarName(unsigned int index);
  std::string getVarSources(unsigned int index);
  std::string getVarSources(const std::string& varname)
    {return(getVarSources(getVarIndex(varname)));};

  double getVarFirstTime(unsigned int index);
  double getVarLastTime(unsigned int index);
  
  double getVarFirstTime(const std::string& varname)
    {return(getVarFirstTime(getVarIndex(varname)));};
  double getVarLastTime(const std::string& varname)
    {return(getVarLastTime(getVarIndex(varname)));};
  
  unsigned int getVarCount(unsigned int index);
  unsigned int getVarChars(unsigned int index);

  unsigned int getVarCount(const std::string& varname)
    {return(getVarCount(getVarIndex(varname)));};
  unsigned int getVarChars(const std::string& varname)
    {return(getVarChars(getVarIndex(varname)));};

  // Max values are useful for making column format decisions
  unsigned int getMaxLines();
  unsigned int getMaxChars();
  unsigned int getVarNameMaxLength();
  double       getMinStartTime();
  double       getMaxStartTime();
  double       getMaxStopTime();
    
  // Sorting routines for better user output options
  void         sort(const std::string& style);
  void         sortByVarName(bool ascending=true);
  void         sortByChars(bool ascending=true);
  void         sortByLines(bool ascending=true);
  void         sortByStartTime(bool ascending=true);
  void         sortByStopTime(bool ascending=true);

  // Routines for building and retrieving app stat summary info
  void fillAppStats();
  std::vector<std::string> getAllSources()  {return(m_all_sources);};
  double getLinesBySource(std::string s)    {return(m_app_lines[s]);};
  double getCharsBySource(std::string s)    {return(m_app_chars[s]);};
  double getLinesPctBySource(std::string s) {return(m_app_lines_pct[s]);};
  double getCharsPctBySource(std::string s) {return(m_app_chars_pct[s]);};

 protected:
  void switchItems(unsigned int, unsigned int);

 private:
  std::vector<std::string>  m_var_names;
  std::vector<std::string>  m_var_sources;
  std::vector<double>       m_var_first;
  std::vector<double>       m_var_last;
  std::vector<unsigned int> m_var_lines;
  std::vector<unsigned int> m_var_chars;

  // A convenience map for finding the var index in above vectors
  std::map<std::string, int>  m_vmap;

  // Application statistics  - Done on optionsl Pass #2A
  std::map<std::string, double>  m_app_lines;
  std::map<std::string, double>  m_app_chars;
  std::map<std::string, double>  m_app_vars;
  
  // Application statistics  - Done on optionsl Pass #2B
  std::map<std::string, double>  m_app_lines_pct;
  std::map<std::string, double>  m_app_chars_pct;
  std::vector<std::string>       m_all_sources;

  unsigned int m_lines;
};

#endif 


