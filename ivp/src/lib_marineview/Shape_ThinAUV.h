/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Shape_ThinAUV.h                                      */
/*    DATE: September 21st 2007                                  */
/*****************************************************************/

#ifndef SHAPE_THIN_AUV_HEADER
#define SHAPE_THIN_AUV_HEADER

float g_thin_auvBody[]=
{
   5.0,    0.0,    
  10.5,   18.0,
  10.5,   90.0,     
  10.0,   92.0,
   9.0,   95.0,     
   7.0,   97.0,
   5.0,   99.0,     
   2.0,  100.0,
  -2.0,  100.0,   
  -5.0,   99.0,
  -7.0,   97.0,    
  -9.0,   95.0,
 -10.0,   92.0,    
 -10.5,   90.0, 
 -10.5,   18.0,    
  -5.0,    0.0
};

unsigned int g_thin_auvBodySize = 16;

float g_thin_propUnit[]=
{
  11.0,  0.0,   
  11.0, -9.0,
 -11.0, -9.0, 
 -11.0,  0.0
};

unsigned int g_thin_propUnitSize = 4;

double  g_thin_auvCtrX =  0.0;
double  g_thin_auvCtrY = 45.5;

#endif
