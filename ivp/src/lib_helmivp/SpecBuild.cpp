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

#include <iostream>
#include "SpecBuild.h"

using namespace std;

//--------------------------------------------------------------
// Procedure: setBehaviorKind()

void SpecBuild::setBehaviorKind(string bhv_kind, unsigned int lnum)
{
  m_bhv_kind = bhv_kind;
  m_bhv_kind_lnum = lnum;
}

//--------------------------------------------------------------
// Procedure: addBadConfig()

void SpecBuild::addBadConfig(string line, unsigned int lnum) 
{
  m_bad_config_lines.push_back(line);
  m_bad_config_lnums.push_back(lnum);
}

//--------------------------------------------------------------
// Procedure: deleteBehavior()

void SpecBuild::deleteBehavior() 
{
  if(m_behavior) 
    delete(m_behavior);
  m_behavior = 0;
}

//--------------------------------------------------------------
// Procedure: getBehaviorName()

string SpecBuild::getBehaviorName() 
{
  if(m_behavior)
    return(m_behavior->getDescriptor());
  return("");
}

//--------------------------------------------------------------
// Procedure: getBadConfigLine()

string SpecBuild::getBadConfigLine(unsigned int ix) 
{
  if(ix < m_bad_config_lines.size())
    return(m_bad_config_lines[ix]);
  return("");
}
  
//--------------------------------------------------------------
// Procedure: getBadConfigLineNum()

unsigned int  SpecBuild::getBadConfigLineNum(unsigned int ix) 
{
  if(ix < m_bad_config_lnums.size())
    return(m_bad_config_lnums[ix]);
  return(0);
}

//--------------------------------------------------------------
// Procedure: print()

void SpecBuild::print()
{
  cout << "------------------------------------" << endl;
  cout << "SpecBuild Report on BHV_Kind: " << m_bhv_kind << endl;
  cout << "  valid: " << valid() << endl;
  cout << "  bhv_kind_result: " << m_bhv_kind_result << endl;
  cout << "  # of bad config lines: " << m_bad_config_lines.size();
  unsigned int i, isize = m_bad_config_lines.size(); 
  for(i=0; i<isize; i++) {
    cout << "[" << i << "]: Line #" << m_bad_config_lnums[i] << endl;
    cout << "[" << i << "]:" << m_bad_config_lines[i] << endl;
  }
}

