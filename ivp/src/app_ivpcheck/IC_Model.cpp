/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IC_Model.cpp                                         */
/*    DATE: Apr 19th 2011                                        */
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

#include <iostream>
#include "MBUtils.h"
#include "IC_Model.h"
#include "ColorParse.h"


#define GL_PI 3.1415926f

using namespace std;

//-------------------------------------------------------------
// Constructor

IC_Model::IC_Model()
{
  m_collective = false;
}

//-------------------------------------------------------------
// Procedure: addIPF()

void IC_Model::addIPF(string str)
{
  QueueEntry entry(str);
  m_queue_entries.push_back(entry);
}

//-------------------------------------------------------------
// Procedure: modColorMap

void IC_Model::modColorMap(const string &str)
{
  //m_fqueue.modColorMap(str);
}

//-------------------------------------------------------------
// Procedure: getQuadSet()

const QuadSet* IC_Model::getQuadSet()
{
  if(m_collective)
    quadset = m_fqueue.getCollectiveQuadSet();
  else
    quadset = m_fqueue.getQuadSet(m_curr_descriptor);

  return(quadset);
}






