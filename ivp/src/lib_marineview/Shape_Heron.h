/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Shape_Heron.h                                        */
/*    DATE: April 30th 2022                                      */
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

#ifndef SHAPE_HERON_HEADER
#define SHAPE_HERON_HEADER

double g_heronBody[]=
  {
   1.5, 0,
   1.8, 14,
   1.7, 18,
   1.5, 20,
   1.0, 22,
   0.7, 23,
   0.2, 24,
   -0.2, 24,
   -0.7, 23,
   -1.0, 22,
   -1.5, 20,
   -1.7, 18,
   -1.8, 14,
   -1.5, 0
  };
unsigned int g_heronBodySize = 14;
double       g_heronLength   = 24;

double g_heronBack[]=
  {
   3.7, 4.5,
   3.7, 12.5,
   -3.7, 12.5,
   -3.7, 4.5
  };
unsigned int g_heronBackSize =  4;

double g_heronFront[]=
  {
   3.7,  12.5,
   3.7,  18.5,
   -3.7, 18.5,
   -3.7, 12.5
  };
unsigned int g_heronFrontSize =  4;


double  g_heronCtrX = 0.0;
double  g_heronCtrY = 12.0;
double  g_heronBase = 12.0;

#endif





