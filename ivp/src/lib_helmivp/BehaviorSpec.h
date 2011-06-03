/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BehaviorSpec.h                                       */
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

#ifndef BEHAVIOR_SPEC_HEADER
#define BEHAVIOR_SPEC_HEADER

#include <string>
#include <vector>
#include "InfoBuffer.h"
#include "IvPBehavior.h"

class BehaviorSpec {
 public:
  BehaviorSpec();
  ~BehaviorSpec() {};
  
 public: // Setting Parameters 
  void setBehaviorKind(std::string, unsigned int line_num);
  void addBehaviorConfig(std::string, unsigned int line_num);

  std::vector<std::string>  checkForSpawningStrings();

  void setInfoBuffer(InfoBuffer *b) {m_info_buffer=b;};
  bool setTemplatingType(std::string s);
  void clear();
  void print();
    
 public: // Getting Parameters 
  unsigned int  size()              {return(m_config_lines.size());};
  std::string   getTemplatingType() {return(m_templating);};
  bool          templating()        {return(m_templating_enabled);};
  std::string   getKind()           {return(m_behavior_kind);};
  unsigned int  getKindLine()       {return(m_kind_line_num);};
  std::string   getUpdatesVar()     {return(m_updates_var);};
  
  std::string   getConfigLine(unsigned int);
  unsigned int  getConfigLineNum(unsigned int);

 private: // State Variables
  std::string   m_templating;
  std::string   m_updates_var;

  std::string   m_behavior_kind;
  unsigned int  m_kind_line_num;

  std::vector<std::string>   m_config_lines;
  std::vector<unsigned int>  m_config_line_num;

  InfoBuffer   *m_info_buffer;
  bool          m_templating_enabled;
};

#endif

