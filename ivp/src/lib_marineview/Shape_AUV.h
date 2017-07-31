/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Shape_AUV.h                                          */
/*    DATE: September 21st 2007                                  */
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
double       g_auvLength   = 100.0;

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










