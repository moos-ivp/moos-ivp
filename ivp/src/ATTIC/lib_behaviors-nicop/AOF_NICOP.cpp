/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI - MIT Dept. Mechanical Engineering */
/*    FILE: AOF_NICOP.cpp                                        */
/*    DATE: Dec 8th 2005                                         */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <math.h> 
#include <assert.h>
#include <newmat.h>
#include "AOF_NICOP.h"
#include "AngleUtils.h"

//using namespace std;
using namespace NEWMAT;

//----------------------------------------------------------
// Procedure: Constructor

AOF_NICOP::AOF_NICOP(IvPDomain g_domain, double g_os_x,
		     double g_os_y, double g_bc_x, double g_bc_y)
  : AOF(g_domain)
{
  int crs_ix = m_domain.getIndex("course");
  int spd_ix = m_domain.getIndex("speed");
  int tol_ix = m_domain.getIndex("tol");
  
  osX = g_os_x;
  osY = g_os_y;

  assert(crs_ix != -1);
  assert(spd_ix != -1);
  assert(tol_ix != -1);
  
  crsDelta = m_domain.getVarDelta(crs_ix);
  crsBase  = m_domain.getVarLow(crs_ix);

  spdDelta = m_domain.getVarDelta(spd_ix);
  spdBase  = m_domain.getVarLow(spd_ix);
  spdHigh  = m_domain.getVarHigh(spd_ix);

  tolDelta = m_domain.getVarDelta(tol_ix);
  tolBase  = m_domain.getVarLow(tol_ix);
}

//----------------------------------------------------------------
// Procedure: evalBox

void Trace(Matrix & M,const std::string & sName)
{
  cout<<sName.c_str()<<":"<<endl;
  for(int i = 1;i<=M.Nrows();i++)
    {
      for(int j = 1;j<=M.Ncols();j++)
	{
	  cout<<M(i,j)<<" ";
	}
      cout<<endl;
    }
}

double AOF_NICOP::evalBox(const IvPBox *b) const
{
  double evalCRS  = crsBase + ((double)(b->pt(0,0)) * crsDelta);
  double evalSPD  = spdBase + ((double)(b->pt(1,0)) * spdDelta);
  double evalTOL  = tolBase + ((double)(b->pt(2,0)) * tolDelta);
  

  //input decision space
  double dfV = evalSPD;
  double dfT = evalTOL;
  double dfA = -degToRadians(evalCRS - 90);
  
  double dfXv = osX;
  double dfYv = osY;
  
  double dfSensePeriod = 1.0;
  
  double dfFx = 50;
  double dfFy = 100;

  
  
  double dfI11=0;
  double dfI22=0;
  double dfI21=0;
  for (double dft = 0; dft <= dfT; dft += dfSensePeriod)
    {
      double dfX = dfXv+dfV*dft*cos(dfA);
      double dfY = dfYv+dfV*dft*sin(dfA);

      double dfDX = dfX-dfFx;
      double dfDY = dfY-dfFy;
      double dfR = hypot(dfDX,dfDY);
      
      double dfJ1 = -dfDX/dfR;
      double dfJ2 = -dfDY/dfR;

      dfI11+=dfJ1*dfJ1;
      dfI22+=dfJ2*dfJ2;
      dfI21+=dfJ2*dfJ1;
     
    }


  double dfI = dfI22*dfI11-dfI21*dfI21;

  if((dfI<=0) && (dfV > 0))
    cout << "evalCRS: " << evalCRS << "   dfA:" << dfA << "   PaulCRS:" << -(evalCRS-90) << endl;

  double mval =dfI >0 ?  metric(log(dfI)): -100;
  mval/=dfT>0 ? dfT :1;

  return(mval);
}

//----------------------------------------------------------------
// Procedure: metric
//                                                   

double AOF_NICOP::metric(double foo) const
{
  return(1.0 * foo);
}


