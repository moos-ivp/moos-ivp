/*****************************************************************/
/*    NAME: Charles Benjamin                                     */
/*    ORGN: Massachusetts Institute of Technology                */
/*    FILE: Shape_Quadcopter.h                                   */
/*    DATE: July 14th 2026                                       */
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

#ifndef SHAPE_QUADCOPTER_HEADER
#define SHAPE_QUADCOPTER_HEADER

// MarineViewer treats +Y as the nose/forward direction. The two
// rectangles form an X frame, with rotor centers at (+/-1.48,+/-1.48).
double g_quadArmA[] =
{
  -1.56, -1.35,
  -1.35, -1.56,
   1.56,  1.35,
   1.35,  1.56,
  -1.56, -1.35
};
unsigned int g_quadArmASize = 5;

double g_quadArmB[] =
{
  -1.56,  1.35,
  -1.35,  1.56,
   1.56, -1.35,
   1.35, -1.56,
  -1.56,  1.35
};
unsigned int g_quadArmBSize = 5;

// One rotor disc centered on the origin. The renderer reuses it with
// four translations, which avoids maintaining four duplicate arrays.
double g_quadRotor[] =
{
   0.62,  0.00,
   0.57,  0.24,
   0.44,  0.44,
   0.24,  0.57,
   0.00,  0.62,
  -0.24,  0.57,
  -0.44,  0.44,
  -0.57,  0.24,
  -0.62,  0.00,
  -0.57, -0.24,
  -0.44, -0.44,
  -0.24, -0.57,
   0.00, -0.62,
   0.24, -0.57,
   0.44, -0.44,
   0.57, -0.24,
   0.62,  0.00
};
unsigned int g_quadRotorSize = 17;

// The body takes the configured vehicle color. Its pointed front makes
// heading readable even when the four rotors are visually symmetric.
double g_quadBody[] =
{
  -0.76, -0.85,
   0.76, -0.85,
   0.76,  0.49,
   0.00,  1.22,
  -0.76,  0.49,
  -0.76, -0.85
};
unsigned int g_quadBodySize = 6;

double g_quadHeading[] =
{
  -0.26, 0.39,
   0.26, 0.39,
   0.00, 0.91,
  -0.26, 0.39
};
unsigned int g_quadHeadingSize = 4;

double g_quadRotorOffset = 1.48;
double g_quadLength      = 4.20;
double g_quadCtrX        = 0.0;
double g_quadCtrY        = 0.0;

#endif
