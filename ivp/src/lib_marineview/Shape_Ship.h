/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Shape_Ship.h                                         */
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

#ifndef SHAPE_SHIP_HEADER
#define SHAPE_SHIP_HEADER

double g_shipBody[]=
{
  30.0,   0.0,      
  39.0,  40.0,
  45.0,  80.0,     
  46.0, 100.0,
  45.0, 120.0,     
  35.0, 160.0,
   3.0, 212.0, 
  -3.0, 212.0,
 -35.0, 160.0, 
 -45.0, 120.0,
 -46.0, 100.0,  
 -45.0,  80.0,
 -39.0,  40.0, 
 -30.0,   0.0,
  30.0,   0.0
};

double       g_shipCtrX     = 0.0;
double       g_shipCtrY     = 106.5;
unsigned int g_shipBodySize = 15;
double       g_shipScale    = 0.8;

double g_shipFatBody[]=
{
  40.0,   0.0,      
  52.0,  40.0,
  60.0,  80.0,     
  60.8, 100.0,
  60.0, 120.0,     
  40.0, 160.0,
   4.0, 212.0, 
  -4.0, 212.0,
 -40.0, 160.0, 
 -60.0, 120.0,
 -60.8, 100.0,  
 -60.0,  80.0,
 -60.0,  40.0, 
 -52.0,   0.0,
  40.0,   0.0
};

double       g_shipFatCtrX     = 0.0;
double       g_shipFatCtrY     = 106.5;
unsigned int g_shipFatBodySize = 15;
double        g_shipFatScale    = 1.0;

#endif




