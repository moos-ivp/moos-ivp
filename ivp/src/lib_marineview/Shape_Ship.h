/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Shape_Ship.h                                         */
/*    DATE: September 21st 2007                                  */
/*****************************************************************/

#ifndef SHAPE_SHIP_HEADER
#define SHAPE_SHIP_HEADER

float g_shipBody[]=
{
  30.0,   0.0,      
  39.0,  40.0,
  45.0,  80.0,     
  46.0, 100.0,
  45.0, 120.0,     
  39.0, 160.0,
   3.0, 212.0, 
  -3.0, 212.0,
 -30.0, 160.0, 
 -45.0, 120.0,
 -46.0, 100.0,  
 -45.0,  80.0,
 -39.0,  40.0, 
 -30.0,   0.0,
  30.0,   0.0
};

double       g_shipCtrX     = 0.0;
double       g_shipCtrY     = 106.5;
unsigned int g_shipBodySize = 15;
float        g_shipScale    = 0.8;

float g_shipFatBody[]=
{
  40.0,   0.0,      
  52.0,  40.0,
  60.0,  80.0,     
  60.8, 100.0,
  60.0, 120.0,     
  40.0, 160.0,
   4.0, 212.0, 
  -4.0, 212.0,
 -40.0, 160.0, 
 -60.0, 120.0,
 -60.8, 100.0,  
 -60.0,  80.0,
 -60.0,  40.0, 
 -52.0,   0.0,
  40.0,   0.0
};

double       g_shipFatCtrX     = 0.0;
double       g_shipFatCtrY     = 106.5;
unsigned int g_shipFatBodySize = 15;
float        g_shipFatScale    = 1.0;

#endif



