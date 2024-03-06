/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Shape_CRay.h                                         */
/*    DATE: March 5th 2023                                       */
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

#ifndef SHAPE_CRAY_HEADER
#define SHAPE_CRAY_HEADER

double g_crayBody[]=
  {
    1.5, 0,
    2.0, 0.10,
    2.5, 0.3,
    2.5, 33.2,
    2.0, 33.4,
    1.5, 33.5,

    -1.5, 33.5,
    -2.0, 33.4,
    -2.5, 33.2,
    -2.5, 0.3,
    -2.0, 0.1,
    -1.5, 0
  };

unsigned int g_crayBodySize = 12;
double       g_crayLength   = 33.5;

//======================================
double g_crayBaseFinR[]=
  {
    2.5,  0.25, // a
    4.0 , 3.5,  // c
    4.0,  30.0,   
    2.5,  33.25,  
  };
unsigned int g_crayBaseFinRSize =  4;

double g_crayBaseFinL[]=
  {
    -2.5,  0.25, 
    -4.0 , 3.5,  
    -4.0,  30.0,   
    -2.5,  33.25,  
  };
unsigned int g_crayBaseFinLSize =  4;

//======================================
double g_crayFinR1[]=
  {
    4,   3.5,  // c
    6,   5.9,  // d
    6.8, 7.9,  // e
    6,   9.9,  // f
    4,   12.3, // h
  };
unsigned int g_crayFinR1Size = 5;

double g_crayFinL1[]=
  {
    -4,   3.5,  // c
    -6,   5.9,  // d
    -6.8, 7.9,  // e
    -6,   9.9,  // f
    -4,   12.3, // h
  };
unsigned int g_crayFinL1Size = 5;

//======================================
double g_crayFinR2[]=
  {
    4,   12.3, // h
    6,   14.7, // j
    6.8, 16.7, // k
    6,   18.7, // l
    4,   21.1, // n
  };
unsigned int g_crayFinR2Size = 5;

double g_crayFinL2[]=
  {
    -4,   12.3, // h
    -6,   14.7, // j
    -6.8, 16.7, // k
    -6,   18.7, // l
    -4,   21.1, // n
  };
unsigned int g_crayFinL2Size = 5;

//======================================
double g_crayFinR3[]=
  {
    4,   21.1, // n
    6.0, 23.5, // q
    6.8, 25.5, // r
    6.0, 27.5, // s
    4,   30,   // w
  };
unsigned int g_crayFinR3Size =  5;

double g_crayFinL3[]=
  {
    -4,   21.1, // n
    -6.0, 23.5, // q
    -6.8, 25.5, // r
    -6.0, 27.5, // s
    -4,   30,   // w
  };
unsigned int g_crayFinL3Size =  5;

double  g_crayCtrX = 0.0;
double  g_crayCtrY = 16.25;
double  g_crayBase = 12.0;

#endif
