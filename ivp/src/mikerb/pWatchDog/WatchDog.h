/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: WatchDog.h                                           */
/*    DATE: December 9th 2004                                    */
/*****************************************************************/

#ifndef WatchDog_HEADER
#define WatchDog_HEADER

#include <string>
#include <vector>
#include "MOOSLib.h"

class WatchDog : public CMOOSApp
{
public:
  WatchDog();
  virtual ~WatchDog();

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
  std::string db_clients;

  std::vector<std::string>  m_procname;
  std::vector<std::string>  m_varname;
  std::vector<std::string>  m_vartype;
  std::vector<std::string>  m_varsval;
  std::vector<double>       m_vardval;
  std::vector<bool>         m_isalive;
};

#endif 
