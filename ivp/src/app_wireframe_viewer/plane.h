/*****************************************************************/
/*    NAME: Doug Maxwell and Michael Benjamin                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: plane.h                                              */
/*    DATE: January 2008                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef PLANE_H
#define PLANE_H

class Plane {
  public:
    Plane( 
	float = 0.0, float = 0.0, float = 0.0, 
	float = 0.0, float = 0.0, float = 0.0, float = 0.0, float = 0.0 );
    ~Plane(){};
    void drawPlane( float, float, float, float, float, float, float, float);

};
#endif
