/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: SpoofNode.h                                     */
/*    DATE: May 16th, 2022                                  */
/************************************************************/

#ifndef SPOOF_NODE_HEADER
#define SPOOF_NODE_HEADER

#include <map>
#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "NodeRecord.h"

class SpoofNode : public AppCastingMOOSApp
{
 public:
  SpoofNode();
  ~SpoofNode() {};
  
protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();

 protected:
  bool handleConfigDefaultVType(std::string);
  bool handleSpoofRequest(std::string);
  bool handleMailSpoofCancel(std::string);

  void advancePositions(double);
  void postNodeReports();

  void checkForExpiredNodes();

 protected:
  void registerVariables();

 private: // Configuration variables

  double m_refresh_interval;

  std::string m_default_vtype;
  std::string m_default_group;
  std::string m_default_color;
  std::string m_default_vsource;

  double m_default_hdg;
  double m_default_spd;
  double m_default_duration; // Default 0 means no time limit
  
  double m_default_length;
  
 private: // State variables

  double m_last_posting;

  unsigned int m_total_postings;
  unsigned int m_total_spoof_reqs;
  
  std::map<std::string, NodeRecord> m_map_node_records;
  std::map<std::string, double> m_map_node_durations;
  std::map<std::string, double> m_map_spoof_start;
};

#endif 
