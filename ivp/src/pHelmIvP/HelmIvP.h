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
  void postCharStatus();
  void postBehaviorMessages();
  void postInitialVariables();
  void postDefaultVariables();
  void postSummary();
  void registerVariables();
  void registerNewVariables();
  void requestBehaviorLogging();
  void refreshDetectionBuffers(double);

  bool detectChangeOnKey(const std::string& key, 
			 const std::string& sval);
  bool detectChangeOnKey(const std::string& key, 
			 double dval);
  
  
protected:
  InfoBuffer*   m_info_buffer;
  bool          m_has_control;
  bool          m_allow_overide;
  IvPDomain     m_ivp_domain;
  BehaviorSet*  m_bhv_set;
  std::string   m_verbose;

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

  HelmEngine*   m_hengine;
  std::string   m_ownship;

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

};

#endif 









