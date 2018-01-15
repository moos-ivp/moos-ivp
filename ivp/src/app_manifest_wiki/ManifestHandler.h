/************************************************************/
/*   NAME: Michael Benjamin                                 */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA */
/*   FILE: ManifestHandler.h                                */
/*   DATE: Dec 17th, 2017                                   */
/************************************************************/
 
#ifndef MANIFEST_HANDLER_HEADER
#define MANIFEST_HANDLER_HEADER

#include <string>
#include "ManifestSet.h"

class ManifestHandler {
public:
  ManifestHandler() {m_verbose=false;}
  virtual ~ManifestHandler() {};

  void setManifestSet(ManifestSet mset) {m_manifest_set=mset;}
  void setVerbose(bool v) {m_verbose=v;}
  
  void genAllManifestPages();
  void genGroupManifestPages();

  bool genManifestModulePage(Manifest);
  bool genManifestGroupPage(std::string group_name);

 protected:
  std::string applyLinks(std::string);
  std::string whiteSpace(unsigned int pixels, std::string wiki_base="moos-ivp");
  
 protected:
  ManifestSet m_manifest_set;

  bool m_verbose;
};

#endif
