/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Shape_AUV.h                                          */
/*    DATE: September 21st 2007                                  */
/*****************************************************************/

#ifndef SHAPE_AUV_HEADER
#define SHAPE_AUV_HEADER

float g_auvBody[]=
{
    5.0,    0.0,    
   15.5,   18.0,
   15.5,   90.0,   
   15.0,   92.0,
   12.0,   95.0,  
    8.0,   97.0,
    5.0,   99.0,   
    2.0,  100.0,
   -2.0,  100.0,  
   -5.0,   99.0,
   -8.0,   97.0,   
  -12.0,   95.0,
  -15.0,   92.0,  
  -15.5,   90.0, 
  -15.5,   18.0,   
   -5.0,    0.0
};
unsigned int g_auvBodySize = 16;
float        g_auvScale = 1.0;

float g_propUnit[]=
{
   16.0,  0.0,   
   16.0, -9.0,
  -16.0, -9.0, 
  -16.0,  0.0
};

unsigned int g_propUnitSize =  4;

double  g_auvCtrX = 0.0;
double  g_auvCtrY = 45.5;

#endif
