/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SRS_Model.h                                          */
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

#ifndef SRS_MODEL_HEADER
#define SRS_MODEL_HEADER

#include <vector>
#include <string>
#include <map>
#include "BeaconBuoy.h"
#include "Figlog.h"
#include "VarDataPair.h"
#include "NodeRecord.h"

class SRS_Model
{
 public:
  SRS_Model();
  virtual ~SRS_Model() {};

  bool  setParam(std::string param, std::string value, unsigned int=0);
  bool  handleMsg(std::string key, double dval, std::string sval, 
		  double mtime, bool isdouble, bool isstring, 
		  std::string source);
  void  setCurrTime(double);
  void  setRandomNoiseAlgorithm(const std::string&);
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
  void    postRangeReport(unsigned int bix, double rng, std::string vname);
  void    postBeaconRangePulse(unsigned int bix, std::string color="");

 protected: // Utilities
  double  getTrueBeaconNodeRange(unsigned int bix, unsigned int vix) const;
  double  getNoisyBeaconNodeRange(double) const;
  void    report(std::string message="", std::string termcolor="");

 protected: // State variables
  double       m_curr_time;
  unsigned int m_iterations;

  std::vector<BeaconBuoy>  m_beacons;
  std::vector<NodeRecord>  m_vnodes;
  std::vector<VarDataPair> m_messages;

  std::map<std::string, char> m_vnode_map;

  Figlog   m_figlog;


 protected: // Configuration variables
  double      m_default_node_query_range;  
  double      m_default_node_query_freq;

  double      m_default_beacon_report_range;
  double      m_default_beacon_width;
  std::string m_default_beacon_color;
  std::string m_default_beacon_shape;
  std::string m_default_beacon_freq;

  std::string m_query_payments;

  std::string m_report_vars;
  bool        m_verbose;

  std::string m_rn_algorithm;   // Empty string = no random noise
  double      m_rn_uniform_pct;

};

#endif 

