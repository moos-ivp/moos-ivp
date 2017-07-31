/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Shape_Glider.h                                       */
/*    DATE: October 5th 2007                                     */
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

#ifndef SHAPE_GLIDER_HEADER
#define SHAPE_GLIDER_HEADER

double g_gliderBody[]=
{
   2.0,   0.0,
   3.0,   2.0, 
   6.0,  13.0,
   9.0,  32.0,
   9.2,  35.0, 
   9.0,  38.0, 
   6.0,  57.0, 
   3.0,  68.0, 
   2.0,  70.0, 
   0.0,  70.5, 
  -2.0,  70.0,
  -3.0,  68.0, 
  -6.0,  57.0, 
  -9.0,  38.0, 
  -9.2,  35.0, 
  -9.0,  32.0, 
  -6.0,  13.0, 
  -3.0,   2.0, 
  -2.0,   0.0,
   2.0,   0.0
};

double       g_gliderCtrX     = 0.0;
double       g_gliderCtrY     = 35.0;
unsigned int g_gliderBodySize = 20;
double       g_gliderLength   = 70.5;

double g_gliderWing[]=
{
    5.0,  10.0, 
   33.0,  12.0, 
   33.0,  19.0, 
   12.0,  26.0, 
    0.0,  28.0, 
  -12.0,  26.0, 
  -33.0,  19.0, 
  -33.0,  12.0,
   -5.0,  10.0
};

unsigned int g_gliderWingSize = 9;

#endif













