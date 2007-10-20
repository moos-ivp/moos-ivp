#include "WayPointVW.h"
#include "BuildUtils.h"
#include "OF_Reflector.h"
#include <MOOSGenLib/MOOSGenLibGlobalHelper.h>
#include <math.h>

CWayPointVW::CWayPointVW(IvPDomain HelmDomain):IvPBehavior(HelmDomain)
{
  domain = subDomain(HelmDomain,"v,w");
  setParam("unifbox","v=3,w=3");
  setParam("gridbox","v=6,w=6");

}




bool CWayPointVW::GetDynamic(const std::string & sName,std::string & sVal)
{
  bool bParam;
  std::string sT = info_buffer->sQuery(sName, bParam);
  if(bParam)
    sVal = sT;
  
  return bParam;
  
  
}
IvPFunction* CWayPointVW::produceOF()
{
  std::string sPose;
#if(0)
  if(!GetDynamic(m_sPoseInput,sPose))
    return NULL;
#endif
  std::vector<double> V(3,0);
  
#if(0)
  int nRows,nCols;
  if(!MOOSValFromString(V,nRows,nCols,sPose,"pose"))
    return NULL;
#endif
  
  MyAOF TheAOF(domain);
  TheAOF.SetParams(V[0],V[1],V[2],0,10);
  OF_Reflector Reflector((const AOF*)&TheAOF);
  
  Reflector.createUniform(unif_box,grid_box);
  IvPFunction* ipf = Reflector.extractOF();
  
  return ipf;
	
	
}

bool CWayPointVW::setParam(std::string sToken, std::string sVal)
{
  if(BASE::setParamCommon(sToken,sVal))
    return true;
  
  if(MOOSStrCmp("PoseInput",sToken))
    {
      m_sPoseInput = sVal;
      info_vars.push_back(m_sPoseInput);
    }
  
  return true;
}


CWayPointVW::MyAOF::MyAOF(IvPDomain D):AOF(D)
{
  m_nVidx  = D.getIndex("v");
  m_nWidx  = D.getIndex("w");
  
}

bool CWayPointVW::MyAOF::SetParams(double dfX,double dfY,double dfTheta,double dfGoalX,double dfGoalY)
{
  m_dfX = dfX;
  m_dfY = dfY;
  m_dfTheta = dfTheta;
  m_dfGoalX = dfGoalX;
  m_dfGoalY = dfGoalY;
  
  return true;
}



double CWayPointVW::MyAOF::evalBox(const IvPBox* pBox) const
{
  
  double dfW;
  double dfV;
  
  m_domain.getVal(m_nVidx,pBox->pt(m_nVidx,0),dfV);
  m_domain.getVal(m_nWidx,pBox->pt(m_nWidx,0),dfW);
  
  return 1+1/(hypot(dfV-1,dfW)+0.1);
}



