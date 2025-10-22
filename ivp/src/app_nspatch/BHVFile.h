/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHVFile.h                                            */
/*    DATE: July 24th, 2025                                      */
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
 
#ifndef BHV_FILE_HEADER
#define BHV_FILE_HEADER

#include <string>
#include <vector>
#include <map>

class BHVFile {
public:
  BHVFile();
  virtual ~BHVFile() {};

public: // building from input file
  std::string addLine(std::string bhvname, std::string line);

  void addPatchLine(std::string bhvname, std::string patch_line);
  void addInitLine(std::string init_line);
  void addModeLine(std::string mode_line);

public: // applying patches
  void applyBlock(std::string bhvname, std::vector<std::string>);
  void applyLine(std::string bhvname, std::string line);
  void applyInitLine(std::string init_line);
  void applyModeLines(std::vector<std::string> mode_lines);

  BHVFile applyToStemFile(BHVFile);

public: // getters
  std::vector<std::string> getLines();

  void print();

protected: // utility
  std::string whitePad(std::string);
  
 protected: 

  std::string m_curr_block;
  
  // Ordered vector of "initialize MOOSVar=value" lines
  std::vector<std::string> m_init_lines;

  // Ordered vector of "mode" lines
  std::vector<std::string> m_mode_lines;

  // Keyed on bhvname, or "global" for global params
  std::map<std::string, std::vector<std::string> > m_blocks;

  // Keyed on bhvname, or "global" for global params
  std::map<std::string, std::vector<std::string> > m_patch_lines;
};

#endif
