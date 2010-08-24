/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_BehaviorSet.h                              */
/*    DATE: Jan 27 2005 NICOP funding approved by D.Wagner       */
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

#ifndef POPULATOR_BEHAVIORSET_HEADER
#define POPULATOR_BEHAVIORSET_HEADER

#include <string>
#include <set>
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
  virtual ~Populator_BehaviorSet() {};

  BehaviorSet* populate(std::set<std::string>);
  BehaviorSet* populate(std::string filename);

  void loadEnvVarDirectories(std::string envVar, bool verbose) {
    m_bfactory_dynamic.loadEnvVarDirectories(envVar, verbose);
  }

  void printBehaviorSpecs();

protected:
  bool handleLine(std::string, unsigned int line_num);
  void closeSetMode(); 

protected:
  std::vector<VarDataPair>   initial_vars;
  std::vector<VarDataPair>   default_vars;

  std::vector<BehaviorSpec>  m_behavior_specs;
  BehaviorSpec m_curr_bhv_spec;

  IvPDomain    m_domain;
  InfoBuffer*  m_info_buffer;
  ModeSet      m_mode_set;
  ModeEntry    m_mode_entry;
  std::string  m_parse_mode;

  BFactoryStatic  m_bfactory_static;
  BFactoryDynamic m_bfactory_dynamic;
};
#endif

