/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Shape_AUV.h                                          */
/*    DATE: September 21st 2007                                  */
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

#ifndef SHAPE_AUV_HEADER
#define SHAPE_AUV_HEADER

double g_auvBody[]=
{
    5.0,    0.0,    
   15.5,   18.0,
   15.5,   90.0,   
   15.0,   92.0,
   12.0,   95.0,  
    8.0,   97.0,
    5.0,   99.0,   
    2.0,  100.0,
   -2.0,  100.0,  
   -5.0,   99.0,
   -8.0,   97.0,   
  -12.0,   95.0,
  -15.0,   92.0,  
  -15.5,   90.0, 
  -15.5,   18.0,   
   -5.0,    0.0
};
unsigned int g_auvBodySize = 16;
double       g_auvScale = 1.0;

double g_propUnit[]=
{
   16.0,  0.0,   
   16.0, -9.0,
  -16.0, -9.0, 
  -16.0,  0.0
};

unsigned int g_propUnitSize =  4;

double  g_auvCtrX = 0.0;
double  g_auvCtrY = 45.5;

#endif

