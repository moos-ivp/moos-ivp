/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CMAlert.h                                            */
/*    DATE: Mar 26th 2014                                        */
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

#ifndef CONTACT_MANAGER_ALERT_HEADER
#define CONTACT_MANAGER_ALERT_HEADER

#include <string>
#include "XYPolygon.h"
#include "VarDataPair.h"
#include "ExFilterSet.h"

class CMAlert
{
 public:
  CMAlert();
  virtual ~CMAlert() {}

 public: // Setters
  bool setAlertRange(double);
  bool setAlertRangeFar(double);

  bool setAlertRange(std::string);
  bool setAlertRangeFar(std::string);

  bool addAlertOnFlag(std::string);
  bool addAlertOffFlag(std::string);

  bool setAlertSource(std::string);
  
  bool configFilter(std::string, std::string);

  bool filterCheck(NodeRecord) const;
  bool filterCheck(NodeRecord, double osx, double osy) const;
  
 public: // Getters
  double    getAlertRange() const    {return(m_range);}
  double    getAlertRangeFar() const {return(m_range_far);}

  bool valid() const;
  
  bool hasAlertOnFlag() const {return(m_on_flags.size() > 0);}
  bool hasAlertOffFlag() const {return(m_off_flags.size() > 0);}

  std::vector<VarDataPair> getAlertOnFlags() const;
  std::vector<VarDataPair> getAlertOffFlags() const;

  std::vector<std::string> getSummary() const;
  
 private:
  double      m_range;
  double      m_range_far;

  ExFilterSet m_filter_set; 

  std::string m_alert_source;
  
  std::vector<VarDataPair> m_on_flags;
  std::vector<VarDataPair> m_off_flags;

  std::set<std::string> m_on_flags_raw;
  std::set<std::string> m_off_flags_raw;
};

#endif 

