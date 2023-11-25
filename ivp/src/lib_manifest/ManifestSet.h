/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ManifestSet.h                                        */
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

#ifndef MANIFEST_SET_HEADER
#define MANIFEST_SET_HEADER

#include <string>
#include <vector>
#include <map>
#include "Manifest.h"

class ManifestSet
{
 public:
  ManifestSet();
  virtual ~ManifestSet() {}

  // Setters
  bool      addManifest(Manifest manifest);
  void      setLinesOfCode(std::string, unsigned int);
  void      setFilesOfCode(std::string, unsigned int);
  void      setWorkYears(std::string, double);
  
  // Getters
  Manifest  getManifestByModule(std::string mod_name) const;
  Manifest  getManifestByIndex(unsigned int index) const;

  ManifestSet getManifestSetByKey(std::string key) const;
  ManifestSet getManifestSetByAuthor(std::string key) const;
  ManifestSet getManifestSetByOrg(std::string key) const;
  ManifestSet getManifestSetByGroup(std::string key) const;
  ManifestSet getManifestSetByDistro(std::string key) const;
  ManifestSet getManifestSetByDependency(std::string key) const;
  ManifestSet getManifestSetByType(std::string key) const;

  std::vector<std::string> getAllAuthors() const;
  std::vector<std::string> getAllAuthorsX() const;
  std::vector<std::string> getAllOrgs() const;
  std::vector<std::string> getAllGroups() const;
  std::vector<std::string> getAllDistros() const;
  std::vector<std::string> getAllDependencies() const;
  std::vector<std::string> getAllKeyWords() const;
  std::vector<std::string> getAllTypes() const;

  // Getters based on GroupName
  std::vector<std::string> getGroupSynopsis(std::string grpname) const;
  std::string getGroupDocURL(std::string grpname) const;
  std::string getGroupDistro(std::string grpname) const;
  std::string getGroupDownload(std::string grpname) const;

  // Modifiers
  void orderNewToOld();
  void orderOldToNew();
  void associateDependers();
  void associateLinesOfCode();
  
  // Analysis/Utilities
  unsigned int size() const;

  unsigned int getAllLinesOfCode() const;
  unsigned int getAllFilesOfCode() const;
  double       getAllWorkYears() const;
  bool         containsLibrary(std::string) const;
  
  void print() const;
  void printTerse() const;
  
 protected: // Key properties
  std::vector<Manifest>  m_manifests;

  std::map<std::string, unsigned int> m_map_loc;
  std::map<std::string, unsigned int> m_map_foc;  
  std::map<std::string, double>       m_map_wyr;  
};

#endif 

