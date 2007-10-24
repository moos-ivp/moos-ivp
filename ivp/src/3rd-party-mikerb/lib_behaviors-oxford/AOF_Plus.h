/*****************************************************************/
/*    NAME: Paul Newman - Michael Benjamin                       */
/*    ORGN: NAVSEA Newport RI and Oxford                         */
/*    FILE: AOF_Plus.h                                           */
/*    DATE: Mar 30, 2007                                         */
/*****************************************************************/

#ifndef AOF_PLUS_HEADER
#define AOF_PLUS_HEADER

#include "IvPDomain.h"
#include "AOF.h"

class AOF_Plus : public AOF
{
public:
  AOF_Plus(){};
  AOF_Plus(IvPDomain);
  ~AOF_Plus();

  void*    m_pUserParam;
  double*  m_dfUserArray;
  int*     m_nIndexArray;
  int      m_nArraySize;

  double (*m_pfnUserCallBack)( double*,int,void*);		
  
  bool SetCallBack(double (*pfn)(double*,int,void*),void * pUserParam);
  
  
  bool SetDimension(int n,const std::string & sName);
  
  double evalBox(const IvPBox* pBox) const;
  
};

#endif
