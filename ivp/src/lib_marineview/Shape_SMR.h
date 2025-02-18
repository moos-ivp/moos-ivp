/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Shape_SMR.h                                          */
/*    DATE: Feb 17th 2025                                        */
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

#ifndef SHAPE_SMR_HEADER
#define SHAPE_SMR_HEADER

double g_SMR_Body[]=
  {
   1,    -0.2,
   3,    -1.3,
   5,    -7,
   6,    -12,
   6.3,  -14,
   6.3,  -36,
   6,    -37,
   5,    -37.5,
   -5,   -37.5,
   -6,   -37,
   -6.3, -36,
   -6.3, -14,
   -6,   -12,
   -5,   -7,
   -3,   -1.3,
   -1,   -0.2,
  };
unsigned int g_SMR_BodySize = 16;
double       g_SMR_Length   = 24;

double g_SMR_IBody[]=
  {
    2,   -2,
    3.2, -5,
    5,   -11,
    5.5, -15,
    5.5, -38,
    5.0, -38,
    5.0, -37.5,
    -5.0, -37.5,
    -5.0, -38,
    -5.5, -38,
    -5.5, -15,
    -5,   -11,
    -3.2, -5,
    -2,  -2,    
  };
unsigned int g_SMR_IBodySize =  14;

double g_SMR_IMBody[]=
  {
    0, -10,
    4.5, -12,
    4.5, -26.5,
    -4.5, -26.5,
    -4.5, -12,
    0, -10,
  };
unsigned int g_SMR_IMBodySize =  6;

double g_SMR_MotorA[]=
  {
    0.8,  -37.5,
    0.3,  -39.3, 
    0.8,  -40.4,
    2.8,  -40.4,
    3.3,  -39.3,
    2.8,  -37.5
  };
unsigned int g_SMR_MotorASize =  6;

double g_SMR_MotorB[]=
  {
    -0.8,  -37.5,
    -0.3,  -39.3, 
    -0.8,  -40.4,
    -2.8,  -40.4,
    -3.3,  -39.3,
    -2.8,  -37.5
  };
unsigned int g_SMR_MotorBSize =  6;

double g_SMR_Slash[]=
  {
    -3.5, -33.5,
    1,    -34,
    1,    -32.5,
    -1,   -32.5,
    -1,   -31,
    3.5,  -31.5
  };
unsigned int g_SMR_SlashSize =  6;

double  g_SMR_CtrX = 0.0;
double  g_SMR_CtrY = -18.75;
double  g_SMR_Base = 12.6;

#endif





