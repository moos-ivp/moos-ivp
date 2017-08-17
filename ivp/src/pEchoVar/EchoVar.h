/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: EchoVar.cpp                                          */
/*    DATE: July 22 2006                                         */
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

#ifndef TRANSLATOR_HEADER
#define TRANSLATOR_HEADER

#include <vector>
#include <list>
#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "EFlipper.h"
#include "LogicBuffer.h"

class EchoVar : public AppCastingMOOSApp
{
public:
  EchoVar();
  virtual ~EchoVar() {}
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload
  bool buildReport();

protected:
  void registerVariables();
  bool addMapping(std::string, std::string, bool boolswitch=false);
  bool noCycles();
  bool handleFlipEntry(std::string key, std::string line);
  
  void holdMessage(CMOOSMsg);
  void releaseMessages();

  std::vector<std::string> expand(std::vector<std::string> v);

 protected: // Configuration variables
  bool m_hold_messages_during_pause;
  bool m_echo_latest_only;

 protected: // State variables
  bool m_no_cycles;

  // Index: one for each mapping
  std::vector<std::string>  m_var_source;
  std::vector<std::string>  m_var_target;
  std::vector<bool>         m_boolswitch;

  // map from variable source to num times received mail
  std::map<std::string, unsigned int>  m_map_hits;

  // map from variable source/dest to num times posted
  std::map<std::string, unsigned int>  m_map_posts;

  // Index: one for each unique source. A source may have >1 mappings
  std::vector<std::string>  m_unique_sources;

  std::vector<EFlipper>     m_eflippers;
  std::vector<unsigned int> m_eflip_count;

  std::list<CMOOSMsg>       m_held_messages;

  LogicBuffer  m_logic_buffer;
  bool         m_conditions_met;
};

#endif









