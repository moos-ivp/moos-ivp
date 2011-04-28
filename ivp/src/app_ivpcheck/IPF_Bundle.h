/*****************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                     */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: IPF_Bundle.h                                         */
/*    DATE: Apr 19th 2011                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef FUNCTION_BUNDLE_HEADER
#define FUNCTION_BUNDLE_HEADER

#include <string>
#include "QuadSet.h"
#include "IPF_Entry.h"

// Contains all the functions (IPF_Entries) for a given iteration

class IPF_Bundle
{
public:
  IPF_Bundle() {};
  ~IPF_Bundle() {};
  
  void addIPF(const std::string&);
  
  QuadSet getCollectiveQuadSet();
  QuadSet getQuadSet(unsigned int);
  QuadSet getQuadSet(std::string source);

  double       getPriority(std::string source) const;
  unsigned int getPieces(std::string source) const;
  unsigned int size() const   {return(m_entries.size());};

  std::string getSource(unsigned int);
  std::string getIPFString(unsigned int);

  std::vector<std::string> getIPFStrings();
  std::vector<std::string> getSources() const {return(m_sources);};
  
 protected: // both vectors keyed on a single IPF string
  std::vector<IPF_Entry>   m_entries;  
  std::vector<std::string> m_sources;  // source behavior name 

  unsigned int  m_iteration;
  IvPDomain     m_ivp_domain;
};

#endif
