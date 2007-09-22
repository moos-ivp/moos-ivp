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

