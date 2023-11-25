/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: IPF_BundleSeries.h                                   */
/*    DATE: Apr 19th 2011                                        */
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

#ifndef IPF_BUNDLE_SERIES_HEADER
#define IPF_BUNDLE_SERIES_HEADER

#include <vector>
#include <string>
#include <map>
#include "IPF_Bundle.h"

// Contains function bundles indexed by iteration

class IPF_BundleSeries
{
public:
  IPF_BundleSeries();
  ~IPF_BundleSeries() {}
  
  void      clear();
  void      addIPF(const std::string&);
  
  QuadSet   getCollectiveQuadSet(unsigned int iter, 
				 std::string ctype="");
  QuadSet   getQuadSet(unsigned int iter, std::string source,
		       bool dense=false);

  IvPDomain getDomain(unsigned int iter);
  IvPDomain getDomain(unsigned int iter, std::string source);


  double    getPriority(unsigned int iter, std::string source);

  void      popFront(unsigned int=1);
  void      popBack(unsigned int=1);
  
  void      print() const;
  
  unsigned int getMinIteration() const {return(m_min_iteration);}
  unsigned int getMaxIteration() const {return(m_max_iteration);}
  unsigned int getPieces(unsigned int iter, std::string src);
  unsigned int getTotalFunctions(unsigned int iter);
  unsigned int size() const {return(m_bundles.size());}
  
  std::vector<std::string> getIPFStrings(unsigned int iteration);
  std::vector<std::string> getAllSources() {return(m_all_bhv_sources);}
  std::string  getFirstSource() const;
  
 protected:
  std::map<unsigned int, IPF_Bundle>  m_bundles;
  
  unsigned int m_min_iteration;
  unsigned int m_max_iteration;
  
  std::vector<std::string> m_all_bhv_sources;
};

#endif









