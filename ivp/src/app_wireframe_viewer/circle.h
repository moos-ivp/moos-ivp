/*****************************************************************/
/*    NAME: Doug Maxwell and Michael Benjamin                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: circle.h                                             */
/*    DATE: December 2007                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef CIRCLE_H
#define CIRCLE_H

#include "point.h"

class Circle {
  public:
    //Constructor:  Radius, Center xyz, Color rgba
    Circle( 
	float = 0.0, 
	float = 0.0, float = 0.0, float = 0.0, 
	float = 0.0, float = 0.0, float = 0.0, float = 0.0);
    ~Circle(){};
    void setRadius( float );
    float getRadius();
    void drawCircle(float, float, float, float, float, float, float, float);
    float radius;
};

#endif
