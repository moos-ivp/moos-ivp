/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BFactoryDynamic.h                                    */
/*    DATE:                                                      */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef BFACTORY_DYNAMIC_HEADER
#define BFACTORY_DYNAMIC_HEADER

#include <string>
#include <map>
#include <vector>

#include "IvPDomain.h"
#include "IvPBehavior.h"

class BFactoryDynamic {
 public:
  BFactoryDynamic() {};
  virtual ~BFactoryDynamic();
  
  // Configuring the domain and loading directories for search.
  void   loadDirectory(std::string dirname);
  void   loadEnvVarDirectories(std::string envVar, bool verbose=false);
  void   setDomain(IvPDomain domain) {m_domain = domain;};

  // Building Behaviors
  bool         isKnownBehavior(std::string bhv_name) const;
  IvPBehavior* newBehavior(std::string name);
  
 private:
  // These describe the signature of the function that creates an IPF
  // object. Each shared library that implements a behavior must have one 
  // of each of these functions, and it must be named "createBehavior".
  typedef IvPBehavior* (*TFuncPtrCreateBehavior) (std::string, IvPDomain);

  std::map<std::string, TFuncPtrCreateBehavior> m_creation_funcs_map;
  
  std::vector<void*> m_open_library_handles;

  IvPDomain m_domain;
};

#endif
