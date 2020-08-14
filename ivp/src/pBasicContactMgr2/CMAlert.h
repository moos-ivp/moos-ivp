/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CMAlert.h                                            */
/*    DATE: Mar 26th 2014                                        */
/*    DATE: Oct 7th 2017   major mods mikerb                     */
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

class CMAlert
{
 public:
  CMAlert();
  virtual ~CMAlert() {}

 public: // Setters
  bool setAlertRange(double);
  bool setAlertRangeFar(double);

  bool setAlertRegion(std::string);
  bool addAlertOnFlag(std::string);
  bool addAlertOffFlag(std::string);

  bool addMatchType(std::string);
  bool addIgnoreType(std::string);
  
  bool addMatchGroup(std::string);
  bool addIgnoreGroup(std::string);
  
  bool setAlertRangeColor(std::string);
  bool setAlertRangeFarColor(std::string);

 public: // Getters
  double    getAlertRange() const    {return(m_range);}
  double    getAlertRangeFar() const {return(m_range_far);}
  bool      hasAlertRegion() const   {return(m_region.is_convex());}
  XYPolygon getAlertRegion() const   {return(m_region);}
  
  bool hasAlertOnFlag() const {return(m_on_flags.size() > 0);}
  bool hasAlertOffFlag() const {return(m_off_flags.size() > 0);}

  std::vector<VarDataPair> getAlertOnFlags() const;
  std::vector<VarDataPair> getAlertOffFlags() const;

  std::vector<std::string> getMatchTypes() const {return(m_match_type);}
  std::vector<std::string> getIgnoreTypes() const {return(m_ignore_type);}
  
  std::vector<std::string> getMatchGroups() const {return(m_match_group);}
  std::vector<std::string> getIgnoreGroups() const {return(m_ignore_group);}
  
  std::string getAlertRangeColor() const   {return(m_rng_color);}
  std::string getAlertRangeFarColor() const {return(m_rng_far_color);}

 private:
  double      m_range;
  double      m_range_far;
  XYPolygon   m_region;

  std::vector<std::string> m_match_type;
  std::vector<std::string> m_ignore_type;
  
  std::vector<std::string> m_match_group;
  std::vector<std::string> m_ignore_group;
  
  std::vector<VarDataPair> m_on_flags;
  std::vector<VarDataPair> m_off_flags;
  
  std::string m_rng_color;
  std::string m_rng_far_color;  

};

#endif 








