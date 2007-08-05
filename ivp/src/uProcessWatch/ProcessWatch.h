/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ProcessWatch.h                                       */
/*    DATE: May 27th 2007  (MINUS-07)                            */
/*****************************************************************/

#ifndef PROCESS_WATCH_HEADER
#define PROCESS_WATCH_HEADER

#include <string>
#include <vector>
#include "MOOSLib.h"

class ProcessWatch : public CMOOSApp
{
public:
  ProcessWatch() {};
  virtual ~ProcessWatch() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

protected:
  void addItem(std::string procname, std::string varname, 
	       bool isdouble, double dval, std::string sval);

  bool parseAddItem(std::string);
  bool isAlive(std::string);

  int  size() {return(m_procname.size());};

protected:
  std::string               m_db_clients;
  std::vector<std::string>  m_procname;
  std::vector<bool>         m_isalive;
  std::vector<bool>         m_reported;
  std::string               m_awol_procs;
};

#endif 
