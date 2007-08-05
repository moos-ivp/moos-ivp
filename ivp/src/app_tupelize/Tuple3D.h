/***********************************************************/
/*    NAME: Michael Benjamin                               */
/*    FILE: Tuple3D.h                                      */
/*    DATE: Sep 15 2005                                    */
/***********************************************************/

#ifndef TUPLE3D_HEADER
#define TUPLE3D_HEADER

class Tuple3D {
public:
  Tuple3D() {};
  Tuple3D(double gx, double gy, double gz) {x=gx; y=gy; z=gz;};
  ~Tuple3D() {};

  double x;
  double y;
  double z;
};

#endif
