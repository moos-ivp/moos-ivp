/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PokeDB.h                                             */
/*    DATE: May 9th 2008                                         */
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

#include <string>
#include "MOOS/libMOOS/MOOSLib.h"
#include "VarDataPair.h"

class PokeDB : public CMOOSApp  
{
 public:
  PokeDB(std::string g_server, long int g_port); 
  PokeDB();

  virtual ~PokeDB() {}
  
  bool Iterate();
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool OnConnectToServer();
  bool OnStartUp();

  void setConfigureCommsLocally(bool v) {m_configure_comms_locally=v;}

  bool setPoke(std::string pokestr);
  void setPokeDouble(std::string varname, std::string val);
  void setPokeString(std::string varname, std::string val);
  void setTimeWarp();
  void setUseCache(bool v=true) {m_use_cache=v;}
  
 protected:
  void registerVariables();
  void updateVariable(CMOOSMsg& msg);
  void printReport();

  bool anyVarsReceived() const;
  bool allVarsReceived() const;
  void clearVarsReceived();
  
  bool ConfigureComms();
  void postFlags(const std::vector<VarDataPair>& flags);
  
 protected: // Index for each is unique per variable name
  std::vector<std::string>  m_varname;
  std::vector<std::string>  m_valtype;
  std::vector<std::string>  m_varvalue;
  std::vector<std::string>  m_svalue_read;
  std::vector<std::string>  m_dvalue_read;
  std::vector<std::string>  m_source_read;
  std::vector<std::string>  m_valtype_read;
  std::vector<double>       m_wrtime_read;
  std::vector<bool>         m_varname_recd;

  double m_db_time;

  double m_db_start_time;
  int    m_iteration;
  bool   m_configure_comms_locally;

  bool   m_time_warp_set;
  bool   m_values_poked;
  bool   m_priors_reported;
  bool   m_poked_reported;

  bool   m_use_cache;
  
  unsigned int m_poke_iteration;
};

