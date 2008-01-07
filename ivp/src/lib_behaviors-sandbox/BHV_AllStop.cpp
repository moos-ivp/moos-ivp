/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_AllStop.cpp                                      */
/*    DATE: May 26th 2007                                        */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include "BHV_AllStop.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_AllStop::BHV_AllStop(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_allstop");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_AllStop::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  return(false);
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_AllStop::onRunState() 
{
  int dsize = m_domain.size();

  if(dsize == 0) 
    return(0);

  ZAIC_PEAK zaic(m_domain, m_domain.getVarName(0));
  zaic.setSummit(0);

  IvPFunction *ipf = zaic.extractOF();
  
  OF_Coupler coupler;
  for(int i=1; i<dsize; i++) {
    ZAIC_PEAK zaic(m_domain, m_domain.getVarName(i));
    zaic.setSummit(0);
    IvPFunction *new_ipf = zaic.extractOF();
    ipf = coupler.couple(ipf, new_ipf);
  }
  return(ipf);
}






