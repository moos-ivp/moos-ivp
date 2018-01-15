/************************************************************/
/*   NAME: Michael Benjamin                                 */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA */
/*   FILE: Populator_Manifest.h                             */
/*   DATE: Dec 17th, 2017                                   */
/************************************************************/
 
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
