/*****************************************************************/
/*    NAME: Doug Maxwell - Mike Benjamin                         */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: arc.h                                                */
/*    DATE: December 2007                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef ARC_H
#define ARC_H

#include "point.h"

class Arc : public Point {
  public:
    Arc( 
	float = 0.0, float = 0.0, float = 0.0, 
	float = 0.0, float = 0.0, float = 0.0, 
	float = 0.0, float = 0.0, float = 0.0, float = 0.0);
    ~Arc(){};
    void setRadius( float );
    float getRadius();
    void drawArc(float, float, float, float, float, float, float, float, float, float);
    float radius;
};

#endif
