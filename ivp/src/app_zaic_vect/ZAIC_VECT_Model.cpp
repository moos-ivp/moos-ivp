/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ZAIC_VECT_Model.cpp                                  */
/*    DATE: May 16th, 2016                                       */
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
#include "ZAIC_VECT_Model.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: getIvPFunction()
//      Note: Virtual function overloaded

IvPFunction *ZAIC_VECT_Model::getIvPFunction()
{
  if(!m_zaic_vect)
    return(0);
  m_zaic_vect->setTolerance(m_tolerance);
  return(m_zaic_vect->extractOF());
}

//-------------------------------------------------------------
// Procedure: setZAIC()

void ZAIC_VECT_Model::setZAIC(ZAIC_Vector *zaic)
{
  if(m_zaic_vect)
    delete(m_zaic_vect);
  m_zaic_vect = zaic;
}

//----------------------------------------------------------------
// Procedure: getMinUtil()

double ZAIC_VECT_Model::getMinUtil()
{
  if(!m_zaic_vect)
    return(0);
  return(m_zaic_vect->getParam("minutil"));
}

//----------------------------------------------------------------
// Procedure: getMaxUtil()

double ZAIC_VECT_Model::getMaxUtil()
{
  if(!m_zaic_vect)
    return(0);
  return(m_zaic_vect->getParam("maxutil"));
}

//----------------------------------------------------------------
// Procedure: getTolerance()

double ZAIC_VECT_Model::getTolerance()
{
  if(!m_zaic_vect)
    return(0);
  return(m_tolerance);
}

//----------------------------------------------------------------
// Procedure: setTolerance()

void ZAIC_VECT_Model::setTolerance(double dval)
{
  if(!m_zaic_vect)
    return;

  m_tolerance = dval;
  if(m_tolerance < 0)
    m_tolerance = 0;
}

//----------------------------------------------------------------
// Procedure: modTolerance()

void ZAIC_VECT_Model::modTolerance(double delta)
{
  if(!m_zaic_vect)
    return;
  
  m_tolerance += delta;  
  if(m_tolerance < 0)
    m_tolerance = 0;
}

//----------------------------------------------------------------
// Procedure: getTotalPieces()

unsigned int ZAIC_VECT_Model::getTotalPieces()
{
  if(!m_zaic_vect)
    return(0);
  return(m_zaic_vect->getTotalPieces());
}

