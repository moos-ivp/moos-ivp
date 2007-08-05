/*****************************************************************/
/*    NAME: Paul Newman - Michael Benjamin                       */
/*    ORGN: NAVSEA Newport RI and Oxford                         */
/*    FILE: IvPBehaviorPlus.cpp                                  */
/*    DATE: Mar 30, 2007                                         */
/*****************************************************************/

#include "IvPBehaviorPlus.h"
#include <math.h>
#include "OF_Reflector.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

IvPBehaviorPlus::IvPBehaviorPlus(IvPDomain g_domain)
  : IvPBehavior(g_domain)
{
  m_pAOF  = new AOF_Plus(g_domain);
  m_pAOF->SetCallBack(_EvalCB,this);
}


//-----------------------------------------------------------
// Procedure: 

bool IvPBehaviorPlus::setParam(std::string s1, std::string s2)
{
  return setParamCommon(s1,s2);
}
  
//-----------------------------------------------------------
// Procedure: 

bool IvPBehaviorPlus::SetDimension(int n, const std::string &sName)
{
  return m_pAOF->SetDimension(n,sName);
}
  
//-----------------------------------------------------------
// Procedure: 

IvPFunction *IvPBehaviorPlus::produceOF()	
{
  if(!OnPreEvaluate())
    return NULL;
  
  OF_Reflector Reflector((const AOF*)m_pAOF);
  
  Reflector.createUniform(unif_box,grid_box);
  IvPFunction* ipf = Reflector.extractOF();  
}
  
//-----------------------------------------------------------
// Procedure: 

bool IvPBehaviorPlus::SetSubDomain(std::string sDomain)
{
  domain = subDomain(domain,sDomain);
  return true;
}

