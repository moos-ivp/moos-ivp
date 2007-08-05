#include <iostream>
#include <math.h> 
#include "AOF_StayInRange.h"
#include "IvPDomain.h"

using namespace std;

AOF_StayInRange::AOF_StayInRange(IvPDomain gdomain, double gcnlat, double
   gcnlon, double gcncrs, double gcnspd, double goslat, double goslon) {
  
  int courseIndex = gdomain.getIndex("course");
  int speedIndex = gdomain.getIndex("speed");

  courseBase = gdomain.get_dlow(courseIndex);
  courseDelta = gdomain.get_ddelta(courseIndex);
  coursePts = gdomain.get_dpoints(courseIndex);

  speedBase  = gdomain.get_dlow(speedIndex);
  speedDelta = gdomain.get_ddelta(speedIndex);

  //  this is set to -0.x so that we are able to get 0.0 as speed
  //  might also look in to the possibility of going revers, that is setting the
  //  speed range to [-max, max]
  speedBase -= speedDelta;
  speedPts = gdomain.get_dpoints(speedIndex);

  universe = IvPBox(2, 1);
  universe.setPTS(0, 0, coursePts - 1);
  universe.setPTS(1, 0, speedPts - 1);

  cpa_engine = new CPAEngine(gcnlat, gcnlon, gcncrs, gcnspd, goslat, goslon);
}

AOF_StayInRange::~AOF_StayInRange() {
}

double AOF_StayInRange::evalBox(const IvPBox *box) const {
  double course = courseBase + ((double)(box->pt(0,0)) * courseDelta);
  double speed = speedBase + ((double)(box->pt(1,0)) * speedDelta);
 
  double evalDist = cpa_engine->evalManeuver(course, speed, 0.1);

  return metric(evalDist);
}

double AOF_StayInRange::metric(double gval) const {
  double pct = gval / (sqrt(cpa_engine->getK0()));
  pct = pct * pct;
  pct = 1.0 - pct;
  return 1000.0 * pct;
}
