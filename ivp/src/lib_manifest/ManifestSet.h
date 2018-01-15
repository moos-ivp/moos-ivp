/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ManifestSet.h                                        */
/*    DATE: Dec 17th, 2017                                       */
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
  ManifestSet getManifestSetByDependency(std::string key) const;
  ManifestSet getManifestSetByType(std::string key) const;

  std::vector<std::string> getAllAuthors() const;
  std::vector<std::string> getAllAuthorsX() const;
  std::vector<std::string> getAllOrgs() const;
  std::vector<std::string> getAllGroups() const;
  std::vector<std::string> getAllDependencies() const;
  std::vector<std::string> getAllKeyWords() const;
  std::vector<std::string> getAllTypes() const;

  // Getters based on GroupName
  std::vector<std::string> getGroupSynopsis(std::string grpname) const;
  std::string getGroupDocURL(std::string grpname) const;

  // Modifiers
  void orderNewToOld();
  void orderOldToNew();
  void associateDependers();
  void associateLinesOfCode();
  
  // Analysis/Utilities
  unsigned int size() const {return(m_manifests.size());}

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
