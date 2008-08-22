/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Shape_EField.h                                       */
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

#ifndef SHAPE_EFIELD_HEADER
#define SHAPE_EFIELD_HEADER

double g_efieldBody[]=
{
  0.00,   0.00,
  0.00,   5.00,
  0.44,   4.98,
  1.29,   4.83,
  2.11,   4.53,
  2.87,   4.10,
  3.54,   3.54,
  4.10,   2.87,
  4.53,   2.11,
  4.83,   1.29,
  4.98,   0.44,
  4.98,  -0.44,
  4.83,  -1.29,
  4.53,  -2.11,
  4.10,  -2.87,
  3.54,  -3.54,
  2.87,  -4.10,
  2.11,  -4.53,
  1.29,  -4.83,
  0.44,  -4.98,
  0.00,  -5.00,
  0.00,   0.00,
};

double       g_efieldCtrX     = 0.0;
double       g_efieldCtrY     = 0.0;
unsigned int g_efieldBodySize = 22;

double g_efieldMidBody[]=
{
  0.00,   0.00,
  0.00,  -5.00,
 -0.44,  -4.98,
 -1.29,  -4.83,
 -2.11,  -4.53,
 -2.87,  -4.10,
 -3.54,  -3.54,
 -4.1,   -2.87,
 -4.53,  -2.11,
 -4.83,  -1.29,
 -4.98,  -0.44,
 -4.98,   0.44,
 -4.83,   1.29,
 -4.53,   2.11,
 -4.1,    2.87,
 -3.54,   3.54,
 -2.87,   4.10,
 -2.11,   4.53,
 -1.29,   4.83,
 -0.44,   4.98,
  0.00,   5.00,
  0.00,   0.00,
};

double       g_efieldMidCtrX     = 0.0;
double       g_efieldMidCtrY     = 0.0;
unsigned int g_efieldMidBodySize = 22;

#endif




