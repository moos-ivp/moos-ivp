/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: AOF_2VAngle.cpp                                */
/*    BORN: 23 July 05                                     */
/***********************************************************/
#include <iostream>
#include <math.h> 
#include <assert.h>
#include "AOF_2VAngle.h"
#include "AngleUtils.h"
#include "IvPDomain.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_2VAngle::AOF_2VAngle(IvPDomain g_domain, double tgheading,
			 double tgspeed, double px, double py, 
			 double tgx, double tgy, 
			 double v2x, double v2y)
  : AOF(g_domain)
{
  int crs_ix = g_domain.getIndex("course");
  int spd_ix = g_domain.getIndex("speed");
  int tol_ix = g_domain.getIndex("tol");

  assert(crs_ix != -1);
  assert(spd_ix != -1);
  assert(tol_ix != -1);

  crsDelta = g_domain.getVarDelta(crs_ix);
  crsBase  = g_domain.getVarLow(crs_ix);

  spdDelta = g_domain.getVarDelta(spd_ix);
  spdBase  = g_domain.getVarLow(spd_ix);

  tolDelta = g_domain.getVarDelta(tol_ix);
  tolBase  = g_domain.getVarLow(tol_ix);

  osX = px;
  osY = py;

  tx = tgx;
  ty = tgy;

  curr_dist = sqrt(pow(tx-osX,2)+pow(ty-osY,2));
  
  thead  = tgheading;
  tspeed = tgspeed;

  v2_x = v2x;
  v2_y = v2y;

  gamCN = degToRadians(thead);
  cgamCN = cos(gamCN);             
  sgamCN = sin(gamCN);

}

//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given course
//     

double AOF_2VAngle::evalBox(const IvPBox *b) const
{
 
  double evalCRS  = crsBase + ((double)(b->pt(0,0)) * crsDelta);
  double evalSPD  = spdBase + ((double)(b->pt(1,0)) * spdDelta);
  double evalTOL  = tolBase + ((double)(b->pt(2,0)) * tolDelta);


  double gamOS  = degToRadians(evalCRS);    // Angle in radians.
  double cgamOS = cos(gamOS);             // Cosine of Angle (osCRS).
  double sgamOS = sin(gamOS);             // Sine   of Angle (osCRS).

  double new_os_lat = (cgamOS  * evalSPD * evalTOL) + osY;
  double new_os_lon = (sgamOS  * evalSPD * evalTOL) + osX;

  double new_cn_lat = (cgamCN*tspeed*evalTOL) + ty;
  double new_cn_lon = (sgamCN*tspeed*evalTOL) + tx;

  
  double angle1 = atan2(new_cn_lat-new_os_lat,new_cn_lon-new_os_lon);
  double angle2 = atan2(new_cn_lat-v2_y,new_cn_lon-v2_x);

  double sep_angle = (angle1-angle2)*180/M_PI;

  if (sep_angle < -180.0)
    sep_angle += 360.0;

  if (sep_angle > 180.0)
    sep_angle = 360.0 - sep_angle;

  sep_angle = fabs(sep_angle);

  double p_dist = sqrt(pow(new_cn_lon-new_os_lon,2)+pow(new_cn_lat-new_os_lat,2));

  double mval = AngleMetric(sep_angle,p_dist);
 
  return mval;
}

//----------------------------------------------------------------
// Procedure: metric


double AOF_2VAngle::AngleMetric(double sep_angle,double dist) const
  {

    if (dist < 70.0)
      return (-100.0);

    double hval = 100.0-fabs(90.0-sep_angle);

  return hval;
  }
