/*****************************************************************/
/*    NAME: Paul Newman - Michael Benjamin                       */
/*    ORGN: NAVSEA Newport RI and Oxford                         */
/*    FILE: IvPBehaviorPlus.cpp                                  */
/*    DATE: Mar 30, 2007                                         */
/*****************************************************************/

#include "AOF_Plus.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

AOF_Plus::AOF_Plus(IvPDomain gdomain) : AOF(gdomain)
{
  m_nArraySize      = gdomain.size();
  m_dfUserArray     = new double[m_nArraySize];
  m_nIndexArray     = new int[m_nArraySize];
  m_pUserParam      = NULL;
  m_pfnUserCallBack = NULL;
}

//-----------------------------------------------------------
// Procedure: Destructor

AOF_Plus::~AOF_Plus()
{
  if(m_dfUserArray)
    delete [] m_dfUserArray;

  if(m_nIndexArray)
    delete [] m_nIndexArray;
}

  
//-----------------------------------------------------------
// Procedure: setCallBack

bool AOF_Plus::SetCallBack(double (*pfn)(double*,int,void*), 
			   void * pUserParam)
{
  m_pUserParam = pUserParam;
  m_pfnUserCallBack  = pfn;
  return true;
}
  
  
//-----------------------------------------------------------
// Procedure: setDimension

bool AOF_Plus::SetDimension(int n, const std::string& sName)
{
  if(n >= m_nArraySize)
    return(false);
  
  int index = m_domain.getIndex(sName);

  if(index < 0)
    return(false);
  
  m_nIndexArray[index] = n;
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: evalBox

double AOF_Plus::evalBox(const IvPBox* pBox) const
{
  if(m_pfnUserCallBack != 0) {

    //do index->action space look up
    for(int i=0; i<m_domain.size(); i++) {
      double dfT;

      if(m_domain.getVal(i,pBox->pt(i,0), dfT)) {		
	int kUser = m_nIndexArray[i];
	m_dfUserArray[kUser] = dfT;
      }
      else {
	return(0);
      }
    }
    return(m_pfnUserCallBack(m_dfUserArray, m_nArraySize, m_pUserParam));
  }
  else
    return(0);
}

