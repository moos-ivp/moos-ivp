/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: InfoBuffer.H                                         */
/*    DATE: Dec 29th 2024                                        */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef LEDGER_SNAP_HEADER
#define LEDGER_SNAP_HEADER

#include <string>
#include <vector>
#include <map>

class LedgerSnap {
public:
  LedgerSnap() {m_curr_time_utc=0;}
  ~LedgerSnap() {}

public:

  void setX(std::string vname, double dval);
  void setY(std::string vname, double dval);
  void setHdg(std::string vname, double dval);
  void setSpd(std::string vname, double dval);
  void setDep(std::string vname, double dval);
  void setLat(std::string vname, double dval);
  void setLon(std::string vname, double dval);
  void setUTC(std::string vname, double dval);
  void setUTCAge(std::string vname, double dval);
  void setUTCReceived(std::string vname, double dval);
  void setUTCAgeReceived(std::string vname, double dval);

  void setGroup(std::string vname, std::string sval);
  void setType(std::string vname, std::string sval);

  void setCurrTimeUTC(double v) {m_curr_time_utc=v;}
  
  double getInfoDouble(std::string vname,
		       std::string field,
		       bool& ok) const;

  std::string getInfoString(std::string vname,
			    std::string field,
			    bool& ok) const;

  void clear();
  unsigned int size() const;

public: // debugging
  std::string getSpec() const;
  std::string getSpec(std::string vname) const;
    
protected:
  std::map<std::string, double> m_map_xpos;
  std::map<std::string, double> m_map_ypos;
  std::map<std::string, double> m_map_hdg;
  std::map<std::string, double> m_map_spd;
  std::map<std::string, double> m_map_dep;
  std::map<std::string, double> m_map_lat;
  std::map<std::string, double> m_map_lon;
  std::map<std::string, double> m_map_utc;
  std::map<std::string, double> m_map_utc_age;
  std::map<std::string, double> m_map_utc_received;
  std::map<std::string, double> m_map_utc_age_received;

  std::map<std::string, std::string> m_map_group;
  std::map<std::string, std::string> m_map_type;

  double m_curr_time_utc;
};
#endif











