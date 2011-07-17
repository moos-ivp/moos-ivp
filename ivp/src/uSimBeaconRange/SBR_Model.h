/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SBR_Model.h                                          */
/*    DATE: Feb 1st, 2011                                        */
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

#ifndef SIM_BEACON_RANGE_MODEL_HEADER
#define SIM_BEACON_RANGE_MODEL_HEADER

#include <vector>
#include <string>
#include <map>
#include "BeaconBuoy.h"
#include "Figlog.h"
#include "VarDataPair.h"
#include "NodeRecord.h"

class SBR_Model
{
 public:
  SBR_Model();
  virtual ~SBR_Model() {};

  bool  setParam(std::string param, std::string value, unsigned int=0);
  bool  handleMsg(std::string key, double dval, std::string sval, 
		  double mtime, bool isdouble, bool isstring, 
		  std::string source);
  void  setCurrTime(double);
  bool  verbose() {return(m_verbose);};
  void  iterate();
  void  print() const;

  std::vector<VarDataPair> getMessages(bool clear=true);
  std::vector<VarDataPair> getVisuals();

  Figlog getFigLog() {return(m_figlog);};

 protected: // Configuration utility
  bool    addBeaconBuoy(const std::string&);

 protected: // Incoming mail utility
  bool    handleNodeReport(const std::string&);
  bool    handleRangeRequest(const std::string&);

 protected: // Outgoing mail utility
  void    addMessage(const std::string&, const std::string&);
  void    addMessage(const std::string&, double);
  void    handleBeaconReport(unsigned int bix);
  void    postBeaconRangeReport(unsigned int bix, double rng, 
				std::string rec_node);
  void    postBeaconRangePulse(unsigned int bix, std::string color="");

  void    postRangePulse(double x, double y, std::string color,
			 std::string label, double dur, double radius);
 protected: // Utilities
  double  getTrueBeaconNodeRange(unsigned int bix, unsigned int vix) const;
  double  getNoisyBeaconNodeRange(double) const;
  void    report(std::string message="", std::string termcolor="");
  bool    updateNodeRecords(NodeRecord);
  bool    setReachDistance(std::string);
  bool    setPingWait(std::string);
  bool    setPingPayments(std::string);
  bool    setReportVars(std::string);
  bool    setRandomNoiseAlgorithm(std::string);

 protected: // State variables
  double       m_curr_time;
  unsigned int m_iterations;

  // Vector of N beacons 
  std::vector<BeaconBuoy>  m_beacons;

  // Vector of M contacts, i.e., Nodes
  std::vector<NodeRecord>  m_node_records;

  // Messages to be grabbed by the MOOSApp for posting to MOOSDB
  std::vector<VarDataPair> m_messages;

  Figlog   m_figlog;

 protected: // Configuration variables
  double      m_default_reach_dist;  
  double      m_default_ping_wait;

  std::map<std::string, double> m_reach_map;
  std::map<std::string, double> m_pingw_map;
  
  BeaconBuoy  m_default_beacon_buoy;

  std::string m_ping_payments;

  std::string m_report_vars;
  bool        m_verbose;
  bool        m_ground_truth;

  std::string m_rn_algorithm;   // Empty string = no random noise
  double      m_rn_uniform_pct;

};

#endif 

