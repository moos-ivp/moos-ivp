/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Populator_BehaviorSet.h                              */
/*    DATE: Jan 27 2005 NICOP funding approved by D.Wagner       */
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

#ifndef POPULATOR_BEHAVIORSET_HEADER
#define POPULATOR_BEHAVIORSET_HEADER

#include <string>
#include <vector>
#include "BehaviorSet.h"
#include "BehaviorSpec.h"
#include "VarDataPair.h"
#include "IvPDomain.h"
#include "IvPBehavior.h"
#include "InfoBuffer.h"
#include "ModeSet.h"
#include "ModeEntry.h"
#include "BFactoryStatic.h"
#include "BFactoryDynamic.h"

class Populator_BehaviorSet {

public:
  Populator_BehaviorSet(IvPDomain, InfoBuffer*);
  virtual ~Populator_BehaviorSet() {}

  void addBehaviorDir(std::string dir) {m_dir_names.push_back(dir);}
  void setBHVDirNotFoundOK(bool v)     {m_bhv_dir_not_found_ok=v;}
  void setOwnship(std::string s)       {m_ownship=s;}

  BehaviorSet* populate(std::set<std::string>);
  BehaviorSet* populate(std::string filename);

  void printBehaviorSpecs() const;

  std::vector<std::string> getConfigWarnings() const {return(m_config_warnings);}

protected:

  bool   handleLine(std::string, std::string, unsigned int line_num);
  void   closeSetMode(); 
  void   addConfigWarning(const std::string&);
  void   addConfigWarnings(std::vector<std::string>);
  void   clearConfigWarnings() {m_config_warnings.clear();}

protected:
  std::vector<VarDataPair>   initial_vars;
  std::vector<VarDataPair>   default_vars;

  std::vector<std::string>   m_dir_names;
  std::vector<std::string>   m_config_warnings;

  std::vector<BehaviorSpec>  m_behavior_specs;
  BehaviorSpec m_curr_bhv_spec;

  IvPDomain    m_domain;
  InfoBuffer*  m_info_buffer;
  ModeSet      m_mode_set;
  ModeEntry    m_mode_entry;
  std::string  m_parse_mode;
  std::string  m_ownship;

  bool         m_bhv_dir_not_found_ok;
};
#endif

