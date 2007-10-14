/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Shape_EField.h                                       */
/*    DATE: Oct 4th  2007                                         */
/*****************************************************************/

#ifndef SHAPE_EFIELD_HEADER
#define SHAPE_EFIELD_HEADER

float g_efieldBody[]=
{
  0.00,   0.00,
  0.00,   5.00,
  0.44,   4.98,
  1.29,   4.83,
  2.11,   4.53,
  2.87,   4.10,
  3.54,   3.54,
  4.10,   2.87,
  4.53,   2.11,
  4.83,   1.29,
  4.98,   0.44,
  4.98,  -0.44,
  4.83,  -1.29,
  4.53,  -2.11,
  4.10,  -2.87,
  3.54,  -3.54,
  2.87,  -4.10,
  2.11,  -4.53,
  1.29,  -4.83,
  0.44,  -4.98,
  0.00,  -5.00,
  0.00,   0.00,
};

double       g_efieldCtrX     = 0.0;
double       g_efieldCtrY     = 0.0;
unsigned int g_efieldBodySize = 22;

float g_efieldMidBody[]=
{
  0.00,   0.00,
  0.00,  -5.00,
 -0.44,  -4.98,
 -1.29,  -4.83,
 -2.11,  -4.53,
 -2.87,  -4.10,
 -3.54,  -3.54,
 -4.1,   -2.87,
 -4.53,  -2.11,
 -4.83,  -1.29,
 -4.98,  -0.44,
 -4.98,   0.44,
 -4.83,   1.29,
 -4.53,   2.11,
 -4.1,    2.87,
 -3.54,   3.54,
 -2.87,   4.10,
 -2.11,   4.53,
 -1.29,   4.83,
 -0.44,   4.98,
  0.00,   5.00,
  0.00,   0.00,
};

double       g_efieldMidCtrX     = 0.0;
double       g_efieldMidCtrY     = 0.0;
unsigned int g_efieldMidBodySize = 22;

#endif



