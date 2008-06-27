/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: Relayer.h                                       */
/*    DATE: Jun 26th 2008                                   */
/************************************************************/

#ifndef P_RELAY_VAR_HEADER
#define P_RELAY_VAR_HEADER

#include "MOOSLib.h"

class Relayer : public CMOOSApp
{
 public:
  Relayer();
  virtual ~Relayer() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void RegisterVariables();

 protected:
  unsigned long int m_tally;
  std::string       m_incoming_var;
  std::string       m_outgoing_var;

  double            m_start_time;
};

#endif 
