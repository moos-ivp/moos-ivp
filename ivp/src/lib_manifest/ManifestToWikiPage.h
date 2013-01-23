/************************************************************/
/*   NAME: Michael Benjamin                                 */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA */
/*   FILE: ManifestToWikiPage.h                             */
/*   DATE: Dec 17th, 2012                                   */
/************************************************************/
 
#ifndef MANIFEST_TO_WIKIPAGE_HEADER
#define MANIFEST_TO_WIKIPAGE_HEADER

#include <string>
#include "Manifest.h"

class ManifestToWikiPage {
public:
  ManifestToWikiPage() {};
  virtual ~ManifestToWikiPage() {};

  void setManifest(Manifest manifest) {m_manifest = manifest;};

  bool generate_wikilines();

 protected:
  Manifest m_manifest;
  
};

#endif
