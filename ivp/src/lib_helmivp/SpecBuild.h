/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SpecBuild.h                                          */
/*    DATE: Feb 11, 2010                                         */
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

#ifndef SPEC_BUILD_HEADER
#define SPEC_BUILD_HEADER

#include <string>
#include <vector>
#include "IvPBehavior.h"

class SpecBuild {
 public:
  SpecBuild() {m_behavior=0;};
  ~SpecBuild() {};

 public: // Setting Parameters
  void setIvPBehavior(IvPBehavior *bhv)   {m_behavior=bhv;};
  void setKindResult(std::string str)     {m_bhv_kind_result=str;};

  void setBehaviorKind(std::string bhv_kind, unsigned int lnum);
  void addBadConfig(std::string line, unsigned int lnum);
  void deleteBehavior();

 public: // Getting Parameters
  bool          valid()           {return(m_behavior != 0);};
  IvPBehavior*  getIvPBehavior()  {return(m_behavior);};
  
  std::string   getBehaviorName();
  std::string   getBehaviorKind() {return(m_bhv_kind);};
  std::string   getKindResult()   {return(m_bhv_kind_result);};
  std::string   getBadConfigLine(unsigned int ix);

  unsigned int  getKindLine()     {return(m_bhv_kind_lnum);};
  unsigned int  numBadConfigs()   {return(m_bad_config_lines.size());};
  unsigned int  getBadConfigLineNum(unsigned int ix);

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

