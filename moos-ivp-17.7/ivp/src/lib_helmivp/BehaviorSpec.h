/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BehaviorSpec.h                                       */
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

#ifndef BEHAVIOR_SPEC_HEADER
#define BEHAVIOR_SPEC_HEADER

#include <string>
#include <vector>
#include "InfoBuffer.h"
#include "IvPBehavior.h"

class BehaviorSpec {
 public:
  BehaviorSpec();
  ~BehaviorSpec() {}
  
 public: // Setting Parameters 
  void setBehaviorKind(std::string, unsigned int line_num);
  void addBehaviorConfig(std::string, unsigned int line_num);


  std::vector<std::string>  checkForSpawningStrings();

  void setFileName(std::string filename)  {m_filename = filename;}
  void setInfoBuffer(InfoBuffer *b)       {m_info_buffer=b;}

  bool setTemplatingType(std::string s);

  void spawnTried() {m_spawns_tried++;}
  void spawnMade()  {m_spawns_made++;}
  
  void clear();
  void print() const;
    
 public: // Getting Parameters 
  unsigned int  size() const              {return(m_config_lines.size());}
  std::string   getTemplatingType() const {return(m_templating);}
  bool          templating() const        {return(m_templating_enabled);}
  std::string   getKind() const           {return(m_behavior_kind);}
  unsigned int  getKindLine() const       {return(m_kind_line_num);}
  std::string   getUpdatesVar() const     {return(m_updates_var);}
  std::string   getFileName() const       {return(m_filename);}
  std::string   getNamePrefix() const     {return(m_name_prefix);}

  std::string   getConfigLine(unsigned int) const;
  unsigned int  getConfigLineNum(unsigned int) const;

  unsigned int  getSpawnsMade() const  {return(m_spawns_made);}
  unsigned int  getSpawnsTried() const {return(m_spawns_tried);}
  
 private: // State Variables
  std::string   m_templating;
  std::string   m_updates_var;
  std::string   m_name_prefix;

  std::string   m_filename;
  std::string   m_behavior_kind;
  unsigned int  m_kind_line_num;

  std::vector<std::string>   m_config_lines;
  std::vector<unsigned int>  m_config_line_num;

  unsigned int  m_spawns_tried;
  unsigned int  m_spawns_made;
    
  InfoBuffer   *m_info_buffer;
  bool          m_templating_enabled;
};

#endif




