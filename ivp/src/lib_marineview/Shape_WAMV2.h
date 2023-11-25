/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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

unsigned int g_wamvSize   = 5;
double       g_wamvLength = 200.0;
double       g_wamvCtrX   = 0.0;  
double       g_wamvCtrY   = 0.0;  
double       g_wamvBase   = 60.0; 

double g_wamvPontoonSize = 7.0;
double g_wamvPontoon[] =
{
 5.0,     100.0,
 15.0,     60.0,
 15.0,     -100.0,
 -15.0,     -100.0,
 -15.0,     60.0,
 -5.0,     100.0,
 5.0,     100.0
};

double g_wamvTopSize = 8.0;
double g_wamvTop[]=
{
 60.0,     50.0,
 70.0,     50.0,
 105.0,    30.0,
 105.0,   -90.0,
 15.0,    -90.0,
 15.0,     30.0,
 50.0,     50.0,
 60.0,     50.0
};

#endif





