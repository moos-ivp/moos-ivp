/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_BehaviorSet.h                              */
/*    DATE: Jan 27 2005 NICOP funding approved by D.Wagner       */
/*                                                               */
/* (IvPHelm) The IvP autonomous control Helm is a set of         */
/* classes and algorithms for a behavior-based autonomous        */
/* control architecture with IvP action selection.               */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
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


