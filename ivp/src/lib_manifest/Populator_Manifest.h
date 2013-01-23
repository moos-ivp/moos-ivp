/************************************************************/
/*   NAME: Michael Benjamin                                 */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA */
/*   FILE: Populator_Manifest.h                             */
/*   DATE: Dec 14th, 2012                                   */
/************************************************************/
 
#ifndef POPULATOR_MANIFEST_HEADER
#define POPULATOR_MANIFEST_HEADER

#include <string>
#include "Manifest.h"

class Populator_Manifest {
public:
  Populator_Manifest() {};
  virtual ~Populator_Manifest() {};

  bool populate(std::string filename);

  Manifest getManifest() {return(m_manifest);};

 protected:
  Manifest m_manifest;
};

#endif
