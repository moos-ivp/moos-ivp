/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Shape_Kelp.h                                         */
/*    DATE: Oct 4th 2007                                         */
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

#ifndef SHAPE_KELP_HEADER
#define SHAPE_KELP_HEADER

double g_kelpBody[]=
{
   1.95,   9.81,
   1.94,   4.00,
   5.56,   8.31,
   8.31,   5.56,
   4.00,   1.95, 
   9.81,   1.95,
   9.81,  -1.95,
   4.00,  -1.95,
   8.31,  -5.56,
   5.56,  -8.31,
   1.95,  -4.00,
   1.95,  -9.81,
  -1.95,  -9.81,
  -1.95,  -4.00,
  -5.56,  -8.31,
  -8.31,  -5.56,
  -4.00,  -1.95,
  -9.81,  -1.95,
  -9.81,   1.95,
  -4.00,   1.95, 
  -8.31,   5.56,
  -5.56,   8.31,
  -1.95,   4.00,
  -1.95,   9.81
};

double       g_kelpCtrX     = 0.0;
double       g_kelpCtrY     = 0.0;
double       g_kelpWidth    = 19.62;
unsigned int g_kelpBodySize = 24;

#endif











