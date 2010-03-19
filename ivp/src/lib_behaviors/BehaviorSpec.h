/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BehaviorSpec.h                                       */
/*    DATE: Feb 11, 2010                                         */
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
