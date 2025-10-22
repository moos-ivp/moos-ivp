/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PatchApplicator.h                                    */
/*    DATE: July 4th, 2025                                       */
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

#ifndef PATCH_APPLICATOR_HEADER
#define PATCH_APPLICATOR_HEADER

#include <vector>
#include <string>

class PatchApplicator
{
 public:
  PatchApplicator();
  ~PatchApplicator() {}

  bool addXMoosFile(std::string); // patch file
  bool setStemMoosFile(std::string);
  bool setTargMoosFile(std::string);
  
  bool addXBhvFile(std::string); // patch file
  bool setStemBhvFile(std::string);
  bool setTargBhvFile(std::string);
  
  bool applyPatch();
  void setVerbose() {m_verbose=true;}
  
 protected:
  bool applyPatchMoos();
  bool applyPatchBhv();
  void printConfig();
  
 private:
  std::vector<std::string> m_files_xmoos; //patch files
  std::string              m_file_stem_moos;
  std::string              m_file_targ_moos;

  std::vector<std::string> m_files_xbhv; // patch files
  std::string              m_file_stem_bhv;
  std::string              m_file_targ_bhv;

  bool m_verbose;
};

#endif 

