/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Shape_Kelp.h                                         */
/*    DATE: Oct 4th  2007                                        */
/*****************************************************************/

#ifndef SHAPE_KELP_HEADER
#define SHAPE_KELP_HEADER

float g_kelpBody[]=
{
   1.95,   9.81,
   1.94,   4.00,
   5.56,   8.31,
   8.31,   5.56,
   4.00,   1.95, 
   9.81,   1.95,
   9.81,  -1.95,
   4.00,  -1.95,
   8.31,  -5.56,
   5.56,  -8.31,
   1.95,  -4.00,
   1.95,  -9.81,
  -1.95,  -9.81,
  -1.95,  -4.00,
  -5.56,  -8.31,
  -8.31,  -5.56,
  -4.00,  -1.95,
  -9.81,  -1.95,
  -9.81,   1.95,
  -4.00,   1.95, 
  -8.31,   5.56,
  -5.56,   8.31,
  -1.95,   4.00,
  -1.95,   9.81
};

double       g_kelpCtrX     = 0.0;
double       g_kelpCtrY     = 0.0;
unsigned int g_kelpBodySize = 24;

#endif

