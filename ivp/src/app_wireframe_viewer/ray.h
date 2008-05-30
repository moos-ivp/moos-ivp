/*****************************************************************/
/*    NAME: Doug Maxwell and Michael Benjamin                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ray.h                                                */
/*    DATE: December 2007                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef RAY_HEADER
#define RAY_HEADER

#include <FL/Fl_Gl_Window.H>

class Ray {
  public:
/*
    Ray(
	float start_x = 0, float start_y = 0, float start_z = 0, 
	float end_x = 0, float end_y = 0, float end_z = 0
	);
*/
    Ray(
	float = 0.0, float = 0.0, float = 0.0, 
	float = 0.0, float = 0.0, float = 0.0, 
	float = 0.0, float = 0.0, float = 0.0, float = 0.0);
    void setLine(float, float, float, float, float, float);
    void drawLine(float, float, float, float, float, float,float, float, float, float);

    float start_x, start_y, start_z, end_x, end_y, end_z;
};

#endif
