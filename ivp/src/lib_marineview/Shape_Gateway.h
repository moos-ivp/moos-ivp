/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Shape_GateWay.h                                      */
/*    DATE: Oct 4th 2007                                         */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
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




