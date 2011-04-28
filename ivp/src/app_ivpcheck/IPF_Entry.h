/****************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                    */
/*    ORGN: MIT Cambridge MA                                    */
/*    FILE: IPF_Entry.h                                         */
/*    DATE: Apr 19th 2011                                       */
/****************************************************************/

#ifndef BUNDLE_ENTRY_HEADER
#define BUNDLE_ENTRY_HEADER

#include <string>
#include "QuadSet.h"

class IPF_Entry
{
public:
  IPF_Entry(const std::string&);
  ~IPF_Entry() {};
  
  QuadSet       getQuadSet(IvPDomain);
  IvPFunction*  getIvPFunction();

  std::string   getIPFString() const  {return(m_ipf_str);};
  unsigned int  getPieces() const     {return(m_pieces);};
  double        getPriority() const   {return(m_priority);};

 protected:
  std::string    m_ipf_str;
  unsigned int   m_pieces;
  double         m_priority;

  QuadSet        m_quadset;
};

#endif
