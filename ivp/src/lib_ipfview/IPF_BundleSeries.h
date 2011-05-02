/*****************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                     */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: IPF_BundleSeries.h                                   */
/*    DATE: Apr 19th 2011                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
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
  ~IPF_BundleSeries() {};
  
  void      clear();
  void      addIPF(const std::string&);
  
  QuadSet   getCollectiveQuadSet(unsigned int iter);
  QuadSet   getQuadSet(unsigned int iter, std::string source);

  IvPDomain getDomain(unsigned int iter);
  IvPDomain getDomain(unsigned int iter, std::string source);


  double    getPriority(unsigned int iter, std::string source);

  void      popFront(unsigned int=1);
  void      popBack(unsigned int=1);
  
  void      print() const;
  
  unsigned int getMinIteration() const {return(m_min_iteration);};
  unsigned int getMaxIteration() const {return(m_max_iteration);};
  unsigned int getPieces(unsigned int iter, std::string src);
  unsigned int getTotalFunctions(unsigned int iter);
  unsigned int size() const {return(m_bundles.size());};
  
  std::vector<std::string> getIPFStrings(unsigned int iteration);
  std::vector<std::string> getAllSources() {return(m_all_bhv_sources);};
  
 protected:
  std::map<unsigned int, IPF_Bundle>  m_bundles;
  
  unsigned int m_min_iteration;
  unsigned int m_max_iteration;
  
  std::vector<std::string> m_all_bhv_sources;
};

#endif
