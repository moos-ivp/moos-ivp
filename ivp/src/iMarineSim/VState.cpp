/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VState.cpp                                           */
/*    DATE: Oct 25th 2004                                        */
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

#include "VState.h"
#include <stdio.h>

using namespace std;

VState::VState() 
{
  m_dfX          =  0.0;
  m_dfY          =  0.0;
  m_dfHeading    =  3.14;  // PI
  m_dfSpeed      =  0.0;
  m_dfDepth      =  0.0;
  m_dfTime       = -1.0;
}
  
//-------------------------------------------------------------------
// Procedure: toString
//   Purpose: 

string VState::toString()
{
  double heading_degrees = m_dfHeading * 180.0 / 3.1415926;

  char buf[256];
  sprintf(buf,"timestamp=%.3f,x=%f,y=%f,theta=%.4g,speed=%f",
	  m_dfTime, m_dfX, m_dfY, heading_degrees, m_dfSpeed);
  
  return(string(buf));
}








