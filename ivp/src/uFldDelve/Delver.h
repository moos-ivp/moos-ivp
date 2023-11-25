/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Delver.h                                             */
/*    DATE: Sep 2nd, 2022                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef UFLD_DELVER_HEADER
#define UFLD_DELVER_HEADER

#include <string>
#include <map>
#include <set>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "TransRecord.h"

class Delver : public AppCastingMOOSApp
{
 public:
  Delver();
  ~Delver() {};
  
protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
  void registerVariables();

protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();
  void handleMailNodePShareVars(std::string);

  void handleMailQBridgeVars(std::string);
  void handleMailBridgeVars(std::string);

  void handleMailPosting(const CMOOSMsg&, bool shore);

  void updateCurrTime();
  
  void calcTotalRates();
  
  double getTotalCharRateShore() const {return(m_total_char_rate_shore);}
  double getTotalCharRateNode() const  {return(m_total_char_rate_node);}
  double getTotalCharRate() const      {return(m_total_char_rate);}

  double getTotalMsgRateShore() const {return(m_total_msg_rate_shore);}
  double getTotalMsgRateNode() const  {return(m_total_msg_rate_node);}
  double getTotalMsgRate() const      {return(m_total_msg_rate);}

  double getMaxTotalMsgRateShore() const {return(m_max_total_msg_rate_shore);}
  double getMaxTotalMsgRateNode() const  {return(m_max_total_msg_rate_node);}
  double getMaxTotalMsgRate() const      {return(m_max_total_msg_rate);}

  
private: // Configuration variables

  double m_rate_frame;
  
private: // State variables

  unsigned int m_node_count;

  double m_total_char_rate;
  double m_total_char_rate_node;
  double m_total_char_rate_shore;
  
  double m_total_msg_rate;
  double m_total_msg_rate_node;
  double m_total_msg_rate_shore;
  
  double m_max_total_msg_rate;
  double m_max_total_msg_rate_node;
  double m_max_total_msg_rate_shore;
  
  std::map<std::string, TransRecord> m_map_node_recs;
  std::map<std::string, TransRecord> m_map_shore_recs;

  std::set<std::string> m_set_qbridge_vars;
};

#endif 

