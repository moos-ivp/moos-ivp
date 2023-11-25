/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Populator_Manifest.h                                 */
/*    DATE: Dec 17th, 2017                                       */
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
 
#ifndef POPULATOR_MANIFEST_SET_HEADER
#define POPULATOR_MANIFEST_SET_HEADER

#include <string>
#include "ManifestSet.h"

class Populator_ManifestSet {
public:
  Populator_ManifestSet();
  virtual ~Populator_ManifestSet() {};

  void setVerbose(bool v)             {m_verbose=v;}
  void addManifestFile(std::string s) {m_manifest_files.push_back(s);}
  void addLOCFile(std::string s)      {m_loc_files.push_back(s);}
  void setExitOnFailure(bool v)       {m_exit_on_failure=v;}
  
  bool populate();

  ManifestSet getManifestSet() {return(m_manifest_set);};

 protected:
  bool populateManifest(std::string filename);
  bool populateLOC(std::string filename);

  std::string getLineKey(std::string) const;
  bool        validLineKey(std::string) const;
  
 protected:
  ManifestSet m_manifest_set;
  Manifest    m_curr_manifest;

  std::vector<std::string> m_manifest_files;
  std::vector<std::string> m_loc_files;

  bool m_verbose;
  bool m_exit_on_failure;
  
};

#endif

