/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Shape_Skywalker.h                                    */
/*    DATE: January 1st 2026                                     */
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

#ifndef SHAPE_SKYWALKER_HEADER
#define SHAPE_SKYWALKER_HEADER

double g_skywBody[] =
{
  //  0,     1,
  12.8,  1,
  //  13,    0,
  13.2,  1,
  13.2,  3.3,
  10,    5,
  2,     8,
  -2,    8,
  -10,   5,
  -13.2, 3.3,
  -13.2, 1,
  //  -13,   0,
  -12.8, 1,
  12.8, 1, 
  //  0,     1
};

unsigned int g_skywBodySize = 11;
double       g_skywScale  = 1.0;
double       g_skywLength = 25;

double g_skywMid[] =
{
  0,    0,
  1.3,  0,
  2,    1,
  2,    8,
  1.5,  9,
  0.5,  10,
  0,    10.3, 
  -0.5, 10,
  -1.5, 9,
  -2,   8,
  -2,   1,
  -1.3, 0,
  0,    0
};

unsigned int g_skywMidSize   = 13;
double       g_skywMidScale  = 1.0;
double       g_skywMidLength = 1.0;

double g_skywCtrX = 0.0;
double g_skywCtrY = 5.15;

#endif











