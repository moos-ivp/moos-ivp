/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SpecBuild.h                                          */
/*    DATE: Feb 11, 2010                                         */
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

#ifndef SPEC_BUILD_HEADER
#define SPEC_BUILD_HEADER

#include <string>
#include <vector>
#include "IvPBehavior.h"

class SpecBuild {
 public:
  SpecBuild() {m_behavior=0;}
  ~SpecBuild() {}

 public: // Setters
  void setIvPBehavior(IvPBehavior *bhv)   {m_behavior=bhv;}
  void setKindResult(std::string str)     {m_bhv_kind_result=str;}

  void setBehaviorKind(std::string bhv_kind, unsigned int lnum);
  void addBadConfig(std::string line, unsigned int lnum);
  void deleteBehavior();

 public: // Getters
  bool          valid()           {return(m_behavior != 0);}
  IvPBehavior*  getIvPBehavior()  {return(m_behavior);}
  
  std::string   getBehaviorName();
  std::string   getBehaviorKind() {return(m_bhv_kind);}
  std::string   getKindResult()   {return(m_bhv_kind_result);}
  std::string   getBadConfigLine(unsigned int ix);

  unsigned int  getKindLine()     {return(m_bhv_kind_lnum);}
  unsigned int  numBadConfigs()   {return(m_bad_config_lines.size());}
  unsigned int  getBadConfigLineNum(unsigned int ix);

  std::vector<VarDataPair> getHelmStartMessages();

  void print();

 private: // State Variables
  IvPBehavior  *m_behavior;

  std::string   m_bhv_kind;  
  std::string   m_bhv_kind_result;  // "failed", "static", "dynamic"
  unsigned int  m_bhv_kind_lnum; 

  std::vector<std::string>   m_bad_config_lines;
  std::vector<unsigned int>  m_bad_config_lnums;
};

#endif










