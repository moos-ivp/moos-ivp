/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Shape_LongShip.h                                     */
/*    DATE: February 25th 2021                                   */
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

#ifndef SHAPE_LONG_SHIP_HEADER
#define SHAPE_LONG_SHIP_HEADER

double g_longShipBody[]=
{
 3.5, 0,
 3.85, 5,
 4.0, 10,
 4.27, 15,
 4.4, 20,
 4.2, 25,
 4.0, 30,
 3.4, 35,
 2.8, 40,
 1.8, 45,
 1.4, 48,
 1.2, 49, 
 1, 50,
 0.2, 51,
 0, 51.2,
 -0.2, 51,
 -1, 50,
 -1.2, 49, 
 -1.4, 48,
 -1.8, 45,
 -2.8, 40,
 -3.4, 35,
 -4.0, 30,
 -4.2, 25,
 -4.4, 20,
 -4.27, 15,
 -4.0, 10,
 -3.85, 5,
 -3.5, 0
};

double       g_longShipCtrX     = 0.0;
double       g_longShipCtrY     = 15.6;
unsigned int g_longShipBodySize = 29;
double       g_longShipLength   = 51.2;

#endif














