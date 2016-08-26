/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_VECT_Model.cpp                                  */
/*    DATE: May 16th, 2016                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
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
