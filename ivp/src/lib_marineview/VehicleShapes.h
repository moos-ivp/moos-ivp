/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: KayakViewer.cpp                                      */
/*    DATE: Nov 19th 2004                                        */
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

#define BOXW 0.275f
#define BOXL 0.275f
#define SHAPESIZE(s) (sizeof(s)/sizeof(float)/2/2)


// g_boxShape describe the square robot-like rendering used in 
// pRobotoViewer. It is described in terms GL_LINES, so each vertex
// is listed twice.
float g_boxShape[]=
{
     BOXL,  BOXW,    -BOXL,  BOXW,
    -BOXL,  BOXW,    -BOXL, -BOXW,
    -BOXL, -BOXW,     BOXL, -BOXW,
     BOXL, -BOXW,     BOXL,  BOXW,
    -BOXL,  0,        BOXL,  0,
     BOXL,  0,        0,     BOXW,
     BOXL,  0,        0,    -BOXW
};
unsigned int g_boxShapeSize = SHAPESIZE(g_boxShape);


float g_triangleShape[]=
{
   0.0,    0.0,    
   14.625, 42.0,
   11.375, 90.0, 
   2.0,    119.0,
   0.5,    119.875,
   -0.5,    119.875,
   -2.0,    119.0,
   -11.375, 90.0, 
   -14.625, 42.0,
   0.0,     0.0
};
unsigned int g_triangleShapeSize = SHAPESIZE(g_triangleShape);




float g_kayakBody[]=
{
   0.0,    0.0,    
   5.25,   6.0,
   7.25,   9.0,    
   8.75,   12.0,
   12.0,   20.0, 
   13.25,  26.0, 
   14.00,  32.0, 
   14.625, 40.0,
   14.625, 42.0,
   14.875, 48.0, 
   15.0,   54.0, 
   14.875, 60.0, 
   14.5,   66.0, 
   13.875, 72.0, 
   13.75,  78.0, 
   12.5,   84.0, 
   11.375, 90.0, 
   10.375, 96.0, 
   9.25,   100.0, 
   7.875,  104.0, 
   6.50,   108.0, 
   5.375,  112.0, 
   3.375,  116.0, 
   2.375,  118.0, 
   2.0,    119.0,
   1.5,    119.5,
   1.0,    119.75, 
   0.5,    119.875,
   -0.5,    119.875,
   -1.0,    119.75, 
   -1.5,    119.5,
   -2.0,    119.0,
   -2.375,  118.0, 
   -3.375,  116.0, 
   -5.375,  112.0, 
   -6.50,   108.0, 
   -7.875,  104.0, 
   -9.25,   100.0, 
   -10.375, 96.0, 
   -11.375, 90.0, 
   -12.5,   84.0, 
   -13.75,  78.0, 
   -13.875, 72.0, 
   -14.5,   66.0, 
   -14.875, 60.0, 
   -15.0,   54.0, 
   -14.875, 48.0, 
   -14.625, 42.0,
   -14.625, 40.0,
   -14.00,  32.0, 
   -13.25,  26.0, 
   -12.0,   20.0, 
   -8.75,   12.0,
   -7.25,   9.0,    
   -5.25,   6.0,
   0.0,     0.0
};
unsigned int g_kayakBodySize = 58;

float g_kayakMidOpen[]=
{
  0.0,     37.5,
  9.75,    40.0,
  10.125,  42.0,
  10.625,  48.0,
  10.50,   54.0, 
  10.375,  60.0,
  9.875,   66.0, 
  9.00,    72.0,  
  7.25,    78.0,  
  6.75,    81.0,  
  3.25,    84.0,  
  1.50,    84.5,  
  0.00,    85.0,  
  -1.50,   84.5, 
  -3.25,   84.0, 
  -6.75,   81.0, 
  -7.25,   78.0, 
  -9.00,   72.0, 
  -9.875,  66.0,
  -10.375, 60.0, 
  -10.50,  54.0,  
  -10.625, 48.0, 
  -10.125, 42.0, 
  -9.75,   40.0,   
  0.0,     39.5
};
unsigned int g_kayakMidOpenSize = 25;

//  Thin AUV    --------------------------------------

float g_thin_auvBody[]=
{
  5.0,   0.0,
  10.5,  18.0,
  10.5,  90.0,
  10.0,  92.0,
  9.0,   95.0,
  7.0,   97.0,
  5.0,   99.0,
  2.0,   100.0,
  -2.0,   100.0,
  -5.0,   99.0,
  -7.0,   97.0,
  -9.0,   95.0,
  -10.0,  92.0,
  -10.5, 90.0, 
  -10.5, 18.0,
  -5.0,  0.0
};
unsigned int g_thin_auvBodySize = 16;

float g_thin_propUnit[]=
{
  11.0, 0.0, 
  11.0, -9.0,
  -11.0, -9.0,
  -11.0, 0.0
};

unsigned int g_thin_propUnitSize = 4;


//  (Fat) AUV    --------------------------------------

float g_auvBody[]=
{
  5.0,   0.0,
  15.5,  18.0,
  15.5,  90.0,
  15.0,  92.0,
  12.0,   95.0,
  8.0,   97.0,
  5.0,   99.0,
  2.0,   100.0,
  -2.0,   100.0,
  -5.0,   99.0,
  -8.0,   97.0,
  -12.0,   95.0,
  -15.0,  92.0,
  -15.5, 90.0, 
  -15.5, 18.0,
  -5.0,  0.0
};
unsigned int g_auvBodySize = 16;

float g_propUnit[]=
{
  16.0, 0.0, 
  16.0, -9.0,
  -16.0, -9.0,
  -16.0, 0.0
};

unsigned int g_propUnitSize = 4;


//  Ship    --------------------------------------

float g_shipBody[]=
{
  10.0,  -26.0,
  13.0,  -16.0,
  15.0,  -6.0,
  15.2,  -1.0,
  15.0,  4.0,
  10.0,  14.0,
  1.0,   27.0, 
  -1.0,  27.0, 
  -10.0, 14.0, 
  -15.0, 4.0, 
  -15.2, -1.0, 
  -15.0, -6.0,
  -13.0, -16.0,
  -10.0, -26.0,
  10.0,  -26.0
};
unsigned int g_shipBodySize = 15;

#if 0
float g_shipBody[]=
{
  10.0,  0.0,
  13.0,  10.0,
  15.0,  20.0,
  15.2,  25.0,
  15.0,  30.0,
  10.0,  40.0,
  1.0,   53.0,
  -1.0,  53.0,
  -10.0, 40.0,
  -15.0, 30.0,
  -15.2, 25.0,
  -15.0, 20.0,
  -13.0, 10.0, 
  -10.0, 0.0,
   10.0, 0.0
};
unsigned int g_shipBodySize = 15;
#endif
