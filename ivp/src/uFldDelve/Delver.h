/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: Delver.h                                        */
/*    DATE: Sep 2nd, 2022                                   */
/************************************************************/

#ifndef UFLD_DELVER_HEADER
#define UFLD_DELVER_HEADER

#include <string>
#include <map>
#include <set>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "TransRecord.h"

class Delver : public AppCastingMOOSApp
{
 public:
  Delver();
  ~Delver() {};
  
protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
  void registerVariables();

protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();
  void handleMailNodePShareVars(std::string);

  void handleMailQBridgeVars(std::string);
  void handleMailBridgeVars(std::string);

  void handleMailPosting(const CMOOSMsg&, bool shore);

  void updateCurrTime();
  
  void calcTotalRates();
  
  double getTotalCharRateShore() const {return(m_total_char_rate_shore);}
  double getTotalCharRateNode() const  {return(m_total_char_rate_node);}
  double getTotalCharRate() const      {return(m_total_char_rate);}

  double getTotalMsgRateShore() const {return(m_total_msg_rate_shore);}
  double getTotalMsgRateNode() const  {return(m_total_msg_rate_node);}
  double getTotalMsgRate() const      {return(m_total_msg_rate);}

  double getMaxTotalMsgRateShore() const {return(m_max_total_msg_rate_shore);}
  double getMaxTotalMsgRateNode() const  {return(m_max_total_msg_rate_node);}
  double getMaxTotalMsgRate() const      {return(m_max_total_msg_rate);}

  
private: // Configuration variables

  double m_rate_frame;
  
private: // State variables

  unsigned int m_node_count;

  double m_total_char_rate;
  double m_total_char_rate_node;
  double m_total_char_rate_shore;
  
  double m_total_msg_rate;
  double m_total_msg_rate_node;
  double m_total_msg_rate_shore;
  
  double m_max_total_msg_rate;
  double m_max_total_msg_rate_node;
  double m_max_total_msg_rate_shore;
  
  std::map<std::string, TransRecord> m_map_node_recs;
  std::map<std::string, TransRecord> m_map_shore_recs;

  std::set<std::string> m_set_qbridge_vars;
};

#endif 
