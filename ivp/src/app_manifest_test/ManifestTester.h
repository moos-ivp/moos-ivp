/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ManifestTester.h                                     */
/*    DATE: Jan 9th, 2018                                        */
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

