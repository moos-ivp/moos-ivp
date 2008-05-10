/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PokeDB.h                                             */
/*    DATE: May 9th 2008                                         */
/*          Motivated by Matt Grund's uMOOSPoke App              */
/*****************************************************************/

#include <string>
#include "MOOSLib.h"

class PokeDB : public CMOOSApp  
{
public:
  PokeDB() {};
  virtual ~PokeDB() {};
  
  bool Iterate();
  bool OnNewMail(MOOSMSG_LIST &NewMail) {return(true);};
  bool OnConnectToServer();
  bool OnStartUp();

  void setPokeDouble(const std::string& varname, double val);
  void setPokeString(const std::string& varname, const std::string& val);

protected:
  std::vector<std::string>  m_varname;
  std::vector<std::string>  m_svalue;
  std::vector<double>       m_dvalue;
  std::vector<std::string>  m_valtype;

  double m_start_time;
};
