/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BehaviorReport.h                                     */
/*    DATE: May 10th 2011                                        */
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

#ifndef BEHAVIOR_REPORT_HEADER
#define BEHAVIOR_REPORT_HEADER

#include <string>
#include <vector>
#include "IvPFunction.h"

class BehaviorReport {
public:
  BehaviorReport();
  BehaviorReport(std::string bhv_name, unsigned int iteration=0);
  virtual ~BehaviorReport() {}

  // Setting Context information relevant to each IvPFunction
  void  setBehaviorName(std::string name)   {m_bhv_name  = name;}
  void  setHelmIteration(unsigned int iter) {m_iteration = iter;}

  // Populating the report with IvP Functions
  void  addIPF(IvPFunction*, std::string="");
  void  setPriority(double pwt);

  // Post-processing and error checking of IvP functions
  void  makeKeysUnique();
  bool  checkForNans();
  void  setIPFStrings();
  
  // Member functions for getting information
  unsigned int size() const    {return(m_ipf.size());}
  bool         isEmpty() const {return(m_ipf.empty());}

  IvPFunction *getIPF(std::string key) const;
  IvPFunction *getIPF(unsigned int index) const;
  std::string  getKey(unsigned int index) const;
  std::string  getIPFString(unsigned int index) const;
  bool         hasIPFString(unsigned int index) const;
  bool         hasUniqueKey(unsigned int index) const;
  double       getAvgPieces() const;
  std::string  getBHVName() const   {return(m_bhv_name);}
  unsigned int getIteration() const {return(m_iteration);}
  double       getPriority() const  {return(m_priority);}

 protected:
  std::vector<std::string>  m_key;
  std::vector<IvPFunction*> m_ipf;
  std::vector<std::string>  m_ipf_string;
  std::vector<bool>         m_key_unique;

  std::string               m_bhv_name;
  unsigned int              m_iteration;
  double                    m_priority;
};

#endif 










