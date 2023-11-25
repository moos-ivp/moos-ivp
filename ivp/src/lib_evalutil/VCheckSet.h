/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: VCheck.h                                             */
/*    DATE: Oct 3rd, 2020                                        */
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

#ifndef V_CHECK_SET_HEADER
#define V_CHECK_SET_HEADER

#include <vector>
#include <string>
#include <list> 
#include <set>
#include "VCheck.h"
#include "VarDataPair.h"

class VCheckSet
{
 public:
  VCheckSet();
  ~VCheckSet() {};
  
  bool addVCheck(std::string);
  bool setStartTrigger(std::string);
  bool setFailOnFirst(std::string);
  bool setMaxReportSize(std::string);
  std::string checkConfig() const;
  
  void update(double curr_time);

  bool enabled() const;
  bool isEvaluated() const;
  bool isSatisfied() const;

  unsigned int getTotalPassed() const {return(m_total_passed);}
  unsigned int getTotalFailed() const {return(m_total_failed);}
  double       getPassRate() const;
  std::string  getFailReport() const {return(m_fail_report);}
  std::string  getStatus() const;

  void handleMail(std::string, std::string, double, double);

  std::set<std::string> getVars() const {return(m_vcheck_vars);}

  void augFailReport(std::string, double, double, double, double);
  void augFailReport(VCheck, double, double);
  
  std::list<std::string> getReport() const;

  std::set<std::string> getInfoBufferReport() const;
  

  
 protected:
  void handleMailKey(std::string, std::string, double, double);  
  void handleMailAll(std::string, std::string, double, double);
  
  
 private: // Config vars
  std::vector<VCheck> m_vchecks;
  std::set<std::string> m_vcheck_vars;
  
  VarDataPair  m_start_trigger;
  bool         m_fail_on_first;
  unsigned int m_max_report_size;
  
 private: // State vars
  double  m_start_time;
  double  m_elapsed_time;

  unsigned int m_total_passed;
  unsigned int m_total_failed;

  std::string m_fail_report;
};

#endif 
 

