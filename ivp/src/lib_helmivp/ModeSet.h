/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ModeSet.h                                            */
/*    DATE: Dec 26th, 2008                                       */
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

#ifndef MODE_SET_HEADER
#define MODE_SET_HEADER

#include <vector>
#include <string>
#include "ModeEntry.h"
#include "InfoBuffer.h"
#include "VarDataPair.h"

class ModeSet {
public:
  ModeSet() {m_info_buffer=0;};
  ~ModeSet() {};

  void addEntry(ModeEntry entry) {m_entries.push_back(entry);};

  void setInfoBuffer(InfoBuffer *b) {m_info_buffer = b;};
  
  unsigned int size() {return(m_entries.size());};
  
  void evaluate();
  void print();

  std::vector<VarDataPair> getVarDataPairs() 
    {return(m_mode_var_data_pairs);};
  std::string getModeSummary();

  std::vector<std::string> getConditionVars();

  std::string getStringDescription();

 protected:
  void consultFromInfoBuffer();
  void updateInfoBuffer();

protected:
  std::vector<ModeEntry>    m_entries;
  std::vector<VarDataPair>  m_mode_var_data_pairs;

  InfoBuffer *m_info_buffer;
};

#endif

