/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SIMAS_Model.h                                        */
/*    DATE: June 25th, 2011                                      */
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

#ifndef SIMAS_MODEL_HEADER
#define SIMAS_MODEL_HEADER

#include <vector>
#include <string>
#include <map>
#include "VarDataPair.h"
#include "NodeRecord.h"

class SIMAS_Model
{
 public:
  SIMAS_Model();
  virtual ~SIMAS_Model() {};

  bool  setParam(std::string param, std::string value);
  bool  handleMsg(std::string key, double dval, std::string sval, 
		  double mtime, bool isdouble, bool isstring, 
		  std::string source);
  void  setCurrTime(double);
  bool  verbose() {return(m_verbose);};
  void  iterate();
  void  print() const;

  std::vector<VarDataPair> getMessages(bool clear=true);

 protected: // Incoming mail utility
  bool    handleNodeReport(const std::string&);
  bool    handleRangeRequest(const std::string&);

 protected: // Outgoing mail utility
  void    addMessage(const std::string&, const std::string&);
  void    addMessage(const std::string&, double);
  void    postNodeRangeReport(unsigned int, unsigned int, double rng); 
  void    postRangePulse(unsigned int tix, std::string color,
			 std::string label, double dur, double radius);

 protected: // Utilities
  double  getTrueNodeNodeRange(unsigned int bix, unsigned int tix) const;
  double  getNoisyNodeNodeRange(double true_range) const;
  void    report(std::string message="", std::string termcolor="");
  bool    updateNodeRecords(const NodeRecord&);
  bool    setReachDistance(std::string);
  bool    setReplyDistance(std::string);
  bool    setPingWait(std::string);
  bool    setReportVars(std::string);
  bool    setRandomNoiseAlgorithm(std::string);

  void    printNodeRecords();

 protected: // State variables
  double       m_curr_time;
  unsigned int m_iterations;

  // Record of all known contacts
  std::vector<NodeRecord>  m_node_records; 

  std::vector<VarDataPair> m_messages;

 protected: // Configuration variables
  double      m_default_reach_dist;  
  double      m_default_reply_dist;  
  double      m_default_ping_wait;

  std::map<std::string, double> m_reach_map;
  std::map<std::string, double> m_reply_map;
  std::map<std::string, double> m_pingw_map;
  
  std::string m_ping_color;
  std::string m_reply_color;
  std::string m_report_vars;
  bool        m_verbose;
  bool        m_ground_truth;

  std::string m_rn_algorithm;   // Empty string = no random noise
  double      m_rn_uniform_pct;

};

#endif 

