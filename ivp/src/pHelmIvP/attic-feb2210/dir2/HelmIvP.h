/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HelmIvP.h                                            */
/*    DATE: Oct 12th 2004                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef HelmIvP_HEADER
#define HelmIvP_HEADER

#include <string>
#include <set>
#include <map>
#include "MOOSLib.h"
#include "InfoBuffer.h"
#include "IvPDomain.h"
#include "BehaviorSet.h"
#include "HelmEngine.h"
#include "HelmEngineBeta.h"

class HelmIvP : public CMOOSApp
{
public:
  HelmIvP();
  virtual ~HelmIvP();
  
  void cleanup();
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void addBehaviorFile(std::string);
  
protected:
  bool handleDomainEntry(const std::string&);
  bool updateInfoBuffer(CMOOSMsg &Msg);
  void postEngagedStatus();
  void postCharStatus();
  void postBehaviorMessages();
  void postModeMessages();
  void postInitialVariables();
  void postDefaultVariables();
  void registerVariables();
  void registerNewVariables();
  void requestBehaviorLogging();

  void postAllStop();  
  bool processNodeReport(const std::string &);

protected:
  InfoBuffer*   m_info_buffer;
  bool          m_has_control;
  bool          m_allow_overide;
  bool          m_allstop_posted;
  IvPDomain     m_ivp_domain;
  BehaviorSet*  m_bhv_set;
  double        m_last_heartbeat;

  int           m_iteration;
  double        m_curr_time;

  HelmEngineBeta* m_hengine;
  std::string   m_ownship;
  std::vector<std::string> m_node_report_vars;

  // For each decision variable in decision space, note if it is 
  // optional. Optional means a decision need not be rendered on it.
  std::map<std::string, bool> m_optional_var;

  // List of behavior input files. To be fed to Populator. Also sent
  // to the logger so it may record the .bhv files alongside others.
  std::set<std::string> m_bhv_files;

  bool m_curr_time_updated;
};

#endif 
