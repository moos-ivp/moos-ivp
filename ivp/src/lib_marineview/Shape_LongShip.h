/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Shape_LongShip.h                                     */
/*    DATE: February 25th 2021                                   */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef SHAPE_LONG_SHIP_HEADER
#define SHAPE_LONG_SHIP_HEADER

double g_longShipBody[]=
{
    3.1218,        0,
    3.3104,   3.0517,
    3.4186,   5.6635,
    3.5265,   8.5396,
    3.5537,  10.6235,
    3.5808,  12.5605,
    3.6350,  14.5271,
    3.6082,  16.9636,
    3.5814,  19.5471,
    3.5275,  22.1894,
    3.3930,  24.7443,
    3.2583,  27.1229,
    3.0968,  29.0615,
    2.8541,  31.9985,
    2.6118,  34.2607,
    2.3692,  36.1411,
    2.1266,  37.9628,
    1.8303,  40.2546,
    1.4798,  42.5175,
    1.0756,  44.8687,
    0.6712,  46.7502,
    0.3746,  47.8972,
    0.1859,  48.5150,
         0,  48.7680,
   -0.1859,  48.5150,
   -0.3746,  47.8972,
   -0.6712,  46.7502,
   -1.0756,  44.8687,
   -1.4798,  42.5175,
   -1.8303,  40.2546,
   -2.1266,  37.9628,
   -2.3692,  36.1411,
   -2.6118,  34.2607,
   -2.8541,  31.9985,
   -3.0968,  29.0615,
   -3.2583,  27.1229,
   -3.3930,  24.7443,
   -3.5275,  22.1894,
   -3.5814,  19.5471,
   -3.6082,  16.9636,
   -3.6350,  14.5271,
   -3.5808,  12.5605,
   -3.5537,  10.6235,
   -3.5265,   8.5396,
   -3.4186,   5.6635,
   -3.3104,   3.0517,
   -3.1218,        0
};

double       g_longShipCtrX     = 0.0;
double       g_longShipCtrY     = 21.184;
unsigned int g_longShipBodySize = 47;
double       g_longShipLength   = 48.768;

#endif














