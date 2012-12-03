/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AppMsg.cpp                                           */
/*    DATE: Jun 14th 2012                                        */
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

#include "AppMsg.h"

using namespace std;

AppMsg::AppMsg()
{
  m_cDataType = MSG_DOUBLE;
  m_dfTime    = -1;
  m_dfVal     = -1;
}

AppMsg::AppMsg(const std::string& sKey, double dfVal, double dfTime)
{
  m_cDataType = MSG_DOUBLE;
  m_dfVal     = dfVal;
  m_sKey      = sKey;
  m_dfTime    = dfTime;
}


AppMsg::AppMsg(const string& sKey, const string& sVal, double dfTime)
{
  m_cDataType = MSG_STRING;
  m_dfVal     = -1;
  m_sKey      = sKey;
  m_sVal      = sVal;
  m_dfTime    = dfTime;
}



