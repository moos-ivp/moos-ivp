/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: FldNodeComms.h                                       */
/*   DATE: Dec 4th 2011                                         */
/****************************************************************/

#ifndef FLD_NODE_COMMS_HEADER
#define FLD_NODE_COMMS_HEADER

#include <map>
#include <string>
#include "MOOSLib.h"
#include "NodeRecord.h"

class FldNodeComms : public CMOOSApp
{
 public:
  FldNodeComms();
  virtual ~FldNodeComms() {};

  // MOOSApp overloaded virtual functions
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected:
  void registerVariables();
  void checkForDeletableNodes();
  bool handleNodeReport(const std::string& str);
  bool handleStealth(const std::string&);
  bool handleEarange(const std::string&);

  void distributeNodeInfo(const std::string& uname);
  
  bool meetsRangeThresh(const std::string& v1, const std::string& v2);
  bool meetsCriticalRangeThresh(const std::string&, const std::string&);
  void postViewCommsPulse(const std::string& v1, const std::string& v2);


 protected: // Configuration variables

  bool    m_verbose;
  bool    m_apply_groups;

  // Default range to source threshold for vehicle to receive
  // node report from a source vehicle.
  double  m_comms_range;
  double  m_critical_range;
  double  m_default_stealth;
  double  m_default_earange;
  double  m_min_stealth;
  double  m_max_earange;

  // Seconds after which a vehicle does not received node reports 
  // unless a node report has been received from it.
  double  m_stale_time;
  double  m_delete_time;

  std::map<std::string, double>     m_map_stealth;
  std::map<std::string, double>     m_map_earange;

 protected: // State variables
  std::map<std::string, NodeRecord>   m_map_records;   // key:vname
  std::map<std::string, bool>         m_map_newinfo;   // key:vname
  std::map<std::string, double>       m_map_timestamp; // key:vname
  std::map<std::string, unsigned int> m_map_vindex;    // key:vname
  std::map<std::string, std::string>  m_map_vgroup;    // key:vname

  std::vector<std::string> m_colors;
  double                   m_curr_time;
};

#endif 
