/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PokeDB.h                                             */
/*    DATE: May 9th 2008                                         */
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

#include <string>
#include "MOOS/libMOOS/MOOSLib.h"

class PokeDB : public CMOOSApp  
{
 public:
  PokeDB(std::string g_server, long int g_port); 
  PokeDB();

  virtual ~PokeDB() {};
  
  bool Iterate();
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool OnConnectToServer();
  bool OnStartUp();

  void setConfigureCommsLocally(bool v) {m_configure_comms_locally=v;};

  void setPokeDouble(const std::string& varname, const std::string& val);
  void setPokeString(const std::string& varname, const std::string& val);

 protected:
  void registerVariables();
  void updateVariable(CMOOSMsg& msg);
  void printReport();

  bool ConfigureComms();

 protected: // Index for each is unique per variable name
  std::vector<std::string>  m_varname;
  std::vector<std::string>  m_valtype;
  std::vector<std::string>  m_varvalue;
  std::vector<std::string>  m_svalue_read;
  std::vector<std::string>  m_dvalue_read;
  std::vector<std::string>  m_source_read;
  std::vector<std::string>  m_valtype_read;
  std::vector<std::string>  m_wrtime_read;
  std::vector<bool>         m_varname_recd;

  double m_db_start_time;
  int    m_iteration;
  bool   m_configure_comms_locally;
};


