/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmIvP.h                                            */
/*    DATE: Oct 12th 2004                                        */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/

#ifndef HELMIVP_HEADER
#define HELMIVP_HEADER

#include <string>
#include <set>
#include <map>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "InfoBuffer.h"
#include "IvPDomain.h"
#include "BehaviorSet.h"
#include "HelmEngine.h"

class HelmIvP : public AppCastingMOOSApp
{
public:
  HelmIvP();
  virtual ~HelmIvP();
  
  void cleanup();
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

  bool buildReport();

  bool addBehaviorFile(std::string);
  bool setVerbosity(const std::string&);
  
 protected:
  bool handleConfigNodeSkew(const std::string&);
  bool handleConfigSkewAny(const std::string&);
  bool handleConfigStandBy(const std::string&);
  bool handleConfigDomain(const std::string&);
  
 protected:
  bool handleHeartBeat(const std::string&);
  bool updateInfoBuffer(CMOOSMsg &Msg);
  void postHelmStatus();
  void postCharStatus();
  void postBehaviorMessages();
  void postLifeEvents();
  void postModeMessages();
  void postDefaultVariables();
  void handleHelmStartMessages();
  void handleInitialVarsPhase1();
  void handleInitialVarsPhase2();
  void registerVariables();
  void registerSingleVariable(std::string var, double freq=0.0);
  void registerNewVariables();
  void requestBehaviorLogging();
  void checkForTakeOver();

  bool detectChangeOnKey(const std::string& key, 
			 const std::string& sval);
  bool detectChangeOnKey(const std::string& key, 
			 double dval);
  bool detectRepeatOnKey(const std::string& key);

  void postAllStop(std::string msg="");
  bool processNodeReport(const std::string &);

  std::string helmStatus() const {return(m_helm_status);}
  void        helmStatusUpdate(const std::string& val="");
  bool        helmStatusEnabled() const;

protected:
  InfoBuffer*   m_info_buffer;
  std::string   m_helm_status;   // STANDBY,PARK,DRIVE,DISABLED,MALCONFIG
  bool          m_has_control;

  bool          m_allow_override;
  bool          m_park_on_allstop;
  std::string   m_allstop_msg;
  IvPDomain     m_ivp_domain;
  BehaviorSet*  m_bhv_set;
  std::string   m_verbose;
  bool          m_verbose_reset;
  double        m_last_heartbeat;
  std::string   m_helm_alias;

  // The helm may be configured to be in "standby" mode, waiting for an
  // absence of another helm's heartbeat for "standby_threshold" seconds.
  bool          m_standby_helm;           // config variable
  double        m_standby_threshold;      // config variable
  double        m_standby_last_heartbeat; // state variable

  bool          m_rejournal_requested;
  bool          m_init_vars_ready;
  bool          m_init_vars_done;

  unsigned int  m_no_decisions;

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
  
  unsigned int  m_helm_iteration;
  double        m_ok_skew;
  bool          m_skews_matter;

  HelmReport    m_helm_report;
  HelmReport    m_prev_helm_report;
  HelmEngine*   m_hengine;

  std::string   m_bhvs_active_list;
  std::string   m_bhvs_running_list;
  std::string   m_bhvs_idle_list;

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
  std::map<std::string, std::string> m_outgoing_key_strings;
  std::map<std::string, double>      m_outgoing_key_doubles;

  // Maps for keeping track of when the last time a post happened for
  // a particular variable, and whether or not repeat posts are wanted.
  std::map<std::string, double>       m_outgoing_timestamp;
  std::map<std::string, std::string>  m_outgoing_sval;
  std::map<std::string, double>       m_outgoing_dval;
  std::map<std::string, unsigned int> m_outgoing_iter;
  std::map<std::string, std::string>  m_outgoing_bhv;
  
  std::map<std::string, double>       m_outgoing_repinterval;
  
  std::map<std::string, double>       m_var_reg_time;

  // A flag maintained on each iteration indicating whether the 
  // info_buffer curr_time has yet to be synched to the app curr_time.
  bool m_ibuffer_curr_time_updated;

  // A mapping of vehicle node_report skews  VEHICLE_NAME --> SKEW
  std::map<std::string, double>  m_node_skews;
};
#endif 
