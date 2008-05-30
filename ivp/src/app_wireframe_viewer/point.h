/*****************************************************************/
/*    NAME: Doug Maxwell and Michael Benjamin                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: point.h                                              */
/*    DATE: December 2007                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef POINT_H
#define POINT_H

class Point {
  public:
    Point( float = 0.0, float = 0.0, float = 0.0, float = 0.0, float = 0.0, float = 0.0, float = 0.0 );
    ~Point(){};
    void drawPoint( float, float, float, float, float, float, float);

};
#endif
