/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: AOF_llaborativeTracking.cpp                    */
/*    BORN: 23 July 2007                                   */
/***********************************************************/
#include <iostream>
#include <math.h> 
#include <assert.h>
#include "AOF_CollaborativeTracking.h"
#include "AngleUtils.h"
#include "IvPDomain.h"

using namespace std;

/******************************************************************************
 *                                                                            *
 *  Method:AOF_CollaborativeTracking(IvPDomain g_domain): AOF(g_domain)       *
 *                                                                            *
 *  Description: Class constructor 
 *                                                                            * 
 ******************************************************************************/

AOF_CollaborativeTracking::AOF_CollaborativeTracking(IvPDomain g_domain): AOF(g_domain)
{
  int crs_ix = g_domain.getIndex("course");
  
  assert(crs_ix != -1);
  
  crsDelta = g_domain.getVarDelta(crs_ix);
  crsBase  = g_domain.getVarLow(crs_ix);

}

/******************************************************************************
 *                                                                            *
 *  Method: setParam(const string& param, double param_val)                   *
 *                                                                            *
 *  Description: This method is called by BHV_CollaborativeTracking and sets  *
 *  the needed parameters.                                                    * 
 *                                                                            * 
 ******************************************************************************/

bool AOF_CollaborativeTracking::setParam(const string& param, double param_val)
{
  if(param == "osX") {
    osx = param_val;
    return(true);
  }
  else if(param == "osY") {
    osy = param_val;
    return(true);
  }  
   else if(param == "target_x") {
    targx = param_val;
    return(true);
  }
  else if(param == "target_y") {
    targy = param_val;
    return(true);
  }
  else if(param == "cs_x") {
    csx = param_val;
    return(true);
  }
  else if(param == "cs_y") {
    csy = param_val;
    return(true);
  }
  else if(param == "sigma_pos") {
    sigmapos = param_val;
    return(true);
  }
  else if(param == "sigma_theta") {
    sigmatheta = param_val;
    return(true);
  }
  else
    return(false);
  
}

/******************************************************************************
 *                                                                            *
 *  Method: AOF_CollaborativeTracking::evalBox(const IvPBox *b) const         *
 *                                                                            *
 *  Description: This method is called to evaluate a proposed course and      *
 *  returns a metric which corresponds to the utility of the proposed course. * 
 *                                                                            * 
 ******************************************************************************/ 

double AOF_CollaborativeTracking::evalBox(const IvPBox *b) const
{
 
  double evalCRS  = crsBase + ((double)(b->pt(0,0)) * crsDelta);
  
  //look 1 meter in each direction
  double osxp = osx + sin(evalCRS*M_PI/180.0);
  double osyp = osy + cos(evalCRS*M_PI/180.0);
  
  //Compute the uncertainty for this proposed direction
  double U = Uncertainty(osxp,osyp,csx,csy,targx,targy);

  //want higher uncertainty to receive lower metric value
  return -U;
}


/******************************************************************************
 *                                                                            *
 *  Method:Uncertainty(double osx, double osy, double csx, double csy,        *
 *                     double targx, double targy) const                      *
 *                                                                            *
 *  Description: This method is called to estimate the uncertainty of a       *
 *  target triangulation given a particular geometry between the ownship,     *
 *  collaborating ship, and target location.Details on the calculations are   *
 *  in my thesis and published papers.                                        *
 *                                                                            * 
 ******************************************************************************/ 

double AOF_CollaborativeTracking::Uncertainty(double osx, double osy, double csx, double csy, double targx, double targy) const
{

  double theta1 = (M_PI/2.0) - atan2(targy-osy,targx-osx);
  double theta2 = (M_PI/2.0) - atan2(targy-csy,targx-csx);

  double tt1    = tan(theta1);
  double tt2    = tan(theta2);
  double ttdiff = (tt1-tt2);
  double dxtdx1 = -(tt2/ttdiff);
  double dxtdx2 = tt1/ttdiff;
  double dxtdy1 = (tt1*tt2)/ttdiff;
  double dxtdy2 = -dxtdy1;
  double dytdy1 = dxtdx2;
  double dytdy2 = dxtdx1;
  double dytdx1 = (-1)/ttdiff;
  double dytdx2 = 1/ttdiff;
  double ydiff  = (osy-csy);
  double term2  = ((csx*tt1) - (osx*tt2) + (ydiff*tt1*tt2))/(ttdiff*ttdiff);
  double sect1  = (1/cos(theta1));
  double sect2  = (1/cos(theta2));
  double dxtdt1 = (((csx + (ydiff*tt2))/ttdiff) - term2)*pow(sect1,2);
  double dxtdt2 = (((-osx + (ydiff*tt1))/ttdiff) + term2)*pow(sect2,2);
  double dytdt1 = ((osy/ttdiff)-(((osy*tt1) - (csy*tt2) + csx - osx)/(ttdiff*ttdiff)))*pow(sect1,2);
  double dytdt2 = ((-csy/ttdiff) + (((osy*tt1) - (csy*tt2) + csx-osx)/(ttdiff*ttdiff)))*pow(sect2,2);
  
  double C1 = pow(dxtdx1,2)+pow(dxtdx2,2)+pow(dxtdy1,2)+pow(dxtdy2,2);
  double C2 = pow(dxtdt1,2)+pow(dxtdt2,2);
  double C3 = pow(dytdx1,2)+pow(dytdx2,2)+pow(dytdy1,2)+pow(dytdy2,2);
  double C4 = pow(dytdt1,2)+pow(dytdt2,2);
  
  double sigmaxt = pow(((C1*sigmapos*sigmapos) + (C2*sigmatheta*sigmatheta)),0.5);
  double sigmayt = pow(((C3*sigmapos*sigmapos) + (C4*sigmatheta*sigmatheta)),0.5);
  
  double error = pow(((sigmaxt*sigmaxt) + (sigmayt*sigmayt)),2);

  return error;
}
