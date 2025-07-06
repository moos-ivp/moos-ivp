/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: MissionFile.h                                        */
/*    DATE: May 7th, 2025                                        */
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
 
#ifndef MOOS_FILE_HEADER
#define MOOS_FILE_HEADER

#include <string>
#include <vector>
#include <map>

class MOOSFile {
public:
  MOOSFile();
  virtual ~MOOSFile() {};

public: // building from input file
  void addLine(std::string app, std::string line);
  void addPatchLine(std::string app, std::string patch_line);

public: // applying patches
  void applyBlock(std::string app, std::vector<std::string>);
  void applyLine(std::string app, std::string line);

  MOOSFile applyToStemFile(MOOSFile);

public: // getters
  std::vector<std::string> getLines();

  void print();
  void print2();

protected: // utility
  std::string whitePad(std::string);
  
 protected: 

  std::string m_curr_block;
  
  // Keyed on appname, or "global" for global params
  std::map<std::string, std::vector<std::string> > m_blocks;

  // Keyed on appname, or "global" for global params
  std::map<std::string, std::vector<std::string> > m_patch_lines;
};

#endif
