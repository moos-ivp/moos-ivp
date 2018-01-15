/************************************************************/
/*   NAME: Michael Benjamin                                 */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA */
/*   FILE: ManifestTester.h                                 */
/*   DATE: Jan 9th, 2018                                    */
/************************************************************/
 
#ifndef MANIFEST_TESTER_HEADER
#define MANIFEST_TESTER_HEADER

#include <string>
#include <vector>
#include "ManifestSet.h"

class ManifestTester {
public:
  ManifestTester();
  virtual ~ManifestTester() {};

  void setManifestSet(ManifestSet mset) {m_manifest_set=mset;}
  void setVerbose(bool v)          {m_verbose=v;}
  void setTestWarnings(bool v)     {m_warnings=v;}
  void setTestDepends(bool v)      {m_depends=v;}
  void setTestStats(bool v)        {m_stats=v;}
  void addIgnoreLib(std::string s) {m_ignore_libs.push_back(s);}
  void setIgnoreLibs(std::string);
  
  bool test();

 protected:
  bool testForDuplicates();
  bool testForErrors();
  bool testForWarnings();
  bool testDependencies();
  bool testStatistics();

 protected:
  ManifestSet m_manifest_set;

  bool m_verbose;
  bool m_warnings;
  bool m_depends;
  bool m_stats;

  std::vector<std::string> m_ignore_libs;
};

#endif
