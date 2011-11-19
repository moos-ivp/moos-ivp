/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BFactoryDynamic.h                                    */
/*                                                               */
/* (IvPHelm) The IvP autonomous control Helm is a set of         */
/* classes and algorithms for a behavior-based autonomous        */
/* control architecture with IvP action selection.               */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/

#ifndef BFACTORY_DYNAMIC_HEADER
#define BFACTORY_DYNAMIC_HEADER

#include <string>
#include <map>
#include <set>
#include <vector>

#include "IvPDomain.h"
#include "IvPBehavior.h"

class BFactoryDynamic {
 public:
  BFactoryDynamic() {};
  virtual ~BFactoryDynamic();
  
  // Configuring the domain and loading directories for search.
  void   loadDirectory(std::string dirname);
  void   loadEnvVarDirectories(std::string envVar);
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

  // A map from bhv_name to directory. Two behaviors of the same name
  // should not be loaded dynamically. Dangerously ambiguous.
  std::map<std::string, std::string> m_map_bhv_dir;

  std::set<std::string> m_loaded_dirs;

  IvPDomain m_domain;
};

#endif

