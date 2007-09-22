/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VehicleShapes.h                                      */
/*    DATE: Nov 19th 2004                                        */
/*****************************************************************/

#define BOXW 0.275f
#define BOXL 0.275f
#define SHAPESIZE(s) (sizeof(s)/sizeof(float)/2/2)

// g_boxShape describe the square robot-like rendering used in 
// pRobotoViewer. It is described in terms GL_LINES, so each vertex
// is listed twice.
float g_boxShape[]=
{
     BOXL,  BOXW,    -BOXL,  BOXW,
    -BOXL,  BOXW,    -BOXL, -BOXW,
    -BOXL, -BOXW,     BOXL, -BOXW,
     BOXL, -BOXW,     BOXL,  BOXW,
    -BOXL,  0,        BOXL,  0,
     BOXL,  0,        0,     BOXW,
     BOXL,  0,        0,    -BOXW
};
unsigned int g_boxShapeSize = SHAPESIZE(g_boxShape);

