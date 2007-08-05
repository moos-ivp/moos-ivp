/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IvPBehaviorPlus.cpp                                  */
/*    DATE: Mar 30, 2007                                         */
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


