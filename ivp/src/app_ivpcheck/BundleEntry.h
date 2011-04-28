/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: BundleEntry.h                                       */
/*    DATE: Apr 19th 2011                                       */
/****************************************************************/

#ifndef BUNDLE_ENTRY_HEADER
#define BUNDLE_ENTRY_HEADER

#include <string>
#include "QuadSet.h"

class BundleEntry
{
public:
  BundleEntry(const std::string&);
  ~BundleEntry() {};
  
  QuadSet       getQuadSet(IvPDomain);
  IvPFunction*  getIvPFunction();

  std::string   getIPFString()  {return(m_ipf_str);};
  unsigned int  getPieces()     {return(m_pieces);};
  double        getPriority()   {return(m_priority);};

 protected:
  std::string    m_ipf_str;
  unsigned int   m_pieces;
  double         m_priority;
};

#endif
