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

  unsigned int getMaxLines();
  unsigned int getMaxChars();
  unsigned int getVarNameMaxLength();
  double       getMinStartTime();
  double       getMaxStartTime();
  double       getMaxStopTime();
    


 protected:

 private:
  std::vector<std::string>  m_var_names;
  std::vector<std::string>  m_var_sources;
  std::vector<double>       m_var_first;
  std::vector<double>       m_var_last;
  std::vector<unsigned int> m_var_count;
  std::vector<unsigned int> m_var_chars;

  std::map<std::string, int>  m_vmap;

  unsigned int m_lines;
};

#endif 


