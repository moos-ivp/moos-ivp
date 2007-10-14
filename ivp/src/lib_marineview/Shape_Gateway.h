/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Shape_GateWay.h                                      */
/*    DATE: Oct 4th  2007                                         */
/*****************************************************************/

#ifndef SHAPE_GATEWAY_HEADER
#define SHAPE_GATEWAY_HEADER

float g_gatewayBody[]=
{
   0.0,   0.0,
   3.0,   0.0,
   5.0,   2.0,
   5.0,   8.0,
   3.0,  10.0,
  -3.0,  10.0,
  -5.0,   8.0,
  -5.0,   2.0,
   -3.0,  0.0,
   0.0,   0.0
};

double       g_gatewayCtrX     = 0.0;
double       g_gatewayCtrY     = 5.0;
unsigned int g_gatewayBodySize = 10;

float g_gatewayMidBody[]=
{
   1.0,   3.0,
   2.0,   4.0,
   2.0,   6.0,
   1.0,   7.0,
  -1.0,   7.0,
  -2.0,   6.0,
  -2.0,   4.0,
  -1.0,   3.0
};

double       g_gatewayMidCtrX     = 0.0;
double       g_gatewayMidCtrY     = 5.0;
unsigned int g_gatewayMidBodySize = 8;

#endif



