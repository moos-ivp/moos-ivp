/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CMAlert.h                                            */
/*    DATE: March 26th 2014                                      */
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

class Alert
{
 public:
  CMAlert();
  virtual ~CMAlert() {}

  void setAlertVarName(std::string s)      {m_alert_varname=s;}
  void setAlertPattern(std::string s)      {m_alert_pattern=s;}
  void setAlertRngColor(std::string s)     {m_alert_rng_color=s;}
  void setAlertRngCPAColor(std::string s)  {m_alert_rng_cpa_color=s;}
  void setAlertRng(double v);              {m_alert_rng=v;}
  void setAlertRngCPA(double v);           {m_alert_rng_cpa=v;}

  std::string getAlertVarName()     {return(m_alert_varname);}
  std::string getAlertVarPattern()  {return(m_alert_pattern);}
  std::string getAlertRngColor()    {return(m_alert_rng_color);}
  std::string getAlertRngCPAColor() {return(m_alert_rng_cpa_color);}

  double getAlertRng()    {return(m_alert_rng);}
  double getAlertRngCPA() {return(m_alert_rng_cpa);}

 private:
  std::string m_alert_varname;
  std::string m_alert_pattern;
  std::string m_alert_rng_color;
  std::string m_alert_rng_cpa_color;

  double      m_alert_rng;
  double      m_alert_rng_cpa;
};

#endif 




