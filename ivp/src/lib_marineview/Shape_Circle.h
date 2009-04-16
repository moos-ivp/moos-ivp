/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Shape_Circle.h                                       */
/*    DATE: Feb 16th 2009                                        */
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

#ifndef SHAPE_CIRCLE_HEADER
#define SHAPE_CIRCLE_HEADER

double g_circleBody[]=
{
  0.4,     5,
  1.3,     4.8,
  2.1,     4.5,
  2.9,     4.1,
  3.5,     3.5,
  4.1,     2.9,
  4.5,     2.1,
  4.8,     1.3,
  5,       0.4,
  5,      -0.4,
  4.8,    -1.3,
  4.5,    -2.1,
  4.1,    -2.9,
  3.5,    -3.5,
  2.9,    -4.1,
  2.1,    -4.5,
  1.3,    -4.8,
  0.4,      -5,
  -0.4,     -5,
  -1.3,    -4.8,
  -2.1,    -4.5,
  -2.9,    -4.1,
  -3.5,    -3.5,
  -4.1,    -2.9,
  -4.5,    -2.1,
  -4.8,    -1.3,
  -5,      -0.4,
  -5,       0.4,
  -4.8,     1.3,
  -4.5,     2.1,
  -4.1,     2.9,
  -3.5,     3.5,
  -2.9,     4.1,
  -2.1,     4.5,
  -1.3,     4.8,
  -0.4,     5,
   0.4,     5
};

double       g_circleCtrX     = 0.0;
double       g_circleCtrY     = 0.0;
unsigned int g_circleBodySize = 37;
double       g_circleWidth    = 10;

#endif
