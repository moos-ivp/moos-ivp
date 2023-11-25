/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Shape_Kayak.h                                        */
/*    DATE: April 20th 2016                                      */
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

#ifndef SHAPE_MOKAI_HEADER
#define SHAPE_MOKAI_HEADER

//==================================================
// STERN
//==================================================
double g_mokaiStern[]=
{
    0.0,      0.0,
    2.0,      0.0,
    2.5,      0.5,
    2.5,      8.0,
   -2.5,      8.0,     
   -2.5,      0.5,
   -2.0,      0.0,     
};
unsigned int g_mokaiSternSize = 7;

//==================================================
// STERN SLICES (The non-convex part of the body)
//==================================================
double g_mokaiSlice1[]=
{
    2.5,      8.0,
    2.7,      9.0,     
   -2.7,      9.0,     
   -2.5,      8.0,
};
double g_mokaiSlice2[]=
{
    2.7,      9.0,     
    3.5,     10.0,     
   -3.5,     10.0,     
   -2.7,      9.0,     
};
double g_mokaiSlice3[]=
{
    3.5,     10.0,     
    4.3,     11.0,     
   -4.3,     11.0,     
   -3.5,     10.0,     
};
unsigned int g_mokaiSlice1Size = 4;
unsigned int g_mokaiSlice2Size = 4;
unsigned int g_mokaiSlice3Size = 4;

//==================================================
// BOW 
//==================================================
double g_mokaiBow[]=
{
    3.5,     10.0,
    4.3,     11.0,
    4.5,     12.0,
    4.5,     29.0,
    4.0,     31.0,     
    3.3,     33.0,     
    2.5,     34.3,     
    1.5,     35.5,     
    0.5,     36.0,
   -0.5,     36.0,
   -1.5,     35.5,     
   -2.5,     34.3,     
   -3.3,     33.0,     
   -4.0,     31.0,     
   -4.5,     29.0,
   -4.5,     12.0,
   -4.3,     11.0,     
   -3.5,     10.0,     
};
unsigned int g_mokaiBowSize = 18;


//==================================================
// FULL BODY (non-convex, good for drawing outline)
//==================================================

double g_mokaiBody[]=
{
    0.0,      0.0,
    2.0,      0.0,
    2.5,      0.5,
    2.5,      8.0,
    2.7,      9.0,
    3.5,     10.0,
    4.3,     11.0,
    4.5,     12.0,
    4.5,     29.0,
    4.0,     31.0,     
    3.3,     33.0,     
    2.5,     34.3,     
    1.5,     35.5,     
    0.5,     36.0,
   -0.5,     36.0,
   -1.5,     35.5,     
   -2.5,     34.3,     
   -3.3,     33.0,     
   -4.0,     31.0,     
    -4.5,     29.0,
   -4.5,     12.0,
   -4.3,     11.0,     
   -3.5,     10.0,     
   -2.7,      9.0,     
   -2.5,      8.0,     
   -2.5,      0.5,
   -2.0,      0.0,     
};
unsigned int g_mokaiBodySize = 27;



double       g_mokaiScale    = 1.0;
double       g_mokaiLength   = 36;

double g_mokaiMidOpen[]=
{
  2.5,     11.0,
  2.5,     22.0,
  1.5,     24.0,
  -1.5,    24.0,
  -2.5,    22.0,
  -2.5,    11, 
};

unsigned int g_mokaiMidOpenSize = 6;

double  g_mokaiCtrX = 0.0;
double  g_mokaiCtrY = 18;

#endif











