/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ClusterManager.h                                     */
/*    DATE: May 27th 2007                                        */
/*****************************************************************/

#ifndef CLUSTER_MANAGER_HEADER
#define CLUSTER_MANAGER_HEADER

#include "MOOSLib.h"
#include "Migrator.h"

class ClusterManager : public CMOOSApp
{
public:
  ClusterManager();
  virtual ~ClusterManager() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool OnConnectToServer();
  bool OnStartUp();
  bool Iterate();


 protected:
  void postCharStatus();
  void registerVariables();
  void alterMigrateState(bool);
  
protected:
  Migrator     m_migrator;

  bool         m_skews_matter;
  bool         m_ok_skew;
  bool         m_migrate_active;
  std::string  m_verbosity;
  
  double       m_cmg_x;
  double       m_cmg_y;
};

#endif 
