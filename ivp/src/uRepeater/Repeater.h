/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: Repeater.h                                      */
/*    DATE: Oct 5th 2007                                    */
/************************************************************/

#ifndef REPEATER_HEADER
#define REPEATER_HEADER

#include "MOOSLib.h"

class Repeater : public CMOOSApp
{
 public:
  Repeater() {};
  virtual ~Repeater() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void RegisterVariables();

 protected:
  bool handleDeclaration(std::string);
  bool addVariable(std::string, int, double);
  int  findVarIndex(std::string);

 protected:
  std::vector<std::string> m_rpt_variable;
  std::vector<int>         m_rpt_count;
  std::vector<double>      m_rpt_interval;

  std::vector<int>         m_count_remaining;
  std::vector<double>      m_time_stamp;
  std::vector<double>      m_dfVal;
  std::vector<std::string> m_sVal;
  std::vector<std::string> m_type;
  
};

#endif 
