/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Shape_Ship.h                                         */
/*    DATE: September 21st 2007                                  */
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

#ifndef SHAPE_SHIPX_HEADER
#define SHAPE_SHIPX_HEADER

double g_shipXBody[]=
{
  4.0,   0.0,  // Starting point
  4.25,  1.0,
  4.45,  2.0,
  4.70,  3.0,
  4.90,  4.0,
  5.00,  5.0,
  5.15,  6.0,
  5.30,  7.0,
  5.50,  8.0,
  5.75,  9.0,
  5.90, 10.0,
  6.10, 11.0,
  6.20, 12.0,
  6.35, 13.0,
  6.45, 14.0,
  6.50, 15.0,
  6.57, 16.0,
  6.65, 17.0,
  6.55, 18.0,
  6.50, 19.0,
  6.35, 20.0,
  6.15, 21.0,
  5.90, 22.0,
  5.50, 23.0,
  5.05, 24.0,
  4.55, 25.0,
  4.00, 26.0,
  3.35, 27.0,
  2.80, 28.0,
  2.20, 29.0,
  1.50, 30.0,
  0.80, 31.0,
  //-------------------
  0.00, 31.9,    // tip
  //-------------------

  -0.80, 31.0,
  -1.50, 30.0,
  -2.20, 29.0,
  -2.80, 28.0,
  -3.35, 27.0,
  -4.00, 26.0,
  -4.55, 25.0,
  -5.05, 24.0,
  -5.50, 23.0,
  -5.90, 22.0,
  -6.15, 21.0,
  -6.35, 20.0,
  -6.50, 19.0,
  -6.55, 18.0,
  -6.65, 17.0,
  -6.57, 16.0,
  -6.50, 15.0,
  -6.45, 14.0,
  -6.35, 13.0,
  -6.20, 12.0,
  -6.10, 11.0,
  -5.90, 10.0,
  -5.75,  9.0,
  -5.50,  8.0,
  -5.30,  7.0,
  -5.15,  6.0,
  -5.00,  5.0,
  -4.90,  4.0,
  -4.70,  3.0,
  -4.45,  2.0,
  -4.25,  1.0,
  -4.0,   0.0,

  4.0,    0.0   // Starting point
};

double       g_shipXCtrX      = 0.0;
double       g_shipXCtrY      = 15.75;
unsigned int g_shipXBodySize  = 132;
double       g_shipXLength    = 31.5;


#endif













