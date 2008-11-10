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
  PokeDB(std::string g_server, long int g_port); 
  PokeDB();

  virtual ~PokeDB() {};
  
  bool Iterate();
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool OnConnectToServer();
  bool OnStartUp();

  void setConfigureCommsLocally(bool v) {m_configure_comms_locally=v;};

  void setPokeDouble(const std::string& varname, double val);
  void setPokeString(const std::string& varname, const std::string& val);

 protected:
  void registerVariables();
  void updateVariable(CMOOSMsg& msg);
  void printReport();

  bool ConfigureComms();

protected:
  std::vector<std::string>  m_varname;
  std::vector<std::string>  m_valtype;
  std::vector<std::string>  m_svalue_poke;
  std::vector<double>       m_dvalue_poke;

  std::vector<std::string>  m_svalue_read;
  std::vector<std::string>  m_dvalue_read;
  std::vector<std::string>  m_source_read;
  std::vector<std::string>  m_valtype_read;
  std::vector<std::string>  m_wrtime_read;
  std::vector<bool>         m_varname_recd;

  double m_db_start_time;
  int    m_iteration;
  bool   m_configure_comms_locally;
};
