/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Shape_WAMV.h                                         */
/*    DATE: April 30th 2014                                      */
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

#ifndef SHAPE_WAMV_HEADER
#define SHAPE_WAMV_HEADER

double g_wamvBody[]=
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
unsigned int g_wamvBodySize = 16;
double       g_wamvLength   = 100.0;

double g_wamvpropUnit[]=
{
   16.0,  0.0,   
   16.0, -9.0,
  -16.0, -9.0, 
  -16.0,  0.0
};

unsigned int g_wamvpropUnitSize =  4;

double g_wamvPontoonConnector[]=
  {
    0.0, 40.0,
    0.0, 50.0,
    -50.0, 50.0,
    -50.0, 40.0
  };

unsigned int g_pontoonConnectorSize = 4;

double  g_wamvCtrX = 0.0;
double  g_wamvCtrY = 45.5;

double  g_wamvBase = 50.0;

#endif










