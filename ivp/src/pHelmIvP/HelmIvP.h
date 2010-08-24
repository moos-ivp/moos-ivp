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
  void postLifeEvents();
  void postModeMessages();
  void postInitialVariables();
  void postDefaultVariables();
  void registerVariables();
  void registerNewVariables();
  void requestBehaviorLogging();

  bool detectChangeOnKey(const std::string& key, 
			 const std::string& sval);
  bool detectChangeOnKey(const std::string& key, 
			 double dval);
  bool detectRepeatOnKey(const std::string& key);

  void postAllStop(std::string msg="");  
  bool processNodeReport(const std::string &);

protected:
  InfoBuffer*   m_info_buffer;
  bool          m_has_control;
  bool          m_allow_override;
  bool          m_disengage_on_allstop;
  std::string   m_allstop_msg;
  IvPDomain     m_ivp_domain;
  BehaviorSet*  m_bhv_set;
  std::string   m_verbose;
  double        m_last_heartbeat;

  // The refresh vars handle the occasional clearing of the m_outgoing
  // maps. These maps will be cleared when MOOS mail is received for the
  // variable given by m_refresh_var. The user can set minimum interval
  // between refreshes so the helm retains some control over refresh rate.
  // Motivated by the need for a viewer handling geometric postings from
  // behaviors. The new arrival of a viewer into the MOOS community can 
  // request a refresh and then get new geometry mail to process.
  std::string   m_refresh_var;
  bool          m_refresh_pending;
  double        m_refresh_time;
  double        m_refresh_interval;
  
  int           m_iteration;
  double        m_ok_skew;
  bool          m_skews_matter;
  int           m_warning_count;
  double        m_curr_time;
  double        m_start_time;

  bool          m_use_beta_engine;
  HelmEngine*   m_hengine;
  HelmEngineBeta* m_hengine_beta;
  std::string   m_ownship;
  std::vector<std::string> m_node_report_vars;

  // An additional MOOS variable other than MOOS_MANUAL_OVERRIDE 
  // that may be used for overriding the helm.
  std::string   m_additional_override;

  // For each decision variable in decision space, note if it is 
  // optional. Optional means a decision need not be rendered on it.
  std::map<std::string, bool> m_optional_var;

  // List of behavior input files. To be fed to Populator. Also sent
  // to the logger so it may record the .bhv files alongside others.
  std::set<std::string> m_bhv_files;

  // Maps for keeping track of the previous outgoing behavior postings
  // for comparison on current posting. Possibly supress if they match
  std::map<std::string, std::string> m_outgoing_strings;
  std::map<std::string, double>      m_outgoing_doubles;

  // Maps for keeping track of when the last time a post happened for
  // a particular variable, and whether or not repeat posts are wanted.
  std::map<std::string, double>      m_outgoing_timestamp;
  std::map<std::string, double>      m_outgoing_repinterval;
  
  // A flag maintained on each iteration indicating whether curr_time
  // has yet to be updated.
  bool m_curr_time_updated;
};

#endif 
