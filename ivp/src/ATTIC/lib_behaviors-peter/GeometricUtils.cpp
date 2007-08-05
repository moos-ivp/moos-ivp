#include <math.h>
#include "GeometricUtils.h"

double GeometricUtils::invertAngle(double angle) {
  angle = canonicalizeAngle(angle);

  angle += 180;

  if(angle > 359)
    angle -= 360;

  return angle;
}

double GeometricUtils::canonicalizeAngle(double angle) {
  // add epsilon to this?
  if((angle >= 0) && (angle < 360))
    return angle;
  
  if((angle < -360) || (angle > 360)) {
    double n = floor(angle/360);
    angle /= n;
  }
  
  if(angle < 0)
    return angle + 360;

  return angle - 360;
}

double GeometricUtils::distance(double x1, double y1, double x2, double y2) {
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}
